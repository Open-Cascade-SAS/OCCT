// Modified	27/12/97 : FMN ; PERF: OPTIMISATION LOADER (LOPTIM)
// Modified	08/12/98 : FMN ; SGI : Changement option de compilation
//

#include <Aspect_ColorRampColorMap.hxx>
#include <Image_PseudoColorImage.hxx>
#include <AlienImage_SGIRGBFileHeader.hxx>
#include <AlienImage_SGIRGBAlienData.ixx>
#include <Standard.hxx>

#define TRACE

#include <stdio.h>
#if !defined(_IOERR)
# define       _IOERR          0040    /* I/O error from system */
#endif
#if !defined(_IOREAD)
# define       _IOREAD         0001    /* currently reading */
#endif
#if !defined(_IOWRT)
# define       _IOWRT          0002    /* currently writing */
#endif
#if !defined(_IORW)
# define       _IORW           0200    /* opened for reading and writing */
#endif
 
#ifdef TRACE
static int Verbose = 0 ;
#endif

#define LOPTIM
#ifndef LOPTIM
static Handle(Aspect_ColorRampColorMap) GrayScaleColorMap = 
	new Aspect_ColorRampColorMap( 0,256, Quantity_Color(1.,1.,1.,Quantity_TOC_RGB));
#else 
static Handle(Aspect_ColorRampColorMap)& _GrayScaleColorMap() {
static Handle(Aspect_ColorRampColorMap) GrayScaleColorMap =
      new Aspect_ColorRampColorMap( 0,256, Quantity_Color(1.,1.,1.,Quantity_TOC_RGB));
return GrayScaleColorMap;
}
#define GrayScaleColorMap _GrayScaleColorMap()
#endif // LOPTIM
  

//============================================================================
//==== C Class Method.
//============================================================================
static Standard_Boolean iopen(	OSD_File &File,
					AlienImage_SGIRGBFileHeader *,
					char ,
					unsigned int ,
					unsigned int ,
					unsigned int ,
					unsigned int ,
					unsigned int  );
static void		iclose(   	OSD_File &File,AlienImage_SGIRGBFileHeader *);
int 		iflush(   	OSD_File &File,AlienImage_SGIRGBFileHeader *);
static void 		isetname( 	AlienImage_SGIRGBFileHeader *, const char * );
#ifdef DEB
static void 		isetcolormap(	AlienImage_SGIRGBFileHeader *, int );
#endif
static int 		putrow(		OSD_File &File,
				AlienImage_SGIRGBFileHeader *,
				unsigned short	*,
				unsigned int , 
				unsigned int );
static int 		getrow(		OSD_File &File,
				AlienImage_SGIRGBFileHeader *,
				unsigned short	*,
				unsigned int , 
				unsigned int );


//------------------------------------------------------------------------------
//		Private Method
//------------------------------------------------------------------------------

#define DataSize() (myHeader.ysize*myHeader.xsize*sizeof(short))

Handle(Image_ColorImage) AlienImage_SGIRGBAlienData::ToColorImage() const

{ Handle(Image_ColorImage) ret_image = 
	new Image_ColorImage( 	0,0,
				(Standard_Integer)myHeader.xsize,
				(Standard_Integer)myHeader.ysize ) ;
  unsigned short *rbuf, *gbuf, *bbuf ;
  Aspect_ColorPixel CPixel ;
  Quantity_Color    acolor ;
  Standard_Real r,g,b ;
  unsigned short x,y ;
  Standard_Integer LowX = ret_image->LowerX() ;
  Standard_Integer LowY = ret_image->LowerY() ;

  rbuf = ( unsigned short * ) myRedData ;
  gbuf = ( unsigned short * ) myGreenData ;
  bbuf = ( unsigned short * ) myBlueData ;

  for(y=0; y<myHeader.ysize; y++) {
	for(x=0; x<myHeader.xsize; x++) {
		r = Standard_Real( rbuf[x] ) ;
		r /= 255. ;
		g = Standard_Real( gbuf[x] ) ;
		g /= 255. ;
		b = Standard_Real( bbuf[x] ) ;
		b /= 255. ;
		acolor.SetValues( r,g,b, Quantity_TOC_RGB ) ;
		CPixel.SetValue ( acolor ) ;

         	ret_image->SetPixel( LowX+x , LowY+y, CPixel ) ;
	}
        rbuf += myHeader.xsize ;
        gbuf += myHeader.xsize ;
        bbuf += myHeader.xsize ;
  }

  return( ret_image ) ;
}

Handle_Image_PseudoColorImage AlienImage_SGIRGBAlienData::ToPseudoColorImage() 
								const

{ Handle(Image_PseudoColorImage) ret_image = 
	new Image_PseudoColorImage( 	0,0,
					(Standard_Integer)myHeader.xsize,
					(Standard_Integer)myHeader.ysize,
					GrayScaleColorMap ) ;

  unsigned short *rbuf ;
  Aspect_IndexPixel CPixel ;
  unsigned short x,y ;
  Standard_Integer LowX = ret_image->LowerX() ;
  Standard_Integer LowY = ret_image->LowerY() ;

  rbuf = ( unsigned short * ) myRedData ;

  for(y=0; y<myHeader.ysize; y++) {
	for(x=0; x<myHeader.xsize; x++) {
		CPixel.SetValue ( Standard_Integer( rbuf[x] ) ) ;

         	ret_image->SetPixel( LowX+x , LowY+y, CPixel ) ;
	}
        rbuf += myHeader.xsize ;
  }

  return ret_image ;

}

