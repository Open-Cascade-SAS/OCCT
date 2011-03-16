 
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_IMAGE_INFO
#endif

/*
   XW_STATUS Xw_get_image_info (aimage,zoom,width,height,depth):
   XW_EXT_IMAGEDATA *aimage     Image
   float *zoom                  returned Zoom factor 
   int *width,*height		returned Image pixel size
   int *depth			returned Image planes depth.

	Get image size and zoom factor.

        returns  ERROR if bad image extended address 
        returns  SUCCESS if successfull

*

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_image_info (void* aimage,float *zoom,int* width,int* height,int *depth)
#else
XW_STATUS Xw_get_image_info (aimage,zoom,width,height,depth)
void *aimage;
float *zoom;
int *width;
int *height;
int *depth;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_IMAGEDATA *pimage = (XW_EXT_IMAGEDATA*)aimage;

    if( !Xw_isdefine_image(pimage) ) {
        /*ERROR*Bad EXT_IMAGE Address*/
        Xw_set_error(25,"Xw_get_image_info",pimage) ;
        return (XW_ERROR) ;
    }

    *zoom = pimage->zoom;
    *width = _IIMAGE->width;
    *height = _IIMAGE->height;
    *depth = _IIMAGE->depth;

#ifdef  TRACE_GET_IMAGE_INFO
if( Xw_get_trace() ) {
    printf (" Xw_get_image_info('%lx',%f,%d,%d,%d)\n",(long ) aimage,
				*zoom,*width,*height,*depth);
}
#endif

    return (XW_SUCCESS);
}
