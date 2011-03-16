#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#define CTS17988	/*GG_240697
//			Ne pas chercher a ajuster la profondeur du visual
//			recherche en TrueColor par rapport aux MultiBuffers disponibles
*/

#define GG250997	/*
//			Pour le 2D et plans overlay prendre de
//			preference une colormap existante afin
//			d'eviter des pb d'installation sur la nouvelle
//			colormap.
*/

#define TEST	/*051297
//		Ameliorer la recuperation de la colormap OVERLAY lorsque
//		celle ci est invalide voir ligne 925-928
*/

#define PURIFY	/*GG+STT 110199
//		Avoid memory leak
*/

#define IMP040100	/*GG_040100 Overlay planes are not usable
//			when the alone installed colormap is used
//			by the default root visual.
*/

#include <Xw_Extension.h>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#if (!defined (CTS17988))  && defined(HAVE_X11_EXTENSIONS_MULTIBUF_H)
# include <X11/extensions/multibuf.h>
#endif

#include <X11/Xmu/StdCmap.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_DEF_COLORMAP
#define TRACE_CLOSE_COLORMAP
#define TRACE_SET_COLORMAP_MAPPING
#endif

/*
   XW_EXT_COLORMAP* Xw_def_colormap(adisplay,pclass,ncolor,basemap,mapping):
   XW_EXT_DISPLAY *adisplay Extended Display structure
   Xw_TypeOfVisual	pclass	Visual Class,must be one of :

		        Xw_TOV_STATICGRAY,
			Xw_TOV_GRAYSCALE,
			Xw_TOV_STATICCOLOR,
			Xw_TOV_PSEUDOCOLOR,
        		Xw_TOV_TRUECOLOR,
			Xw_TOV_DIRECTCOLOR,
			Xw_TOV_DEFAULT,
			Xw_TOV_PREFERRED_PSEUDOCOLOR,
			Xw_TOV_PREFERRED_TRUECOLOR,
			Xw_TOV_PREFERRED_OVERLAY,
			Xw_TOV_OVERLAY

   int ncolor		Maximum Number of color to be allocated
   Aspect_Handle basemap	User Colormap (Can be NULL)
   Xw_TypeOfMapping	Colormap mapping ,must be ONE of

			XW_TOM_HARDRAMP,
        		XW_TOM_SIMPLERAMP,
			XW_TOM_BESTRAMP,
			XW_TOM_COLORCUBE,
			XW_TOM_READONLY

	Create a colormap extension with a colormap ID
	compatible with the visual class required
        allocate the max color cells in the colormap as if possible
	depending of the actual Standard Colormap size.
	NOTE than the resulting color number must be less than 
	the required color number depending of the System color number used .
	See Xw_get_colormap_info for more Informations

	NOTE than if ncolor <= 0 this allocates the Maximum color
	     number in the colormap .

	Returns Colormap extension address if successuful
 	Returns NULL if Bad Allocation 
	        or BadVisual class
		or Bad resulting Color Number

   STATUS Xw_close_colormap(acolormap):
   XW_EXT_COLORMAP* acolormap	Extended Colormap address


	Destroy The specified Extended Colormap 

	Returns SUCCESS if successuful
 	Returns ERROR if Bad Extended Colormap

   STATUS Xw_set_colormap_mapping(acolormap,mode)
   XW_EXT_COLORMAP* acolormap	Extended Colormap address
   Xw_TypeOfMapping mode

	Update the specified ColorMap in the requested Color Mapping

	Returns SUCCESS if successuful
 	Returns ERROR if Bad Extended Colormap
*/

#define MAXGRAY 13
#define MAXCOLORCUBE 216
static char SetColorCube = 'Y' ;
static char UseDefaults  = 'U' ;
static int  MinColors = MINCOLOR ;
static int  FreColors = FRECOLOR ;
static int  TrueColorMinDepth  = 12 ;
static int  OverlayColorMinDepth  = 4 ;
static int  MaxGray = MAXGRAY ;
static int  MaxColorCube = MAXCOLORCUBE ;
static int  EnableSunOverlay = False;	
static char svalue[80] ;
static Atom DefaultAtom = 0 ; 
static const char *XW_RGB_BEST_MAP = "Xw_RGB_BEST_MAP" ;

#ifdef XW_PROTOTYPE
void* Xw_def_colormap (void* adisplay,Xw_TypeOfVisual pclass,
			int ncolor,Aspect_Handle basemap,Xw_TypeOfMapping mapping)
#else
void* Xw_def_colormap (adisplay,pclass,ncolor,basemap,mapping)
void *adisplay ;
Xw_TypeOfVisual pclass ;
int ncolor ;
Aspect_Handle basemap ;
Xw_TypeOfMapping mapping ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*)adisplay ;
XW_EXT_COLORMAP *pcolormap = NULL ;
XVisualInfo *ginfo = NULL ;
int i,j,n,b,d,acolor,mcolor,fcolor,cclass ;
unsigned long backpixel = 0;

    if( !Xw_isdefine_display(pdisplay) ) {
        /*ERROR*Bad EXT_DISPLAY Address*/
        Xw_set_error(96,"Xw_def_colormap",pdisplay) ;
        return (NULL) ;
    }

    if( Xw_get_env("Xw_USE_DEFAULTS",svalue,sizeof(svalue)) ){
	if( strlen(svalue) ) {
	    UseDefaults = svalue[0] ;
	}
	printf( " Xw_USE_DEFAULTS is '%c'\n",UseDefaults) ;
    }
    if( Xw_get_env("Xw_SET_COLOR_CUBE",svalue,sizeof(svalue)) ){
	if( strlen(svalue) ) {
	    SetColorCube = svalue[0] ;
	}
	printf( " Xw_SET_COLOR_CUBE is '%c'\n",SetColorCube) ;
    }
    if( Xw_get_env("Xw_MIN_COLORS",svalue,sizeof(svalue)) ){
	if( strlen(svalue) ) {
	    sscanf(svalue,"%d",&MinColors)  ;
	}
	printf( " Xw_MIN_COLORS is %d\n",MinColors) ;
    }
    if( Xw_get_env("Xw_FRE_COLORS",svalue,sizeof(svalue)) ){
	if( strlen(svalue) ) {
	    sscanf(svalue,"%d",&FreColors)  ;
	}
	printf( " Xw_FRE_COLORS is %d\n",FreColors) ;
    }
    if( pdisplay->server == XW_SERVER_IS_HP ) TrueColorMinDepth = 8;
    if( Xw_get_env("Xw_SET_TRUE_COLOR_MIN_DEPTH",svalue,sizeof(svalue)) ){
	if( strlen(svalue) ) {
	    sscanf(svalue,"%d",&TrueColorMinDepth)  ;
	}
	printf( " Xw_SET_TRUE_COLOR_MIN_DEPTH is %d\n",TrueColorMinDepth) ;
    }
    if( Xw_get_env("Xw_SET_OVERLAY_COLOR_MIN_DEPTH",svalue,sizeof(svalue)) ){
	if( strlen(svalue) ) {
	    sscanf(svalue,"%d",&OverlayColorMinDepth)  ;
	}
	printf( " Xw_SET_OVERLAY_COLOR_MIN_DEPTH is %d\n",OverlayColorMinDepth) ;
    }
    if( Xw_get_env("Xw_MAX_GRAY",svalue,sizeof(svalue)) ){
	if( strlen(svalue) ) {
	    sscanf(svalue,"%d",&MaxGray)  ;
	}
	printf( " Xw_MAX_GRAY is %d\n",MaxGray) ;
    }
    if( Xw_get_env("Xw_MAX_COLORCUBE",svalue,sizeof(svalue)) ){
	if( strlen(svalue) ) {
	    sscanf(svalue,"%d",&MaxColorCube)  ;
	}
	printf( " Xw_MAX_COLORCUBE is %d\n",MaxColorCube) ;
    }
    if( Xw_get_env("Xw_ENABLE_SUNOVERLAY",svalue,sizeof(svalue)) ){
	if( svalue[0] == 'Y' ) EnableSunOverlay = True;	
	else EnableSunOverlay = False;	

	printf( " Xw_ENABLE_SUNOVERLAY is %d\n",EnableSunOverlay) ;
    }

    if( UseDefaults == 'P' ) pclass = Xw_TOV_PREFERRED_PSEUDOCOLOR ;
    else if( UseDefaults == 'T' ) pclass = Xw_TOV_PREFERRED_TRUECOLOR ;
    else if( UseDefaults == 'O' ) pclass = Xw_TOV_PREFERRED_OVERLAY ;
    else if( UseDefaults == 'S' ) pclass = Xw_TOV_STATICCOLOR ;
    else if( UseDefaults == 'Y' ) pclass = Xw_TOV_DEFAULT ;

    if( pclass == Xw_TOV_OVERLAY ) {
      ginfo = Xw_get_overlay_visual_info(pdisplay,
				Xw_TOV_PSEUDOCOLOR,&backpixel) ;
      if( !ginfo ) return (NULL) ;
    } else if( pclass == Xw_TOV_PREFERRED_OVERLAY ) {
      ginfo = Xw_get_overlay_visual_info(pdisplay,
				Xw_TOV_PSEUDOCOLOR,&backpixel) ;
      if( !ginfo ) pclass = Xw_TOV_PREFERRED_PSEUDOCOLOR;
    }

    if( !ginfo ) {
      ginfo = Xw_get_visual_info(pdisplay,pclass) ;
    }

    if( !ginfo ) return (NULL) ;

    if( !(pcolormap = Xw_add_colormap_structure(sizeof(XW_EXT_COLORMAP))) ) 
								return (NULL) ;
    pcolormap->connexion = pdisplay ;
    pcolormap->visual = ginfo->visual ;
    pcolormap->backpixel = backpixel ;
    XFree((char*)ginfo) ;