#include	<stdlib.h>
#include	<string.h>


//============================================================================
//==== C Class Method.
static unsigned short 	*ibufalloc(AlienImage_SGIRGBFileHeader *);
#ifdef DEB
static unsigned int 	reverse(unsigned int ) ;
#endif
static void 		cvtshorts( unsigned short [], int );
static void 		cvtlongs( int [], int  ) ;
static void 		cvtimage(int [] );
static int 		img_seek( 	OSD_File &File,
					AlienImage_SGIRGBFileHeader *,
				  	unsigned int , unsigned int  );
static int 		img_badrow(	AlienImage_SGIRGBFileHeader *, 
					int , int  );
static int 		img_write( 	OSD_File &File,
					AlienImage_SGIRGBFileHeader *,
					char *,	int  );
static int 		img_read( 	OSD_File &File,
					AlienImage_SGIRGBFileHeader *,
					char *,	int  );
static int 		img_optseek(	OSD_File &File,
					AlienImage_SGIRGBFileHeader *,
					unsigned int );
static int 		img_getrowsize( AlienImage_SGIRGBFileHeader * );
static void 		img_setrowsize( AlienImage_SGIRGBFileHeader *,
					int , int , int  );
static int 		img_rle_compact(	unsigned short *,
						int ,
						unsigned short *,
						int , int );
static void 		img_rle_expand(	unsigned short *,
					int ,
					unsigned short *,
					int  );

void i_errhdlr(const char *fmt)
{
		printf("%s",fmt);
		return;
}

/*
 *	iclose and iflush -
 *
 */

static void iclose( OSD_File& File, AlienImage_SGIRGBFileHeader *image)
{
    int tablesize;

    iflush( File, image);
    img_optseek( File, image, 0);
    if (image->flags&_IOWRT) {
	if(image->dorev)
	    cvtimage((int *)image);
	if (	img_write( File, image,
			(char *)image,sizeof(AlienImage_SGIRGBFileHeader)) != 	
		sizeof(AlienImage_SGIRGBFileHeader)) {
	    i_errhdlr("iclose: error on write of image header\n");
	    return ;
	}
	if(image->dorev)
	    cvtimage((int *)image);
	if(ISRLE(image->type)) {
	    img_optseek( File, image, 512);
	    tablesize = image->ysize*image->zsize*sizeof(int);
	    if(image->dorev)
		cvtlongs((int *)image->rowstart,tablesize);
	    if (img_write( File, image,(char *)image->rowstart,tablesize) != tablesize) {
		i_errhdlr("iclose: error on write of rowstart\n");
		return ;
	    }
	    if(image->dorev)
		cvtlongs((int *)image->rowsize,tablesize);
	    if (img_write( File, image,(char *)image->rowsize,tablesize) != tablesize) {
		i_errhdlr("iclose: error on write of rowsize\n");
		return ;
	    }
	}
    }
    if(image->base) {
	free(image->base);
	image->base = 0;
    }
    if(image->tmpbuf) {
	free(image->tmpbuf);
	image->tmpbuf = 0;
    }
    if(ISRLE(image->type)) {
	free(image->rowstart);
	image->rowstart = 0;
	free(image->rowsize);
	image->rowsize = 0;
    }

//    return(close(image->file));
}

int iflush( OSD_File& File, AlienImage_SGIRGBFileHeader *image)
{
    unsigned short *base;

    if ( (image->flags&_IOWRT)
     && (base=image->base)!=NULL && (image->ptr-base)>0) {
	    if (putrow(File,image, base, image->y,image->z)!=image->xsize) {
		    image->flags |= _IOERR;
		    return(EOF);
	    }
    }
    return 0 ;
}
/*
 *	isetname and isetcolormap -
 *
 */

static void isetname( AlienImage_SGIRGBFileHeader *image, const char *name )
{
    strncpy(image->name,name,80);
}

// Unused :
#ifdef DEB
static void isetcolormap(AlienImage_SGIRGBFileHeader *image, int colormap)
{
    image->colormap = colormap;
}
#endif
/*
 *	iopen - 
 *
 */
