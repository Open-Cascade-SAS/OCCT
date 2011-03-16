#define GG002	/*GG_020197
		Renvoyer la hauteur du texte et non de la font
*/

#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_TEXT_SIZE
#endif

/*
   XW_STATUS Xw_get_text_size (awindow,index,string,width,height,xoffset,yoffset):
   XW_EXT_WINDOW *awindow
   int index			Font Index
   char *string			String to evaluate 
   float *width,*height		Returned String size in DWU
   float *xoffset,*yoffset	Returned text offsets from the text boundary box.

	Evaluate String size depending of the specified font Index. 

	Returns XW_ERROR if Font Index is NOT defined
	Returns XW_SUCCESS if successfull     

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_text_size (void *awindow,int index,char *string,float *width,float *height,float *xoffset,float *yoffset)
#else
XW_STATUS Xw_get_text_size (awindow,index,string,width,height,xoffset,yoffset)
void *awindow;
int index ;
char *string ;
float *width,*height,*xoffset,*yoffset ;
#endif /*XW_PROTOTYPE*/
{
  XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;

	*width = *height = *xoffset = *yoffset = 0.;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_get_text_size",pwindow) ;
            return (XW_ERROR) ;
        }

        if( !Xw_isdefine_font(_FONTMAP,index) ) {
            /*ERROR*Bad Font Index*/
            Xw_set_error(7,"Xw_get_text_size",&index) ;
            return (XW_ERROR) ;
        }

#ifdef GG002
	{
	int dir,fascent,fdescent;
 	XCharStruct overall;
        XTextExtents(_FONTMAP->fonts[index],string,strlen(string),
				&dir,&fascent,&fdescent,&overall);
	*width = UVALUE(overall.width);
	*height = UVALUE(overall.ascent + overall.descent);
	*xoffset = UVALUE(overall.lbearing);
	*yoffset = UVALUE(overall.descent);
	}
#else		/* Renvoyer la hauteur du texte et non de la font */
	*width = UVALUE(XTextWidth(_FONTMAP->fonts[index],
						string,strlen(string))) ;

	*height = UVALUE((_FONTMAP->fonts[index])->ascent +
	  		  	(_FONTMAP->fonts[index])->descent) ;
	*xoffset = 0.;
	*yoffset = UVALUE((_FONTMAP->fonts[index])->descent) ;
#endif


#ifdef  TRACE_GET_TEXT_SIZE
if( Xw_get_trace() > 1 ) {
    printf (" Xw_get_text_size(%lx,'%s',%f,%f,%f,%f\n",
			(long ) awindow,string,*width,*height,*xoffset,*yoffset) ;
}
#endif

	return (XW_SUCCESS);
}
