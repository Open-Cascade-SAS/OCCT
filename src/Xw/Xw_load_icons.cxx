#define NO_PERF01	/*GG_160996
//			Mesure de performances sur l'iconification
*/

#define CTS17035	/*GG_191296
//			Chargement conditionnel des icones
//			Syntaxe:
//			#ifdef code cond value
//			code : DEPTH,CLASS or SCREEN_SIZE
//			cond : > ou >= ou < ou <= ou == ou !=
//			value: valeur entiere ou real fonction du code
//			#else
//			#endif
*/

#define QUANTIFY	/*GG_180297
//			Charger dynamiquement les icones au fur et a mesure
//			de leur utilisation
*/

#define PRO16753    /* GG 261198
//              1) Don't free pname because this pointer is a
//              static address in Xw_get_filename() space.
//              2) Increments nicons value in Xw_load_icons
//              3) Flush pixmap filling (XPutImage) before returning.
*/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Xw_Extension.h>
#ifdef PERF01
# ifdef HAVE_TIME_H
#  include <time.h>
# endif
#endif

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_LOAD_ICONS
#define TRACE_SAVE_ICONS
#define TRACE_CLEAR_ICONS
#define TRACE_SHOW_ICONS
#define TRACE_SAVE_ICONS
#define TRACE_GET_ICON_PIXMAP
#define TRACE_CREATE_ICON_PIXMAP
#define TRACE_GET_ICON_SIZE
#define TRACE_GET_ICON_NAME
#define TRACE_GET_ICON
#define TRACE_PUT_ICON
#define TRACE_PUT_ICON_NAME
#define TRACE_PUT_WINDOW_ICON
#define TRACE_DEL_ICON
#endif

/*
   int Xw_load_icons (awindow,dirname):
   XW_EXT_WINDOW *awindow
   char *dirname	Icon File Directory name

	LOAD Icons File directory 

	NOTE : Icons File must be ".ifd" postfixed file and contains
	       the sets of icons to be loaded in the IconBox
	       Each Icon is a complete filename of type ".xwd" followed
	       bye the icon-name .

	       e.g : 

		Sample.ifd is my Icons File directory and contains :

		chrono.xwd    'chrono'
		$DIRICON/clock.xwd	'montre'
		/usr/users/gg/XWD/toys.xwd	'toys'


	returns  0 if something is failed.
	returns  the number of loaded icons if successfull.

   int Xw_save_icons (awindow):
   XW_EXT_WINDOW *awindow

	SAVE just created Icons in corresponding .xwd files. 

	returns  0 if something is failed.
	returns  the number of saved icons if successfull.

   STATUS Xw_show_icons (awindow):
   XW_EXT_WINDOW *awindow

	SHOW Icons to the Screen

	Returns ERROR if IconBox is Empty
	Returns SUCCESS if successfull

   int Xw_clear_icons (awindow,dirname):
   XW_EXT_WINDOW *awindow
   char *dirname	Icon File Directory name

	CLEAR the corresponding "dirname" Icons in the IconBox 
	      or ALL icons if dirname is NULL
	      or ALL Added icons if dirname is ""

	Returns 0 if nothing has been cleared
	Returns the cleared icons number if successfull


   static XW_EXT_ICON* Xw_put_icon (awindow,iconname):  
   XW_EXT_WINDOW *awindow
   char *iconname	Icon name

	ADD an Icon in the IconBox but DON'T load it.

        Returns Icon Ext Address if Successfull
        Returns NULL if Icon don't exist

   static XW_EXT_ICON* Xw_get_icon (awindow,iconname): 
   XW_EXT_WINDOW *awindow
   char *iconname       Icon name

        GET the Image associated to the Icon in the IconBox

        Returns Icon Ext Address if Successfull
        Returns NULL if Icon don't exist

   STATUS Xw_put_window_icon (awindow,fwindow,iconname,width,height):  
   XW_EXT_WINDOW *awindow
   XW_EXT_WINDOW *fwindow
   char *iconname	Icon name
   int width,height     Icon size

	ADD an Icon of size width,height in the IconBox 
	from the Full Window background fwindow

	Returns ERROR if name is wrong
	Returns SUCCESS if successfull

   Aspect_Handle Xw_get_icon_pixmap (awindow,width,height,iconname):
   XW_EXT_WINDOW *awindow
   int	width,height	Pixmap Size
   char *iconname	Icon name

	GET the Pixmap associated to the Icon in the IconBox

	Returns Pixmap Xid if Successfull
	Returns NULL if Icon don't exist

   Xw_STATUS Xw_get_icon_size (awindow,iconname,width,height):
   XW_EXT_WINDOW *awindow
   char *iconname	Icon name
   int	*width,*height	Returned Pixmap Size

	GET the Pixmap associated to the Icon in the IconBox

	Returns ERROR if Iconname is not Known
	Returns SUCCES if Successfull 

   char* Xw_get_icon_name (awindow,index):
   XW_EXT_WINDOW *awindow
   int	index		Icon Index >1 & <= IconNumber

	GET the Icon Name from the IconBox Index

	Returns IconName if Successfull
	Returns NULL if Index is wrong
*/

#include <string.h>

static char ShowIconBox = 'U' ;