RESTART :

    if( mapping == Xw_TOM_READONLY ) {
        cclass = StaticColor;
    } else {
        cclass = _CCLASS;
    }
 
    switch ( cclass ) {

	case TrueColor :
	    acolor = MAXCOLOR ;
	    pcolormap->maxucolor = acolor ; 
	    pcolormap->maxhcolor = _CVISUAL->map_entries *
					_CVISUAL->map_entries *
						_CVISUAL->map_entries ; 
	    pcolormap->info.base_pixel = 0 ;
            if( (_DCLASS == TrueColor) &&
                        (UseDefaults == 'Y' || UseDefaults == 'P') ) {
              basemap = _CCOLORMAP ;
            } else if( UseDefaults == 'N' ) {
              basemap = 0 ;
            }  

	    if( basemap ) {
	      _CINFO.colormap = basemap ; 
	    }

	    _CINFO.killid = 0 ;
	    _CINFO.visualid = _CVISUAL->visualid ;
	    if( !_CINFO.colormap ) {
	      _CINFO.killid = getpid();
	      _CINFO.colormap = XCreateColormap(_CDISPLAY,_CROOT,
							_CVISUAL,AllocNone) ;
	    }
	    break ;

	case PseudoColor :
	    {
	      unsigned long stdpixels[MAXCOLOR] ;
	      unsigned long usrpixels[MAXCOLOR] ;

	      if( ncolor ) {
		  acolor = ncolor+1 ;
	      } else {
	          acolor = MAXCOLOR ;
	      }
	      mcolor = 0 ;
	      fcolor = 0 ;

      	      acolor = min(_CVISUAL->map_entries,acolor);
	      pcolormap->maxhcolor = min(_CVISUAL->map_entries,MAXCOLOR) ;

	      _CINFO.colormap = 0 ;
	      _CINFO.base_pixel = 0 ;
	      _CINFO.red_max = _CINFO.green_max = _CINFO.blue_max = 0 ;
	      _CINFO.killid = 1 ;
	      _CINFO.visualid = _CVISUAL->visualid ;

              if( (_DCLASS == PseudoColor) &&
                        (UseDefaults == 'Y' || UseDefaults == 'P') ) {
                  basemap = _CCOLORMAP ;
              } else if( UseDefaults == 'N' ) {
                  basemap = 0 ;
              }  

	      if( basemap ) {
		  _CINFO.colormap = basemap ; 
	      }

	      if( !_CINFO.colormap ) {	/* Create colormap */
	        _CINFO.colormap = XCreateColormap(_CDISPLAY,_CROOT,
						_CVISUAL,AllocNone) ;
		usrpixels[0] = 0 ;
	        if( !_CINFO.colormap ) {
		  /*ERROR*Colormap creation failed*/
		  Xw_set_error(2,"Xw_def_colormap",NULL) ;
		  Xw_del_colormap_structure(pcolormap) ;
		  return (NULL) ;
     	        }
	      } 
					/* Allocate colors */
	      usrpixels[0] = 0;
    	      while ( acolor && !XAllocColorCells(_CDISPLAY,
			_CINFO.colormap,True,NULL,0,usrpixels,acolor) ) {
		  --acolor ;
	      }
				/* Verify if COLORS are Contigues */
	      for( i=j=1,n=b=d=0 ; i<acolor ; i++ ) {
		  if( usrpixels[i] == usrpixels[i-1] + 1 )  {
		      j++ ;
		  } else {
		      if( j > n ) {	/* Take the largest hole */
		        b = d ;
		        n = j ;
		      }
		      d = i ;
		      j = 1 ;
		  }
	      }
	      if( j > n ) {
		  b = d ;
		  n = j ;
	      }
				/* Verify FREE Space for other applications */
	      if( basemap ) {
		  j = pcolormap->maxhcolor - (usrpixels[n-1]+1) ;
		  if( b+j < fcolor ) { /* Let Free space as specified */
		      j = fcolor - (b+j) ;
		      b += j ; n -= j ;
		  }
    	          if( n < mcolor ) {	/* Too few Colors */
	              XFreeColors(_CDISPLAY,_CINFO.colormap,usrpixels,acolor,0);
		      basemap = 0 ;
		      goto RESTART ;
		  }
	      }
    
	      if( mapping == Xw_TOM_BESTRAMP ) {
				/* Base pixel must be EVEN */
    	        if( usrpixels[b] & 1 ) {
		  b++ ; --n ;	
	        }
	      }

	      if( n < acolor ) {
		  		/* Allocate ONLY %d contiguous colors */
		  if( b ) {
		      XFreeColors(_CDISPLAY,_CINFO.colormap,usrpixels,b,0) ;
		  }
		  if( b+n < acolor ) {
		      XFreeColors(_CDISPLAY,_CINFO.colormap,
						&usrpixels[b+n],acolor-b-n,0) ;
		  }
		  for( i=0 ; i<n ; i++,b++ ) usrpixels[i] = usrpixels[b] ;
		  acolor = n ;
#ifdef TRACE_DEF_COLORMAP
		  if( Xw_get_trace() ) {
		      Xw_set_error(105,"Xw_def_colormap",&acolor) ;
		  }
#endif
	      }

	      _CINFO.base_pixel = usrpixels[0] ;
	      pcolormap->maxucolor = acolor ;

    	      if( (_CINFO.colormap != _CCOLORMAP) && 
			((MinCmapsOfScreen(_CSCREEN) < 1) ||
					(MaxCmapsOfScreen(_CSCREEN) == 1)) ) { 
		/* Get the First usable color in DEFAULT colormap */
	        int scolor = pcolormap->maxhcolor ; 
	        XColor color ;

    	        color.flags = DoRed | DoGreen | DoBlue ;
		while ( scolor && !XAllocColorCells(_CDISPLAY,
		    _CCOLORMAP,True,NULL,0,stdpixels,scolor) ) --scolor ;

	        XFreeColors(_CDISPLAY,_CCOLORMAP,stdpixels,scolor,0) ;

	        for( i=0 ; i<pcolormap->maxhcolor ; i++ ) {
	            color.pixel =  _CINFO.base_pixel + i ;
	            XQueryColor(_CDISPLAY,_CCOLORMAP,&color) ;
	            XStoreColor(_CDISPLAY,_CINFO.colormap,&color) ;
	        }

		if( scolor >= mcolor ) {
		    pcolormap->maxucolor = scolor ;
		    _CINFO.base_pixel = stdpixels[0] ;
		}
	
    	      } 
	      acolor = pcolormap->maxucolor ;
    	    } 
	    break ;

	case StaticColor :
	    {
//	      unsigned long usrpixel ;
              if( (_DCLASS == PseudoColor) && (_CCLASS == PseudoColor) &&
                        (UseDefaults == 'Y' || UseDefaults == 'P') ) {
                basemap = _CCOLORMAP ;
              } else if( UseDefaults == 'N' ) {
                basemap = 0 ;
              }  
	      _CINFO.visualid = _CVISUAL->visualid ;
	      _CINFO.red_max = _CINFO.green_max = _CINFO.blue_max = 0 ;
	      _CINFO.red_mult = _CINFO.green_mult = _CINFO.blue_mult = 0 ;
	      _CINFO.killid = 0 ;
	      if( basemap ) {
	        _CINFO.colormap = basemap ; 
	      }

	      if( !_CINFO.colormap ) {	/* Create colormap */
	        _CINFO.colormap = XCreateColormap(_CDISPLAY,_CROOT,
							_CVISUAL,AllocNone) ;
	        if( !_CINFO.colormap ) {
		    /*ERROR*Colormap creation failed*/
		    Xw_set_error(2,"Xw_def_colormap",NULL) ;
		    Xw_del_colormap_structure(pcolormap) ;
		    return (NULL) ;
     	        }
	      }
	      _CINFO.base_pixel = 0 ;
    	      acolor = (ncolor > 0) ? ncolor : MAXCOLOR ;
	      if( _CCLASS != TrueColor ) {
	        pcolormap->maxhcolor = min(_CVISUAL->map_entries,MAXCOLOR) ; 
	      } else {
	        pcolormap->maxhcolor = _CVISUAL->map_entries *
					_CVISUAL->map_entries *
						_CVISUAL->map_entries ; 
	      }
	      pcolormap->maxucolor = min(acolor,pcolormap->maxhcolor);
	    }
	    break ;

	case StaticGray :
	case GrayScale :
	case DirectColor :
	    /*ERROR*Unimplemented Visual class*/
	    Xw_set_error(5,"Xw_def_colormap",&pclass) ;
	    return (NULL) ;
    }

    Xw_set_colormap_mapping(pcolormap,mapping) ;
    Xw_def_highlight_color(pcolormap,1.,1.,1.) ;

