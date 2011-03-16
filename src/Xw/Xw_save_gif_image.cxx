/*
 * Created: Oleg Zelenkov (zov) : 1-Apr-1998
 * Portions Copyright (C) 1989, 1991 by Jef Poskanzer.  See copyright notice
 * below, at the beginning of the relevant code.
 *
 * Modified: Apr 20 1998 (zov)
 *


   XW_STATUS Xw_save_gif_image (awindow,aimage,filename):
   XW_EXT_WINDOW *awindow
   XW_EXT_IMAGEDATA   *aimage
   char *filename	GIF Image name

	Save an image to a GIF file.

	returns  SUCCESS if successfull
	returns  ERROR if something is wrong

*/

#define PRO16753       /* GG 261198
//              Don't free pname because this pointer is a
//              static address in Xw_get_filename() space.
*/

#include <Xw_Extension.h>
#include <X11/Xfuncs.h>

#define PRO5356 /* GG_130996: Compatibillite vis a vis de XReadDisplay (IRIX)
                   GG_240898  Compatibillite vis a vis de XReadScreen (SOLARIS)
*/

#ifdef TRACE
#define TRACE_SAVE_IMAGE
#endif

#define DECOSF1_COMPATIBILITY
#define CONV24_BEST   /* Defines conversion of color map to 256 colors */
                      /* Use CONV24_FAST to do it quickly;
                         otherwise 'BEST' method is used. */
#undef PARM
#if defined(__STDC__) || defined(HPUX) || defined(__hpux) || defined(__cplusplus)
#  define PARM(a) a
#else
#  define PARM(a) ()
#endif



typedef unsigned int   DWORD; /* 32-bit signed */
typedef int            LONG;  /* 32-bit unsigned */
typedef unsigned int   ULONG, UINT;  /* 32-bit signed */
typedef unsigned short WORD;  /* 16-bit unsigned */


#define LOWBIT(x) ((x) & (~(x) + 1))

/* RANGE forces a to be in the range b..c (inclusive) */
#define RANGE(a,b,c) { if (a < b) a = b;  if (a > c) a = c; }


/* NOTE, different platforms store WORDs in different ways */
/* You have to select from the given macros those that you need */
#if  defined(__osf__) || defined(DECOSF1)

#define SWAP_WORD(__w)  (__w)
#define SWAP_DWORD(__w)  (__w)

#else

#define SWAP_WORD(__w) ((((__w)&0xFF) << 8) | (((__w)&0xFF00) >> 8))
#define SWAP_DWORD(__w) ((((__w)&0x000000FF) << 24) | (((__w)&0x0000FF00) << 8) \
 | (((__w)&0xFF0000) >> 8) | (((__w)&0xFF000000) >> 24)  )

#endif


typedef struct _screen_descr {
  char gifID[ 6 ];
  WORD scrnWidth;
  WORD scrnHeight;
  BYTE scrnFlag;
} SCREEN_DESCR;

typedef struct _image_descr {
  WORD imgX;
  WORD imgY;
  WORD imgWidth;
  WORD imgHeight;
  BYTE imgFlag;
} IMAGE_DESCR;

typedef struct _lzw_dict {
  int code;
  int prnt;
  BYTE byte;
} LZWDict;

static LZWDict* dict;
static int startBits, codeBits, nextCode, bumpCode, rack, mask, putIdx,
           ClearCode, EOFCode, FreeCode;

#define NBITS     12
#define TBL_SIZE  5021
#define MAX_CODE  (  ( 1 << NBITS ) - 1  )
#define BUFF_SIZE 255
#define UNUSED -1
#define TRUE 1
#define FALSE 0

static int   _lzw_encode PARM((FILE*, BYTE*, int, int, int));
static void  _init_dict  PARM((void));
static int   _find_child PARM((int, int));
static BOOL  _put_bits   PARM((BYTE *, ULONG, UINT, FILE *));
static BOOL  _flush_bits PARM((BYTE *, FILE *));

static BOOL  _convert_image_to_256_RGB_indices PARM((XImage*, XColor*, BYTE*, BYTE*));
static int    quick_check PARM((BYTE*, int, int, BYTE*, BYTE*));
#if defined(CONV24_FAST)    /* Use quick color transformation */
static int    quick_quant PARM((BYTE*, int, int, BYTE*, BYTE*));
#endif
static int    ppm_quant   PARM((BYTE*, int, int, BYTE*, BYTE*));

static int _lzw_encode
#ifdef XW_PROTOTYPE
 (FILE* fimage, BYTE* pData, int width, int height, int inc)
#else
 (fimage, pData, width, height, inc)
  FILE* fimage;
  BYTE* pData;
  int width, height, inc;
#endif
{

  int   i, x, y;
  BYTE  byte, *pLine, OutBuff [BUFF_SIZE];
  int   strCode, chr;

  
  dict = (LZWDict *) Xw_malloc (sizeof (LZWDict)*TBL_SIZE);
  if (dict == NULL)
    goto _ExitError;


  x     = y = 0;
  pLine = pData;

  OutBuff[ 0 ] = 0;
  putIdx       = 1;
  mask         = 0x01;
  rack         = 0;
  startBits    = 8;
  ClearCode    = 1 << startBits;
  EOFCode      = ClearCode + 1;
  FreeCode     = EOFCode + 1;

  _init_dict ();

  byte = startBits;

  if ((int)fwrite (&byte, 1, 1, fimage) < 1)
    goto _ExitError;

  strCode = pLine[ x++ ];

  if (!_put_bits (OutBuff, (ULONG) ClearCode, codeBits, fimage))
    goto _ExitError;


  while (y < height) {
  
    chr = pLine[ x++ ];
    
    i = _find_child ( strCode, chr );

    
    if ( dict[ i ].code != UNUSED )
      strCode = dict[ i ].code;

    else {
      dict[ i ].code = nextCode++;
      dict[ i ].prnt = strCode;
      dict[ i ].byte = chr;

      if (!_put_bits (OutBuff, (ULONG) strCode, codeBits, fimage))
	goto _ExitError;

      strCode = chr;

      if (nextCode > MAX_CODE) {
	
	if (!_put_bits (OutBuff, (ULONG) ClearCode, codeBits, fimage))
	  goto _ExitError;

	_init_dict ();
	
      }
      else if (nextCode > bumpCode) {
	
	++codeBits;
	bumpCode <<= 1;
      }
    }

    if (x == width) {
   
      x = 0;
      ++y;
      pLine += inc;
    }
  }

  if (!_put_bits (OutBuff, (ULONG) strCode, codeBits, fimage)) goto _ExitError;
  if (!_put_bits (OutBuff, (ULONG) EOFCode, codeBits, fimage)) goto _ExitError;
  if (!_flush_bits (OutBuff, fimage)) goto _ExitError;


  if (dict)
    Xw_free (dict); 

  return XW_SUCCESS;


 
_ExitError:

  if (dict) 
    Xw_free (dict); 
 
  return XW_ERROR;
}
  /* end _lzw_encode */