#ifdef CTS17035
#define IFD_CODE_UNKNOWN 0
#define IFD_CODE_DEPTH 1
#define IFD_CODE_CLASS 2
#define IFD_CODE_SCREEN_SIZE 3
static int ifd_code = IFD_CODE_UNKNOWN; 
#define IFD_COND_UNKNOWN 0
#define IFD_COND_EQUAL 1
#define IFD_COND_GREATER 2
#define IFD_COND_LESS 3
#define IFD_COND_GREATER_OR_EQUAL 4
#define IFD_COND_LESS_OR_EQUAL 5
#define IFD_COND_NOT_EQUAL 6
#define IFD_COND_SKIP 7
#define IFD_COND_KEEP 8
static int ifd_cond = IFD_COND_UNKNOWN; 
static int ifd_ivalue = 0;
static float ifd_rvalue = 0.;
static char ifd_svalue[256];
#endif

// Unused :
#ifdef DEB
#ifdef XW_PROTOTYPE
static int Strcmp(char* cs1,char* cs2) /* For optimization */
#else
static int Strcmp(cs1,cs2) /* For optimization */
char *cs1,*cs2;
#endif
{
union {
  int *i;
  char *c;
} u1,u2;
int i,n = max(strlen(cs1),strlen(cs2))/sizeof(int);
  
  u1.c = cs1; u2.c = cs2;

  for( i=0 ; i<n ; i++,u1.i++,u2.i++ ) {
    if( *u1.i != *u2.i ) break;
  }

  if( *u1.i && *u2.i && (i<n) ) {
    for( i=0 ; (unsigned int ) i < sizeof(int) ; i++,u1.c++,u2.c++ ) {
      if( *u1.c != *u2.c ) return True;
      else if( !*u1.c ) return False;
    }
  }
  return False;
}
#endif

#ifdef XW_PROTOTYPE
static XW_EXT_ICON* Xw_put_icon(XW_EXT_WINDOW* awindow,char* iconname);
static XW_EXT_ICON* Xw_get_icon(XW_EXT_WINDOW* awindow,char* iconname);
static XW_STATUS Xw_del_icon(XW_EXT_WINDOW* awindow,char* iconname);
static Aspect_Handle Xw_create_icon_pixmap (XW_EXT_WINDOW* awindow,int width,int height,XW_EXT_IMAGEDATA *pimage);
#else
static XW_EXT_ICON* Xw_put_icon();
static XW_EXT_ICON* Xw_get_icon();
static XW_STATUS Xw_del_icon();
static Aspect_Handle Xw_create_icon_pixmap();
#endif /*XW_PROTOTYPE*/

#ifdef XW_PROTOTYPE
int Xw_load_icons (void *awindow,char *dirname)
#else
int Xw_load_icons (awindow,dirname)
void *awindow;
char *dirname ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_ICON *piconinfo;
int status ;
//Window window ;
FILE *ficon = NULL;
char *pname,filename[80],iconname[80],lf ;
int skip,index,nicons = 0 ;
float r,g,b,wsize,hsize,ssize ;

    	if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_load_icons",pwindow) ;
            return (0) ;
    	}

    	if( Xw_get_env("Xw_SHOW_ICONBOX",ifd_svalue,sizeof(ifd_svalue)) ){
            if( (int)strlen(ifd_svalue) > 0 ) {
                ShowIconBox = ifd_svalue[0] ;
            }
            printf( " Xw_SHOW_ICONBOX is '%c'\n",ShowIconBox) ;
    	}

	status = Xw_get_color_name(_COLORMAP,"Green",&r,&g,&b) ;

	if( status ) {
	    status = Xw_get_color_index(_COLORMAP,r,g,b,&index) ;
	    if( status ) Xw_set_line_attrib(pwindow,index,0,0,XW_REPLACE) ;
	}

	status = Xw_get_color_name(_COLORMAP,"White",&r,&g,&b) ;

	if( status ) {
	    status = Xw_get_color_index(_COLORMAP,r,g,b,&index) ;
	    if( status ) Xw_set_text_attrib(pwindow,index,0,0,XW_REPLACE) ;
	}

	pname = Xw_get_filename(dirname,"ifd") ;

	if( pname ) ficon = fopen(pname,"r") ;

	if( !ficon ) {
	    /*ERROR*Bad ICON File Directory Name */
            Xw_set_error(100,"Xw_load_icons",pname) ;
            return (0) ;
	}

#ifndef PRO16753	
	Xw_free(pname) ;
