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
#define TRACE_DEF_TYPEMAP
#endif

/*
   XW_EXT_TYPEMAP* Xw_def_typemap(adisplay,ntype):
   XW_EXT_DISPLAY *adisplay Extended Display structure

   int ntype		Number of type cells to be allocated

	Create a typemap extension 
        allocate the type cells in the typemap as if possible
	depending of the MAXTYPE define .

	Returns Typemap extension address if successuful
		or NULL if ERROR

   STATUS = Xw_close_typemap(atypemap)
   XW_EXT_TYPEMAP* atypemap Extended typemap


	Destroy The Extended TypeMap 

	Returns ERROR if Bad Extended TypeMap Address
	        SUCCESS if successfull

*/

#ifdef XW_PROTOTYPE
void* Xw_def_typemap (void* adisplay,int ntype)
#else
void* Xw_def_typemap (adisplay,ntype)
void *adisplay ;
int ntype ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*)adisplay ;
XW_EXT_TYPEMAP *ptypemap = NULL ;
//int i,type ;
int i ;

    if( !Xw_isdefine_display(pdisplay) ) {
        /*ERROR*Bad EXT_DISPLAY Address*/
        Xw_set_error(96,"Xw_def_typemap",pdisplay) ;
        return (NULL) ;
    }

    if( !(ptypemap = Xw_add_typemap_structure(sizeof(XW_EXT_TYPEMAP))) ) 
								return (NULL) ;

    if( ntype <= 0 ) ntype = MAXTYPE ;

    ptypemap->connexion = pdisplay ;
    ptypemap->maxtype = min(ntype,MAXTYPE) ; 

    for( i=0 ; i<ptypemap->maxtype ; i++ ) {
        ptypemap->types[i] = NULL ;
    }

#ifdef TRACE_DEF_TYPEMAP
if( Xw_get_trace() ) {
    printf(" %lx = Xw_def_typemap(%lx,%d)\n", (long ) ptypemap,(long ) adisplay,ntype) ;
}
#endif

    return (ptypemap);
}

static XW_EXT_TYPEMAP *PtypemapList =NULL ;

#ifdef XW_PROTOTYPE
XW_EXT_TYPEMAP* Xw_add_typemap_structure(int size)
#else
XW_EXT_TYPEMAP* Xw_add_typemap_structure(size)
int size ;
#endif /*XW_PROTOTYPE*/
/*
        Create and Insert one Extended typemap structure in the
        EXtended typemap List

        returns Extended typemap address if successful
                or NULL if Bad Allocation
*/
{
XW_EXT_TYPEMAP *ptypemap = (XW_EXT_TYPEMAP*) Xw_malloc(size) ;
int i ;

        if( ptypemap ) {
	    ptypemap->type = TYPEMAP_TYPE ;
            ptypemap->link = PtypemapList ;
            PtypemapList = ptypemap ;
	    ptypemap->connexion = NULL ;
	    ptypemap->maxtype = 0 ;
	    ptypemap->maxwindow = 0 ;
	    for( i=0 ; i<MAXTYPE ; i++ ) ptypemap->types[i] = NULL ;
        } else {
	    /*EXT_TYPEMAP Allocation failed*/
	    Xw_set_error(20,"Xw_add_typemap_structure",0) ;
        }

        return (ptypemap) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_close_typemap(void* atypemap)
#else
XW_STATUS Xw_close_typemap(atypemap)
void *atypemap ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_TYPEMAP* ptypemap = (XW_EXT_TYPEMAP*) atypemap ;
XW_STATUS status ;

    if( !Xw_isdefine_typemap(ptypemap) ) {
	/*Bad EXT_TYPEMAP Address*/
	Xw_set_error(51,"Xw_close_typemap",ptypemap) ;
	return (XW_ERROR) ;
    }

    status = Xw_del_typemap_structure(ptypemap) ;

#ifdef TRACE_DEF_TYPEMAP
if( Xw_get_trace() ) {
    printf(" %d = Xw_close_typemap(%lx)\n",status,(long ) ptypemap) ;
}
#endif

    return (status) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_del_typemap_structure(XW_EXT_TYPEMAP* atypemap)
#else
XW_STATUS Xw_del_typemap_structure(atypemap)
XW_EXT_TYPEMAP *atypemap;
#endif /*XW_PROTOTYPE*/
/*
        Remove the Extended typemap address from the Extended List

        returns ERROR if the typemap address is not Found in the list
        returns SUCCESS if successful
*/
{
XW_EXT_TYPEMAP *ptypemap = PtypemapList ;
//int i ;

    if( !atypemap ) return (XW_ERROR) ;

    if( atypemap->maxwindow ) {
	return (XW_ERROR) ;
    } else {
        if( atypemap == ptypemap ) {
            PtypemapList = (XW_EXT_TYPEMAP*) atypemap->link ;
        } else {
            for( ; ptypemap ; ptypemap = (XW_EXT_TYPEMAP*) ptypemap->link ) {
                if( ptypemap->link == atypemap ) {
                    ptypemap->link = atypemap->link ;
                    break ;
                }
            }
        }
	Xw_free(atypemap) ;
    }
    return (XW_SUCCESS) ;
}