static void _init_dict ( ) 
{
 memset (dict, UNUSED, sizeof (LZWDict)*TBL_SIZE);

 nextCode = FreeCode;
 codeBits = startBits + 1;
 bumpCode = 1 << codeBits;
}

static int _find_child
#ifdef XW_PROTOTYPE
 (int prntCode, int chr)
#else
 (prntCode, chr)
 int prntCode; int chr;
#endif
{
  int idx, offset;

  idx = (  chr << ( NBITS - 8 )  ) ^ prntCode;
  offset = idx ? TBL_SIZE - idx : 1;

  for (;;) {

    if (dict[ idx ].code == UNUSED ||
	dict[ idx ].prnt == prntCode && dict[ idx ].byte == ( BYTE )chr)
      return idx;

    idx = ( idx >= offset ) ? idx - offset : idx + TBL_SIZE - offset;
  }
}

static BOOL _put_bits
#ifdef XW_PROTOTYPE
 (BOOL* OutBuff, ULONG code, UINT nBits, FILE* fimage)
#else
 (OutBuff, code, nBits, fimage)
     BYTE *OutBuff; ULONG code; UINT nBits; FILE *fimage;
#endif
{
  BOOL  retVal = TRUE;
  ULONG msk;

  msk = 1;

  while (msk != (ULONG) (1 << nBits)) {
 
    if ( msk & code )
      rack |= mask;

    mask <<= 1;

    if ((mask & 0xFF) == 0) {
      OutBuff[ putIdx++ ] = rack;
      ++OutBuff[ 0 ];
      
      if (putIdx == BUFF_SIZE) {

	if ((int)fwrite (OutBuff, BUFF_SIZE, 1, fimage) < 1) {

	  retVal = FALSE;
	  break;
	}
	
	putIdx = 1;
	OutBuff[ 0 ] = 0;
      }
      
      rack = 0;
      mask = 0x01;
    }

    msk <<= 1;
  }
  
  return retVal;
}

static BOOL _flush_bits
#ifdef XW_PROTOTYPE
 (BYTE* OutBuff, FILE* fimage)
#else
 (OutBuff, fimage)
     BYTE *OutBuff;
     FILE *fimage;
#endif
{

  BOOL  retVal = TRUE;
  BYTE  byte;
  //  DWORD dwBytesWritten;
  
  if ( mask != 0x01 ) {
    
    OutBuff[ putIdx++ ] = rack;
    ++OutBuff[ 0 ];
  }
    
  if (putIdx != 1) {
 
    if ((int)fwrite (OutBuff, putIdx, 1, fimage) < 1)
      retVal = FALSE;
  }

  if (retVal) {
 
    byte = 0;

    if ((int)fwrite (&byte, 1, 1, fimage) < 1)
      retVal = FALSE;
  } 

  return retVal;
}

static BOOL _convert_image_to_256_RGB_indices
#ifdef XW_PROTOTYPE
  (XImage* pximage, XColor* pcolors, BYTE* colors256, BYTE* pIndices)
#else
  (pximage, pcolors, colors256, pIndices)
     XImage *pximage;
     XColor *pcolors;
     BYTE* colors256;
     BYTE* pIndices;