#endif
					/* READ Icon File Directory */
	while( !feof(ficon) ) {
	    skip = False ;
	    status = fscanf(ficon,"%s%c",filename,&lf) ;
	    if( filename[0] == '#' ) {
#ifdef CTS17035
		char code[80],cond[80],value[80];
		code[0] = cond[0] = value[0] = '\0';
	        if( !strncmp(filename,"#if",3) ) {
	          status = fscanf(ficon,"%s%c",code,&lf) ;
		  if( lf != '\n' ) status = fscanf(ficon,"%s%c",cond,&lf) ;
	          if( lf != '\n' ) status = fscanf(ficon,"%s%c",value,&lf) ;
#ifdef TRACE
		  if( Xw_get_trace() )
		  	printf(" #ifdef '%s' '%s' '%s'\n",code,cond,value);
#endif
		  ifd_code = IFD_CODE_UNKNOWN;
		  ifd_cond = IFD_COND_UNKNOWN;
		  ifd_ivalue = 0;
		  ifd_rvalue = 0.;
		  ifd_svalue[0] = '\0';
		  if( !strcmp(code,"DEPTH") ) {
		    ifd_code = IFD_CODE_DEPTH;
		  } else if( !strcmp(code,"CLASS") ) {
		    ifd_code = IFD_CODE_CLASS;
		  } else if( !strcmp(code,"SCREEN_SIZE") ) {
		    ifd_code = IFD_CODE_SCREEN_SIZE;
		  }
		  if( !strcmp(cond,">") ) {
		    ifd_cond = IFD_COND_GREATER;
		  } else if( !strcmp(cond,">=") ) {
		    ifd_cond = IFD_COND_GREATER_OR_EQUAL;
		  } else if( !strcmp(cond,"<") ) {
		    ifd_cond = IFD_COND_LESS;
		  } else if( !strcmp(cond,"<=") ) {
		    ifd_cond = IFD_COND_LESS_OR_EQUAL;
		  } else if( !strcmp(cond,"==") ) {
		    ifd_cond = IFD_COND_EQUAL;
		  } else if( !strcmp(cond,"!=") ) {
		    ifd_cond = IFD_COND_NOT_EQUAL;
		  }
		  switch (ifd_code) {
		    case IFD_CODE_DEPTH:
		      sscanf(value,"%d",&ifd_ivalue); 
		      switch (ifd_cond) {
		        case IFD_COND_GREATER:
			  if( _DEPTH > ifd_ivalue ) ifd_cond = IFD_COND_KEEP;
			  else                      ifd_cond = IFD_COND_SKIP;
		          break;
			case IFD_COND_GREATER_OR_EQUAL:
			  if( _DEPTH >= ifd_ivalue ) ifd_cond = IFD_COND_KEEP;
			  else                       ifd_cond = IFD_COND_SKIP;
		          break;
			case IFD_COND_LESS:
			  if( _DEPTH < ifd_ivalue ) ifd_cond = IFD_COND_KEEP;
			  else                      ifd_cond = IFD_COND_SKIP;
		          break;
			case IFD_COND_LESS_OR_EQUAL:
			  if( _DEPTH <= ifd_ivalue ) ifd_cond = IFD_COND_KEEP;
			  else                       ifd_cond = IFD_COND_SKIP;
		          break;
			case IFD_COND_EQUAL:
			  if( _DEPTH == ifd_ivalue ) ifd_cond = IFD_COND_KEEP;
			  else                       ifd_cond = IFD_COND_SKIP;
		          break;
			case IFD_COND_NOT_EQUAL:
			  if( _DEPTH != ifd_ivalue ) ifd_cond = IFD_COND_KEEP;
			  else                       ifd_cond = IFD_COND_SKIP;
		          break;
			default:
		          fprintf(stderr,"*Xw_load_icons.Unrecognize key cond '%s'\n",cond);
		          ifd_cond = IFD_COND_SKIP;
		      } 
		      break;
		    case IFD_CODE_CLASS:
		      sscanf(value,"%s",&ifd_svalue[0]); 
		      ifd_ivalue = 0;
		      if( !strcmp(ifd_svalue,"PseudoColor") )
						ifd_ivalue = PseudoColor; 
		      else if( !strcmp(ifd_svalue,"TrueColor") )
						ifd_ivalue = TrueColor; 
		      switch (ifd_cond) {
			case IFD_COND_EQUAL:
			  if( _CLASS == ifd_ivalue ) ifd_cond = IFD_COND_KEEP;
			  else                       ifd_cond = IFD_COND_SKIP;
		          break;
			case IFD_COND_NOT_EQUAL:
			  if( _CLASS != ifd_ivalue ) ifd_cond = IFD_COND_KEEP;
			  else                       ifd_cond = IFD_COND_SKIP;
		          break;
			default:
		          fprintf(stderr,"*Xw_load_icons.Unrecognize key cond '%s'\n",cond);
		          ifd_cond = IFD_COND_SKIP;
		      } 
		      break;
		    case IFD_CODE_SCREEN_SIZE:
		      sscanf(value,"%f",&ifd_rvalue); 
  		      wsize = (float) WidthMMOfScreen(_SCREEN)/25.4;
  		      hsize = (float) HeightMMOfScreen(_SCREEN)/25.4;
		      ssize = sqrt(wsize*wsize + hsize*hsize);
		      switch (ifd_cond) {
		        case IFD_COND_GREATER:
			  if( ssize > ifd_rvalue ) ifd_cond = IFD_COND_KEEP;
			  else                      ifd_cond = IFD_COND_SKIP;
		          break;
			case IFD_COND_GREATER_OR_EQUAL:
			  if( ssize >= ifd_rvalue ) ifd_cond = IFD_COND_KEEP;
			  else                       ifd_cond = IFD_COND_SKIP;
		          break;
			case IFD_COND_LESS:
			  if( ssize < ifd_rvalue ) ifd_cond = IFD_COND_KEEP;
			  else                      ifd_cond = IFD_COND_SKIP;
		          break;
			case IFD_COND_LESS_OR_EQUAL:
			  if( ssize <= ifd_rvalue ) ifd_cond = IFD_COND_KEEP;
			  else                       ifd_cond = IFD_COND_SKIP;
		          break;
			case IFD_COND_EQUAL:
			  if( ssize == ifd_rvalue ) ifd_cond = IFD_COND_KEEP;
			  else                       ifd_cond = IFD_COND_SKIP;
		          break;
			case IFD_COND_NOT_EQUAL:
			  if( ssize != ifd_rvalue ) ifd_cond = IFD_COND_KEEP;
			  else                       ifd_cond = IFD_COND_SKIP;
		          break;
			default:
		          fprintf(stderr,"*Xw_load_icons.Unrecognize key cond '%s'\n",cond);
		          ifd_cond = IFD_COND_SKIP;
		      } 
		      break;
		    default:
		      fprintf(stderr,"*Xw_load_icons.Unrecognize key code '%s'\n",code);
		      ifd_cond = IFD_COND_SKIP;
		  }
		} else if( !strncmp(filename,"#else",5) ) {
#ifdef TRACE
		  if( Xw_get_trace() )
		  	printf(" #else\n");
#endif
		  switch (ifd_cond) {
		    case IFD_COND_UNKNOWN:
		      break;
		    case IFD_COND_SKIP:
		      ifd_cond = IFD_COND_KEEP;
		      break;
		    case IFD_COND_KEEP:
		      ifd_cond = IFD_COND_SKIP;
		      break;
		  }
		} else if( !strncmp(filename,"#endif",6) ) {
#ifdef TRACE
		  if( Xw_get_trace() )
		  	printf(" #endif\n");
#endif
		  ifd_code = IFD_CODE_UNKNOWN;
		  ifd_cond = IFD_COND_UNKNOWN;
		  ifd_ivalue = 0;
		  ifd_rvalue = 0.;
		  ifd_svalue[0] = '\0';
		}
#endif
		while( lf != '\n' ) status = fscanf(ficon,"%c",&lf) ;
		skip = True ;
	    } else if( lf != '\n' ) {
#ifdef CTS17035
		if( ifd_cond == IFD_COND_SKIP ) skip = True;
#endif
	        status = fscanf(ficon,"%s%c",iconname,&lf) ;
	        while( lf != '\n' ) status = fscanf(ficon,"%c",&lf) ;
	        if( iconname[0] == '#' ) {
		  fprintf(stderr,"*Xw_load_icons*Unimplemented Default Icon Name %s\n",
								filename);
		}
	    } else {
		skip = True ;
	    }

	    if( ferror(ficon) ) break ;

	    if( skip ) continue ;
	    piconinfo = Xw_put_icon (pwindow,iconname) ;
	    if( piconinfo ) {
	      piconinfo->pfile = (char*) strdup(filename);
#ifdef PRO16753
              nicons++;
#endif
	    }
    	}

	fclose(ficon) ;