#ifdef TRACE_DEF_COLORMAP
if( Xw_get_trace() ) {
    printf(" %lx = Xw_def_colormap(%lx,%d,%d)\n",
	   (long ) pcolormap,(long ) adisplay,pclass,acolor) ;
}
#endif

    return (pcolormap);
}

static XW_EXT_COLORMAP *PcolormapList =NULL ;

#ifdef XW_PROTOTYPE
XW_EXT_COLORMAP* Xw_add_colormap_structure(int size)
#else
XW_EXT_COLORMAP* Xw_add_colormap_structure(size)
int size ;
#endif /*XW_PROTOTYPE*/
/*
        Create and Insert one Extended colormap structure in the
        EXtended colormap List

        returns Extended colormap address if successful
                or NULL if Bad Allocation
*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*) Xw_malloc(size) ;
//int i ;

        if( pcolormap ) {
	    pcolormap->type = COLORMAP_TYPE ;
            pcolormap->link = PcolormapList ;
            PcolormapList = pcolormap ;
	    pcolormap->connexion = NULL ;
	    pcolormap->visual = NULL ;
	    pcolormap->maxcolor = 0 ;
	    pcolormap->maxhcolor = 0 ;
	    pcolormap->maxucolor = 0 ;
	    pcolormap->maxwindow = 0 ;
	    pcolormap->mapping = (Xw_TypeOfMapping)-1 ;
	    pcolormap->highpixel = 0 ;
	    pcolormap->backpixel = 0 ;
	    pcolormap->info.base_pixel = 0 ;
	    pcolormap->info.red_max = 0 ;
	    pcolormap->info.red_mult = 0 ;
	    pcolormap->info.green_max = 0 ;
	    pcolormap->info.green_mult = 0 ;
	    pcolormap->info.blue_max = 0 ;
	    pcolormap->info.blue_mult = 0 ;
	    pcolormap->info.colormap = 0 ;
	    pcolormap->ginfo.base_pixel = 0 ;
	    pcolormap->ginfo.red_max = 0 ;
	    pcolormap->ginfo.red_mult = 0 ;
	    pcolormap->ginfo.green_max = 0 ;
	    pcolormap->ginfo.green_mult = 0 ;
	    pcolormap->ginfo.blue_max = 0 ;
	    pcolormap->ginfo.blue_mult = 0 ;
	    pcolormap->ginfo.colormap = 0 ;
        } else {
	    /*ERROR*EXT_COLORMAP allocation failed*/
	    Xw_set_error(6,"Xw_add_colormap_structure",NULL) ;
        }

        return (pcolormap) ;
}

#ifdef XW_PROTOTYPE
XW_EXT_COLORMAP* Xw_get_colormap_structure(XW_EXT_COLORMAP* pcolormap)
#else
XW_EXT_COLORMAP* Xw_get_colormap_structure(pcolormap)
XW_EXT_COLORMAP *pcolormap ;
#endif /*XW_PROTOTYPE*/
/*
        Get the NEXT ColorMap structure from the List

        returns Extended colormap address if successful
                or NULL if No MORE Colormap is found
*/
{

	if( pcolormap ) return (XW_EXT_COLORMAP*)pcolormap->link ;
	else		return (PcolormapList) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_close_colormap(void* acolormap)
#else
XW_STATUS Xw_close_colormap(acolormap)
void *acolormap;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*)acolormap ;
XW_STATUS status ;

    if( !Xw_isdefine_colormap(pcolormap) ) {
        /*ERROR*Bad EXT_COLORMAP Address*/
        Xw_set_error(42,"Xw_close_colormap",pcolormap) ;
        return (XW_ERROR) ;
    }

    status = Xw_del_colormap_structure(pcolormap) ;

#ifdef TRACE_CLOSE_COLORMAP
if( Xw_get_trace() ) {
    printf(" %d = Xw_close_colormap(%lx)\n",status,(long ) pcolormap) ;
}
#endif

    return (status) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_del_colormap_structure(XW_EXT_COLORMAP* pcolormap)