#endif
{
  int   mult_r, mult_g, mult_b, blue_mask, red_mask, green_mask;
  int   i, x, y;
//  BYTE  rmap[256],gmap[256],bmap[256];
//  BYTE  imap[256], nc;
  BYTE *pic24 = (BYTE*) malloc (pximage->width*pximage->height*3);
  BYTE *pic8 = pIndices, *pbCell;



  if (pic24 == NULL)
    return FALSE;


  if (pcolors == NULL) {

    blue_mask = pximage->blue_mask;
    green_mask = pximage->green_mask;
    red_mask = pximage->red_mask;

    if (!red_mask || !green_mask || !blue_mask)
      return FALSE;


    mult_r = mult_g = mult_b = 0;
      
    for (i=red_mask; (i&1)^1; i >>= 1)
      mult_r++;
    for (i=green_mask; (i&1)^1; i >>= 1)
      mult_g++;
    for (i=blue_mask; (i&1)^1; i >>= 1)
      mult_b++;
      

    /* Prepare 24-bit picture format (in order R-byte, G-byte, B-byte). */
    pbCell = pic24;
    for (y=0; y<pximage->height; y++)
      for (x=0; x<pximage->width; x++) {
	
	unsigned long pix = XGetPixel (pximage, x, y);
	
	*pbCell++ = (pix & red_mask) >> mult_r;
	*pbCell++ = (pix & green_mask) >> mult_g;
	*pbCell++ = (pix & blue_mask) >> mult_b;
      }
  }
  else { /* using PseudoColors indices from pcolors to convert to 24-bit format */
    
    /* Prepare 24-bit picture format using given colors */
    pbCell = pic24;
    for (y=0; y<pximage->height; y++)
      for (x=0; x<pximage->width; x++) {

	unsigned long pix = XGetPixel (pximage, x, y);

	/* Assume pix is the index to colormap */
	*pbCell++ = pcolors[pix].flags & DoRed? pcolors[pix].red>>8: 0;
	*pbCell++ = pcolors[pix].flags & DoGreen? pcolors[pix].green>>8: 0;
	*pbCell++ = pcolors[pix].flags & DoBlue? pcolors[pix].blue>>8: 0;
      }
  }


  /* How much colors do we have used in the image ? */
  if (quick_check(pic24, pximage->width, pximage->height, pic8, colors256)) {

    free (pic24);
    return TRUE; /* less than 256 colors found. No conversion is necessary. */
  }


  /* Over 256 colors used! We need to convert them. */

#if defined(CONV24_FAST)    /* Use quick color transformation */

#ifdef DEBUG
  fprintf(stderr,"Xw_save_gif_image: Doing 'quick' 24-bit to 8-bit conversion.\r\n");
#endif

  i = quick_quant(pic24, pximage->width, pximage->height, pic8, colors256);
  
#else /* assume defined(CONV24_BEST) */   /* Do it slowly */

#ifdef DEBUG
  fprintf(stderr,"Xw_save_gif_image: Doing 'best' 24-bit to 8-bit conversion.\r\n");
#endif

  i = ppm_quant(pic24, pximage->width, pximage->height, pic8, colors256);
#endif /* CONV24_FAST */

  free (pic24);

  return (!i);
}
#ifdef XW_PROTOTYPE
XW_STATUS Xw_save_gif_image_adv (Display *aDisplay,Window aWindow,XWindowAttributes aWinAttr,XImage *aPximage,Colormap aColormap,int aNcolors,char *filename)
#else
XW_STATUS Xw_save_gif_image_adv (aDisplay,aWindow,aWinAttr,aPximage,aColormap,aNcolors,filename)
Display *aDisplay;
Window aWindow;
XWindowAttributes aWinAttr;
XImage *aPximage;
Colormap aColormap;
int ncolors;
char *filename;
#endif /*XW_PROTOTYPE*/
{
  BYTE colors256[256][3];
  BYTE *pBits = NULL;
  char *wname = NULL,*pname = NULL ;
  int i,isize,lname,iclass,ncolors, x,y;
  Visual *pvisual = aWinAttr.visual;
  XColor *pcolors = NULL;
  XWDColor *qcolors = NULL;
  FILE *fimage = NULL ;
  XW_STATUS status = XW_SUCCESS;
  BYTE image_sep = 0x2C; /* gif colormap delimiter */
  WORD wZero = 0x00;
#if defined(LIN) || defined(linux)
  SCREEN_DESCR* sd;
  IMAGE_DESCR*  id;
#else
  SCREEN_DESCR sd;
  IMAGE_DESCR id;
#endif
  BYTE *pbCell;

  /* Open GIF File */
  pname = Xw_get_filename (filename, "gif");
  if (pname)
    fimage = fopen(pname,"w") ;
  
  if( !fimage ) {	/*ERROR*Bad Filename*/
    Xw_set_error(55,"Xw_save_gif_image",filename) ;
    return (XW_ERROR) ;
  }

  /* Get Window name */	
  XFetchName(aDisplay,aWindow,&wname);
  
  if (!wname || !strlen (wname))
    wname = (char*) strdup(pname);
  lname = strlen (wname) + 1;
  
  iclass = pvisual->c_class;
#ifdef PRO5356
  if( (iclass == PseudoColor) && (aPximage->red_mask > 0) ) iclass = TrueColor;
/*    iclass = (aPximage->depth > 8) ? TrueColor : pvisual->c_class;  GG_240898 */
#endif
  
#ifdef DEBUG
  fprintf (stderr, "Xw_save_gif_image: max colors available = %d\r\n",
	   aColormap? aNcolors: 0);
#endif

  /* Get color table */ 
  switch (iclass) {
    
  case PseudoColor :
    ncolors = aNcolors;
    if ((pcolors = (XColor*) Xw_calloc(ncolors,sizeof(XColor)))) {
      for (i=0 ; i<ncolors ; i++) {
	pcolors[i].pixel = i;
	pcolors[i].pad = 0;
      }
      XQueryColors(aDisplay,aColormap,pcolors,ncolors);
    }
    else {
      /*ERROR*Bad Image allocation*/
      Xw_set_error(60,"Xw_save_gif_image",0) ;
      Xw_free(wname) ;
#ifndef PRO16753       
      Xw_free(pname) ;
#endif
      fclose(fimage) ;
      return (XW_ERROR) ;
    } 
    break ;

    
  case DirectColor :
    ncolors = aNcolors;

    if (ncolors > 256) {
      ncolors = 0;
      break;
    }

    if(( pcolors = (XColor*) Xw_calloc(ncolors,sizeof(XColor)) )) {
      unsigned long red = 0,lred = LOWBIT(pvisual->red_mask);
      unsigned long green = 0,lgreen = LOWBIT(pvisual->green_mask);
      unsigned long blue = 0,lblue = LOWBIT(pvisual->blue_mask);
      for( i=0 ; i<ncolors ; i++) {
	pcolors[i].pixel = red|green|blue;
	pcolors[i].pad = 0;
	red += lred;
	if( red > pvisual->red_mask ) red = 0;
	green += lgreen;
	if( green > pvisual->green_mask ) green = 0;
	blue += lblue;
	if( blue > pvisual->blue_mask ) blue = 0;
      }
      XQueryColors(aDisplay,aColormap,pcolors,ncolors);
    }
    else {  /*ERROR*Bad Image allocation*/
      Xw_set_error(60,"Xw_save_gif_image",0) ;
      Xw_free(wname) ;
#ifndef PRO16753       
      Xw_free(pname) ;
#endif
      fclose(fimage) ;
      return (XW_ERROR) ;
    } 
    break ;


  case TrueColor :
    ncolors = 0;
    break;

    
  default :  /*ERROR*Unimplemented Image Visual class*/
    Xw_set_error(59,"Xw_save_bmp_image",&iclass);
#ifndef PRO16753       
    Xw_free(pname);
#endif
    Xw_free(wname);
    fclose(fimage);
    return (XW_ERROR);
  }

  isize = aPximage->width * aPximage->height; /* size of array of pixel color indices */
  pBits = (BYTE *) Xw_malloc (isize);

  if (pBits == NULL) {  /*ERROR*Bad Image allocation*/
    Xw_set_error(60,"Xw_save_gif_image",0) ;
    Xw_free(wname);
#ifndef PRO16753       
    Xw_free(pname);
#endif
    fclose(fimage);
    return (XW_ERROR) ;
  } 
  
  /* Build file header */
#if defined(LIN) || defined(linux)
  sd = (SCREEN_DESCR *) Xw_malloc(11);
  memcpy (sd->gifID, "GIF87a", 6);
  sd->scrnWidth  = DisplayWidth(aDisplay, DefaultScreen(aDisplay));
  sd->scrnHeight = DisplayHeight(aDisplay, DefaultScreen(aDisplay));
  sd->scrnFlag   = 0x80 | (  ( 7/*[=depth-1]*/ << 4 ) & 0x70  ) | 0x07;

  id = (IMAGE_DESCR*) Xw_malloc(9);
  id->imgX      = 0;
  id->imgY      = 0;
  id->imgWidth  = (WORD) aPximage->width;
  id->imgHeight = (WORD) aPximage->height;
#else
  memcpy (sd.gifID, "GIF87a", 6);
  sd.scrnWidth  = SWAP_WORD (DisplayWidth(aDisplay, DefaultScreen(aDisplay)));
  sd.scrnHeight	= SWAP_WORD (DisplayHeight(aDisplay, DefaultScreen(aDisplay)));
  sd.scrnFlag   = 0x80 | (  ( 7/*[=depth-1]*/ << 4 ) & 0x70  ) | 0x07;

  id.imgX      = 0;
  id.imgY      = 0;
  id.imgWidth  = SWAP_WORD ((WORD) aPximage->width);
  id.imgHeight = SWAP_WORD ((WORD) aPximage->height);
#endif
  
/*
  	  imgFlag
        +-+-+-+-+-+-----+       M=0 - Use global color map, ignore 'pixel'
        |M|I|0|0|0|pixel| 10    M=1 - Local color map follows, use 'pixel'
        +-+-+-+-+-+-----+       I=0 - Image formatted in Sequential order
                                I=1 - Image formatted in Interlaced order
                                pixel+1 - # bits per pixel for this image
*/
#if defined(LIN) || defined(linux)
  id->imgFlag   = 0x07; /* Global color map, Sequential order, 8 bits per pixel */
#else
  id.imgFlag = 0x07;
#endif
  
  /* Prepare color table for saving (256 RGB tripplets) and array of pixel
     that refers to that table.
     Note, that the table is limited up to 256 entries.
     So, we'll probably have to ignore some rairly used colors.
     In the case we have less than 256 colors in the image,
     we'll expand them up to 256 using 'black' RGB (i.e. 000).
  */
  memset (colors256, 0, sizeof (colors256)); /* black if unused */

  if (ncolors > 0 && ncolors <= 256)
  {
    /* Use just all colors that have been specified in pximage */

    for (i=0; i<ncolors; i++) {
      colors256[i] [0/*R*/] = pcolors[i].flags & DoRed ? pcolors[i].red>>8: 0;
      colors256[i] [1/*G*/] = pcolors[i].flags & DoGreen ? pcolors[i].green>>8: 0;
      colors256[i] [2/*B*/] = pcolors[i].flags & DoBlue ? pcolors[i].blue>>8: 0;
    }

    pbCell = pBits;
    for (y=0; y<aPximage->height; y++)
      for (x=0; x<aPximage->width; x++) {
	unsigned long pixel = XGetPixel (aPximage, x, y);
	*pbCell++ = pixel;
      }
  }
  else {

    /* Too big color map */
    /* Substitute rairly used colors in order to have exactly 256 colors */

    if (iclass == PseudoColor && pcolors == NULL) {
      fprintf (stderr, "%s: Impossible situation at line %d! Abort.\n",
	       __FILE__, __LINE__);
      exit (1); /* Impossible situation! */
    }
    if (iclass != PseudoColor && pcolors != NULL) {
      fprintf (stderr, "%s: Impossible situation at line %d! Abort.\n",
	       __FILE__, __LINE__);
      exit (1); /* Impossible situation! */
    }

    /* Search for the top 256 colors and convert pixels to indices */
    if (! _convert_image_to_256_RGB_indices (aPximage,
					     pcolors,
					     (BYTE*)colors256, pBits)) {

      /*ERROR*Bad Image allocation*/
      Xw_set_error(60,"Xw_save_gif_image",0) ;
      Xw_free(wname);
#ifndef PRO16753       
      Xw_free(pname);
#endif
      Xw_free(pBits);
      fclose(fimage);
      return (XW_ERROR);
    }
  }

  /* write off the buffers */
#if defined(LIN) || defined(linux)
  if (status && (int)fwrite(sd, 11, 1, fimage) < 1)
    status = XW_ERROR;
  Xw_free(sd);
#else
  if (status && (int)fwrite(&sd, 11, 1, fimage) < 1)
    status = XW_ERROR;
#endif
  
  if (status && (int)fwrite(&wZero, 2, 1, fimage) < 1) /*screen bacground byte folowed by zero*/
    status = XW_ERROR;
 
  if (status && (int)fwrite(&colors256, 256*3, 1, fimage) < 1)
    status = XW_ERROR;
 

/* Write comment blocks (the window name) 

  if (lname > 1) {
    char *sp;
    int   i, blen;

    fputc(0x21, fimage);
    fputc(0xFE, fimage);

    sp = wname;
    while ( (blen=strlen(sp)) > 0) {
      if (blen>255) blen = 255;
      fputc(blen, fimage);
      for (i=0; i<blen; i++, sp++) fputc(*sp, fimage);
    }
    fputc(0, fimage);
  }
*/

  if (status && (int)fwrite(&image_sep, 1, 1, fimage) < 1)
    status = XW_ERROR;
#if defined(LIN) || defined(linux) 
  if (status && (int)fwrite(id, 9, 1, fimage) < 1)
    status = XW_ERROR;
  Xw_free(id);
#else
  if (status && (int)fwrite(&id, 9, 1, fimage) < 1)
    status = XW_ERROR;
#endif
//  status && (status
//    = _lzw_encode (fimage, pBits, aPximage->width, aPximage->height, aPximage->width));
  _lzw_encode (fimage, pBits, aPximage->width, aPximage->height, aPximage->width);
  
  fclose(fimage);
  
#ifndef PRO16753       
  Xw_free(pname);
#endif
  Xw_free(wname);
  if (pcolors)
    Xw_free (pcolors);
  if (pBits)
    Xw_free (pBits);

  return status;
}

