#define PRO5676 /*GG_231296
//              Calculer la hauteur exacte de la police de caracteres
//              par rapport a la partie du texte situee au dessus de la
//              ligne de base.
*/

#define TEST

#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_DEF_FONT
#endif

#define MAXNAMES 32

/*
   STATUS Xw_def_font (afontmap,index,size,fontname):
   XW_EXT_FONTMAP *afontmap
   int index			Font index
   float size			Font size in MM
   char fontname[]		Font description

	Update Text Font Extended fontmap index with the specified 
	FONT size and description
	NOTE than font index 0 is the default font and cann't be REDEFINED .
	NOTE than fontname can be defined in three forms :
	1) Simple form is "fmly"  			Ex: "helvetica"
	2) More complex form is "fmly-wght"		Ex: "helvetica-bold"
	3) Full form is "-fndry-fmly-wght-slant-swdth-adstyl-pxlsz
			 -ptSz-resx-resy-spc-avgWdth-rgstry-encdn"
	where each field must be replaced by an "*"
	See Xlib User Guide for more information or /usr/bin/X11/xfontsel"
	utility .

	Returns ERROR if BadFont Index or name
	Returns SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_def_font (void* afontmap,
			int index,float size,char* fontname)
#else
XW_STATUS Xw_def_font (afontmap,index,size,fontname)
void *afontmap;
int index ;
float size ;
char *fontname ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_FONTMAP *pfontmap = (XW_EXT_FONTMAP*)afontmap ;
Screen *screen ;
int i,j,n,prefered_size,count,psize,qsize,retry = True;
char **fontlist,*prefered_name ;
XFontStruct *tmpfont,*selected_font,*prefered_font ;
//char **fontdir ;
float rsize = fabs(size);

	if ( !Xw_isdefine_fontindex(pfontmap,index) ) {
	    /*ERROR*Bad Font Index*/
	    Xw_set_error(7,"Xw_def_font",&index) ;
	    return (XW_ERROR) ;
	}

	if( !fontname || !strlen(fontname) ) {
	    /*ERROR*Bad Font Name*/
	    Xw_set_error(8,"Xw_def_font",fontname) ;
	    return (XW_ERROR) ;
	}

	screen = ScreenOfDisplay(_FDISPLAY,DefaultScreen(_FDISPLAY)) ;

	fontlist = XListFonts(_FDISPLAY,fontname,MAXNAMES,&count) ;
RETRY:
	prefered_size = 
		(unsigned int) (rsize*(float)WidthOfScreen(screen)/
					(float)WidthMMOfScreen(screen)) ;
	prefered_font = NULL;
	prefered_name = NULL;
	if( count ) {
	    char *psub,*pfont,scalable_font[128];
	    qsize = 0;
	    for( i=0 ; i<count ; i++ ) {
		pfont = fontlist[i];
#ifdef TEST
		if(( psub = strstr(pfont,"-0-0-") )) {
		  int l = psub - pfont + 1;
		  pfont = scalable_font;
		  strncpy(pfont,fontlist[i],l);
		  sprintf(&pfont[l],"%d",prefered_size);
		  strcat(pfont,&fontlist[i][l+1]);
		  selected_font = XLoadQueryFont(_FDISPLAY,pfont) ;
		} else {
		  selected_font = XLoadQueryFont(_FDISPLAY,pfont) ;
		}
#else
		selected_font = XLoadQueryFont(_FDISPLAY,fontlist[i]) ;
#endif
		if( !selected_font ) break;

	        psize = selected_font->max_bounds.ascent +
				selected_font->max_bounds.descent ;

		if( !prefered_font || 
		   (abs(psize - prefered_size) < abs(qsize - prefered_size)) ) {
		    tmpfont = prefered_font ;
		    prefered_font = selected_font ;
		    selected_font = tmpfont ;
		    qsize = psize;
#ifdef TEST
		    prefered_name = pfont ;
#else
		    prefered_name = fontlist[i] ;
#endif
		}
		if( selected_font && (prefered_font != selected_font) ) {
		    for( j=1,n=0 ; j<pfontmap->maxfont ; j++ ) {
			 if( pfontmap->fonts[j] && (selected_font->fid == 
					     pfontmap->fonts[j]->fid) ) n++ ;
		    }
		    if( !n ) {
			XFreeFont(_FDISPLAY,selected_font);
		    }
		}
	    }
	}
	    
	if( pfontmap->gnames[index] ) Xw_free(pfontmap->gnames[index]) ;
	if( pfontmap->snames[index] ) Xw_free(pfontmap->snames[index]) ;
	pfontmap->gnames[index] = (char*) Xw_malloc(strlen(fontname)+1) ;
	strcpy(pfontmap->gnames[index],fontname) ;
	if( strstr(fontname,"Defaultfont") ) {
	    pfontmap->snames[index] = 
			(char*) Xw_malloc(strlen(pfontmap->snames[0])+1) ;
	    strcpy(pfontmap->snames[index],pfontmap->snames[0]) ;
	    pfontmap->fonts[index] = pfontmap->fonts[0] ;
	} else if( prefered_font && prefered_name ) {
	    pfontmap->snames[index] = 
				(char*) Xw_malloc(strlen(prefered_name)+1) ;
	    strcpy(pfontmap->snames[index],prefered_name) ;
	    pfontmap->fonts[index] = prefered_font ;
	} else {
	    pfontmap->snames[index] = 
			(char*) Xw_malloc(strlen(pfontmap->snames[0])+1) ;
	    strcpy(pfontmap->snames[index],pfontmap->snames[0]) ;
	    pfontmap->fonts[index] = pfontmap->fonts[0] ;
	    /*ERROR*Bad Font Name*/
	    Xw_set_error(8,"Xw_def_font",fontname) ;
	    return (XW_ERROR) ;
	}

#ifdef PRO5676
	if( size < 0. && retry ) {
	  float ratio,tsize;
	  psize = (pfontmap->fonts[index])->max_bounds.ascent;
	  tsize = (float)psize*HeightMMOfScreen(screen)/
                                               (float)HeightOfScreen(screen) ;
	  ratio = rsize/tsize;
	  rsize *= ratio;
	  retry = False;
	  if( fabs(ratio - 1.) > 0.001 ) goto RETRY;
	}
#endif
	psize = (pfontmap->fonts[index])->max_bounds.ascent +
				(pfontmap->fonts[index])->max_bounds.descent ;
	rsize = (float)psize*HeightMMOfScreen(screen)/
                                               (float)HeightOfScreen(screen) ;

	pfontmap->gsizes[index] = size ;
	pfontmap->fsizes[index] = rsize ;
        pfontmap->fratios[index] = 
		(float)(pfontmap->fonts[index])->max_bounds.descent/
				(pfontmap->fonts[index])->max_bounds.ascent;
	pfontmap->ssizey[index] = 
	pfontmap->ssizex[index] = pfontmap->ssizey[index] = rsize;
	pfontmap->gslants[index] = pfontmap->sslants[index] = 0. ;

	if( fontlist ) XFreeFontNames(fontlist) ;

#ifdef  TRACE_DEF_FONT
if( Xw_get_trace() ) {
    printf (" Xw_def_font(%lx,%d,%f,'%s')\n",(long ) pfontmap,index,size,fontname) ;
}
#endif

	return (XW_SUCCESS);
}