#ifdef  TRACE_LOAD_ICONS
if( Xw_get_trace() ) {
    printf (" %d = Xw_load_icons(%lx,'%s')\n",nicons,(long ) pwindow,dirname);
}
#endif
 
        return (nicons);
}

#ifdef XW_PROTOTYPE
static XW_EXT_ICON* Xw_put_icon(XW_EXT_WINDOW* pwindow,char* iconname)
#else
static XW_EXT_ICON* Xw_put_icon(pwindow,iconname)
XW_EXT_WINDOW *pwindow ;
char *iconname ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_ICON *piconinfo ;

	piconinfo = (XW_EXT_ICON*) Xw_malloc(sizeof(XW_EXT_ICON)) ;

	if( piconinfo ) {
	    piconinfo->type = ICON_TYPE ;
	    piconinfo->pname = (char*) strdup(iconname);
	    piconinfo->pfile = NULL;
	    piconinfo->pimage = NULL ;
	    piconinfo->pixmap = 0 ;
	    piconinfo->update = 0 ;
	    piconinfo->link = pwindow->other;
	    pwindow->other = piconinfo;
	} else {
            /*ERROR*EXT_ICON allocation failed*/
            Xw_set_error(101,"Xw_put_icon",NULL) ;
            return (NULL) ;
	}

#ifdef  TRACE_PUT_ICON_NAME
if( Xw_get_trace() > 1 ) {
    printf (" %lx = Xw_put_icon(%lx,'%s')\n",(long ) piconinfo,(long ) pwindow,iconname);
}
#endif

    	return (piconinfo) ;

}

#ifdef XW_PROTOTYPE
static XW_EXT_ICON* Xw_get_icon(XW_EXT_WINDOW* pwindow,char* iconname)
#else
static XW_EXT_ICON* Xw_get_icon(pwindow,iconname)
XW_EXT_WINDOW *pwindow ;
char *iconname ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_ICON *piconinfo ;

	for( piconinfo=(XW_EXT_ICON*) pwindow->other ; piconinfo ;
				piconinfo=(XW_EXT_ICON*) piconinfo->link ) {
	  if( !strcmp(piconinfo->pname,iconname) ) break;
	}

#ifdef  TRACE_GET_ICON
if( Xw_get_trace() > 1 ) {
    printf (" %lx = Xw_get_icon(%lx,'%s')\n",(long ) piconinfo,(long ) pwindow,iconname);
}
#endif

    	return (piconinfo) ;

}