static Standard_Boolean iopen( 	OSD_File &File,
				AlienImage_SGIRGBFileHeader *image,
				char mode,
				unsigned int type,
				unsigned int dim,
				unsigned int xsize,
				unsigned int ysize,
				unsigned int zsize )
{
	
	int tablesize;
	register int i, max;

	Standard_Integer bblcount ;

	int calsize	= sizeof( AlienImage_SGIRGBFileHeader ) ;
	if (mode=='w') { //------------------WRITE------------------
		image->imagic = IMAGIC;
		image->type = type;
		image->xsize = xsize;
		image->ysize = 1;
		image->zsize = 1;
		if (dim>1)
		    image->ysize = ysize;
		if (dim>2)
		    image->zsize = zsize;
		if(image->zsize == 1) {
		    image->dim = 2;
		    if(image->ysize == 1)
			image->dim = 1;
		} else {
		    image->dim = 3;
		}
		image->min = 10000000;
		image->max = 0;
		isetname(image,"no name"); 
		image->wastebytes = 0;
		image->dorev = 0;

		File.Write( ( Standard_Address ) image, calsize ) ;
  		if ( File.Failed() ) {
			// ERROR
		    	i_errhdlr("iopen: error on write of image header\n");
			return( Standard_False ) ;
  		}
	} else {//--------------------------READ------------------------
		File.Read( ( Standard_Address& )image, calsize, bblcount ) ;
  		if ( File.Failed() || 
		     ( bblcount != calsize ) ) {
#ifdef TRACE
			// ERROR
		    	i_errhdlr("iopen: error on read of image header\n");
#endif
			return( Standard_False ) ;
  		}

		if( ((image->imagic>>8) | ((image->imagic&0xff)<<8)) 
							     == IMAGIC ) {
		    image->dorev = 1;
		    cvtimage((int *)image);
		} else
		    image->dorev = 0;
		if (image->imagic != IMAGIC) {
#ifdef TRACE
			i_errhdlr("iopen: bad magic in image file \n");
#endif
		    return Standard_False ;
		}
	}
	if (mode != 'r')
	    image->flags = _IOWRT;
	else
	    image->flags = _IOREAD;
	if(ISRLE(image->type)) {
	    tablesize = image->ysize*image->zsize*sizeof(int);
	    image->rowstart = (unsigned int *)malloc(tablesize);
	    image->rowsize = (int *)malloc(tablesize);
	    if( image->rowstart == 0 || image->rowsize == 0 ) {
		i_errhdlr("iopen: error on table alloc\n");
		return Standard_False ;
	    }
	    image->rleend = 512+2*tablesize;
	    if (mode=='w') {
		max = image->ysize*image->zsize;
		for(i=0; i<max; i++) {
		    image->rowstart[i] = 0;
		    image->rowsize[i] = -1;
		}
	    } else {
		tablesize = image->ysize*image->zsize*sizeof(int);
		File.Seek( 512, OSD_FromBeginning ) ;
		File.Read( (Standard_Address&) image->rowstart,tablesize, bblcount);
  		if ( File.Failed() || ( bblcount != tablesize ) ) {
			// ERROR
		    	i_errhdlr("iopen: error on read of rowstart\n");
			return( Standard_False ) ;
  		}

		if(image->dorev)
		    cvtlongs((int *)image->rowstart,tablesize);
		File.Read( (Standard_Address&) image->rowsize,tablesize, bblcount);
  		if ( File.Failed() || ( bblcount != tablesize ) ) {
			// ERROR
		    	i_errhdlr("iopen: error on read of rowsize\n");
			return( Standard_False ) ;
  		}

		if(image->dorev)
		    cvtlongs((int *)image->rowsize,tablesize);
	    }
	}
	image->cnt = 0;
	image->ptr = 0;
	image->base = 0;
	if( (image->tmpbuf = ibufalloc(image)) == 0 ) {	
	    i_errhdlr("iopen: error on tmpbuf alloc\n");
	    return Standard_False ;
	}
	image->x = image->y = image->z = 0;
	image->file = 0;
	image->offset = 512;			/* set up for img_optseek */

	File.Seek( 512, OSD_FromBeginning ) ;

	return(Standard_True);
}


static unsigned short *ibufalloc(AlienImage_SGIRGBFileHeader *image)
{
    return (unsigned short *)malloc(IBUFSIZE(image->xsize));
}

// Unused :
#ifdef DEB
static unsigned int reverse(unsigned int lwrd) 
{
    return ((lwrd>>24) 		| 
	   (lwrd>>8 & 0xff00) 	| 
	   (lwrd<<8 & 0xff0000) | 
	   (lwrd<<24) 		);
}
#endif

static void cvtshorts( unsigned short buffer[], int n)
{
    register short i;
    register int nshorts = n>>1;
    register unsigned short swrd;

    for(i=0; i<nshorts; i++) {
	swrd = *buffer;
	*buffer++ = (swrd>>8) | (swrd<<8);
    }
}

static void cvtlongs( int buffer[], int n ) 
{
    register short i;
    register int nlongs = n>>2;
    register unsigned int lwrd;

    for(i=0; i<nlongs; i++) {
	lwrd = buffer[i];
	buffer[i] =     ((lwrd>>24) 		| 
	   		(lwrd>>8 & 0xff00) 	| 
	   		(lwrd<<8 & 0xff0000) 	| 
	   		(lwrd<<24) 		);
    }
}

static void cvtimage(int buffer[] )
{
    cvtshorts((unsigned short *)buffer,12);
    cvtlongs(buffer+3,12);
    cvtlongs(buffer+26,4);
}

/*
 *	img_seek, img_write, img_read, img_optseek -
 *
 *
 */

