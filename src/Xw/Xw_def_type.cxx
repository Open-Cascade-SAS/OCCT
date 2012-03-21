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
#define TRACE_DEF_TYPE
#endif

/*
   STATUS Xw_def_type (atypemap,index,length,desc):
   XW_EXT_TYPEMAP *atypemap
   int index			Line type index
   int length			Descriptor length
   float *desc			Type Descriptor

	Update Line Type Extended typemap index with the specified TYPE values . 	Descriptor is an array of float.Each value give the subline length
	in MM .First subline is the draw part,Second the hidden part,
	Thirst the drawn part ...
	NOTE than type index 0 is the Default Type (SOLID) 
					and cann't be REDEFINED

	Returns ERROR if BadType Index
	Returns SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_def_type (void* atypemap,
				int index,int length,float* desc)
#else
XW_STATUS Xw_def_type (atypemap,index,length,desc)
void *atypemap;
int index ;
int length ;
float *desc ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_TYPEMAP *ptypemap = (XW_EXT_TYPEMAP*)atypemap ;
Screen *screen ;
unsigned char *pdesc ;
int i,ptype ;

    if ( !Xw_isdefine_typeindex(ptypemap,index) ) {
        /*Bad Type index*/
        Xw_set_error(18,"Xw_def_type",&index) ;
        return (XW_ERROR) ;
    }
    
    if( !desc || length < 0 ) {
        /*Bad Type descriptor data*/
        Xw_set_error(19,"Xw_def_type",&index) ;
        return (XW_ERROR) ;
    }
    
    if( ptypemap->types[index] ) Xw_free(ptypemap->types[index]) ;
    ptypemap->types[index] = NULL ;

    if( length ) {
	ptypemap->types[index] = pdesc = (unsigned char*) Xw_malloc(length+1) ;

        screen = ScreenOfDisplay(_TDISPLAY,DefaultScreen(_TDISPLAY)) ;

	for( i=0 ; i<length ; i++ ) {
	    pdesc[i] = 1 ;
	    ptype = (int) (0.5 + (desc[i] * (float)WidthOfScreen(screen)/
					(float)WidthMMOfScreen(screen))) ;
	    if( desc[i] == 0. || ptype < 0 || ptype > 255 ) {
	        /*Bad Type descriptor data*/
	        Xw_set_error(19,"Xw_def_type",&index) ;
	    }
	    pdesc[i] = (unsigned char) (ptype) ? ptype : 1 ;
	}

	pdesc[length] ='\0' ;
    }

#ifdef  TRACE_DEF_TYPE
if( Xw_get_trace() ) {
    printf (" Xw_def_type(%lx,%d,%d,{",(long ) ptypemap,index,length);
    for ( i=0 ; i<length-1 ; i++ ) printf("%f,",desc[i]) ;
    printf("%f})\n",desc[length-1]) ;
}
#endif

	return (XW_SUCCESS);
}