#ifdef XW_PROTOTYPE
static XW_STATUS Xw_del_icon(XW_EXT_WINDOW* pwindow,char* iconname)
#else
static XW_STATUS Xw_del_icon(pwindow,iconname)
XW_EXT_WINDOW *pwindow ;
char *iconname ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_ICON *piconinfo,*qiconinfo = (XW_EXT_ICON*) pwindow->other ;
XW_STATUS status = XW_SUCCESS;

	for( piconinfo=qiconinfo ; piconinfo ;
		qiconinfo=piconinfo,piconinfo=(XW_EXT_ICON*) piconinfo->link ) {
	  if( !strcmp(piconinfo->pname,iconname) ) break;
	}

	if( piconinfo ) {
	  if( piconinfo == qiconinfo ) { 
	    pwindow->other = piconinfo->link;
	  } else {
	    qiconinfo->link = piconinfo->link;
	  }
	  if( piconinfo->pname ) Xw_free(piconinfo->pname);
	  if( piconinfo->pfile ) Xw_free(piconinfo->pfile);
	  if( piconinfo->pixmap ) XFreePixmap(_DISPLAY,piconinfo->pixmap);
	  if( piconinfo->pimage ) Xw_close_image(piconinfo->pimage);
	  Xw_free(piconinfo);
	} else {
	  status = XW_ERROR;
	}

#ifdef  TRACE_DEL_ICON
if( Xw_get_trace() > 1 ) {
    printf (" %d = Xw_get_icon(%lx,'%s')\n",status,(long ) pwindow,iconname);
}
#endif

    	return (status) ;

}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_put_window_icon(void* awindow,void *fwindow,char* iconname,int width,int height)
#else
XW_STATUS Xw_put_window_icon(awindow,fwindow,iconname,width,height)
void *awindow ;
void *fwindow ;
char *iconname ;
int width;
int height;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
XW_EXT_WINDOW *qwindow = (XW_EXT_WINDOW*)fwindow ;
XW_EXT_DISPLAY *pdisplay;
XW_EXT_IMAGEDATA *pimage ;
XW_EXT_ICON *piconinfo = NULL;
XW_WINDOWSTATE state ;
XW_STATUS status ;
//int iimage,iwidth,iheight,wxc,wyc,wwidth,wheight ;
int iwidth,iheight,wxc,wyc,wwidth,wheight ;
float xi,yi ;
//unsigned long bpixel = 0;
#ifdef PERF01
time_t t_start,t_finish;
#endif

    	if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_put_window_icon",pwindow) ;
            return (XW_ERROR) ;
    	}

    	if( !Xw_isdefine_window(qwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_put_window_icon",qwindow) ;
            return (XW_ERROR) ;
    	}

	piconinfo = Xw_get_icon(pwindow,iconname) ;

	if( piconinfo ) {		/* Take the same place for the new icon */
	  if( piconinfo->pimage ) Xw_close_image(piconinfo->pimage);
//	  piconinfo->pimage = NULL;
	  piconinfo->pimage = 0;
	  if( piconinfo->pixmap ) XFreePixmap(_DISPLAY,piconinfo->pixmap);
//	  piconinfo->pixmap = NULL;
	  piconinfo->pixmap = 0;
	} else {
	  piconinfo = Xw_put_icon(pwindow,iconname) ;
	  if( !piconinfo ) {
            return (XW_ERROR) ;
	  }
	}

	pdisplay = pwindow->connexion;
	if( width <= 0 ) width = _DWIDTH;
	if( height <= 0 ) height = _DHEIGHT;

	state = Xw_get_window_position(qwindow,
					&wxc,&wyc,&wwidth,&wheight) ;

	status = Xw_get_window_pixelcoord(qwindow,wwidth/2,wheight/2,&xi,&yi) ;

#ifdef PERF01
	time(&t_start);
#endif
	pimage = (XW_EXT_IMAGEDATA*) Xw_get_image (qwindow,NULL,(int)xi,(int)yi,wwidth,wheight) ;

	if( !pimage || !_IIMAGE ) {
	  Xw_del_icon(pwindow,iconname) ;
	  return (XW_ERROR) ;
	}
#ifdef PERF01
	time(&t_finish);
	printf(" Image CAPTURE time is %lf\n",difftime(t_finish,t_start));
#endif

        status = XW_SUCCESS ;
	piconinfo->pimage = pimage;
	piconinfo->update = True;
	iwidth = _IIMAGE->width ;
	iheight = _IIMAGE->height ;
	if( iwidth > width || iheight > width ) {	/* ZOOM Icon */
	    float zoom = (float)min(width,height)/(float)max(iwidth,iheight) ;
#ifdef PERF01
	time(&t_start);
#endif
	    status = Xw_zoom_image(pimage,zoom) ;
            if( _ZIMAGE && (_ZIMAGE != _IIMAGE) ) {
                XDestroyImage(_IIMAGE);
                pimage->zoom = 1.;
                _IIMAGE = _ZIMAGE;
                _ZIMAGE = NULL;
            }

#ifdef PERF01
	time(&t_finish);
	printf(" Image ZOOM time is %lf\n",difftime(t_finish,t_start));
#endif
	}

#ifdef PERF01
	time(&t_start);
#endif
	status = Xw_convert_image(pwindow,pimage,NULL,0) ;
#ifdef PERF01
	time(&t_finish);
	printf(" Image CONVERT time is %lf\n",difftime(t_finish,t_start));
#endif
	if( !status ) {
	  Xw_del_icon(pwindow,iconname) ;
          Xw_set_error(114,"Xw_put_window_icon",NULL) ;
          return (XW_ERROR) ;
        }

	if( ShowIconBox == 'Y' ) Xw_show_icons(pwindow) ;

#ifdef  TRACE_PUT_WINDOW_ICON
if( Xw_get_trace() ) {
    printf (" %d = Xw_put_window_icons(%lx,%lx,'%s',%d,%d)\n",
				status,(long ) awindow,(long ) qwindow,iconname,width,height);
}
#endif

    	return (status) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_show_icons (void* awindow)
#else
XW_STATUS Xw_show_icons (awindow)
void *awindow ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
XW_EXT_IMAGEDATA *pimage;
XW_EXT_ICON *piconinfo ;
//XW_WINDOWSTATE state ;
XW_STATUS status ;
float xi,yi,tw,th,x0,y0,iw,ih ;
//int i,j,n,wxc,wyc,wsize,wxi,wyi,nimage = 0,maximagewidth = 0,maximageheight = 0 ;
int i,j,wxc,wyc,wsize,wxi,wyi,nimage = 0,maximagewidth = 0,maximageheight = 0 ;

    	if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(25,"Xw_show_icons",pwindow) ;
            return (XW_ERROR) ;
    	}

				/* Compute IconBox Size */
	for( piconinfo=(XW_EXT_ICON*) pwindow->other ; piconinfo ;
				piconinfo=(XW_EXT_ICON*) piconinfo->link ) {
	    if(( pimage = piconinfo->pimage )) {
	       nimage++;
	       maximagewidth = max(maximagewidth,_IIMAGE->width);
	       maximageheight = max(maximageheight,_IIMAGE->height);
	    }
        }

	if( !nimage ) {
            /*ERROR*EMPTY IconBox*/
            Xw_set_error(102,"Xw_show_icons",0) ;
            return (XW_ERROR) ;
	}


	{
	    int wwidth,wheight,usize = 1+(int)sqrt((double)nimage-1) ;
	    XW_WINDOWSTATE state ;

	    state = Xw_get_window_position(pwindow,
					&wxc,&wyc,&wwidth,&wheight) ;
	    wsize = min(wwidth,wheight)/max(maximagewidth,maximageheight) ;

	    if( usize > wsize ) {	/* Grows IconBox */
		wsize = usize ;
		status = Xw_set_window_position(pwindow,
			wxc,wyc,wsize*maximagewidth,wsize*maximageheight) ;
		Xw_resize_window(pwindow) ;
	    }
	}

				/* POP and erase IconBox */
	status = Xw_set_window_state(pwindow,XW_POP) ;

	status = Xw_erase_window(pwindow) ;

	nimage = 0 ;
	for( piconinfo=(XW_EXT_ICON*) pwindow->other ; piconinfo ;
				piconinfo=(XW_EXT_ICON*) piconinfo->link ) {
	    if(( pimage = piconinfo->pimage )) {
		i = nimage%wsize ; j = nimage/wsize ;
		wxi = i*maximagewidth+maximagewidth/2 ;
		wyi = j*maximageheight+maximageheight/2 ;
		status = Xw_get_window_pixelcoord(pwindow,wxi,wyi,&xi,&yi) ;
		Xw_draw_image(pwindow,piconinfo->pimage,xi,yi) ;
		Xw_get_text_size(pwindow,0,piconinfo->pname,&tw,&th,&x0,&y0) ;
		Xw_draw_text(pwindow,xi-tw/2.,yi-th/2.,piconinfo->pname,0.,0) ;
		iw = Xw_get_window_pixelvalue(pwindow,_IIMAGE->width) ;
		ih = Xw_get_window_pixelvalue(pwindow,_IIMAGE->height) ;
		Xw_begin_line(pwindow,5) ;
		Xw_line_point(pwindow,xi-iw/2.,yi-ih/2.) ;
		Xw_line_point(pwindow,xi-iw/2.,yi+ih/2.) ;
		Xw_line_point(pwindow,xi+iw/2.,yi+ih/2.) ;
		Xw_line_point(pwindow,xi+iw/2.,yi-ih/2.) ;
		Xw_line_point(pwindow,xi-iw/2.,yi-ih/2.) ;
		Xw_close_line(pwindow) ;
	    }
	}

	Xw_flush(pwindow,False) ;

