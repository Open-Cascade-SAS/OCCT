
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_DEF_FONTMAP
#endif

/*
   XW_EXT_FONTMAP* Xw_def_fontmap(adisplay,nfont):
   XW_EXT_DISPLAY *adisplay Extended Display structure

   int nfont		Number of font cells to be allocated

	Create a fontmap extension 
        allocate the font cells in the fontmap as if possible
	depending of the MAXFONT define .

	Returns Fontmap extension address if successuful
		or NULL if ERROR

   STATUS = Xw_close_fontmap(afontmap)
   XW_EXT_FONTMAP* afontmap Extended fontmap


        Destroy The Extended TypeMap

        Returns ERROR if Bad Extended TypeMap Address
                SUCCESS if successfull

*/

#ifdef XW_PROTOTYPE
void* Xw_def_fontmap (void* adisplay,int nfont)
#else
void* Xw_def_fontmap (adisplay,nfont)
void *adisplay ;
int nfont ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*)adisplay ;
XW_EXT_FONTMAP *pfontmap = NULL ;
XFontStruct *dfstruct ;
XGCValues values ;
GC gc ;
//int i,font,psize ;
int i,psize ;
char *dfstring = NULL ;

    if( !Xw_isdefine_display(pdisplay) ) {
        /*ERROR*Bad EXT_DISPLAY Address*/
        Xw_set_error(96,"Xw_def_fontmap",pdisplay) ;
        return (NULL) ;
    }

    gc = DefaultGCOfScreen(_DSCREEN) ;
    XGetGCValues(_DDISPLAY,gc,GCFont,&values) ;
    dfstruct = XQueryFont(_DDISPLAY,XGContextFromGC(gc)) ;
    for( i=0 ; i<dfstruct->n_properties ; i++ ) {
	if( dfstruct->properties[i].name == XA_FONT ) {
	    dfstring = XGetAtomName(_DDISPLAY,dfstruct->properties[i].card32) ;
	    break ;
	}
    }


    if( !(pfontmap = Xw_add_fontmap_structure(sizeof(XW_EXT_FONTMAP))) ) 
								return (NULL) ;

    if( nfont <= 0 ) nfont = MAXFONT ;


    pfontmap->connexion = pdisplay ;
    pfontmap->maxfont = min(nfont,MAXFONT) ; 
    pfontmap->gnames[0] = (char*) "Defaultfont";
    pfontmap->snames[0] = dfstring ;
    pfontmap->fonts[0] = dfstruct ;
    pfontmap->fonts[0]->fid = values.font ;

    psize = (pfontmap->fonts[0])->max_bounds.ascent + 
				(pfontmap->fonts[0])->max_bounds.descent ;

    pfontmap->gsizes[0] = (float)psize*HeightMMOfScreen(_DSCREEN)/
                                               (float)HeightOfScreen(_DSCREEN) ;
    pfontmap->fsizes[0] = pfontmap->gsizes[0];
    pfontmap->fratios[0] = 0.;
    pfontmap->ssizex[0] = pfontmap->ssizey[0] = pfontmap->fsizes[0] ;
    pfontmap->gslants[0] = pfontmap->sslants[0] = 0. ;

#ifdef TRACE_DEF_FONTMAP
if( Xw_get_trace() ) {
    printf(" %lx = Xw_def_fontmap(%lx,%d)\n", (long ) pfontmap,(long ) adisplay,nfont) ;
}
#endif

    return (pfontmap);
}

static XW_EXT_FONTMAP *PfontmapList =NULL ;

#ifdef XW_PROTOTYPE
XW_EXT_FONTMAP* Xw_add_fontmap_structure(int size)
#else
XW_EXT_FONTMAP* Xw_add_fontmap_structure(size)
int size ;
#endif /*XW_PROTOTYPE*/
/*
        Create and Insert one Extended fontmap structure in the
        EXtended fontmap List

        returns Extended fontmap address if successful
                or NULL if Bad Allocation
*/
{
XW_EXT_FONTMAP *pfontmap = (XW_EXT_FONTMAP*) Xw_malloc(size) ;
int i ;

        if( pfontmap ) {
	    pfontmap->type = FONTMAP_TYPE ;
            pfontmap->link = PfontmapList ;
            PfontmapList = pfontmap ;
	    pfontmap->connexion = NULL ;
	    pfontmap->maxfont = 0 ;
	    pfontmap->maxwindow = 0 ;
	    for( i=0 ; i<MAXFONT ; i++ ) {
		pfontmap->gnames[i] = NULL ;
		pfontmap->snames[i] = NULL ;
		pfontmap->fonts[i] = NULL ;
		pfontmap->gsizes[i] = 0. ;
		pfontmap->fsizes[i] = 0. ;
		pfontmap->ssizex[i] = 0. ;
		pfontmap->ssizey[i] = 0. ;
		pfontmap->gslants[i] = 0. ;
		pfontmap->sslants[i] = 0. ;
		pfontmap->fratios[i] = 0. ;
	    }
        } else {
	    /*EXT_FONTMAP allocation failed*/
	    Xw_set_error(9,"Xw_add_fontmap_structure",NULL) ;
        }

        return (pfontmap) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_close_fontmap(void* afontmap)
#else
XW_STATUS Xw_close_fontmap(afontmap)
void* afontmap ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_FONTMAP* pfontmap = (XW_EXT_FONTMAP*) afontmap ;
XW_STATUS status ;

    if( !Xw_isdefine_fontmap(pfontmap) ) {
        /*Bad EXT_FONTMAP Address*/
        Xw_set_error(51,"Xw_close_fontmap",pfontmap) ;
        return (XW_ERROR) ;
    }

    status = Xw_del_fontmap_structure(pfontmap) ;

#ifdef TRACE_DEF_FONTMAP
if( Xw_get_trace() ) {
    printf(" %d = Xw_close_fontmap(%lx)\n",status,(long ) pfontmap) ;
}
#endif

    return (status) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_del_fontmap_structure(XW_EXT_FONTMAP* afontmap)
#else
XW_STATUS Xw_del_fontmap_structure(afontmap)
XW_EXT_FONTMAP *afontmap;
#endif /*XW_PROTOTYPE*/
/*
        Remove the Extended fontmap address from the Extended List

        returns ERROR if the fontmap address is not Found in the list
        returns SUCCESS if successful
*/
{
XW_EXT_FONTMAP *pfontmap = PfontmapList ;
int i ;

    if( !afontmap ) return (XW_ERROR) ;

    if( afontmap->maxwindow ) {
	return (XW_ERROR) ;
    } else {
	for( i=1 ; i<MAXFONT ; i++) {
	    if( afontmap->fonts[i] ) {
		if( afontmap->gnames[i] ) Xw_free(afontmap->gnames[i]) ;
		if( afontmap->snames[i] ) Xw_free(afontmap->snames[i]) ;
		if( afontmap->fonts[i]->fid != afontmap->fonts[0]->fid )
				XFreeFont(_FDISPLAY,afontmap->fonts[i]) ;
	    }
	}

        if( afontmap == pfontmap ) {
            PfontmapList = (XW_EXT_FONTMAP*) afontmap->link ;
        } else {
            for( ; pfontmap ; pfontmap = (XW_EXT_FONTMAP*) pfontmap->link ) {
                if( pfontmap->link == afontmap ) {
                    pfontmap->link = afontmap->link ;
                    break ;
                }
            }
        }
	Xw_free(afontmap) ;
    }
    return (XW_SUCCESS) ;
}
