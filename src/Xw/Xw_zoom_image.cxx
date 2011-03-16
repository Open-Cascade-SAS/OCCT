#include <Xw_Extension.h>

#define MAXISIZE  1600*1200

/* ifdef then trace on */
#ifdef TRACE
# define TRACE_ZOOM_IMAGE
#endif

/*
   XW_STATUS Xw_zoom_image (aimage,zoom):
   XW_EXT_IMAGEDATA *aimage     Image to ZOOM
   float zoom                   Zoom Coef to apply (>0.)

        Apply a ZOOM factor to an image

        returns  ERROR if bad zoom factor
        returns  SUCCESS if successfull

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_zoom_image (void* aimage,float zoom)
#else
XW_STATUS Xw_zoom_image (aimage,zoom)
void *aimage;
float zoom ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_IMAGEDATA *pimage = (XW_EXT_IMAGEDATA*)aimage ;
XImage *pximage,*qximage ;
int i,j,isize,wp,hp,xp,yp,xq,yq,xn,yn,dxp,dyp,npixel,num_pixel[MAXCOLOR] ;
unsigned long pixel,bpixel,tab_pixel[MAXCOLOR];
float ratio;


    if( !Xw_isdefine_image(pimage) ) {
        /*ERROR*Bad EXT_IMAGE Address*/
        Xw_set_error(25,"Xw_zoom_image",pimage) ;
        return (XW_ERROR) ;
    }

    if( zoom <= 0. ) {
        /*ERROR*Bad Image Zoom factor*/
        Xw_set_error(113,"Xw_zoom_image",&zoom) ;
        return (XW_ERROR) ;
    }

    pximage = pimage->pximage ;
    qximage = (XImage*) Xw_malloc(sizeof(XImage)) ;

    if( !qximage ) {
        /*ERROR*XImage Allocation failed*/
        Xw_set_error(60,"Xw_zoom_image",NULL) ;
        return (XW_ERROR) ;
    }

    Xw_bytecopy((char*)pximage,(char*)qximage,sizeof(XImage)) ;

    wp = pximage->width; hp = pximage->height;
    ratio = (float)wp/hp;
    qximage->width = (int )( wp * zoom );
    qximage->height = (int )( hp * zoom );
    dxp = dyp = 0;

    double datasize = qximage->width*qximage->height;
    if (datasize > MAXISIZE) {
      // ERROR*ZOOM factor is too big
      printf(" ***Xw_zoom_image(%f).Too BIG zoom, full image can't be zoomed.\n",zoom);
      Xw_set_error(130, "Xw_zoom_image", &zoom);
      return (XW_ERROR);
    }

    if (fabs(zoom - pimage->zoom) < 0.01) {
      return XW_SUCCESS;
    }

    qximage->bytes_per_line = qximage->width * (pximage->bitmap_pad/8) ;
    isize = qximage->bytes_per_line * qximage->height ;
    qximage->data = (char*) Xw_malloc(isize) ;

    if( !qximage->data ) {
        /*ERROR*XImage Allocation failed*/
        Xw_set_error(60,"Xw_zoom_image",NULL) ;
        return (XW_ERROR) ;
    }

    bpixel = XGetPixel(pximage,0,0) ;

    if( zoom > 1. ) {
      for( yp=yq=0 ; yp<hp ; yq=yn,yp++ ) {
        yn = (int )( (yp+1)*zoom+0.5 );
        if( yn >= qximage->height ) yn = qximage->height-1 ;
        for( xp=xq=0 ; xp<wp ; xq=xn,xp++ ) {
            xn = (int )( (xp+1)*zoom+0.5 );
            if( xn >= qximage->width ) xn = qximage->width-1 ;
            pixel = XGetPixel(pximage,xp+dxp,yp+dyp) ;
            if( ((xn-xq) > 1) || ((yn-yq) > 1) ) {
              register int x,y;
              for( y=yq ; y<yn ; y++ ) {
                for( x=xq ; x<xn ; x++ ) {
                  XPutPixel(qximage,x,y,pixel) ;
                }
              } 
            } else {
              XPutPixel(qximage,xq,yq,pixel) ;
            }
        }
      }
    } else {
      for( yp=yq=0 ; yq<qximage->height ; yp=yn,yq++ ) {
        yn = (int )( (yq+1)/zoom+0.5 );
        if( yn >= pximage->height ) yn = pximage->height-1 ;
        for( xp=xq=0 ; xq<qximage->width ; xp=xn,xq++ ) {
            xn = (int )( (xq+1)/zoom+0.5 );
            if( xn >= pximage->width ) xn = pximage->width-1 ;
            if( xn-1 > xp && yn-1 > yp ) {
              register int x,y,ipixel = 0;
              npixel = 0;
              for( y=yp ; y<yn ; y++ ) {
                for( x=xp ; x<xn ; x++ ) {
                  pixel = XGetPixel(pximage,x,y) ;
                  if( pixel != bpixel ) {
                    for( i=0 ; i<npixel ; i++) {
                      if( pixel == tab_pixel[ipixel] ) {
                        num_pixel[ipixel]++;
                        break;
                      }
                      ipixel++;
                      if( ipixel >= npixel ) ipixel = 0; 
                    }
                    if( i >= npixel ) {
                      ipixel = npixel;
                      num_pixel[npixel] = 1;
                      tab_pixel[npixel] = pixel;
                      if( npixel < MAXCOLOR-1 ) npixel++;
                    }
                  }
                }
              } 

              if( npixel > 0 ) {
                j = 0;
                for( i=1 ; i<npixel ; i++ ) {
                  if( num_pixel[i] > num_pixel[j] ) j = i;
                }
                bpixel = pixel = tab_pixel[j];
              } else {
                pixel = bpixel;
              }
            } else {    
              pixel = XGetPixel(pximage,xp,yp) ;
            }
            XPutPixel(qximage,xq,yq,pixel) ;
        }
      }
    }

    if( _ZIMAGE && (_ZIMAGE != _IIMAGE) ) {
      XDestroyImage(_ZIMAGE);
    }

    _ZIMAGE = qximage;
    pimage->zoom = zoom;

#ifdef  TRACE_ZOOM_IMAGE
if( Xw_get_trace() ) {
    printf (" Xw_zoom_image(%lx,%f)\n",(long ) pimage,zoom);
}
#endif

    return (XW_SUCCESS);
}