#ifdef  TRACE_SHOW_ICONS
if( Xw_get_trace() ) {
    printf (" Xw_show_icons(%lx)\n",(long ) awindow);
}
#endif

    	return (XW_SUCCESS) ;
}

#ifdef XW_PROTOTYPE
int Xw_save_icons (void* awindow)
#else
int Xw_save_icons (awindow)
void *awindow ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
//XW_EXT_IMAGEDATA *pimage ;
//XW_EXT_IMAGE *pimaglist ;
XW_EXT_ICON *piconinfo ;
//int n,nimage = 0 ;
int nimage = 0 ;
//XW_STATUS status;

    	if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(25,"Xw_save_icons",pwindow) ;
            return (0) ;
    	}

	for( piconinfo=(XW_EXT_ICON*) pwindow->other ; piconinfo ;
				piconinfo=(XW_EXT_ICON*) piconinfo->link ) {
	    if( piconinfo->pimage && piconinfo->update ) {
	      if( Xw_save_image(pwindow,piconinfo->pimage,piconinfo->pname) ) {
		nimage++;
		piconinfo->update = False;
	      }
	    }
	}

#ifdef  TRACE_SAVE_ICONS
if( Xw_get_trace() ) {
    printf (" %d = Xw_save_icons(%lx)\n",nimage,(long ) awindow);
}
#endif

    	return (nimage) ;
}

