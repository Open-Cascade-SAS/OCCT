
#include <Xw_FontMap.ixx>
#include <Aspect_TypeOfFont.hxx>
#include <Aspect_FontStyle.hxx>
#include <Standard_ShortReal.hxx>

// Routines C a declarer en extern
//extern "C" {
#include <Xw_Cextern.hxx>
//}

//-Static data definitions

static char *ErrorMessag ;
static int ErrorNumber ;
static int ErrorGravity ;
static XW_STATUS status ;

Xw_FontMap::Xw_FontMap () {
}

Xw_FontMap::Xw_FontMap (const Standard_CString Connexion) {

        MyExtendedDisplay = Xw_open_display((Standard_PCharacter)Connexion) ;

        if( !MyExtendedDisplay ) {
            ErrorMessag = Xw_get_error(&ErrorNumber,&ErrorGravity) ;
            if( ErrorGravity )
                Aspect_FontMapDefinitionError::Raise (ErrorMessag) ;
            else Xw_print_error() ;
        }

	MyExtendedFontMap = Xw_def_fontmap(MyExtendedDisplay,(int)0) ;
	
	if( !Xw_isdefine_fontmap(MyExtendedFontMap) ) {
	    ErrorMessag = Xw_get_error(&ErrorNumber,&ErrorGravity) ; 
	    if( ErrorGravity ) 
		Aspect_FontMapDefinitionError::Raise (ErrorMessag) ;
	    else Xw_print_error() ;
	} 
}

void Xw_FontMap::SetEntry (const Aspect_FontMapEntry& Entry) {
Aspect_FontStyle style = Entry.Type() ;
int ffont = (int) Entry.Index() ;
Standard_CString value = style.FullName() ;
float csize = TOMILLIMETER(style.Size()) ;

    if( ffont ) {	/* Don't modify Index 0,Default font */
	status = Xw_def_font(MyExtendedFontMap,ffont,csize,(char*)value);

	if( !status ) {
	    ErrorMessag = Xw_get_error(&ErrorNumber,&ErrorGravity) ; 
	    if( ErrorGravity > 2 ) 
		Aspect_BadAccess::Raise (ErrorMessag) ;
	    else Xw_print_error() ;
	}
    } 

}

void Xw_FontMap::SetEntries (const Handle(Aspect_FontMap)& Fontmap) {
Standard_Integer size = Fontmap->Size() ;
Standard_Integer i ;

        if( !Xw_isdefine_fontmap(MyExtendedFontMap) ) {
            ErrorMessag = Xw_get_error(&ErrorNumber,&ErrorGravity) ;
            if( ErrorGravity > 2 )
                Aspect_FontMapDefinitionError::Raise (ErrorMessag) ;
            else Xw_print_error() ;
        }

        for( i=1 ; i<=size ; i++ ) {
            SetEntry(Fontmap->Entry(i)) ;
        }
}

void Xw_FontMap::Destroy() {

        if( MyExtendedFontMap ) {
            Xw_close_fontmap(MyExtendedFontMap) ;
            MyExtendedFontMap = NULL ;
        }
}

Standard_Integer Xw_FontMap::FreeFonts( ) const {
Standard_Integer nfont ;
int mfont,ufont,dfont,ffont ;

	status = Xw_get_fontmap_info(MyExtendedFontMap,
				&mfont,&ufont,&dfont,&ffont) ;

	if( !status ) {
	    ErrorMessag = Xw_get_error(&ErrorNumber,&ErrorGravity) ; 
	    if( ErrorGravity > 2 ) 
		Aspect_BadAccess::Raise (ErrorMessag) ;
	    else Xw_print_error() ;
	} 

	nfont = ufont - dfont ;

	return (nfont) ;
}

Standard_Address Xw_FontMap::ExtendedFontMap () const {

        return (MyExtendedFontMap);

}