/*********************************************************************/
/* Xw_save_gif_image                                                 */
/*                                                                   */
/*********************************************************************/
XW_STATUS Xw_save_gif_image
#ifdef XW_PROTOTYPE
 (void *awindow,void *aimage,char *filename)
#else
 (awindow,aimage,filename)
void *awindow;
void *aimage;
char *filename ;
#endif /*XW_PROTOTYPE*/
{
  XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
  XW_EXT_IMAGEDATA *pimage = (XW_EXT_IMAGEDATA*) aimage;
  XImage *pximage = NULL;  
  XW_STATUS status = XW_SUCCESS;

  if (!Xw_isdefine_window(pwindow)) { /*ERROR*Bad EXT_WINDOW Address*/
    Xw_set_error(24,"Xw_save_gif_image",pwindow) ;
    return (XW_ERROR) ;
  }

  if( !Xw_isdefine_image(pimage) ) { /*ERROR*Bad EXT_IMAGEDATA Address*/
    Xw_set_error(25,"Xw_save_gif_image",pimage) ;
    return (XW_ERROR) ;
  }
  
  pximage = (_ZIMAGE) ? _ZIMAGE : _IIMAGE;

  status = Xw_save_gif_image_adv(_DISPLAY,_WINDOW,_ATTRIBUTES,pximage,_COLORMAP->info.colormap,_COLORMAP->maxhcolor,filename);
    
#ifdef  TRACE_SAVE_IMAGE
  if (Xw_get_trace ())
    printf (" %d = Xw_save_gif_image(%lx,%lx,'%s')\n",
	    status, (long ) pwindow, (long ) pimage, filename);
#endif  
  
  return (XW_STATUS) status;
}



/*
   colormap conversion routines

 */

static int quick_check
#ifdef XW_PROTOTYPE
 (BYTE* pic24, int w, int h, BYTE* pic8, BYTE* rgbmap)
#else
 (pic24, w,h, pic8, rgbmap)
     BYTE *pic24, *pic8, *rgbmap;
     int   w,h;