static int img_seek( 	OSD_File &File,
			AlienImage_SGIRGBFileHeader *image,
			unsigned int y, unsigned int z )
{
    if(img_badrow(image,y,z)) {
	i_errhdlr("img_seek: row number out of range\n");
	return EOF;
    }
    image->x = 0;
    image->y = y;
    image->z = z;
    if(ISVERBATIM(image->type)) {
	switch(image->dim) {
	    case 1:
		return img_optseek(File,image, 512);
	    case 2: 
		return img_optseek(File,image,
			512+(y*image->xsize)*BPP(image->type));
	    case 3: 
		return img_optseek(File,image,
		    512+(y*image->xsize+z*image->xsize*image->ysize)*
							BPP(image->type));
	    default:
		i_errhdlr("img_seek: weird dim\n");
		break;
	}
    } else if(ISRLE(image->type)) {
	switch(image->dim) {
	    case 1:
		return img_optseek(File, image, image->rowstart[0]);
	    case 2: 
		return img_optseek(File, image, image->rowstart[y]);
	    case 3: 
		return img_optseek(File, image, 
			image->rowstart[y+z*image->ysize]);
	    default:
		i_errhdlr("img_seek: weird dim\n");
		break;
	}
    } else 
	i_errhdlr("img_seek: weird image type\n");
 return -1;
}

static int img_badrow(AlienImage_SGIRGBFileHeader *image, int y, int z )
{
    if(y>=(int)image->ysize || z>=(int)image->zsize)
	return 1;
//    else
    return 0;
}

static int img_write( OSD_File &File,
			AlienImage_SGIRGBFileHeader *image,char *buffer,
			int bblcount )
{
    int retval;

    File.Write( (Standard_Address) buffer,bblcount ) ;

    if ( File.Failed() ) retval = 0 ;
    else 		 retval = bblcount ;

    if(retval == bblcount) 
	image->offset += bblcount;
    else
	image->offset = (unsigned int ) -1;
    return retval;
}

static int img_read( OSD_File &File,
			AlienImage_SGIRGBFileHeader *image, char *buffer,
				int bblcount )
{
    int retval;
    File.Read( (Standard_Address&)buffer,bblcount, retval ) ;
    if(retval == bblcount && !File.Failed() ) 
	image->offset += bblcount;
    else
	image->offset = (unsigned int ) -1;
    return retval;
}

static int img_optseek(	OSD_File &File,
			AlienImage_SGIRGBFileHeader *image,
			unsigned int offset)
{

    if(image->offset != offset) {
       image->offset = offset;
       	File.Seek( offset, OSD_FromBeginning ) ;

	return( offset ) ;
       //return lseek(image->file,offset,0);
   }
   return offset;
}

/*
 *	img_getrowsize, img_setrowsize, img_rle_compact, img_rle_expand -
 *
 *
 */

static int img_getrowsize( AlienImage_SGIRGBFileHeader *image )
{
    switch(image->dim) {
	case 1:
	    return image->rowsize[0];
	case 2:
	    return image->rowsize[image->y];
	case 3:
	    return image->rowsize[image->y+image->z*image->ysize];
    }
 return -1;
}

static void img_setrowsize( 	AlienImage_SGIRGBFileHeader *image,
				int cnt, int y, int z )
{
    int *sizeptr=NULL;

    if(img_badrow(image,y,z)) 
	return;
    switch(image->dim) {
	case 1:
	    sizeptr = &image->rowsize[0];
	    image->rowstart[0] = image->rleend;
	    break;
	case 2:
	    sizeptr = &image->rowsize[y];
	    image->rowstart[y] = image->rleend;
	    break;
	case 3:
	    sizeptr = &image->rowsize[y+z*image->ysize];
	    image->rowstart[y+z*image->ysize] = image->rleend;
    }	
    if(*sizeptr != -1) 
	image->wastebytes += *sizeptr;
    *sizeptr = cnt;
    image->rleend += cnt;
}

#define docompact 							\
	while(iptr<ibufend) {						\
	    sptr = iptr;						\
	    iptr += 2;							\
	    while((iptr<ibufend)&&((iptr[-2]!=iptr[-1])||(iptr[-1]!=iptr[0])))\
		iptr++;							\
	    iptr -= 2;							\
	    bblcount = iptr-sptr;					\
	    while(bblcount) {						\
		todo = bblcount>126 ? 126:bblcount; 			\
		bblcount -= todo;					\
		*optr++ = 0x80|todo;					\
		while(todo--)						\
		    *optr++ = (unsigned char)*sptr++;			\
	    }								\
	    sptr = iptr;						\
	    cc = *iptr++;						\
	    while( (iptr<ibufend) && (*iptr == cc) )			\
		iptr++;							\
	    bblcount = iptr-sptr;					\
	    while(bblcount) {						\
		todo = bblcount>126 ? 126:bblcount; 			\
		bblcount -= todo;					\
		*optr++ = (unsigned char)todo;				\
		*optr++ = (unsigned char)cc;				\
	    }								\
	}								\
	*optr++ = 0;

