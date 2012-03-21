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
#define TRACE_DEF_WIDTHMAP
#endif

/*
   XW_EXT_WIDTHMAP* Xw_def_widthmap(adisplay,nwidth):
   XW_EXT_DISPLAY *adisplay Extended Display structure

   int nwidth		Number of width cells to be allocated

	Create a widthmap extension 
        allocate the width cells in the widthmap as if possible
	depending of the MAXWIDTH define .

	Returns Widthmap extension address if successful
		or NULL if ERROR

   STATUS Xw_close_widthmap(awidthmap):
   XW_EXT_WIDTHMAP* awidthmap   Extended Widthmap address


        Destroy The specified Extended Widthmap

        Returns SUCCESS if successuful
        Returns ERROR if Bad Extended Widthmap 


*/

#ifdef XW_PROTOTYPE
void* Xw_def_widthmap (void* adisplay,int nwidth)
#else
void* Xw_def_widthmap (adisplay,nwidth)
void *adisplay ;
int nwidth ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*)adisplay ;
XW_EXT_WIDTHMAP *pwidthmap = NULL ;
//int i ;

    if( !Xw_isdefine_display(pdisplay) ) {
        /*ERROR*Bad EXT_DISPLAY Address*/
        Xw_set_error(96,"Xw_def_widthmap",pdisplay) ;
        return (NULL) ;
    }

    if( !(pwidthmap = Xw_add_widthmap_structure(sizeof(XW_EXT_WIDTHMAP))) ) 
								return (NULL) ;

    if( nwidth <= 0 ) nwidth = MAXWIDTH ;

    pwidthmap->connexion = pdisplay ;
    pwidthmap->maxwidth = min(nwidth,MAXWIDTH) ;

#ifdef TRACE_DEF_WIDTHMAP
if( Xw_get_trace() ) {
    printf(" %lx = Xw_def_widthmap(%lx,%d)\n",
				(long ) pwidthmap,(long ) adisplay,nwidth) ;
}
#endif

    return (pwidthmap);
}

static XW_EXT_WIDTHMAP *PwidthmapList =NULL ;

#ifdef XW_PROTOTYPE
XW_EXT_WIDTHMAP* Xw_add_widthmap_structure(int size)
#else
XW_EXT_WIDTHMAP* Xw_add_widthmap_structure(size)
int size ;
#endif /*XW_PROTOTYPE*/
/*
        Create and Insert one Extended widthmap structure in the
        EXtended widthmap List

        returns Extended widthmap address if successful
                or NULL if Bad Allocation
*/
{
XW_EXT_WIDTHMAP *pwidthmap = (XW_EXT_WIDTHMAP*) Xw_malloc(size) ;
int i ;

        if( pwidthmap ) {
	    pwidthmap->type = WIDTHMAP_TYPE ;
            pwidthmap->link = PwidthmapList ;
            PwidthmapList = pwidthmap ;
	    pwidthmap->connexion = NULL ;
	    pwidthmap->maxwidth = 0 ;
	    pwidthmap->maxwindow = 0 ;
	    for( i=0 ; i<MAXWIDTH ; i++ ) {
		pwidthmap->widths[i] = 0 ;
	    }
        } else {
	    /*EXT_WIDTHMAP Allocation Failed*/
	    Xw_set_error(23,"Xw_add_widthmap_structure",0) ;
        }

        return (pwidthmap) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_close_widthmap(void* awidthmap)
#else
XW_STATUS Xw_close_widthmap(awidthmap)
void* awidthmap ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WIDTHMAP *pwidthmap = (XW_EXT_WIDTHMAP*)awidthmap ;
XW_STATUS status ;

    if( !Xw_isdefine_widthmap(pwidthmap) ) {
        /*Bad EXT_WIDTHMAP Address*/
        Xw_set_error(53,"Xw_close_widthmap",pwidthmap) ;
        return (XW_ERROR) ;
    }

    status = Xw_del_widthmap_structure(pwidthmap) ;

#ifdef TRACE_DEF_WIDTHMAP
if( Xw_get_trace() ) {
    printf(" %d = Xw_close_widthmap(%lx)\n",status,(long ) pwidthmap) ;
}
#endif

    return (status) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_del_widthmap_structure(XW_EXT_WIDTHMAP* awidthmap)
#else
XW_STATUS Xw_del_widthmap_structure(awidthmap)
XW_EXT_WIDTHMAP *awidthmap;
#endif /*XW_PROTOTYPE*/
/*
        Remove the Extended widthmap address from the Extended List

        returns ERROR if the widthmap address is not Found in the list
        returns SUCCESS if successful
*/
{
XW_EXT_WIDTHMAP *pwidthmap = PwidthmapList ;
//int i ;

    if( !awidthmap ) return (XW_ERROR) ;

    if( awidthmap->maxwindow ) {
	return (XW_ERROR) ;
    } else {
        if( awidthmap == pwidthmap ) {
            PwidthmapList = (XW_EXT_WIDTHMAP*) awidthmap->link ;
        } else {
            for( ; pwidthmap ; pwidthmap = (XW_EXT_WIDTHMAP*) pwidthmap->link ) {
                if( pwidthmap->link == awidthmap ) {
                    pwidthmap->link = awidthmap->link ;
                    break ;
                }
            }
        }
	Xw_free(awidthmap) ;
    }
    return (XW_SUCCESS) ;
}