#endif
{
  /* scans picture until it finds more than 256 different colors.  If it
     finds more than 256 colors, it returns '0'.  If it DOESN'T, it does
     the 24-to-8 conversion by simply sticking the colors it found into
     a colormap, and changing instances of a color in pic24 into colormap
     indicies (in pic8) */

  unsigned long colors[256],col;
  int           i, nc, low, high, mid;
  BYTE         *p, *pix;


  /* put the first color in the table by hand */
  nc = 0;  mid = 0;  

  for (i=w*h,p=pic24; i; i--) {
    col  = (((unsigned long) *p++) << 16);  
    col += (((unsigned long) *p++) << 8);
    col +=  *p++;

    /* binary search the 'colors' array to see if it's in there */
    low = 0;  high = nc-1;
    while (low <= high) {
      mid = (low+high)/2;
      if      (col < colors[mid]) high = mid - 1;
      else if (col > colors[mid]) low  = mid + 1;
      else break;
    }

    if (high < low) { /* didn't find color in list, add it. */

      if (nc>=256)
	return FALSE; /* over 256 colors used! */

#if defined (__hpux) || defined(HPUX)
//JR/Hp : bcopy
      memmove((char *) &colors[low+1], (char *) &colors[low],
	    (nc - low) * sizeof(unsigned long));
#else
      bcopy((char *) &colors[low], (char *) &colors[low+1],
	    (nc - low) * sizeof(unsigned long));
#endif

      colors[low] = col;
      nc++;
    }
  }
#ifdef DEBUG
  fprintf(stderr,"quick_check:  there're %d colors used in the image.\r\n", nc);
#endif

  /* run through the data a second time, this time mapping pixel values in
     pic24 into colormap offsets into 'colors' */

  for (i=w*h,p=pic24, pix=pic8; i; i--,pix++) {
    col  = (((unsigned long) *p++) << 16);  
    col += (((unsigned long) *p++) << 8);
    col +=  *p++;

    /* binary search the 'colors' array.  It *IS* in there */
    low = 0;  high = nc-1;
    while (low <= high) {
      mid = (low+high)/2;
      if      (col < colors[mid]) high = mid - 1;
      else if (col > colors[mid]) low  = mid + 1;
      else break;
    }

    if (high < low) { /* no RGB ? */
      fprintf(stderr,"quick_check:  impossible situation!\r\n");
      exit(1);
    }
    *pix = mid;
  }

  /* and load up the 'desired colormap' */
  for (i=0, p=rgbmap; i<nc; i++) {
    *p++ /*i-th R*/ =  colors[i]>>16;  
    *p++ /*i-th G*/ = (colors[i]>>8) & 0xff;
    *p++ /*i-th B*/ =  colors[i]     & 0xff;
  }

  return nc; /* # colors udsed */
}




/************************************/
#if defined(CONV24_FAST)    /* Use quick color transformation */
static int quick_quant
#ifdef XW_PROTOTYPE
 (BYTE* p24, int w, int h, BYTE* p8, BYTE* rgbmap)
#else
 (p24,w,h, p8, rgbmap)
     BYTE *p24, *p8, *rgbmap;
     int   w,h;
#endif
{
  /* called after 'pic8' has been alloced */
  
/* up to 256 colors:     3 bits R, 3 bits G, 2 bits B  (RRRGGGBB) */
#define RMASK      0xe0
#define RSHIFT        0
#define GMASK      0xe0
#define GSHIFT        3
#define BMASK      0xc0
#define BSHIFT        6

  BYTE *pp;
  int  r1, g1, b1;
  int  *thisline, *nextline, *thisptr, *nextptr, *tmpptr;
  int  i, j, val, pwide3;
  int  imax, jmax;

  pp = p8;  pwide3 = w * 3;  imax = h-1;  jmax = w-1;


  /* load up colormap:
   *   note that 0 and 255 of each color are always in the map;
   *   intermediate values are evenly spaced.
   */

  for (i=0; i<256; i++) {
    rgbmap[i*3+0] = (((i<<RSHIFT) & RMASK) * 255 + RMASK/2) / RMASK;
    rgbmap[i*3+1] = (((i<<GSHIFT) & GMASK) * 255 + GMASK/2) / GMASK;
    rgbmap[i*3+2] = (((i<<BSHIFT) & BMASK) * 255 + BMASK/2) / BMASK;
  }
  

  thisline = (int *) malloc(pwide3 * sizeof(int));
  nextline = (int *) malloc(pwide3 * sizeof(int));
  if (!thisline || !nextline) {
    if (thisline) free(thisline);
    if (nextline) free(nextline);
    fprintf(stderr,"quick_quant: unable to allocate memory in quick_quant()\r\n");
    return(1);
  }
  
  /* get first line of picture */
  for (j=pwide3, tmpptr=nextline; j; j--) *tmpptr++ = (int) *p24++;
  
  for (i=0; i<h; i++) {
    tmpptr = thisline;  thisline = nextline;  nextline = tmpptr;   /* swap */
    
    if (i!=imax)   /* get next line */
      for (j=pwide3, tmpptr=nextline; j; j--)
	*tmpptr++ = (int) *p24++;
    
    for (j=0, thisptr=thisline, nextptr=nextline; j<w; j++,pp++) {
      r1 = *thisptr++;  g1 = *thisptr++;  b1 = *thisptr++;
      RANGE(r1,0,255);  RANGE(g1,0,255);  RANGE(b1,0,255);  
      
      /* choose actual pixel value */
      val = (((r1&RMASK)>>RSHIFT) | ((g1&GMASK)>>GSHIFT) | 
	     ((b1&BMASK)>>BSHIFT));
      *pp = val;
      
      /* compute color errors */
      r1 -= rgbmap[val*3+0];
      g1 -= rgbmap[val*3+1];
      b1 -= rgbmap[val*3+2];
      
      /* Add fractions of errors to adjacent pixels */
      if (j!=jmax) {  /* adjust RIGHT pixel */
	thisptr[0] += (r1*7) / 16;
	thisptr[1] += (g1*7) / 16;
	thisptr[2] += (b1*7) / 16;
      }
      
      if (i!=imax) {	/* do BOTTOM pixel */
	nextptr[0] += (r1*5) / 16;
	nextptr[1] += (g1*5) / 16;
	nextptr[2] += (b1*5) / 16;

	if (j>0) {  /* do BOTTOM LEFT pixel */
	  nextptr[-3] += (r1*3) / 16;
	  nextptr[-2] += (g1*3) / 16;
	  nextptr[-1] += (b1*3) / 16;
	}

	if (j!=jmax) {  /* do BOTTOM RIGHT pixel */
	  nextptr[3] += (r1)/16;
	  nextptr[4] += (g1)/16;
	  nextptr[5] += (b1)/16;
	}
	nextptr += 3;
      }
    }
  }
  
  free(thisline);
  free(nextline);
  return 0;


#undef RMASK
#undef RSHIFT
#undef GMASK
#undef GSHIFT
#undef BMASK
#undef BSHIFT
}
#endif      






/***************************************************************/
/* The following code based on code from the 'pbmplus' package */
/* written by Jef Poskanzer                                    */
/***************************************************************/


/* ppmquant.c - quantize the colors in a pixmap down to a specified number
**
** Copyright (C) 1989, 1991 by Jef Poskanzer.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
*/


typedef unsigned char pixval;

#define PPM_MAXMAXVAL 255
typedef struct { pixval r, g, b; } pixel;

#define PPM_GETR(p) ((p).r)
#define PPM_GETG(p) ((p).g)
#define PPM_GETB(p) ((p).b)