static int img_rle_compact(	unsigned short *expbuf,
				int ibpp,
				unsigned short *rlebuf,
				int obpp, int cnt)
{
    if(ibpp == 1 && obpp == 1) {
	register unsigned char *iptr = (unsigned char *)expbuf;
	register unsigned char *ibufend = iptr+cnt;
	register unsigned char *sptr;
	register unsigned char *optr = (unsigned char *)rlebuf;
	register short todo, cc;
	register int bblcount;

	docompact;
	return (int)(optr - (unsigned char *)rlebuf);
    } else if(ibpp == 1 && obpp == 2) {
	register unsigned char *iptr = (unsigned char *)expbuf;
	register unsigned char *ibufend = iptr+cnt;
	register unsigned char *sptr;
	register unsigned short *optr = rlebuf;
	register short todo, cc;
	register int bblcount;

	docompact;
	return optr - rlebuf;
    } else if(ibpp == 2 && obpp == 1) {
	register unsigned short *iptr = expbuf;
	register unsigned short *ibufend = iptr+cnt;
	register unsigned short *sptr;
	register unsigned char *optr = (unsigned char *)rlebuf;
	register short todo, cc;
	register int bblcount;


	docompact;
	return (int)(optr - (unsigned char *)rlebuf);
    } else if(ibpp == 2 && obpp == 2) {
	register unsigned short *iptr = expbuf;
	register unsigned short *ibufend = iptr+cnt;
	register unsigned short *sptr;
	register unsigned short *optr = rlebuf;
	register short todo, cc;
	register int bblcount;

	docompact;
	return optr - rlebuf;
    } else  {
	i_errhdlr("rle_compact: bad bpp\n");
	return 0;
    }
}

#define doexpand				\
	while(1) {				\
	    pixel = *iptr++;			\
	    if ( !(bblcount = (pixel & 0x7f)) )	\
		return;				\
	    if(pixel & 0x80) {			\
	       while(bblcount--)		\
		    *optr++ = (unsigned char)*iptr++;		\
	    } else {				\
	       pixel = *iptr++;			\
	       while(bblcount--)		\
		    *optr++ = (unsigned char)pixel; \
	    }					\
	}

static void img_rle_expand(	unsigned short *rlebuf,
				int ibpp,
				unsigned short *expbuf,
				int obpp )

{
    if(ibpp == 1 && obpp == 1) {
	register unsigned char *iptr = (unsigned char *)rlebuf;
	register unsigned char *optr = (unsigned char *)expbuf;
	register unsigned short pixel,bblcount;

	doexpand;
    } else if(ibpp == 1 && obpp == 2) {
	register unsigned char *iptr = (unsigned char *)rlebuf;
	register unsigned short *optr = expbuf;
	register unsigned short pixel,bblcount;

	doexpand;
    } else if(ibpp == 2 && obpp == 1) {
	register unsigned short *iptr = rlebuf;
	register unsigned char  *optr = (unsigned char *)expbuf;
	register unsigned short pixel,bblcount;

	doexpand;
    } else if(ibpp == 2 && obpp == 2) {
	register unsigned short *iptr = rlebuf;
	register unsigned short *optr = expbuf;
	register unsigned short pixel,bblcount;

	doexpand;
    } else 
	i_errhdlr("rle_expand: bad bpp\n");
}
/*
 *	putrow, getrow -
 *
 */

