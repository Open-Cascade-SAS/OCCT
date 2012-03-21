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
#define TRACE_GET_TYPE_INDEX
#endif

/*
   XW_STATUS Xw_get_type_index(atypemap,desc,length,index):

   XW_EXT_TYPEMAP *atypemap	Typemap extension structure
   float desc[] ;	Style Descriptor Values
   int length ;		Stle Descriptor length
   int *index ;		Return Type index 0 >= x < MAXTYPE

	Gets the nearest type index from Style Descriptor values.

	Returns XW_ERROR if BadType Index or type is not defined
	Returns XW_SUCCESS if Successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_type_index(void *atypemap,float desc[],int length,int *index)
#else
XW_STATUS Xw_get_type_index(atypemap,desc,length,index)
void *atypemap ;
float desc[] ;
int length ;
int *index ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_TYPEMAP *ptypemap = (XW_EXT_TYPEMAP*)atypemap ;
XW_STATUS status = XW_ERROR ;
int i,j,ptype ;

    if( !ptypemap ) {
	/*ERROR*Bad EXT_TYPEMAP Address*/
	Xw_set_error( 51,"Xw_get_type_index",ptypemap ) ;
	return( XW_ERROR ) ;
    }

    if( length ) {
        unsigned char *pdesc = (unsigned char*) Xw_malloc(length+1) ;
        Screen *screen = ScreenOfDisplay(_TDISPLAY,DefaultScreen(_TDISPLAY)) ;

        for( i=0 ; i<length ; i++ ) {
            ptype = (int) (0.5 + (desc[i] * (float)WidthOfScreen(screen)/
                                        (float)WidthMMOfScreen(screen))) ;
            pdesc[i] = (unsigned char) (ptype) ? ptype : 1 ;
        }
        pdesc[length] ='\0' ;

        for( i=j=0 ; i<ptypemap->maxtype ; i++ ) {
	    if( ptypemap->types[i] ) {
		if( !strcmp((char*)pdesc,(char*)ptypemap->types[i]) ) break;
	    } else if( !j ) j = i ;
        }

	if( i<ptypemap->maxtype ) {
	    *index = i ;
	    status = XW_SUCCESS ;
	} else {
	    *index = j ;
	    status = Xw_def_type (ptypemap,j,length,desc) ;
	}
	Xw_free(pdesc) ;
    } else {
	*index = 0 ;
	status = XW_SUCCESS ;
    }

#ifdef TRACE_GET_TYPE_INDEX
if( Xw_get_trace() > 1 ) {
    printf(" %d = Xw_get_type_index(%lx,%f,%d,%d)\n",
			status,(long ) ptypemap,desc[0],length,*index) ;
}
#endif

	return( status ) ;
}
