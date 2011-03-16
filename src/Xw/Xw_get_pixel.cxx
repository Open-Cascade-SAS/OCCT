
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_PIXEL
#endif

/*
   STATUS Xw_get_pixel (aimage,x,y,index,npixel):
   XW_EXT_IMAGEDATA 		*aimage
   int x,y			Pixel position
   int *index			return Pixel color index 
   int *npixel			return Pixel number filled with 
				       the same color index

	Extract pixels index from an existing IMAGE created by Xw_get_image
		or fill with Xw_put_pixel

	returns ERROR if No image is defined
			 or No pixel color is defined
			 or Pixel position is wrong (Outside of image)
	returns SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_pixel (void* aimage,int x,int y,int* index,int* npixel)
#else
XW_STATUS Xw_get_pixel (aimage,x,y,index,npixel)
XW_EXT_IMAGEDATA *aimage ;
int x,y ;
int *index,*npixel ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_IMAGEDATA *pimage = (XW_EXT_IMAGEDATA*) aimage ;
register union {
    char  *data ;
    unsigned char *cdata ;
    unsigned short *sdata ;
    unsigned long  *ldata ;
} data ;
int i=0,fpixel,simage ;
unsigned long lpixel=0 ;
XImage *pximage;

        if( !Xw_isdefine_image(pimage) ) {
	    /*ERROR*Bad EXT_IMAGEDATA Address*/
	    Xw_set_error(25,"Xw_get_pixel",pimage) ;
            return (XW_ERROR) ;
        }

        pximage = (_ZIMAGE) ? _ZIMAGE : _IIMAGE;
	fpixel = x*pximage->width + y ;
	simage = pximage->height*pximage->width ;

	if( x < 0 || y < 0 || (fpixel >= simage) ) {
	    /*ERROR*Bad PIXEL position*/
	    Xw_set_error(47,"Xw_get_pixel",&simage) ;
	    return (XW_ERROR) ;
	}

	switch (pximage->bitmap_pad) {
	    case 8 :
		{ register unsigned char cpixel ; 
		data.data = pximage->data + (fpixel) ; simage -= fpixel ;
		cpixel = *data.cdata ;
		for( i=1 ; i<simage ; i++ ) {
		    data.cdata++ ; 
		    if( *data.cdata != cpixel ) break ;
		}
		}
		break ;
	    case 16 :
		{ register unsigned short spixel ; 
		data.data = pximage->data + (fpixel<<1) ; simage -= fpixel ;
		spixel = *data.sdata ;
		for( i=1 ; i<simage ; i++ ) {
		    data.sdata++ ; 
		    if( *data.sdata != spixel ) break ;
		}
		}
		break ;
	    case 32 :
		data.data = pximage->data + (fpixel<<2) ; simage -= fpixel;
		lpixel = *data.ldata ;
		for( i=1 ; i<simage ; i++ ) {
		    data.ldata++ ; 
		    if( *data.ldata != lpixel ) break ;
		}
	}

	*npixel = i ;
	*index = lpixel ;
	for( i=0 ; i<_ICOLORMAP->maxcolor ; i++ ) {
	    if( _ICOLORMAP->define[i] && 
		    (lpixel == _ICOLORMAP->pixels[i]) ) break ;
	}
	if( i < _ICOLORMAP->maxcolor ) {
	    *index = i ;
	} else {
	    /*ERROR*Bad Defined Color*/
	    Xw_set_error(41,"Xw_get_pixel",&index) ;
	    return (XW_ERROR) ;
	}

#ifdef  TRACE_GET_PIXEL
if( Xw_get_trace() > 2 ) {
    printf (" Xw_get_pixel(%lx,%d,%d,%ld,%ld)\n",
					(long ) pimage,x,y,(long ) index,(long ) npixel) ;
}
#endif

	return (XW_SUCCESS);
}