#else
XW_STATUS Xw_del_colormap_structure(pcolormap)
XW_EXT_COLORMAP *pcolormap;
#endif /*XW_PROTOTYPE*/
/*
        Remove the Extended colormap address from the Extended List and
        Free the Extended Colormap

        returns ERROR if the colormap address is not Found in the list
        returns SUCCESS if successful
*/
{
XW_EXT_COLORMAP *fcolormap;
//int i ;

    if( !pcolormap ) return (XW_ERROR) ;

    if( pcolormap->maxwindow ) {
        return (XW_ERROR) ;
    } else {
	if( (_CINFO.killid == (unsigned int ) getpid()) && _CINFO.colormap ) {
	  if( _CINFO.colormap != _CCOLORMAP ) {
            XFreeColormap(_CDISPLAY,_CINFO.colormap) ;
            for( fcolormap = PcolormapList ; fcolormap ; 
				fcolormap = (XW_EXT_COLORMAP*)fcolormap->link ) {
		if( (fcolormap != pcolormap) && 
			(fcolormap->info.colormap == _CINFO.colormap) ) {
	    	  fcolormap->info.colormap = 0 ;
		}
            }
	    _CINFO.colormap = 0;
	  }
	  DefaultAtom = XInternAtom(_CDISPLAY,XW_RGB_BEST_MAP,True) ;
	  if( DefaultAtom ) {
	    XDeleteProperty(_CDISPLAY,_CROOT,DefaultAtom);
	  }
	}
        if( pcolormap == PcolormapList ) {
            PcolormapList = (XW_EXT_COLORMAP*)pcolormap->link ;
        } else {
            for( fcolormap = PcolormapList ; fcolormap ; 
					fcolormap = (XW_EXT_COLORMAP*)fcolormap->link ) {
                if( fcolormap->link == pcolormap ) {
                    fcolormap->link = pcolormap->link ;
                    break ;
                }
            }
        }
        Xw_free(pcolormap) ;
    }
    return (XW_SUCCESS) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_set_colormap_mapping(void* acolormap,Xw_TypeOfMapping mode)
#else
XW_STATUS Xw_set_colormap_mapping(acolormap,mode)
void *acolormap ;
Xw_TypeOfMapping mode ;
#endif /*XW_PROTOTYPE*/
/*
        Set MAPPING Colormap mode

        if mapping is SIMPLERAMP all color cells access is done
           across the PIXEL index array .
        if mapping is BESTRAMP all color cells access is done
           across the PIXEL index array and an HIGHLIGHT plane is created .
        if mapping is HARDRAMP all color cells access is done
           directly (Make becarefull !!)
        if mapping is COLORCUBE all color cells access is done
           across a ColorCube PIXEL index array .
        if mapping is READONLY all color cells access is done
           across a readonly PIXEL index array without color allocation.
*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*)acolormap ;
int status,cstatus,gstatus ;
unsigned char define[MAXCOLOR] ;
unsigned long pixels[MAXCOLOR] ;
XColor color ;
//int i,j,k,n,size,ncolor,ngcolor,nccolor=0 ;
int i,j,n,size,ncolor,ngcolor,nccolor=0 ;
unsigned int iu , ju , ku ;
//unsigned long red,green,blue,mask,rmask,gmask,bmask,pixel ;
unsigned long red,green,blue,pixel ;
XStandardColormap *cinfo = NULL,*cginfo = NULL,*pinfo = NULL;
int nmap;

  if( !pcolormap || (mode == pcolormap->mapping) ) return (XW_ERROR) ;

  for( i=0 ; i<MAXCOLOR ; i++ ) {
    define[i] = pcolormap->define[i] ;
    pixels[i] = pcolormap->pixels[i] ;
  }

  switch (_CCLASS) {

    case Xw_TOV_TRUECOLOR :
      switch (mode) {

	case Xw_TOM_SIMPLERAMP :
	case Xw_TOM_READONLY :
	    pcolormap->maxcolor = pcolormap->maxucolor ;
	    pcolormap->highpixel = AllPlanes ;
	    switch (pcolormap->mapping) {
		case Xw_TOM_READONLY :
		case Xw_TOM_BESTRAMP :
		case Xw_TOM_HARDRAMP :
		case Xw_TOM_COLORCUBE :
		    break ;
		default :
        	    for( i=0 ; i<MAXCOLOR ; i++ ) {
	    	 	pcolormap->define[i] = FREECOLOR ;
	    	 	pcolormap->pixels[i] = 0 ;
        	    }
	    }
	    break ;
	case Xw_TOM_BESTRAMP :
	    pcolormap->maxcolor = pcolormap->maxucolor ;
	    pcolormap->highpixel = AllPlanes ;
	    switch (pcolormap->mapping) {
		case Xw_TOM_READONLY :
		case Xw_TOM_SIMPLERAMP :
		case Xw_TOM_HARDRAMP :
		case Xw_TOM_COLORCUBE :
		    break ;
		default :
        	    for( i=0 ; i<MAXCOLOR ; i++ ) {
	    	 	pcolormap->define[i] = FREECOLOR ;
	    	 	pcolormap->pixels[i] = 0 ;
        	    }
	    }
	    break ;
	case Xw_TOM_HARDRAMP :
	    pcolormap->maxcolor = pcolormap->maxucolor ;
	    pcolormap->highpixel = AllPlanes ;
	    switch (pcolormap->mapping) {
		case Xw_TOM_READONLY :
		case Xw_TOM_SIMPLERAMP :
		case Xw_TOM_BESTRAMP :
		case Xw_TOM_COLORCUBE :
		    break ;
		default :
        	    for( i=0 ; i<MAXCOLOR ; i++ ) {
	    	 	pcolormap->define[i] = FREECOLOR ;
	    	 	pcolormap->pixels[i] = 0 ;
        	    }
	    }
	    break ;
	case Xw_TOM_COLORCUBE :
	    pcolormap->maxcolor = pcolormap->maxucolor ;
	    pcolormap->highpixel = AllPlanes ;
	    switch (pcolormap->mapping) {
		case Xw_TOM_READONLY :
		case Xw_TOM_SIMPLERAMP :
		case Xw_TOM_BESTRAMP :
		case Xw_TOM_HARDRAMP :
		default :
        	    for( i=0 ; i<MAXCOLOR ; i++ ) {
	    	 	pcolormap->define[i] = FREECOLOR ;
	    	 	pcolormap->pixels[i] = 0 ;
        	    }
            	    nccolor = pcolormap->maxhcolor;
		    if( nccolor > 0 ) {
              	      _CINFO.red_max = _CVISUAL->red_mask;
              	      _CINFO.red_mult = 1;
              	      while ( !(_CINFO.red_max & 0x01) ) {
                	      _CINFO.red_max >>= 1;
                	      _CINFO.red_mult <<= 1;
              	      }  
              	      _CINFO.green_max = _CVISUAL->green_mask;
              	      _CINFO.green_mult = 1;
               	      while ( !(_CINFO.green_max & 0x01) ) {
                	      _CINFO.green_max >>= 1;
                	      _CINFO.green_mult <<= 1;
              	      }  
              	      _CINFO.blue_max = _CVISUAL->blue_mask;
              	      _CINFO.blue_mult = 1;
              	      while ( !(_CINFO.blue_max & 0x01) ) {
                	      _CINFO.blue_max >>= 1;
                	      _CINFO.blue_mult <<= 1;
              	      }
              	      status = XmuCreateColormap(_CDISPLAY,&_CINFO);
		    }
            	    ngcolor = _CVISUAL->red_mask + 1;
		    if( ngcolor > 0 ) {
              	      _CGINFO.colormap = _CINFO.colormap;
              	      _CGINFO.base_pixel = _CINFO.base_pixel;
              	      _CGINFO.visualid = _CINFO.visualid;
		      _CGINFO.killid = getpid();
              	      _CGINFO.red_max = _CVISUAL->red_mask;
              	      _CGINFO.red_mult = 1;
              	      while ( !(_CGINFO.red_max & 0x01) ) {
                	      _CGINFO.red_max >>= 1;
                	      _CGINFO.red_mult <<= 1;
              	      }  
              	      _CGINFO.green_max = _CVISUAL->green_mask;
              	      _CGINFO.green_mult = 1;
               	      while ( !(_CGINFO.green_max & 0x01) ) {
                	      _CGINFO.green_max >>= 1;
                	      _CGINFO.green_mult <<= 1;
              	      }  
              	      _CGINFO.blue_max = _CVISUAL->blue_mask;
              	      _CGINFO.blue_mult = 1;
              	      while ( !(_CGINFO.blue_max & 0x01) ) {
                	      _CGINFO.blue_max >>= 1;
                	      _CGINFO.blue_mult <<= 1;
              	      }
              	      _CGINFO.red_mult |= _CGINFO.green_mult | _CGINFO.blue_mult;
                      _CGINFO.green_max = 0;
                      _CGINFO.green_mult = 1;
                      _CGINFO.blue_max = 0;
                      _CGINFO.blue_mult = 1;
              	      status = XmuCreateColormap(_CDISPLAY,&_CGINFO);
		    }
	    }
	    break ;
      }
      break ;

    case Xw_TOV_STATICCOLOR :
      switch (mode) {

	case Xw_TOM_SIMPLERAMP :
	case Xw_TOM_READONLY :
	    pcolormap->maxcolor = pcolormap->maxucolor ;
	    pcolormap->highpixel = _CINFO.base_pixel+1 ;
	    switch (pcolormap->mapping) {
		case Xw_TOM_READONLY :
		case Xw_TOM_BESTRAMP :
		case Xw_TOM_HARDRAMP :
		case Xw_TOM_COLORCUBE :
		    break ;
		default :
        	    for( i=0 ; i<MAXCOLOR ; i++ ) {
	    	 	pcolormap->define[i] = FREECOLOR ;
	    	 	pcolormap->pixels[i] = 0 ;
        	    }
	    }
	    break ;
	case Xw_TOM_BESTRAMP :
	    pcolormap->maxcolor = pcolormap->maxucolor ;
	    pcolormap->highpixel = _CINFO.base_pixel+1 ;
	    switch (pcolormap->mapping) {
		case Xw_TOM_READONLY :
		case Xw_TOM_SIMPLERAMP :
		case Xw_TOM_HARDRAMP :
		case Xw_TOM_COLORCUBE :
		    break ;
		default :
        	    for( i=0 ; i<MAXCOLOR ; i++ ) {
	    	 	pcolormap->define[i] = FREECOLOR ;
	    	 	pcolormap->pixels[i] = 0 ;
        	    }
	    }
	    break ;
	case Xw_TOM_HARDRAMP :
	    pcolormap->maxcolor = pcolormap->maxucolor ;
	    pcolormap->highpixel = _CINFO.base_pixel+1 ;
	    switch (pcolormap->mapping) {
		case Xw_TOM_READONLY :
		case Xw_TOM_SIMPLERAMP :
		case Xw_TOM_BESTRAMP :
		case Xw_TOM_COLORCUBE :
		    break ;
		default :
        	    for( i=0 ; i<MAXCOLOR ; i++ ) {
	    	 	pcolormap->define[i] = FREECOLOR ;
	    	 	pcolormap->pixels[i] = 0 ;
        	    }
	    }
	    break ;
	case Xw_TOM_COLORCUBE :
	    pcolormap->maxcolor = pcolormap->maxucolor ;
	    pcolormap->highpixel = _CINFO.base_pixel+1 ;
	    switch (pcolormap->mapping) {
		case Xw_TOM_HARDRAMP :
		    for( i=0 ; i<pcolormap->maxucolor ; i++ ) {
			pcolormap->define[i] = define[_CINFO.base_pixel+i] ;
		        pcolormap->pixels[i] = pixels[_CINFO.base_pixel+i] ;
		    }
		    break ;
		case Xw_TOM_SIMPLERAMP :
		case Xw_TOM_READONLY :
		case Xw_TOM_BESTRAMP :
		default :
        	    for( i=0 ; i<MAXCOLOR ; i++ ) {
	    	 	pcolormap->define[i] = FREECOLOR ;
	    	 	pcolormap->pixels[i] = 0 ;
        	    }
		    color.flags = DoRed | DoGreen | DoBlue ;
            	    size = (int)(pow((double)pcolormap->maxucolor,
						(double)1./3.)+0.1);
		    for( ; size > 0 ; --size ) {
		        nccolor = size*size*size ;
		        if( (nccolor <= pcolormap->maxucolor) && (nccolor <= MaxColorCube) ) break ;
		    }
            	    _CINFO.red_max = size-1 ; _CINFO.red_mult = 1 ;
            	    _CINFO.green_max = size-1 ; _CINFO.green_mult = size ;
            	    _CINFO.blue_max = size-1 ; _CINFO.blue_mult = size*size ;
            	    for( iu=n=0 ; iu<=_CINFO.blue_max ; iu++ ) {
                      blue = (_CINFO.blue_max > 0) ?
				(0xFFFF*iu)/_CINFO.blue_max : 0xFFFF ;
                      for( ju=0 ; ju<=_CINFO.green_max ; ju++ ) {
                        green = (_CINFO.green_max > 0) ?
				(0xFFFF*ju)/_CINFO.green_max : 0xFFFF ;
                        for( ku=0 ; ku<=_CINFO.red_max ; ku++,n++ ) {
                          red = (_CINFO.red_max > 0) ?
				(0xFFFF*ku)/_CINFO.red_max : 0xFFFF ;
			  color.red = red ;
			  color.green = green ;
			  color.blue = blue ;
			  XAllocColor(_CDISPLAY,_CINFO.colormap,&color) ;
                          pcolormap->define[n] = SYSTEMCOLOR ;
                          pcolormap->pixels[n] = color.pixel ;
                        }
                      }
		    }
	    }
	    break ;
      }
      break ;

    case Xw_TOV_PSEUDOCOLOR :
      switch (mode) {

	case Xw_TOM_SIMPLERAMP :
	case Xw_TOM_READONLY :
	    pcolormap->maxcolor = pcolormap->maxucolor ;
	    pcolormap->highpixel = _CINFO.base_pixel+1 ;
	    switch (pcolormap->mapping) {
		case Xw_TOM_BESTRAMP :
		    for( i=j=0 ; i<pcolormap->maxucolor ; i++,j += 2 ) {
			if( j >= pcolormap->maxucolor ) j = 1 ;
			pcolormap->define[j] = define[i] ;
		        pcolormap->pixels[j] = pixels[i] ;
		    }
		    break ;
		case Xw_TOM_HARDRAMP :
		    for( i=0 ; i<pcolormap->maxucolor ; i++ ) {
			pcolormap->define[i] = define[_CINFO.base_pixel+i] ;
		        pcolormap->pixels[i] = pixels[_CINFO.base_pixel+i] ;
		    }
		    break ;
		case Xw_TOM_COLORCUBE :
		case Xw_TOM_READONLY :
		    break ;
		default :
    	    	    for( i=0 ; i<pcolormap->maxucolor ; i++ ) {
		        pcolormap->pixels[i] = _CINFO.base_pixel + i ;
		        pcolormap->define[i] = FREECOLOR ;
	            }
	    }
#ifdef GG250997
	    if( mode == Xw_TOM_READONLY ) {
              if( !DefaultAtom ) DefaultAtom = XInternAtom(_CDISPLAY,
                                                XW_RGB_BEST_MAP,True) ;
              if( DefaultAtom ) {
                status = XGetRGBColormaps(_CDISPLAY,_CROOT,
			                  &pinfo,&nmap,DefaultAtom);
              } else status = False ;

	      if( status ) {
		status = False;
                for( i=0 ; i<nmap ; i++ ) {
                  if( pinfo[i].visualid == _CINFO.visualid ) {
		    cinfo = &pinfo[i];
                  }
		}
		if( cinfo ) {
//		  XColor color;
		  int error,gravity;
//		  unsigned long pixel;
		  color.red = color.green = color.blue = 0xFFFF;
#ifdef TEST
		  char *serror;
		  Xw_print_error();
		  if( !Xw_get_trace() ) Xw_set_synchronize(_CDISPLAY,True) ;
              	  status = XAllocColor(_CDISPLAY,cinfo->colormap,&color);
		  if( !Xw_get_trace() ) Xw_set_synchronize(_CDISPLAY,False) ;
              	  serror = Xw_get_error(&error,&gravity);
              	  if( status && (error < 1000) ) {
#else
              	  if( XAllocColor(_CDISPLAY,cinfo->colormap,&color) ) {
#endif
                    if( _CINFO.colormap != cinfo->colormap ) {
                      if( _CINFO.colormap && (_CINFO.colormap != _CCOLORMAP) ) {
                        XFreeColormap(_CDISPLAY,_CINFO.colormap) ;
                      }
		    }
                    _CINFO.colormap = cinfo->colormap;
                    _CINFO.red_max = cinfo->red_max;
                    _CINFO.red_mult = cinfo->red_mult;
                    _CINFO.green_max = cinfo->green_max;
                    _CINFO.green_mult = cinfo->green_mult;
                    _CINFO.blue_max = cinfo->blue_max;
                    _CINFO.blue_mult = cinfo->blue_mult;
                    _CINFO.base_pixel = cinfo->base_pixel;
                    _CINFO.visualid = cinfo->visualid;
                    _CINFO.killid = cinfo->killid ;
		    status = True;
		  } else {
		    if( Xw_get_trace() )
		        printf(" Xw_set_colormap_mapping.BAD registered COLORMAP 0x%lx\n",cinfo->colormap);
#ifdef TEST
		    status = False;
		    Xw_print_error();
#endif
		  }
		}
	      }

	      if( !status ) {
                if( !pinfo ) {
                  nmap = 1;
                  pinfo = (XStandardColormap*)
                                malloc(sizeof(XStandardColormap));
                  cinfo = &pinfo[0];
                } else if( !cinfo ) {
                  nmap++;
                  pinfo = (XStandardColormap*) realloc((char*)pinfo,
                                        nmap*sizeof(XStandardColormap));
                  cinfo = &pinfo[nmap-1];
                }
                cinfo->colormap = _CINFO.colormap;
                cinfo->visualid = _CINFO.visualid;
                cinfo->killid = getpid();
                cinfo->base_pixel = _CINFO.base_pixel;
                cinfo->red_max = _CINFO.red_max;
                cinfo->green_max = _CINFO.green_max;
                cinfo->blue_max = _CINFO.blue_max;
                cinfo->red_mult = _CINFO.red_mult;
                cinfo->green_mult = _CINFO.green_mult;
                cinfo->blue_mult = _CINFO.blue_mult;

	      	if( !DefaultAtom ) DefaultAtom = XInternAtom(_CDISPLAY,
						XW_RGB_BEST_MAP,False) ;
                XSetRGBColormaps(_CDISPLAY,_CROOT,pinfo,nmap,DefaultAtom) ;
	      }
	      if( pinfo ) XFree(pinfo); 
              XInstallColormap(_CDISPLAY,_CINFO.colormap);
	    }
#endif
	    break ;

	case Xw_TOM_BESTRAMP :
	    pcolormap->maxcolor = pcolormap->maxucolor ;
	    pcolormap->highpixel = _CINFO.base_pixel+1 ;
	    switch (pcolormap->mapping) {
		case Xw_TOM_SIMPLERAMP :
		    for( i=j=0 ; i<pcolormap->maxucolor ; i++,j += 2 ) {
			if( j >= pcolormap->maxucolor ) j = 1 ;
			pcolormap->define[i] = define[j] ;
			pcolormap->pixels[i] = pixels[j] ;
		    }
		    break ;
		case Xw_TOM_HARDRAMP :
		    for( i=j=0 ; i<pcolormap->maxucolor ; i++,j += 2 ) {
			if( j >= pcolormap->maxucolor ) j = 1 ;
			pcolormap->define[i] = define[_CINFO.base_pixel+j] ;
			pcolormap->pixels[i] = pixels[_CINFO.base_pixel+j] ;
		    }
		    break ;
		case Xw_TOM_COLORCUBE :
		case Xw_TOM_READONLY :
		    break ;
		default :
    	    	    for( i=j=0 ; i<pcolormap->maxucolor ; i++,j += 2 ) {
		        if( j >= pcolormap->maxucolor ) j = 1 ;
		        pcolormap->pixels[i] = _CINFO.base_pixel + j ;
		        pcolormap->define[i] = FREECOLOR ;
	            }
	    }
	    break ;

	case Xw_TOM_HARDRAMP :
	    pcolormap->maxcolor = pcolormap->maxhcolor ;
	    pcolormap->highpixel = _CINFO.base_pixel+1 ;
	    switch (pcolormap->mapping) {
		case Xw_TOM_COLORCUBE :
		case Xw_TOM_READONLY :
		case Xw_TOM_SIMPLERAMP :
		    for( i=0 ; i<pcolormap->maxucolor ; i++ ) {
			pcolormap->define[_CINFO.base_pixel+i] = define[i] ;
			pcolormap->pixels[_CINFO.base_pixel+i] = pixels[i] ;
		    }
		    for( iu=0 ; iu<_CINFO.base_pixel ; iu++ ) {
			pcolormap->define[iu] = SYSTEMCOLOR ;
			pcolormap->pixels[iu] = iu ;
		    }
		    for( i=_CINFO.base_pixel+pcolormap->maxucolor ; 
					i<pcolormap->maxhcolor ; i++ ) {
			pcolormap->define[i] = SYSTEMCOLOR ;
			pcolormap->pixels[i] = i ;
		    }
		    break ;
		case Xw_TOM_BESTRAMP :
		    for( i=j=0 ; i<pcolormap->maxucolor ; i++,j += 2 ) {
			if( j >= pcolormap->maxucolor ) j = 1 ;
			pcolormap->define[_CINFO.base_pixel+j] = define[i] ;
			pcolormap->pixels[_CINFO.base_pixel+j] = pixels[i] ;
		    }
		    for( iu=0 ; iu<_CINFO.base_pixel ; iu++ ) {
			pcolormap->define[iu] = SYSTEMCOLOR ;
			pcolormap->pixels[iu] = iu ;
		    }
		    for( i=_CINFO.base_pixel+pcolormap->maxucolor ; 
					i<pcolormap->maxhcolor ; i++ ) {
			pcolormap->define[i] = SYSTEMCOLOR ;
			pcolormap->pixels[i] = i ;
		    }
		    break ;
		default :
    	    	    for( i=0 ; i<pcolormap->maxucolor ; i++ ) {
		        pcolormap->pixels[i] = i ;
		        pcolormap->define[i] = SYSTEMCOLOR ;
	            }
	    }
	    break ;

	case Xw_TOM_COLORCUBE :
	    pcolormap->maxcolor = pcolormap->maxucolor ;
	    switch (pcolormap->mapping) {
		case Xw_TOM_HARDRAMP :
		    for( i=0 ; i<pcolormap->maxucolor ; i++ ) {
			pcolormap->define[i] = define[_CINFO.base_pixel+i] ;
		        pcolormap->pixels[i] = pixels[_CINFO.base_pixel+i] ;
		    }
		    break ;
		case Xw_TOM_SIMPLERAMP :
		case Xw_TOM_READONLY :
		case Xw_TOM_BESTRAMP :
		default :
    	    	    for( i=0 ; i<pcolormap->maxucolor ; i++ ) {
		        pcolormap->pixels[i] = _CINFO.base_pixel + i ;
		        pcolormap->define[i] = FREECOLOR ;
	            }

                    if( SetColorCube != 'Y' ) status = False ;
		    else status = True ;

		    ngcolor = 0;
		    cstatus = gstatus = False;
                    if( status ) {

	      	      if( !DefaultAtom ) DefaultAtom = XInternAtom(_CDISPLAY,
						XW_RGB_BEST_MAP,True) ;
                      if( DefaultAtom ) { 
		        status = XGetRGBColormaps(_CDISPLAY,_CROOT,
                                                &pinfo,&nmap,DefaultAtom);
  		      } else status = False ;

		      if( status ) {
			XColor color1,color2 ;
			for( i=0 ; i<nmap ; i++ ) {
			  if( pinfo[i].visualid == _CINFO.visualid ) {
			    if( (pinfo[i].red_max > 0) &&
			          (pinfo[i].green_max > 0) &&
			            (pinfo[i].blue_max > 0) ) cinfo = &pinfo[i]; 
			    else
			      if( (pinfo[i].red_max > 0) &&
			          (pinfo[i].green_max == 0) &&
			            (pinfo[i].blue_max == 0) ) cginfo = &pinfo[i]; 
			  }
			}

			if( cinfo && cginfo ) {
			  nccolor = (cinfo->red_max+1)*
					   (cinfo->green_max+1)*
						   (cinfo->blue_max+1) ;
			  ngcolor = cginfo->red_max+1;
			  ncolor = nccolor + ngcolor;
            	          size = (int)(pow((double)pcolormap->maxucolor,(double)1./3.));
			  if( (ncolor <= pcolormap->maxhcolor) &&
						(ncolor >= size*size*size) ) {
			    Colormap colormap = _CINFO.colormap;
			    unsigned long base_pixel = _CINFO.base_pixel;
			    color1.pixel = cinfo->base_pixel ;
			    XQueryColor(_CDISPLAY,cinfo->colormap,&color1) ;
			    color2.pixel = cinfo->base_pixel + nccolor - 1 ;
			    XQueryColor(_CDISPLAY,cinfo->colormap,&color2) ;
			    if( !color1.red && !color1.green && !color1.blue && 
			        color2.red > 0xFE00 && 
					color2.green > 0xFE00 &&
						color2.blue > 0xFE00 ) {
				_CINFO.colormap = cinfo->colormap;
				_CINFO.red_max = cinfo->red_max;
				_CINFO.red_mult = cinfo->red_mult;
				_CINFO.green_max = cinfo->green_max;
				_CINFO.green_mult = cinfo->green_mult;
				_CINFO.blue_max = cinfo->blue_max;
				_CINFO.blue_mult = cinfo->blue_mult;
				_CINFO.base_pixel = cinfo->base_pixel;
				_CINFO.visualid = cinfo->visualid;
				_CINFO.killid = cinfo->killid ;
				cstatus = True;
			    }

			    if( cstatus && (ngcolor > 0)  && 
				(cinfo->colormap == cginfo->colormap) &&
					(cginfo->base_pixel == (cinfo->base_pixel + nccolor)) ) {
			      color1.pixel = cginfo->base_pixel ;
			      XQueryColor(_CDISPLAY,cginfo->colormap,&color1) ;
			      color2.pixel = cginfo->base_pixel + ngcolor - 1 ;
			      XQueryColor(_CDISPLAY,cginfo->colormap,&color2) ;
			      if( !color1.red && !color1.green && !color1.blue && 
			        color2.red > 0xFE00 && 
					color2.green > 0xFE00 &&
						color2.blue > 0xFE00 ) {
				_CGINFO.colormap = cginfo->colormap;
				_CGINFO.red_max = cginfo->red_max;
				_CGINFO.red_mult = cginfo->red_mult;
				_CGINFO.green_max = cginfo->green_max;
				_CGINFO.green_mult = cginfo->green_mult;
				_CGINFO.blue_max = cginfo->blue_max;
				_CGINFO.blue_mult = cginfo->blue_mult;
				_CGINFO.base_pixel = cginfo->base_pixel;
				_CGINFO.visualid = cginfo->visualid;
				_CGINFO.killid = cginfo->killid ;
				gstatus = True ;
						/* FREE out of range colors */
			        if( colormap != cinfo->colormap ) {
				  if( colormap && (colormap != _CCOLORMAP) ) {
			            XFreeColormap(_CDISPLAY,colormap) ;
				  }
				} else {
				  unsigned long lpixel = cginfo->base_pixel+ngcolor-1;
				  for( i=0 ; i<pcolormap->maxucolor ; i++ ) {
				    pixel = base_pixel + i;
				    if( (pixel < cinfo->base_pixel) ||
						(pixel > lpixel) ) {
	        	              XFreeColors(_CDISPLAY,colormap,&pixel,1,0);
				    }
				  }
				  XFlush(_CDISPLAY);
				}
			      }
			    }
			  }
			}
		      }
                    }
                    if( !cstatus ) {		/* Allocate READ-ONLY color-cube */
//		      Atom RgbDefaultAtom = 0 ;
            	      size = (int)(pow((double)MaxColorCube,(double)1./3.)+0.1);

		      if( pcolormap->maxucolor > 0 ) {
	                XFreeColors(_CDISPLAY,_CINFO.colormap,
					pcolormap->pixels,pcolormap->maxucolor,0);
			pcolormap->maxucolor = 0;
		      }
	    	      _CINFO.visualid = _CVISUAL->visualid ;
		      _CINFO.killid = getpid();
		      for( ; size>0 ; --size ) {
                	_CINFO.base_pixel = 0;
                	_CINFO.red_max = size-1;
                	_CINFO.green_max = size-1;
                	_CINFO.blue_max = size-1;
                	_CINFO.red_mult = 1;
                	_CINFO.green_mult = size;
                	_CINFO.blue_mult = size*size;
                	if( XmuCreateColormap(_CDISPLAY,&_CINFO) ) break;
              	      }  
		      nccolor = size*size*size ;
		    }

                    if( !gstatus ) {		/* Allocate READ-ONLY gray-ramp */
//		      Atom GrayDefaultAtom = 0 ;
		      if( pcolormap->maxucolor > 0 ) {
	                XFreeColors(_CDISPLAY,_CINFO.colormap,
					pcolormap->pixels,pcolormap->maxucolor,0);
			pcolormap->maxucolor = 0;
		      }
		      ngcolor = MaxGray ;
		      _CGINFO.colormap = _CINFO.colormap;
	    	      _CGINFO.visualid = _CVISUAL->visualid ;
		      _CGINFO.killid = getpid();
		      size = ngcolor;
		      for( ; size>0 ; size -= 2 ) {
                        _CGINFO.base_pixel = 0;
                	_CGINFO.red_max = size-1;
                	_CGINFO.green_max = 0;
                	_CGINFO.blue_max = 0;
                	_CGINFO.red_mult = 1;
                	_CGINFO.green_mult = 1;
                	_CGINFO.blue_mult = 1;
                	if( XmuCreateColormap(_CDISPLAY,&_CGINFO) ) break;
              	      }  
		      ngcolor = size ;
		    }
		    XInstallColormap(_CDISPLAY,_CINFO.colormap);
		    ncolor = nccolor + ngcolor;
		    pcolormap->maxucolor = ncolor;
		    pcolormap->maxcolor = ncolor;
		    for( i=0 ; i<ncolor ; i++ ) {
                      pcolormap->define[i] = SYSTEMCOLOR ;
		      pcolormap->pixels[i] = _CINFO.base_pixel + i;
		    }
                    if( !cstatus || !gstatus ) {
		      if( !pinfo ) {
			nmap = 2;
		        pinfo = (XStandardColormap*) 
				malloc(2*sizeof(XStandardColormap));
			cinfo = &pinfo[0];
			cginfo = &pinfo[1];
		      } else if( !cinfo || !cginfo ) {
			if( !cinfo ) nmap++;
			if( !cginfo ) nmap++;
		        pinfo = (XStandardColormap*) realloc((char*)pinfo,
                                	nmap*sizeof(XStandardColormap));
			i = nmap-1;
			if( !cginfo ) {
			  cginfo = &pinfo[i]; --i;
			}
			if( !cinfo ) {
			  cinfo = &pinfo[i]; --i;
			}
		      }
		      cinfo->colormap = _CINFO.colormap;
	    	      cinfo->visualid = _CINFO.visualid;
		      cinfo->killid = getpid();
                      cinfo->base_pixel = _CINFO.base_pixel;
                      cinfo->red_max = _CINFO.red_max;
                      cinfo->green_max = _CINFO.green_max;
                      cinfo->blue_max = _CINFO.blue_max;
                      cinfo->red_mult = _CINFO.red_mult;
                      cinfo->green_mult = _CINFO.green_mult;
                      cinfo->blue_mult = _CINFO.blue_mult;

		      cginfo->colormap = _CGINFO.colormap;
	    	      cginfo->visualid = _CGINFO.visualid;
		      cginfo->killid = getpid();
                      cginfo->base_pixel = _CGINFO.base_pixel;
                      cginfo->red_max = _CGINFO.red_max;
                      cginfo->green_max = _CGINFO.green_max;
                      cginfo->blue_max = _CGINFO.blue_max;
                      cginfo->red_mult = _CGINFO.red_mult;
                      cginfo->green_mult = _CGINFO.green_mult;
                      cginfo->blue_mult = _CGINFO.blue_mult;

		      switch (SetColorCube) {
			    case 'N' :
#ifdef TRACE_SET_COLORMAP_MAPPING
				if( Xw_get_trace() ) printf(
				    " Xw_SET_COLOR_CUBE(%ld,%ld,%ld,%ld),(%ld,%ld,%ld,%ld)\n",
					_CINFO.base_pixel,_CINFO.red_max,
					_CINFO.green_max,_CINFO.blue_max,
					_CGINFO.base_pixel,_CGINFO.red_max,
					_CGINFO.green_max,_CGINFO.blue_max) ;
#endif
				break ;
			    case 'Y' :
	      	        	if( !DefaultAtom ) 
					DefaultAtom = XInternAtom(_CDISPLAY,
						XW_RGB_BEST_MAP,False) ;
                                XSetRGBColormaps(_CDISPLAY,_CROOT,
                                                   pinfo,nmap,DefaultAtom) ;
#ifdef TRACE_SET_COLORMAP_MAPPING
				if( Xw_get_trace() ) printf(
				    " Xw_SET_STANDARD_COLOR_CUBE(%ld,%ld,%ld,%ld),(%ld,%ld,%ld,%ld)\n",
					_CINFO.base_pixel,_CINFO.red_max,
					_CINFO.green_max,_CINFO.blue_max,
					_CGINFO.base_pixel,_CGINFO.red_max,
					_CGINFO.green_max,_CGINFO.blue_max) ;
#endif
				break ;
			    case 'T' :
	      	        	if( !DefaultAtom ) 
					DefaultAtom = XInternAtom(_CDISPLAY,
						XW_RGB_BEST_MAP,False) ;
                                XSetRGBColormaps(_CDISPLAY,_CROOT,
                                                   pinfo,nmap,DefaultAtom) ;
                        	XSetCloseDownMode(_CDISPLAY,RetainTemporary) ;
#ifdef TRACE_SET_COLORMAP_MAPPING
				if( Xw_get_trace() ) printf(
				    " Xw_SET_TEMPORARY_COLOR_CUBE(%ld,%ld,%ld,%ld),(%ld,%ld,%ld,%ld)\n",
					_CINFO.base_pixel,_CINFO.red_max,
					_CINFO.green_max,_CINFO.blue_max,
					_CGINFO.base_pixel,_CGINFO.red_max,
					_CGINFO.green_max,_CGINFO.blue_max) ;
#endif
				break ;
			    case 'P' :
	      	        	if( !DefaultAtom ) 
					DefaultAtom = XInternAtom(_CDISPLAY,
						XW_RGB_BEST_MAP,False) ;
                                XSetRGBColormaps(_CDISPLAY,_CROOT,
                                                   pinfo,nmap,DefaultAtom) ;
                        	XSetCloseDownMode(_CDISPLAY,RetainPermanent) ;
				if( Xw_get_trace() ) printf(
				    " Xw_SET_PERMANENT_COLOR_CUBE(%ld,%ld,%ld,%ld),(%ld,%ld,%ld,%ld)\n",
					_CINFO.base_pixel,_CINFO.red_max,
					_CINFO.green_max,_CINFO.blue_max,
					_CGINFO.base_pixel,_CGINFO.red_max,
					_CGINFO.green_max,_CGINFO.blue_max) ;
			}
	    	    }
    	    }
	    if( pinfo ) XFree(pinfo); 
	    pcolormap->highpixel = pcolormap->pixels[pcolormap->maxcolor-1] ;
      }
    }

    pcolormap->mapping = mode ;

    XFlush(_CDISPLAY) ;