#define PPM_ASSIGN(p,red,grn,blu) \
  { (p).r = (red); (p).g = (grn); (p).b = (blu); }

#define PPM_EQUAL(p,q) ( (p).r == (q).r && (p).g == (q).g && (p).b == (q).b )


/* Color scaling macro -- to make writing ppmtowhatever easier. */

#define PPM_DEPTH(newp,p,oldmaxval,newmaxval) \
    PPM_ASSIGN( (newp), \
              ((int) PPM_GETR(p)) * ((int)newmaxval) / ((int)oldmaxval), \
              ((int) PPM_GETG(p)) * ((int)newmaxval) / ((int)oldmaxval), \
              ((int) PPM_GETB(p)) * ((int)newmaxval) / ((int)oldmaxval) )


/* Luminance macro. */

/* 
 * #define PPM_LUMIN(p) \
 *   ( 0.299 * PPM_GETR(p) + 0.587 * PPM_GETG(p) + 0.114 * PPM_GETB(p) )
 */

/* Luminance macro, using only integer ops.  Returns an int (*256)  JHB */
#define PPM_LUMIN(p) \
  ( 77 * PPM_GETR(p) + 150 * PPM_GETG(p) + 29 * PPM_GETB(p) )

/* Color histogram stuff. */

typedef struct chist_item* chist_vec;
struct chist_item { pixel color;
			int value;
		      };

typedef struct chist_list_item* chist_list;
struct chist_list_item { struct chist_item ch;
			     chist_list next;
			   };

typedef chist_list* chash_table;

typedef struct box* box_vector;
struct box {
  int index;
  int colors;
  int sum;
};


#define MAXCOLORS 32767
#define CLUSTER_MAXVAL 63

#define LARGE_LUM
#define REP_AVERAGE_PIXELS

#define FS_SCALE 1024

#define HASH_SIZE 6553

#define ppm_hashpixel(p) ((((int) PPM_GETR(p) * 33023 + \
                          (int) PPM_GETG(p) * 30013 + \
                          (int) PPM_GETB(p) * 27011) & 0x7fffffff) \
                        % HASH_SIZE)



/*** function defs ***/

static chist_vec   mediancut        PARM((chist_vec, int, int, int, int));
static int         redcompare       PARM((const void *, const void *));
static int         greencompare     PARM((const void *, const void *));
static int         bluecompare      PARM((const void *, const void *));
static int         sumcompare       PARM((const void *, const void *));
static chist_vec   ppm_computechist PARM((pixel **, int,int,int,int *));
static chash_table ppm_computechash PARM((pixel **, int,int,int,int *));
static chist_vec   ppm_chashtochist PARM((chash_table, int));
static chash_table ppm_allocchash   PARM((void));
static void        ppm_freechist    PARM((chist_vec));
static void        ppm_freechash    PARM((chash_table));


/****************************************************************************/
static int ppm_quant
#ifdef XW_PROTOTYPE
 (BYTE* pic24, int cols, int rows, BYTE* pic8, BYTE* rgbmap)
#else
 (pic24, cols, rows, pic8, rgbmap)
     BYTE *pic24, *pic8, *rgbmap;
     int  cols, rows;
#endif
{
  pixel**          pixels;
  register pixel*  pP;
  int              row;
  register int     col, limitcol;
  pixval           maxval, newmaxval;
  int              colors;
  register int     index;
  chist_vec chv, colormap;
  chash_table  cht;
  int               i;
  unsigned char     *picptr;
  static const char *fn = "ppmquant()";
  BYTE*             pByte;

  index = 0;
  maxval = 255;

  /*
   *  reformat 24-bit pic24 image (3 bytes per pixel) into 2-dimensional
   *  array of pixel structures
   */

#ifdef DEBUG
  fprintf(stderr,"%s: remapping to ppm-style internal fmt\r\n", fn);
#endif
  
  pixels = (pixel **) malloc(rows * sizeof(pixel *));
  if (!pixels) {
    fprintf (stderr, "%s: couldn't allocate 'pixels' array\r\n", fn);
    return -1;
  }

  for (row=0; row<rows; row++) {
    pixels[row] = (pixel *) malloc(cols * sizeof(pixel));

    if (!pixels[row]) {
      fprintf (stderr, "%s: couldn't allocate a row of pixels array\r\n", fn);
      while (--row >=0 )
	free (pixels[row]);
      free (pixels);
      return -1;
    }

    for (col=0, pP=pixels[row]; col<cols; col++, pP++) {
      pP->r = *pic24++;
      pP->g = *pic24++;
      pP->b = *pic24++;
    }
  }
#ifdef DEBUG
  fprintf(stderr,"%s: done format remapping\r\n", fn);
#endif

    

  /*
   *  attempt to make a histogram of the colors, unclustered.
   *  If at first we don't succeed, lower maxval to increase color
   *  coherence and try again.  This will eventually terminate, with
   *  maxval at worst 15, since 32^3 is approximately MAXCOLORS.
   */

  for ( ; ; ) {
#ifdef DEBUG
    fprintf(stderr, "%s:  making histogram\r\n", fn);
#endif
    chv = ppm_computechist(pixels, cols, rows, MAXCOLORS, &colors);
    if (chv != (chist_vec) 0) break;
    
#ifdef DEB
    fprintf(stderr, "%s: too many colors!\r\n", fn);
#endif
    newmaxval = maxval / 2;
#ifdef DEB
    fprintf(stderr, "%s: rescaling colors (maxval=%d) %s\r\n",
		       fn, newmaxval, "to improve clustering");
#endif
    for (row=0; row<rows; ++row)
      for (col=0, pP=pixels[row]; col<cols; ++col, ++pP)
	PPM_DEPTH( *pP, *pP, maxval, newmaxval );
    maxval = newmaxval;
  }

#ifdef DEBUG
  fprintf(stderr,"%s: %d colors found\r\n", fn, colors);
#endif


  /*
   * Step 3: apply median-cut to histogram, making the new colormap.
   */

#ifdef DEBUG
  fprintf(stderr, "%s: choosing %d colors\r\n", fn, 256);
#endif
  colormap = mediancut(chv, colors, rows * cols, maxval, 256);
  if (colormap == (chist_vec) NULL)
    return -1;
  ppm_freechist(chv);



  /*
   *  Step 4: map the colors in the image to their closest match in the
   *  new colormap, and write 'em out.
   */

#ifdef DEBUG
  fprintf(stderr,"%s: mapping image to new colors\r\n", fn);
#endif
  cht = ppm_allocchash();

  picptr = pic8;
  for (row = 0;  row < rows;  ++row) {
    col = 0;  limitcol = cols;  pP = pixels[row];

    do {
      int hash;
      chist_list chl;

      /* Check hash table to see if we have already matched this color. */

      hash = ppm_hashpixel(*pP);
      for (chl = cht[hash];  chl;  chl = chl->next)
	if (PPM_EQUAL(chl->ch.color, *pP)) {index = chl->ch.value; break;}

      if (!chl /*index = -1*/) {/* No; search colormap for closest match. */
	register int i, r1, g1, b1, r2, g2, b2;
	register long dist, newdist;

	r1 = PPM_GETR( *pP );
	g1 = PPM_GETG( *pP );
	b1 = PPM_GETB( *pP );
	dist = 2000000000;

	for (i=0; i<256; i++) {
	  r2 = PPM_GETR( colormap[i].color );
	  g2 = PPM_GETG( colormap[i].color );
	  b2 = PPM_GETB( colormap[i].color );

	  newdist = ( r1 - r2 ) * ( r1 - r2 ) +
	            ( g1 - g2 ) * ( g1 - g2 ) +
	            ( b1 - b2 ) * ( b1 - b2 );

	  if (newdist<dist) { index = i;  dist = newdist; }
	}

	hash = ppm_hashpixel(*pP);
	chl = (chist_list) malloc(sizeof(struct chist_list_item));
	if (!chl) {
	  fprintf (stderr, "%s: ran out of memory adding to hash table!\r\n", fn);
	  return -1;
	}

	chl->ch.color = *pP;
	chl->ch.value = index;
	chl->next = cht[hash];
	cht[hash] = chl;
      }

      *picptr++ = index;

      ++col;
      ++pP;
    }
    while (col != limitcol);
  }

  /* rescale the colormap and load the XV colormap */
  for (i=0, pByte=rgbmap; i<256; i++) {
    PPM_DEPTH(colormap[i].color, colormap[i].color, maxval, 255);
    *pByte++ /*i-th R*/ = PPM_GETR( colormap[i].color );
    *pByte++ /*i-th G*/ = PPM_GETG( colormap[i].color );
    *pByte++ /*i-th B*/ = PPM_GETB( colormap[i].color );
  }

  /* free the pixels array */
  for (i=0; i<rows; i++) free(pixels[i]);
  free(pixels);

  /* free cht and colormap */
  ppm_freechist(colormap);
  ppm_freechash(cht);

  return 0;
}



