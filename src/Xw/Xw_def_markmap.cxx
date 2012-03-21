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
#define TRACE_DEF_MARKMAP
#endif

/*
   XW_EXT_MARKMAP* Xw_def_markmap(adisplay,nmark):
   XW_EXT_DISPLAY *adisplay Extended Display structure

   int nmark		Number of marker cells to be allocated

	Create a markmap extension 
        allocate the marker cells in the markmap as if possible
	depending of the MAXMARKER define .

	Returns Markmap extension address if successuful
		or NULL if ERROR

*/

#ifdef XW_PROTOTYPE
void* Xw_def_markmap (void* adisplay,int nmark)
#else
void* Xw_def_markmap (adisplay,nmark)
void *adisplay ;
int nmark ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*)adisplay ;
XW_EXT_MARKMAP *pmarkmap = NULL ;
//int i ;

    if( !Xw_isdefine_display(pdisplay) ) {
        /*ERROR*Bad EXT_DISPLAY Address*/
        Xw_set_error(96,"Xw_def_markmap",pdisplay) ;
        return (NULL) ;
    }

    if( !(pmarkmap = Xw_add_markmap_structure(sizeof(XW_EXT_MARKMAP))) ) 
								return (NULL) ;

    if( nmark <= 0 ) nmark = MAXMARKER ;

    pmarkmap->connexion = pdisplay ;
    pmarkmap->maxmarker = min(nmark,MAXMARKER) ; 

#ifdef TRACE_DEF_MARKMAP
if( Xw_get_trace() ) {
    printf(" %lx = Xw_def_markmap(%lx,%d)\n", (long ) pmarkmap,(long ) adisplay,nmark) ;
}
#endif

    return (pmarkmap);
}

static XW_EXT_MARKMAP *PmarkmapList =NULL ;

#ifdef XW_PROTOTYPE
XW_EXT_MARKMAP* Xw_add_markmap_structure(int size)
#else
XW_EXT_MARKMAP* Xw_add_markmap_structure(size)
int size ;
#endif /*XW_PROTOTYPE*/
/*
        Create and Insert one Extended markmap structure in the
        EXtended markmap List

        returns Extended markmap address if successful
                or NULL if Bad Allocation
*/
{
XW_EXT_MARKMAP *pmarkmap = (XW_EXT_MARKMAP*) Xw_malloc(size) ;
int i ;

        if( pmarkmap ) {
	    pmarkmap->type = MARKMAP_TYPE ;
            pmarkmap->link = PmarkmapList ;
            PmarkmapList = pmarkmap ;
	    pmarkmap->connexion = NULL ;
	    pmarkmap->maxmarker = 0 ;
	    pmarkmap->maxwindow = 0 ;
	    for( i=0 ; i<MAXMARKER ; i++ ) {
		pmarkmap->marks[i] = 0 ;
		pmarkmap->npoint[i] = 0 ;
	        pmarkmap->width[i] = 0 ;
	        pmarkmap->height[i] = 0 ;
	    }
        } else {
	    /*EXT_MARKMAP Allocation failed*/
	    Xw_set_error(12,"Xw_add_markmap_structure",0) ;
        }

        return (pmarkmap) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_close_markmap(void* amarkmap)
#else
XW_STATUS Xw_close_markmap(amarkmap)
void* amarkmap ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_MARKMAP *pmarkmap = (XW_EXT_MARKMAP*) amarkmap ;
XW_STATUS status ;

    if( !Xw_isdefine_markmap(pmarkmap) ) {
        /*Bad EXT_MARKMAP Address*/
        Xw_set_error(46,"Xw_close_markmap",pmarkmap) ;
        return (XW_ERROR) ;
    }

    status = Xw_del_markmap_structure(pmarkmap) ;

#ifdef TRACE_DEF_MARKMAP
if( Xw_get_trace() ) {
    printf(" %d = Xw_close_markmap(%lx)\n",status,(long ) pmarkmap) ;
}
#endif

    return (status) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_del_markmap_structure(XW_EXT_MARKMAP* amarkmap)
#else
XW_STATUS Xw_del_markmap_structure(amarkmap)
XW_EXT_MARKMAP *amarkmap;
#endif /*XW_PROTOTYPE*/
/*
        Remove the Extended markmap address from the Extended List

        returns ERROR if the markmap address is not Found in the list
        returns SUCCESS if successful
*/
{
XW_EXT_MARKMAP *pmarkmap = PmarkmapList ;
int i ;

    if( !amarkmap ) return (XW_ERROR) ;

    if( amarkmap->maxwindow ) {
	return (XW_ERROR) ;
    } else {
	for( i=0 ; i<MAXMARKER ; i++) {
	    if( amarkmap->marks[i] ) {
		XFreePixmap(_MDISPLAY,amarkmap->marks[i]) ;
//		amarkmap->marks[i] = NULL;
		amarkmap->marks[i] = 0;
	    }
	    if( amarkmap->npoint[i] > 0 ) {
		Xw_free(amarkmap->spoint[i]);
		Xw_free(amarkmap->xpoint[i]);
		Xw_free(amarkmap->ypoint[i]);
	        amarkmap->npoint[i] = 0;
	    }
	}

        if( amarkmap == pmarkmap ) {
            PmarkmapList = (XW_EXT_MARKMAP*) amarkmap->link ;
        } else {
            for( ; pmarkmap ; pmarkmap = (XW_EXT_MARKMAP*) pmarkmap->link ) {
                if( pmarkmap->link == amarkmap ) {
                    pmarkmap->link = amarkmap->link ;
                    break ;
                }
            }
        }
	Xw_free(amarkmap) ;
    }
    return (XW_SUCCESS) ;
}
