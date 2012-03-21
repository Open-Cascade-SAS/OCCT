// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