static int putrow(	OSD_File& File,
		AlienImage_SGIRGBFileHeader	*image,
		unsigned short	*buffer,
		unsigned int y, unsigned int z )
{
    register unsigned short 	*sptr;
    register unsigned char      *cptr;
    register unsigned int x;
    register unsigned int min, max;
    register int cnt;

    if( !(image->flags & (_IORW|_IOWRT)) )
	return -1;
    if(image->dim<3)
	z = 0;
    if(image->dim<2)
	y = 0;
    if(ISVERBATIM(image->type)) {
	switch(BPP(image->type)) {
	    case 1: 
		min = image->min;
		max = image->max;
		cptr = (unsigned char *)image->tmpbuf;
		sptr = buffer;
		for(x=image->xsize; x--;) { 
		    *cptr = (unsigned char )*sptr++;
		    if (*cptr > max) max = *cptr;
		    if (*cptr < min) min = *cptr;
		    cptr++;
		}
		image->min = min;
		image->max = max;
		img_seek( File, image,y,z);
		cnt = image->xsize;
		if (img_write( File, image,(char *)image->tmpbuf,cnt) != cnt) 
		    return -1;
		else
		    return cnt;
		/* NOTREACHED */

	    case 2: 
		min = image->min;
		max = image->max;
		sptr = buffer;
		for(x=image->xsize; x--;) { 
		    if (*sptr > max) max = *sptr;
		    if (*sptr < min) min = *sptr;
		    sptr++;
		}
		image->min = min;
		image->max = max;
		img_seek( File, image,y,z);
		cnt = image->xsize<<1;
		if(image->dorev)	
		    cvtshorts(buffer,cnt);
		if (img_write( File, image,(char *)buffer,cnt) != cnt) {
		    if(image->dorev)	
			cvtshorts(buffer,cnt);
		    return -1;
		} else {
		    if(image->dorev)	
			cvtshorts(buffer,cnt);
		    return image->xsize;
		}
		/* NOTREACHED */

	    default:
		i_errhdlr("putrow: weird bpp\n");
	}
    } else if(ISRLE(image->type)) {
	switch(BPP(image->type)) {
	    case 1: 
		min = image->min;
		max = image->max;
		sptr = buffer;
		for(x=image->xsize; x--;) { 
		    if (*sptr > max) max = *sptr;
		    if (*sptr < min) min = *sptr;
		    sptr++;
		}
		image->min = min;
		image->max = max;
		cnt = img_rle_compact(buffer,2,image->tmpbuf,1,image->xsize);
		img_setrowsize(image,cnt,y,z);
		img_seek( File, image,y,z);
		if (img_write( File, image,(char *)image->tmpbuf,cnt) != cnt) 
		    return -1;
		else
		    return image->xsize;
		/* NOTREACHED */

	    case 2: 
		min = image->min;
		max = image->max;
		sptr = buffer;
		for(x=image->xsize; x--;) { 
		    if (*sptr > max) max = *sptr;
		    if (*sptr < min) min = *sptr;
		    sptr++;
		}
		image->min = min;
		image->max = max;
		cnt = img_rle_compact(buffer,2,image->tmpbuf,2,image->xsize);
		cnt <<= 1;
		img_setrowsize(image,cnt,y,z);
		img_seek( File, image,y,z);
		if(image->dorev)
		    cvtshorts(image->tmpbuf,cnt);
		if (img_write( File, image,(char *)image->tmpbuf,cnt) != cnt) {
		    if(image->dorev)
			cvtshorts(image->tmpbuf,cnt);
		    return -1;
		} else {
		    if(image->dorev)
			cvtshorts(image->tmpbuf,cnt);
		    return image->xsize;
		}
		/* NOTREACHED */

	    default:
		i_errhdlr("putrow: weird bpp\n");
	}
    } else 
	i_errhdlr("putrow: weird image type\n");
return -1;
}

static int getrow(	OSD_File& File,
		AlienImage_SGIRGBFileHeader *image,
		unsigned short *buffer,
		unsigned int y, unsigned int z)
{
    register short i;
    register unsigned char *cptr;
    register unsigned short *sptr;
    register short cnt; 

    if( !(image->flags & (_IORW|_IOREAD)) )
	return -1;
    if(image->dim<3)
	z = 0;
    if(image->dim<2)
	y = 0;
    img_seek( File, image, y, z);
    if(ISVERBATIM(image->type)) {
	switch(BPP(image->type)) {
	    case 1: 
		if (img_read( File, image,(char *)image->tmpbuf,image->xsize) 
							    != image->xsize) 
		    return -1;
		else {
		    cptr = (unsigned char *)image->tmpbuf;
		    sptr = buffer;
		    for(i=image->xsize; i--;)
			*sptr++ = *cptr++;
		}
		return image->xsize;
		/* NOTREACHED */

	    case 2: 
		cnt = image->xsize<<1; 
		if (img_read( File, image,(char *)buffer,cnt) != cnt)
		    return -1;
		else {
		    if(image->dorev)
			cvtshorts(buffer,cnt);
		    return image->xsize;
		}
		/* NOTREACHED */

	    default:
		i_errhdlr("getrow: weird bpp\n");
		break;
	}
    } else if(ISRLE(image->type)) {
	switch(BPP(image->type)) {
	    case 1: 
		if( (cnt = img_getrowsize(image)) == -1 )
		    return -1;
		if( img_read( File, image,(char *)image->tmpbuf,cnt) != cnt )
		    return -1;
		else {
		    img_rle_expand(image->tmpbuf,1,buffer,2);
		    return image->xsize;
		}
		/* NOTREACHED */

	    case 2: 
		if( (cnt = img_getrowsize(image)) == -1 )
		    return -1;
		if( cnt != img_read( File, image,(char *)image->tmpbuf,cnt) )
		    return -1;
		else {
		    if(image->dorev)
			cvtshorts(image->tmpbuf,cnt);
		    img_rle_expand(image->tmpbuf,2,buffer,2);
		    return image->xsize;
		}
		/* NOTREACHED */

	    default:
		i_errhdlr("getrow: weird bpp\n");
		break;
	}
    } else 
	i_errhdlr("getrow: weird image type\n");
return -1;
}

#ifdef PIX
/*
 *	ifilbuf -
 *
 */
static int 	ifilbuf(  	AlienImage_SGIRGBFileHeader *);

static int ifilbuf( AlienImage_SGIRGBFileHeader *image )
{
	int size;

	if ((image->flags&_IOREAD) == 0)
		return(EOF);
	if (image->base==NULL) {
		size = IBUFSIZE(image->xsize);
		if ((image->base = ibufalloc(image)) == NULL) {
			i_errhdlr("can't alloc image buffer\n");
			return EOF;
		}
	}
	image->cnt = getrow( File, image,image->base,image->y,image->z);
	image->ptr = image->base;
	if (--image->cnt < 0) {
		if (image->cnt == -1) {
			image->flags |= _IOEOF;
			if (image->flags & _IORW)
				image->flags &= ~_IOREAD;
		} else
			image->flags |= _IOERR;
		image->cnt = 0;
		return -1;
	}
	if(++image->y >= (short)image->ysize) {
	    image->y = 0;
	    if(++image->z >= (short)image->zsize) {
		image->z = image->zsize-1;
		image->flags |= _IOEOF;
		return -1;
	    }
	}
	return *image->ptr++ & 0xffff;
}
/*
 *	iflsbuf -
 *
 *
 */