/*
** Here is the fun part, the median-cut colormap generator.  This is based
** on Paul Heckbert's paper "Color Image Quantization for Frame Buffer
** Display", SIGGRAPH '82 Proceedings, page 297.
*/



/****************************************************************************/
static chist_vec mediancut
#ifdef XW_PROTOTYPE
 ( chist_vec chv, int colors, int sum, int maxval, int newcolors )
#else
 ( chv, colors, sum, maxval, newcolors )
     chist_vec chv;
     int colors, sum, newcolors;
     int maxval;
#endif
{
  chist_vec colormap;
  box_vector bv;
  register int bi, i;
  int boxes;

  bv = (box_vector) malloc(sizeof(struct box) * newcolors);
  colormap = (chist_vec) 
             malloc(sizeof(struct chist_item) * newcolors );

  if (!bv || !colormap) {
    fprintf (stderr, "mediancut(): unable to malloc!\r\n");
    free (bv);
    free (colormap);
    return (chist_vec)NULL;
  }

  for (i=0; i<newcolors; i++)
    PPM_ASSIGN(colormap[i].color, 0, 0, 0);

  /*
   *  Set up the initial box.
   */
  bv[0].index = 0;
  bv[0].colors = colors;
  bv[0].sum = sum;
  boxes = 1;


  /*
   ** Main loop: split boxes until we have enough.
   */

  while ( boxes < newcolors ) {
    register int indx, clrs;
    int sm;
    register int minr, maxr, ming, maxg, minb, maxb, v;
    int halfsum, lowersum;

    /*
     ** Find the first splittable box.
     */
    for (bi=0; bv[bi].colors<2 && bi<boxes; bi++) ;
    if (bi == boxes) break;	/* ran out of colors! */

    indx = bv[bi].index;
    clrs = bv[bi].colors;
    sm = bv[bi].sum;

    /*
     ** Go through the box finding the minimum and maximum of each
     ** component - the boundaries of the box.
     */
    minr = maxr = PPM_GETR( chv[indx].color );
    ming = maxg = PPM_GETG( chv[indx].color );
    minb = maxb = PPM_GETB( chv[indx].color );

    for (i=1; i<clrs; i++) {
      v = PPM_GETR( chv[indx + i].color );
      if (v < minr) minr = v;
      if (v > maxr) maxr = v;

      v = PPM_GETG( chv[indx + i].color );
      if (v < ming) ming = v;
      if (v > maxg) maxg = v;

      v = PPM_GETB( chv[indx + i].color );
      if (v < minb) minb = v;
      if (v > maxb) maxb = v;
    }

    /*
     ** Find the largest dimension, and sort by that component.  I have
     ** included two methods for determining the "largest" dimension;
     ** first by simply comparing the range in RGB space, and second
     ** by transforming into luminosities before the comparison.  You
     ** can switch which method is used by switching the commenting on
     ** the LARGE_ defines at the beginning of this source file.
     */
    {
      /* LARGE_LUM version */

      pixel p;
      int rl, gl, bl;

      PPM_ASSIGN(p, maxr - minr, 0, 0);
      rl = PPM_LUMIN(p);

      PPM_ASSIGN(p, 0, maxg - ming, 0);
      gl = PPM_LUMIN(p);

      PPM_ASSIGN(p, 0, 0, maxb - minb);
      bl = PPM_LUMIN(p);

      if (rl >= gl && rl >= bl)
	qsort((char*) &(chv[indx]), (size_t) clrs, sizeof(struct chist_item),
	      redcompare );
      else if (gl >= bl)
	qsort((char*) &(chv[indx]), (size_t) clrs, sizeof(struct chist_item),
	      greencompare );
      else 
	qsort((char*) &(chv[indx]), (size_t) clrs, sizeof(struct chist_item),
	      bluecompare );
    }

    /*
     ** Now find the median based on the counts, so that about half the
     ** pixels (not colors, pixels) are in each subdivision.
     */
    lowersum = chv[indx].value;
    halfsum = sm / 2;
    for (i=1; i<clrs-1; i++) {
      if (lowersum >= halfsum) break;
      lowersum += chv[indx + i].value;
    }

    /*
     ** Split the box, and sort to bring the biggest boxes to the top.
     */
    bv[bi].colors = i;
    bv[bi].sum = lowersum;
    bv[boxes].index = indx + i;
    bv[boxes].colors = clrs - i;
    bv[boxes].sum = sm - lowersum;
    ++boxes;
    qsort((char*) bv, (size_t) boxes, sizeof(struct box), sumcompare);
  }  /* while (boxes ... */
  
  /*
   ** Ok, we've got enough boxes.  Now choose a representative color for
   ** each box.  There are a number of possible ways to make this choice.
   ** One would be to choose the center of the box; this ignores any structure
   ** within the boxes.  Another method would be to average all the colors in
   ** the box - this is the method specified in Heckbert's paper.  A third
   ** method is to average all the pixels in the box.  You can switch which
   ** method is used by switching the commenting on the REP_ defines at
   ** the beginning of this source file.
   */
  
  for (bi=0; bi<boxes; bi++) {
    /* REP_AVERAGE_PIXELS version */
    register int indx = bv[bi].index;
    register int clrs = bv[bi].colors;
    register long r = 0, g = 0, b = 0, sum = 0;

    for (i=0; i<clrs; i++) {
      r += PPM_GETR( chv[indx + i].color ) * chv[indx + i].value;
      g += PPM_GETG( chv[indx + i].color ) * chv[indx + i].value;
      b += PPM_GETB( chv[indx + i].color ) * chv[indx + i].value;
      sum += chv[indx + i].value;
    }

    r = r / sum;  if (r>maxval) r = maxval;	/* avoid math errors */
    g = g / sum;  if (g>maxval) g = maxval;
    b = b / sum;  if (b>maxval) b = maxval;

    PPM_ASSIGN( colormap[bi].color, r, g, b );
  }

  free(bv);
  return colormap;
}


