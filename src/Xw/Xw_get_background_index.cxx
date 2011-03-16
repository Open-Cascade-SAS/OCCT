
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_BACKGROUND_INDEX
#endif

/*
   STATUS Xw_get_background_index(awindow,index):
   XW_EXT_WINDOW *awindow	Window extension structure
   int *index ; 	Returned index
   

	Gets current background index.

	Returns ERROR if Extended Window Address is badly defined
	Returns SUCCESS if Successful      
*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_background_index (void* awindow,int* index)
#else
XW_STATUS Xw_get_background_index (awindow,index)
void *awindow;
int *index ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;

	if ( !Xw_isdefine_window(pwindow) ) {
	    /*ERROR*Bad EXTENDED Window ADDRESS*/
	    Xw_set_error(24,"Xw_get_background_index",pwindow) ;
	    return (XW_ERROR) ;
	}

	*index = pwindow->backindex ;

#ifdef TRACE_GET_BACKGROUND_INDEX
if( Xw_get_trace() ) {
    printf(" Xw_get_background_index(%lx,%d)\n",(long ) pwindow,*index) ;
}
#endif

	return (XW_SUCCESS);
}