static int 	iflsbuf(  	OSD_File&,
				AlienImage_SGIRGBFileHeader *,unsigned int  );

static int iflsbuf( OSD_File& File,
		    AlienImage_SGIRGBFileHeader *image,unsigned int c )
{
	register unsigned short *base;
	register n, rn;
	int size;

	if ((image->flags&_IOWRT)==0)
		return(EOF);
	if ((base=image->base)==NULL) {
		size = IBUFSIZE(image->xsize);
		if ((image->base=base=ibufalloc(image)) == NULL) {
			i_errhdlr("flsbuf: error on buf alloc\n");
			return EOF;
		}
		rn = n = 0;
	} else if ((rn = n = image->ptr - base) > 0)  {
			n = putrow( File, image,base,image->y,image->z);
			if(++image->y >= ( short )image->ysize) {
			    image->y = 0;
			    if(++image->z >= ( short )image->zsize) {
				image->z = image->zsize-1;
				image->flags |= _IOEOF;
				return -1;
			    }
			}
 	}
	image->cnt = image->xsize-1;
	*base++ = c;
	image->ptr = base;
	if (rn != n) {
		image->flags |= _IOERR;
		return(EOF);
	}
	return(c);
}
/*
 *	getpix and putpix -
 *
 *
 */

#undef getpix
#undef putpix

int getpix( AlienImage_SGIRGBFileHeader *image )
{
    if(--(image)->cnt>=0)
    	return *(image)->ptr++;
    else
	return ifilbuf(image);
}