#ifdef XW_PROTOTYPE
int Xw_clear_icons (void* awindow,char *dirname)
#else
int Xw_clear_icons (awindow,dirname)
void *awindow ;
char *dirname;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
XW_EXT_ICON *piconinfo,*qiconinfo ;
int status ;
FILE *ficon = NULL;
char *pname,filename[256],iconname[80],lf ;
//int n,f,skip,nicons = 0 ;
int skip,nicons = 0 ;

    if( !Xw_isdefine_window(pwindow) ) {
        /*ERROR*Bad EXT_WINDOW Address*/
        Xw_set_error(24,"Xw_clear_icons",pwindow) ;
        return (0) ;
    }

    if( dirname && (int)strlen(dirname) > 0 ) {	

	pname = Xw_get_filename(dirname,"ifd") ;

	if( pname ) ficon = fopen(pname,"r") ;

	if( !ficon ) {
	    /*ERROR*Bad ICON File Directory Name */
            Xw_set_error(100,"Xw_clear_icons",pname) ;
            return (0) ;
	}

#ifndef PRO16753	
	Xw_free(pname) ;
#endif
					/* READ Icon File Directory */
	while( !feof(ficon) ) {
	    skip = False ;
	    status = fscanf(ficon,"%s%c",filename,&lf) ;
	    if( filename[0] == '#' ) {
		while( lf != '\n' ) status = fscanf(ficon,"%c",&lf) ;
		skip = True ;
	    } else if( lf != '\n' ) {
	        status = fscanf(ficon,"%s%c",iconname,&lf) ;
	        while( lf != '\n' ) status = fscanf(ficon,"%c",&lf) ;
	        if( iconname[0] == '#' ) {
		  fprintf(stderr,"*Xw_load_icons*Unimplemented Default Icon Name %s\n",
								filename);
		}
	    } else {
		skip = True ;
	    }

	    if( ferror(ficon) ) break ;

	    if( skip ) continue ;

	    Xw_del_icon(pwindow,iconname);
    
	}

	fclose(ficon) ;
    } else if( pwindow->other ) {
	for( piconinfo=(XW_EXT_ICON*) pwindow->other ; piconinfo ; 
						piconinfo=qiconinfo ) {
	   qiconinfo = (XW_EXT_ICON*) piconinfo->link;
	   nicons++;
	   if( piconinfo->pname ) Xw_free(piconinfo->pname);
	   if( piconinfo->pfile ) Xw_free(piconinfo->pfile);
	   if( piconinfo->pixmap ) XFreePixmap(_DISPLAY,piconinfo->pixmap);
	   if( piconinfo->pimage ) Xw_close_image(piconinfo->pimage);
	   Xw_free(piconinfo);
	}
	pwindow->other = NULL;
    }

    if( nicons && ShowIconBox == 'Y' ) Xw_show_icons(pwindow) ;
	
#ifdef  TRACE_CLEAR_ICONS
if( Xw_get_trace() ) {
  if( dirname )
    printf (" %d = Xw_clear_icons(%lx,'%s')\n",nicons,(long ) awindow,dirname);
  else
    printf (" %d = Xw_clear_icons(%lx,NULL)\n",nicons,(long ) awindow);
}
#endif

    	return (nicons) ;
}

#ifdef XW_PROTOTYPE
static Aspect_Handle Xw_create_icon_pixmap (XW_EXT_WINDOW* pwindow,int width,int height,XW_EXT_IMAGEDATA *pimage)
#else
static Aspect_Handle Xw_create_icon_pixmap ( pwindow,width,height,pimage)
XW_EXT_WINDOW* pwindow ;
int	width ;
int	height ;
XW_EXT_IMAGEDATA *pimage;
#endif /*XW_PROTOTYPE*/
{
XImage *pximage = (_ZIMAGE) ? _ZIMAGE : _IIMAGE ;
Pixmap pixmap ;
int iwidth = pximage->width ;
int iheight = pximage->height ;
int wx,wy,ix,iy ;
int error,gravity ;
char *serror;

 	if( width == 0 ) width = iwidth ;
	wx = (width - iwidth)/2 ; ix = 0 ;
	if( wx < 0 ) {
	  ix = -wx ; wx = 0 ; iwidth = width ;
	}

	if( height == 0 ) height = iheight ;
	wy = (height - iheight)/2 ; iy = 0 ;
	if( wy < 0 ) {
	  iy = -wy ; wy = 0 ; iheight = height ;
	}

	Xw_print_error();
	if( !Xw_get_trace() ) Xw_set_synchronize(_DISPLAY,True) ;
	pixmap = XCreatePixmap(_DISPLAY,_WINDOW,width,height,_DEPTH) ;
	if( !Xw_get_trace() ) Xw_set_synchronize(_DISPLAY,False) ;

        serror = Xw_get_error(&error,&gravity);
        if( !pixmap || (error > 1000) ) {
          Xw_set_error(104,"Xw_create_icon_pixmap",0) ;
          return (0) ;
    	}

	if( wx > 0 || wy > 0 ) {
          XFillRectangle(_DISPLAY,pixmap,pwindow->qgwind.gccopy,
                                                        0,0,width,height) ;
	}

        XPutImage(_DISPLAY,pixmap,pwindow->qgwind.gccopy,
                        		pximage,ix,iy,wx,wy,iwidth,iheight) ;

#ifdef PRO16753
        XFlush(_DISPLAY);
#endif


#ifdef  TRACE_CREATE_ICON_PIXMAP
if( Xw_get_trace() ) {
    printf (" %lx = Xw_create_icon_pixmap(%lx,%d,%d,%lx)\n",
			(long ) pixmap,(long ) pwindow,width,height,(long ) pimage);
}
#endif

	return pixmap ;
}