#ifdef TRACE_SET_COLORMAP_MAPPING
if( Xw_get_trace() ) {
    printf(" Xw_set_color_mapping(%lx,%d)\n",(long ) pcolormap,mode) ;
}
#endif

  return (XW_SUCCESS) ;
}

#ifdef XW_PROTOTYPE
XVisualInfo* Xw_get_visual_info(XW_EXT_DISPLAY* pdisplay,Xw_TypeOfVisual pclass)
#else
XVisualInfo* Xw_get_visual_info(pdisplay,pclass)
XW_EXT_DISPLAY *pdisplay ;
Xw_TypeOfVisual pclass ;
#endif /*XW_PROTOTYPE*/
/*
        Return the visual Information matching with the specified class
        or NULL if class is not founded on this Display .
*/
{
Xw_TypeOfVisual class1 = pclass,class2 = Xw_TOV_DEFAULT ;
XVisualInfo *vinfo = NULL,*ginfo = NULL,info1,info2 ;
int i,ninfo;
long mask1 = 0,mask2 = 0;
unsigned long backpixel;

        switch (class1) {
            case Xw_TOV_DEFAULT :
                class1 = (Xw_TypeOfVisual) _DCLASS ;
                break ;
            case Xw_TOV_PSEUDOCOLOR :
                class1 = Xw_TOV_PSEUDOCOLOR ;
		mask1 |= VisualColormapSizeMask;
		info1.colormap_size = 256;
                break ;
            case Xw_TOV_PREFERRED_PSEUDOCOLOR :
                if( MaxCmapsOfScreen(_DSCREEN) > 1 ) {
                    class1 = Xw_TOV_PSEUDOCOLOR ;
		    mask1 |= VisualColormapSizeMask;
		    info1.colormap_size = 256;
                    class2 = Xw_TOV_TRUECOLOR ;
                } else {
                    class1 = (Xw_TypeOfVisual) _DCLASS ;
                }
                break ;
            case Xw_TOV_TRUECOLOR :
                class1 = Xw_TOV_TRUECOLOR ;
                break ;
            case Xw_TOV_OVERLAY :
	        ginfo = Xw_get_overlay_visual_info(pdisplay,
				Xw_TOV_PSEUDOCOLOR,&backpixel);
		return ginfo;
            case Xw_TOV_PREFERRED_TRUECOLOR :
#ifdef OLD
                if( MaxCmapsOfScreen(_DSCREEN) > 1 ) {
                    class1 = Xw_TOV_TRUECOLOR ;
                    class2 = Xw_TOV_PSEUDOCOLOR ;
		    mask2 |= VisualColormapSizeMask;
		    info2.colormap_size = 256;
                } else {
                    class1 = _DCLASS ;
                }
#else
                class1 = Xw_TOV_TRUECOLOR ;
                class2 = Xw_TOV_PSEUDOCOLOR ;
		mask2 |= VisualColormapSizeMask;
		info2.colormap_size = 256;
#endif
                break ;
            case Xw_TOV_PREFERRED_OVERLAY :
                class1 = Xw_TOV_PSEUDOCOLOR ;
		mask1 |= VisualColormapSizeMask;
		info1.colormap_size = 256;
	        ginfo = Xw_get_overlay_visual_info(pdisplay,
					Xw_TOV_PSEUDOCOLOR,&backpixel);
		if( ginfo ) return ginfo;
                break ;
	      default:
                break ;
        }

	if( !ginfo ) {
#if (!defined (CTS17988))  && defined(HAVE_X11_EXTENSIONS_MULTIBUF_H)
	  XmbufBufferInfo *pmono = NULL,*pstereo = NULL;
	  int nmono=0,nstereo=0;
#endif

#if (!defined (CTS17988))  && defined(HAVE_X11_EXTENSIONS_MULTIBUF_H) 
          XmbufGetScreenInfo(_DDISPLAY, _DROOT, &nmono, &pmono,
						  &nstereo, &pstereo);
#endif
#if defined(__cplusplus) || defined(c_plusplus)
          info1.c_class = class1 ;
#else
          info1.class = class1 ;
#endif
	  info1.screen = DefaultScreen(_DDISPLAY) ;
          mask1 |= VisualClassMask | VisualScreenMask ;
          vinfo = XGetVisualInfo(_DDISPLAY,mask1,&info1,&ninfo) ;

          if( ninfo ) {
            ginfo = vinfo ;
            for( i=1 ; i<ninfo ; i++ ) {
              if( (vinfo[i].depth > ginfo->depth) ||
                        (vinfo[i].visualid == _DVISUAL->visualid) ) {
#if (!defined (CTS17988))  && defined(HAVE_X11_EXTENSIONS_MULTIBUF_H) 
		if( pmono ) {
		  for( j=0 ; j<nmono ; j++ ) {
		    if( (pmono[j].visualid == vinfo[i].visualid) &&
				((pmono[j].max_buffers == 0) || 
					(pmono[j].max_buffers > 1)) ) {
	      	      ginfo = &vinfo[i];
		      break;
		    }
		  }
		} else
#endif
		  ginfo = &vinfo[i];
	      }
            }
            if( (ginfo->depth < TrueColorMinDepth) && 
				(class1 == Xw_TOV_TRUECOLOR) ) {
		XFree((char*)vinfo) ;
	        ginfo = vinfo = NULL ;
	    }
          }

          if( !ginfo && (class2 != Xw_TOV_DEFAULT) ) {
#if defined(__cplusplus) || defined(c_plusplus)
            info2.c_class = class2 ;
#else
            info2.class = class2 ;
#endif
	    info2.screen = DefaultScreen(_DDISPLAY) ;
            mask2 |= VisualClassMask | VisualScreenMask ;
            vinfo = XGetVisualInfo(_DDISPLAY,mask2,&info2,&ninfo) ;
            if( ninfo ) {
                ginfo = vinfo ;
                for( i=1 ; i<ninfo ; i++ ) {
                  if( vinfo[i].depth > ginfo->depth ) {
#if (!defined (CTS17988))  && defined(HAVE_X11_EXTENSIONS_MULTIBUF_H)
		    if( pmono ) {
		      for( j=0 ; j<nmono ; j++ ) {
		        if( (pmono[j].visualid == vinfo[i].visualid) &&
				((pmono[j].max_buffers == 0) || 
					(pmono[j].max_buffers > 1)) ) {
	      	          ginfo = &vinfo[i];
		          break;
		        }
		      }
		    } else 
#endif
		      ginfo = &vinfo[i];
		  }
                }
                if( (ginfo->depth < TrueColorMinDepth) && 
				(class2 == Xw_TOV_TRUECOLOR) ) {
		    XFree((char*)vinfo) ;
	            ginfo = vinfo = NULL ;
		}
            }
          }
#if (!defined (CTS17988))  && defined(HAVE_X11_EXTENSIONS_MULTIBUF_H)
	  if( pmono ) XFree(pmono);
	  if( pstereo ) XFree(pstereo);
#endif
        }

 	if( ginfo ) {	
          info1.visualid = ginfo->visualid ;
          info1.screen = ginfo->screen ;
          mask1 = VisualIDMask | VisualScreenMask ;

          ginfo = XGetVisualInfo(_DDISPLAY,mask1,&info1,&ninfo) ;
	} else {
          /*ERROR*Unmatchable Visual class*/
          Xw_set_error(67,"Xw_get_visual_info",&pclass) ;
	}

	if( vinfo ) XFree((char*)vinfo) ;
        return (ginfo) ;
}