int putpix( AlienImage_SGIRGBFileHeader *image,
		unsigned int pix )
{
    if(--(image)->cnt>=0)
        return *(image)->ptr++ = pix;
    else
	return iflsbuf( File, (image,pix);
}
#endif
//------------------------------------------------------------------------------
//		Public Method
//------------------------------------------------------------------------------

AlienImage_SGIRGBAlienData::AlienImage_SGIRGBAlienData()

{ 
  myRedData = myGreenData = myBlueData = NULL ;
}

void AlienImage_SGIRGBAlienData::SetName( const TCollection_AsciiString& aName)

{ myName = aName + TCollection_AsciiString( "\0" ) ;
}

const TCollection_AsciiString&  AlienImage_SGIRGBAlienData::Name() const

{ return ( myName ) ; }

void AlienImage_SGIRGBAlienData::Clear()

{ myName.Clear() ;

  if ( myRedData ) {
	//Free all allocated memory
	Standard::Free( myRedData) ;
	myRedData   = NULL ;
  }
  if ( myGreenData ) {
	//Free all allocated memory
	Standard::Free( myGreenData) ;
	myRedData   = NULL ;
  }
  if ( myBlueData ) {
	//Free all allocated memory
	Standard::Free( myBlueData) ;
	myRedData   = NULL ;
  }

  myHeader.xsize = myHeader.ysize = myHeader.zsize = 0 ;
}


Standard_Boolean AlienImage_SGIRGBAlienData::Read( OSD_File& file )

{  Standard_Boolean Success = iopen(	file,
					&myHeader,
					'r' ,
					0 ,
					0 ,
					0 ,
					0 ,
					0 );

  unsigned short *rbuf, *gbuf, *bbuf ;
  unsigned short y ;

  if ( Success ) {
#ifdef TRACE
  if ( Verbose ) {
    /* print a little info about the image */
    printf("Image x and y size in pixels: %d %d\n",
			myHeader.xsize,myHeader.ysize);
    printf("Image zsize in channels: %d\n",myHeader.zsize);
    printf("Image pixel min and max: %d %d\n",myHeader.min,myHeader.max);
  }
#endif

    myName = TCollection_AsciiString( myHeader.name ) ;

    /* allocate buffers for image data */
    if ( DataSize() ) {
	myRedData = Standard::Allocate( DataSize() ) ;
    }

    if(myHeader.zsize >= 3 && DataSize()) {/*if the image has alpha zsize is 4*/
	myGreenData = Standard::Allocate( DataSize() ) ;
	myBlueData = Standard::Allocate( DataSize() ) ;
    }

    rbuf = ( unsigned short * ) myRedData ;
    gbuf = ( unsigned short * ) myGreenData ;
    bbuf = ( unsigned short * ) myBlueData ;

    /* check to see if the image is B/W or RGB */
    if(myHeader.zsize == 1) {
#ifdef TRACE
  if ( Verbose ) printf("This is a black and write image\n");
#endif
        for(y=0; y<myHeader.ysize; y++) {
            getrow(file, &myHeader,rbuf,y,0); rbuf += myHeader.xsize ;
        }
    } else if(myHeader.zsize >= 3) {  /* if the image has alpha zsize is 4 */
#ifdef TRACE
   if ( Verbose )       printf("This is a rgb image\n");
#endif
        for(y=0; y<myHeader.ysize; y++) {
            	getrow(file, &myHeader,rbuf,myHeader.ysize-1-y,0); 
		rbuf += myHeader.xsize ;
            	getrow(file, &myHeader,gbuf,myHeader.ysize-1-y,1); 
		gbuf += myHeader.xsize ;
            	getrow(file, &myHeader,bbuf,myHeader.ysize-1-y,2); 
		bbuf += myHeader.xsize ;
        }
    }
  }

  return Success ;

  
}

Standard_Boolean AlienImage_SGIRGBAlienData::Write( OSD_File& file ) const

{ unsigned short *rbuf, *gbuf, *bbuf ;
  unsigned short y;
  unsigned int xsize,ysize,zsize ;
  
  AlienImage_SGIRGBFileHeader WritedHeader ;

  xsize = myHeader.xsize ;
  ysize = myHeader.ysize ;
  zsize = myHeader.zsize ;

  Standard_Boolean Success = iopen(	file,
					&WritedHeader,
					'w' ,
				 	RLE(1),
					3,
					xsize,
					ysize,
					zsize);

  if ( Success ) {
#ifdef TRACE
  if ( Verbose ) {
    /* print a little info about the image */
    printf("Image x and y size in pixels: %d %d\n",
			WritedHeader.xsize,WritedHeader.ysize);
    printf("Image zsize in channels: %d\n",WritedHeader.zsize);
    printf("Image pixel min and max: %d %d\n",
			WritedHeader.min,WritedHeader.max);
  }
#endif

    isetname( &WritedHeader, (Standard_PCharacter)myName.ToCString() ) ;

    rbuf = ( unsigned short * ) myRedData ;
    gbuf = ( unsigned short * ) myGreenData ;
    bbuf = ( unsigned short * ) myBlueData ;

    /* check to see if the image is B/W or RGB */
    if(WritedHeader.zsize == 1) {
        for(y=0; y<WritedHeader.ysize; y++) {
            	putrow(file, &WritedHeader,rbuf,WritedHeader.ysize-1-y,0); 
		rbuf += WritedHeader.xsize ;
        }
    } else if(WritedHeader.zsize >= 3) { /* if the image has alpha zsize is 4 */
        for(y=0; y<WritedHeader.ysize; y++) {
            	putrow(file, &WritedHeader,rbuf,WritedHeader.ysize-1-y,0); 
		rbuf += WritedHeader.xsize ;
            	putrow(file, &WritedHeader,gbuf,WritedHeader.ysize-1-y,1); 
		gbuf += WritedHeader.xsize ;
            	putrow(file, &WritedHeader,bbuf,WritedHeader.ysize-1-y,2); 
		bbuf += WritedHeader.xsize ;
        }
    }
  }

  iclose( file, &WritedHeader ) ;

  return Success ;

}

Handle_Image_Image AlienImage_SGIRGBAlienData::ToImage() const

{   if ( myHeader.zsize == 1 ) {
#ifdef TRACE
  if ( Verbose ) printf("This is a gray scale image\n");
#endif
	return( ToPseudoColorImage() ) ;
    }
    else if ( myHeader.zsize >= 3 ) {
#ifdef TRACE
  if ( Verbose ) printf("This is a rgb image\n");
#endif
	return( ToColorImage() ) ;
    }
    else {
  	 Standard_TypeMismatch_Raise_if( Standard_True,
	 "Attempt to convert a SGIRGBAlienData to a unknown Image_Image type");

	return( NULL ) ;
    }
}


void AlienImage_SGIRGBAlienData::FromImage(const Handle(Image_Image)& anImage )

{ unsigned short x,y ;
  unsigned short *rbuf, *gbuf, *bbuf ;
  Standard_Real r,g,b ;
  Standard_Integer LowX = anImage->LowerX() ;
  Standard_Integer LowY = anImage->LowerY() ;

  myHeader.xsize = anImage->Width();
  myHeader.ysize = anImage->Height();
  myHeader.zsize = 3;

  /* allocate buffers for image data */
  if ( DataSize() ) {
	myRedData   = Standard::Allocate( DataSize() ) ;
	myGreenData = Standard::Allocate( DataSize() ) ;
	myBlueData  = Standard::Allocate( DataSize() ) ;
  }

  rbuf = ( unsigned short * ) myRedData ;
  gbuf = ( unsigned short * ) myGreenData ;
  bbuf = ( unsigned short * ) myBlueData ;

  for(y=0; y<myHeader.ysize; y++) {
	for(x=0; x<myHeader.xsize; x++, rbuf++, gbuf++, bbuf++) {
	  (anImage->PixelColor( LowX+x,LowY+y )).Values(r,g,b,Quantity_TOC_RGB);
	  *rbuf = (unsigned short) (r*255.+0.5) ;
	  *gbuf = (unsigned short) (g*255.+0.5) ;
	  *bbuf = (unsigned short) (b*255.+0.5) ;
	}
  }
}
 