#ifdef XW_PROTOTYPE
Aspect_Handle Xw_get_icon_pixmap (void* awindow,int width,int height,char* filename)
#else
Aspect_Handle Xw_get_icon_pixmap ( awindow,width,height,filename)
void* awindow ;
int	width ;
int	height ;
char*   filename ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
XW_EXT_ICON *piconinfo ;
//XW_STATUS status ;
char iconname[256];
char *piconname = filename;

    	if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_get_icon_pixmap",pwindow) ;
            return (0) ;
    	}

	if( !filename || (int)strlen(filename) <= 0 ) {
            /*ERROR*Bad Icon Name*/
            Xw_set_error(103,"Xw_get_icon_pixmap",filename) ;
            return (0) ;
	}
				/* Get icon directly from the file */
	if( filename[0] == '/' || filename[0] == '$' ) { 
	    char *ps1 = strrchr(filename,'/');
	    char *ps2 = strrchr(filename,'.');
	    if( !ps1 ) {
              /*ERROR*Bad Icon Name*/
              Xw_set_error(103,"Xw_get_icon_pixmap",filename) ;
              return (0) ;
	    }
   	    ps1++;
	    if( ps2 ) {
	      int n = min(256,ps2-ps1);
	      strncpy(iconname,filename,n);
	    } else {
	      strcpy(iconname,ps1);
	    }
	    piconname = iconname;
    	}

	if( !(piconinfo = Xw_get_icon(pwindow,piconname)) ) {
				/* Add a new icon in icon-box */
	    piconinfo = Xw_put_icon(pwindow,piconname);
	    if( piconinfo ) {
	      piconinfo->pfile = (char*) strdup(filename);
	    } else {
	      return 0;
	    }
	}

	if( !piconinfo->pixmap ) {
	  XW_EXT_IMAGEDATA *pimage;
	  if( !piconinfo->pimage ) {
	    piconinfo->pimage = (XW_EXT_IMAGEDATA*)
	    	Xw_load_image (pwindow,NULL,piconinfo->pfile) ;
	  }

	  if(( pimage = piconinfo->pimage )) {
	    XImage *pximage = (_ZIMAGE) ? _ZIMAGE : _IIMAGE ;
	    if( width == 0 ) width = pximage->width ;
	    if( height == 0 ) height = pximage->height ;
	    piconinfo->pixmap = 
		Xw_create_icon_pixmap(pwindow,width,height,piconinfo->pimage) ;	
	  }
	}

#ifdef  TRACE_GET_ICON_PIXMAP
if( Xw_get_trace() ) {
    printf (" %lx = Xw_get_icon_pixmap(%lx,%d,%d,'%s')\n",
			(long ) piconinfo->pixmap,(long ) awindow,width,height,filename);
}
#endif

	return (piconinfo->pixmap) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_icon_size (void* awindow,char* iconname,int *width,int *height)
#else
XW_STATUS Xw_get_icon_size ( awindow,iconname,width,height)
void* awindow ;
char*   iconname ;
int	*width ;
int	*height ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
XW_EXT_ICON *piconinfo ;
XW_EXT_IMAGEDATA *pimage ;

    	if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_get_icon_size",pwindow) ;
            return (XW_ERROR) ;
    	}

	*width = *height = 0;

	if(( piconinfo = Xw_get_icon(pwindow,iconname) )) {
	  if( !piconinfo->pimage ) {
	    piconinfo->pimage = (XW_EXT_IMAGEDATA*)
	    	Xw_load_image (pwindow,NULL,piconinfo->pfile) ;
	  }

	  if(( pimage = piconinfo->pimage )) {
            *width = _IIMAGE->width ;
	    *height = _IIMAGE->height ;
	  } else {
            return (XW_ERROR) ;
	  }
	} else {
            /*ERROR*Bad Icon Name*/
            Xw_set_error(103,"Xw_get_icon_size",iconname) ;
            return (XW_ERROR) ;
	}

#ifdef  TRACE_GET_ICON_PIXMAP
if( Xw_get_trace() ) {
    printf (" Xw_get_icon_size(%lx,'%s',%d,%d)\n",
					(long ) awindow,iconname,*width,*height);
}
#endif

	return (XW_SUCCESS) ;
}

#ifdef XW_PROTOTYPE
char* Xw_get_icon_name (void* awindow,int index)
#else
char* Xw_get_icon_name ( awindow,index)
void* awindow ;
int	index ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
XW_EXT_ICON *piconinfo ;
char *iconname ;
int n ;

    	if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_get_icon_name",pwindow) ;
            return (NULL) ;
    	}

        for( n=1,piconinfo=(XW_EXT_ICON*) pwindow->other ; piconinfo ;
			piconinfo = (XW_EXT_ICON*) piconinfo->link,n++ ) {
	  if( n == index ) break;
        }

	if( !piconinfo ) {
            /*ERROR*Bad Icon index*/
            Xw_set_error(106,"Xw_get_icon_name",&index) ;
            return (NULL) ;
	}

	iconname = piconinfo->pname ;

#ifdef  TRACE_GET_ICON_NAME
if( Xw_get_trace() ) {
    printf (" '%s' = Xw_get_icon_name(%lx,%d)\n",iconname,(long ) awindow,index);
}
#endif

	return (iconname) ;
}