#ifdef XW_PROTOTYPE
XVisualInfo* Xw_get_overlay_visual_info(XW_EXT_DISPLAY* pdisplay,Xw_TypeOfVisual /*pclass*/,unsigned long *backpixel)
#else
XVisualInfo* Xw_get_overlay_visual_info(pdisplay,pclass,backpixel)
XW_EXT_DISPLAY *pdisplay ;
Xw_TypeOfVisual pclass ;
unsigned long *backpixel;
#endif /*XW_PROTOTYPE*/
/*
        Return the Overlay visual Information matching with the specified class
        or NULL if the overlay class is not founded on this Display .
*/
{
XVisualInfo *vinfo = NULL,*ginfo = NULL,tinfo ;
int ninfo;
Atom oatom = XInternAtom(_DDISPLAY, "SERVER_OVERLAY_VISUALS", True);

        *backpixel = 0;
	if( oatom ) {
	  unsigned long nitem,left;
	  int aformat;
	  Status status;
	  union {
  	    unsigned char *string;
  	    XOverlayVisualInfo *oinfo;
	  } prop;
	  Atom aatom;
    	  status = XGetWindowProperty(_DDISPLAY, _DROOT,
                                oatom, 0,  100, False,
                                AnyPropertyType, &aatom, &aformat,
                                &nitem, &left, &prop.string);
	  nitem /= 4;
          if( (status == Success) && (aformat == 32) && (nitem > 0) ) {
	    int i,depth = OverlayColorMinDepth,layer = 1;
    	    for( i=0 ; (unsigned int ) i < nitem ; i++ ) {
	      if( prop.oinfo[i].layer < layer || 
			prop.oinfo[i].transparent_type == 0 ) continue;
      	      tinfo.visualid = prop.oinfo[i].overlay_visual;
      	      vinfo = XGetVisualInfo(_DDISPLAY,VisualIDMask,&tinfo,&ninfo);
      	      if( vinfo &&  (vinfo->depth >= depth) ) { 
	        *backpixel = prop.oinfo[i].value;
	        layer = prop.oinfo[i].layer;
		depth = vinfo->depth;
        	ginfo = vinfo;
	      } else if( vinfo ) {
		XFree(vinfo) ; vinfo = NULL;
	      }
	    }
	  }
#ifdef PURIFY
	  if( prop.string && (status == Success) ) Xw_free(prop.string);
#endif
	} else if( (pdisplay->server == XW_SERVER_IS_SUN) && EnableSunOverlay ) {
	  int mOptCode,fEvent,fError;
	  if( XQueryExtension(_DDISPLAY,"SUN_OVL",&mOptCode,&fEvent,&fError) ) {
            tinfo.visualid = 0x2a;	/* Overlay visual for SUN/ULTRA */
            vinfo = XGetVisualInfo(_DDISPLAY,VisualIDMask,&tinfo,&ninfo);
      	    if( vinfo && (vinfo->colormap_size == 224) ) {
	      ginfo = vinfo;
	      *backpixel = 225;
	    } else if( vinfo ) {
	      XFree(vinfo) ; vinfo = NULL;
	    }
	  }
	}

#ifdef IMP040100	
  	if( ginfo && MaxCmapsOfScreen(_DSCREEN) < 2  && 
		ginfo->visualid != _DVISUAL->visualid && 
#if defined(__cplusplus) || defined(c_plusplus)
			ginfo->c_class == _DCLASS )
#else
			ginfo->class == _DCLASS )
#endif
	{
	  XFree(ginfo); ginfo = NULL;
        }
#endif
	return ginfo;
}