/**********************************/
static int redcompare
#ifdef XW_PROTOTYPE
 (const void *p1, const void *p2)
#else
 (p1, p2)
     const void *p1, *p2;
#endif
{
  return (int) PPM_GETR( ((chist_vec)p1)->color ) - 
         (int) PPM_GETR( ((chist_vec)p2)->color );
}

/**********************************/
static int greencompare
#ifdef XW_PROTOTYPE
 (const void *p1, const void *p2)
#else
 (p1, p2)
     const void *p1, *p2;
#endif
{
  return (int) PPM_GETG( ((chist_vec)p1)->color ) - 
         (int) PPM_GETG( ((chist_vec)p2)->color );
}

/**********************************/
static int bluecompare
#ifdef XW_PROTOTYPE
 (const void *p1, const void *p2)
#else
 (p1, p2)
     const void *p1, *p2;
#endif
{
  return (int) PPM_GETB( ((chist_vec)p1)->color ) - 
         (int) PPM_GETB( ((chist_vec)p2)->color );
}

/**********************************/
static int sumcompare
#ifdef XW_PROTOTYPE
 (const void *p1, const void *p2)
#else
 (p1, p2)
     const void *p1, *p2;
#endif
{
  return ((box_vector) p2)->sum - ((box_vector) p1)->sum;
}



/****************************************************************************/
static chist_vec ppm_computechist
#ifdef XW_PROTOTYPE
 (pixel** pixels, int cols, int rows, int maxcolors, int* colorsP)
#else
 (pixels, cols, rows, maxcolors, colorsP)
     pixel** pixels;
     int cols, rows, maxcolors;
     int* colorsP;
#endif
{
  chash_table cht;
  chist_vec chv;

  cht = ppm_computechash(pixels, cols, rows, maxcolors, colorsP);
  if (!cht) return (chist_vec) NULL;

  chv = ppm_chashtochist(cht, maxcolors);
  ppm_freechash(cht);
  return chv;
}


/****************************************************************************/
static chash_table ppm_computechash
#ifdef XW_PROTOTYPE
 (pixel** pixels, int cols, int rows, int maxcolors, int* colorsP )
#else
 (pixels, cols, rows, maxcolors, colorsP )
     pixel** pixels;
     int cols, rows, maxcolors;
     int* colorsP;
#endif
{
  chash_table cht;
  register pixel* pP;
  chist_list chl;
  int col, row, hash;

  cht = ppm_allocchash( );
  *colorsP = 0;

  /* Go through the entire image, building a hash table of colors. */
  for (row=0; row<rows; row++)
    for (col=0, pP=pixels[row];  col<cols;  col++, pP++) {
      hash = ppm_hashpixel(*pP);

      for (chl = cht[hash]; chl != (chist_list) 0; chl = chl->next)
	if (PPM_EQUAL(chl->ch.color, *pP)) break;
      
      if (chl != (chist_list) 0) ++(chl->ch.value);
      else {
	if ((*colorsP)++ > maxcolors) {
	  ppm_freechash(cht);
	  return (chash_table) NULL;
	}
	
	chl = (chist_list) malloc(sizeof(struct chist_list_item));
	if (!chl) {
	  fprintf (stderr,
	   "ppm_computechash(): ran out of memory computing hash table!\r\n");
	  return (chash_table)(NULL);
	}

	chl->ch.color = *pP;
	chl->ch.value = 1;
	chl->next = cht[hash];
	cht[hash] = chl;
      }
    }
  
  return cht;
}


/****************************************************************************/
static chash_table ppm_allocchash()
{
  chash_table cht;
  int i;

  cht = (chash_table) malloc( HASH_SIZE * sizeof(chist_list) );
  if (!cht) {
    fprintf (stderr, "ppm_allochash(): ran out of memory allocating hash table!\r\n");
    return  cht;
  }

  for (i=0; i<HASH_SIZE; i++ )
    cht[i] = (chist_list) 0;

  return cht;
}


/****************************************************************************/
static chist_vec ppm_chashtochist
#ifdef XW_PROTOTYPE
 ( chash_table cht, int maxcolors )
#else
 ( cht, maxcolors )
     chash_table cht;
     int maxcolors;
#endif
{
  chist_vec chv;
  chist_list chl;
  int i, j;

  /* Now collate the hash table into a simple chist array. */
  chv = (chist_vec) malloc( maxcolors * sizeof(struct chist_item) );

  /* (Leave room for expansion by caller.) */
  if (!chv) {
    fprintf (stderr, 
	     "ppm_chashtochist(): ran out of memory generating histogram!\r\n");
    return chv;
  }

  /* Loop through the hash table. */
  j = 0;
  for (i=0; i<HASH_SIZE; i++)
    for (chl = cht[i];  chl != (chist_list) 0;  chl = chl->next) {
      /* Add the new entry. */
      chv[j] = chl->ch;
      ++j;
    }

  return chv;
}


/****************************************************************************/
static void
#ifdef XW_PROTOTYPE
 ppm_freechist( chist_vec chv )
#else
 ppm_freechist( chv )
     chist_vec chv;
#endif
{
  free( (char*) chv );
}


/****************************************************************************/
static void
#ifdef XW_PROTOTYPE
 ppm_freechash( chash_table cht )
#else
 ppm_freechash( cht )
     chash_table cht;
#endif
{
  int i;
  chist_list chl, chlnext;

  for (i=0; i<HASH_SIZE; i++)
    for (chl = cht[i];  chl != (chist_list) 0; chl = chlnext) {
      chlnext = chl->next;
      free( (char*) chl );
    }

  free( (char*) cht );
}

#undef PARM
#undef CONV24_FAST
#undef CONV24_BEST
#undef MAXCOLORS
#undef HASH_SIZE
