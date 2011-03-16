
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
/*
#define TRACE_PUT_RGBPIXEL
*/
#endif

/*
   XW_STATUS Xw_put_rgbpixel (aimage,x,y,r,g,b,npixel):
   XW_EXT_IMAGEDATA image		*aimage
   int x,y			Pixel position
   float r,g,b			Pixel color index 
   int npixel			Pixel number to fill with the same color index

	Fill an existing IMAGE created by Xw_open_image with R,G,B values

	returns XW_ERROR if Extended Image is not defined
			 or VISUAL Image class is not TRUECOLOR
			 or Pixel position is wrong (Outside of image)
	returns XW_SUCCESS if successful

*/
#ifdef XW_PROTOTYPE
XW_STATUS Xw_put_rgbpixel (void *aimage,int x,int y,float r,float g,float b,int npixel)
#else
XW_STATUS Xw_put_rgbpixel (aimage,x,y,r,g,b,npixel)
void *aimage ;
int x,y,npixel ;
float r,g,b ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_IMAGEDATA *pimage = (XW_EXT_IMAGEDATA*)aimage;
XW_EXT_COLORMAP *pcolormap = pimage->pcolormap;
union {
    char  *data ;
    unsigned char *cdata ;
    unsigned short *sdata ;
    unsigned long  *ldata ;
} data ;
int fpixel,lpixel,simage,index,isapproximate ;
register int np ;
unsigned long pixel ;
XW_STATUS status ;
XImage *pximage;

        if( !Xw_isdefine_image(pimage) ) {
	    /*ERROR*Bad EXT_IMAGEDATA Address*/
	    Xw_set_error(25,"Xw_put_rgbpixel",pimage) ;
            return (XW_ERROR) ;
        }

	switch ( _CCLASS ) {
	    case TrueColor :
		status = Xw_get_color_pixel(_ICOLORMAP,r,g,b,&pixel,&isapproximate) ;
		break ;

	    case PseudoColor :
		status = Xw_get_color_index(_ICOLORMAP,r,g,b,&index) ;
		pixel = _ICOLORMAP->pixels[index] ;
		break ;
	    
	    default :
	    	/*ERROR*Unimplemented Visual class*/
	    	Xw_set_error(5,"Xw_put_rgbpixel",&_CCLASS) ;
	    	return (XW_ERROR) ;
	}

	pximage = (_ZIMAGE) ? _ZIMAGE : _IIMAGE;
	simage = pximage->height*pximage->width ;
	fpixel = x*pximage->width + y ;
	lpixel = fpixel + npixel - 1 ;

	if( x < 0 || y < 0 || (lpixel >= simage) ) {
	    /*WARNING*Bad PIXEL position*/
	    Xw_set_error(47,"Xw_put_rgbpixel",&simage) ;
	    return (XW_ERROR) ;
	}

	switch (pximage->bitmap_pad) {
	    case 8 :
		data.data = pximage->data + (fpixel) ;
		for( np=npixel ; np ; --np )
				*data.cdata++ = (unsigned char) pixel ;
		break ;
	    case 16 :
		data.data = pximage->data + (fpixel<<1) ;
		for( np=npixel ; np ; --np )
				*data.sdata++ = (unsigned short) pixel ;
		break ;
	    case 32 :
		data.data = pximage->data + (fpixel<<2) ;
		for( np=npixel ; np ; --np )
				*data.ldata++ = (unsigned long) pixel ;
	}

#ifdef  TRACE_PUT_RGBPIXEL
	printf (" Xw_put_rgbpixel(%x,%d,%d,%f,%f,%f,%d)\n",
					pimage,x,y,r,g,b,npixel) ;
#endif

	return (XW_SUCCESS);
}
