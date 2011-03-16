#include <Xw_Extension.h>
#include <Aspect_Convert.hxx>

/* ifdef then trace on */
#ifdef TRACE
#define TRACE_OPEN_WINDOW
#define TRACE_OPEN_DISPLAY
#endif

/*
   Aspect_Handle Xw_open_window (adisplay,class,parent,sxc,syc,
        swidth,sheight,title,istransparent):
   XW_EXT_DISPLAY *adisplay Extended Display structure
   Xw_TypeOfVisual class ;  must be one of :
      Xw_TOV_STATICGRAY,
      Xw_TOV_GRAYSCALE,
      Xw_TOV_STATICCOLOR,
      Xw_TOV_PSEUDOCOLOR,
      Xw_TOV_TRUECOLOR,
      Xw_TOV_DIRECTCOLOR,
      Xw_TOV_DEFAULT
      Xw_TOV_PREFERRED_PSEUDOCOLOR,
      Xw_TOV_PREFERRED_TRUECOLOR
   Aspect_Handle  parent ; Parent Window (ROOT if 0)

   float sxc,syc ;    Window center position in [0,1] space
   float swidth,sheight   Window size in [0,1] space
   char *title ;  Banner Title NULL if default.
   int istransparent  TRUE for transparent window

  Create the window depending of the requested visual class
  on the specified display name

  returns XW_ERROR if Display string or Visual class is wrong
        returns his ID if successful .

*/

#define BORDER 0

#ifdef XW_PROTOTYPE
Aspect_Handle Xw_open_window (void* adisplay,
                              Xw_TypeOfVisual aclass,
                              Aspect_Handle parent,
                              float sxc,
                              float syc,
                              float swidth,
                              float sheight,
                              char* title,
                              int istransparent)
#else
Aspect_Handle Xw_open_window (adisplay, aclass, parent, sxc, syc, swidth, sheight, title, istransparent)
void *adisplay;
Xw_TypeOfVisual aclass;
Aspect_Handle parent;
float sxc, syc, swidth, sheight;
char *title;
int istransparent;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_DISPLAY* pdisplay = (XW_EXT_DISPLAY* )adisplay;
Window window;
XSetWindowAttributes wattr;
unsigned long mask = 0;

  if (!Xw_isdefine_display (pdisplay))
  {
    /*ERROR*Bad EXT_DISPLAY Address*/
    Xw_set_error (96, "Xw_open_window", pdisplay);
    return (0) ;
  }

  if (swidth <= 0.0 || sheight <= 0.0)
  {
    /*ERROR*Coordinate must be in [0,1] space*/
    Xw_set_error (91, "Xw_open_window", 0);
    return (0);
  }

  XVisualInfo* ginfo = Xw_get_visual_info (pdisplay, aclass);
  if (!ginfo) return (0);

  Standard_Integer aParentSizeX, aParentSizeY;
  if (parent)
  {
    XWindowAttributes attributes;
    if( !XGetWindowAttributes(_DDISPLAY, parent, &attributes))
    {
      /*ERROR*Bad Window Attributes*/
      Xw_set_error (54, "Xw_open_window", &parent);
      return (0);
    }
    aParentSizeX = attributes.width;
    aParentSizeY = attributes.height;
    wattr.override_redirect = True ; mask |= CWOverrideRedirect;
  }
  else
  {
    parent = _DROOT;
    aParentSizeX = _DWIDTH;
    aParentSizeY = _DHEIGHT;
  }
  Standard_Integer aPxLeft, aPxTop, aPxWidth, aPxHeight;
  Aspect_Convert::ConvertCoordinates (aParentSizeX, aParentSizeY,
                                      sxc, syc, swidth, sheight,
                                      aPxLeft, aPxTop, aPxWidth, aPxHeight);
  Aspect_Convert::FitIn (aParentSizeX, aParentSizeY,
                         aPxLeft, aPxTop, aPxWidth, aPxHeight);

  wattr.event_mask = ExposureMask | StructureNotifyMask;
    mask |= CWEventMask;
  wattr.backing_store = NotUseful;
    mask |= CWBackingStore;
  wattr.border_pixel = WhitePixel (_DDISPLAY, DefaultScreen (_DDISPLAY));
    mask |= CWBorderPixel;
  if (!istransparent)
  {
    wattr.background_pixel = BlackPixel (_DDISPLAY, DefaultScreen (_DDISPLAY));
    mask |= CWBackPixel;
  }
  wattr.colormap = XCreateColormap (_DDISPLAY, parent,
                                    ginfo->visual, AllocNone);
    mask |= CWColormap ;

  window = XCreateWindow (_DDISPLAY, parent,
                          aPxLeft, aPxTop, aPxWidth, aPxHeight, BORDER,
                          ginfo->depth, InputOutput, ginfo->visual,
                          mask, &wattr);

  if (window && (parent == _DROOT))
  {
    XSizeHints hints;
    hints.x = aPxLeft; hints.y = aPxTop; hints.flags = PPosition;
    hints.width = aPxWidth; hints.height = aPxHeight; hints.flags |= PSize;
    XSetStandardProperties (_DDISPLAY, window, title, title, None,
                            NULL, 0, &hints);
  }

  XFree ((char* )ginfo);
  XFlush (_DDISPLAY);

#ifdef  TRACE_OPEN_WINDOW
  if (Xw_get_trace())
  {
    printf (" %lx = Xw_open_window(%lx,%d,%f,%f,%f,%f,'%s')\n",
    (long )window, (long )adisplay, aclass, sxc, syc, swidth, sheight, title);
  }
#endif
  return (window);
}
