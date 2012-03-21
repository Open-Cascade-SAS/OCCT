// Copyright (c) 1995-1999 Matra Datavision
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


#include <Xw_WidthMap.ixx>
#include <Aspect_WidthOfLine.hxx>

// Routines C a declarer en extern
//extern "C" {
#include <Xw_Cextern.hxx>
//}

//-Static data definitions

static char *ErrorMessag ;
static int ErrorNumber ;
static int ErrorGravity ;
static XW_STATUS status ;

Xw_WidthMap::Xw_WidthMap () {
}

Xw_WidthMap::Xw_WidthMap (const Standard_CString Connexion) {

        MyExtendedDisplay = Xw_open_display((Standard_PCharacter)Connexion) ;

        if( !MyExtendedDisplay ) {
            ErrorMessag = Xw_get_error(&ErrorNumber,&ErrorGravity) ;
            if( ErrorGravity )
                Aspect_WidthMapDefinitionError::Raise (ErrorMessag) ;
            else Xw_print_error() ;
        }

	MyExtendedWidthMap = Xw_def_widthmap(MyExtendedDisplay,(int)0) ;
	
	if( !Xw_isdefine_widthmap(MyExtendedWidthMap) ) {
	    ErrorMessag = Xw_get_error(&ErrorNumber,&ErrorGravity) ; 
	    if( ErrorGravity ) 
		Aspect_WidthMapDefinitionError::Raise (ErrorMessag) ;
	    else Xw_print_error() ;
	} 
}

void Xw_WidthMap::SetEntry (const Aspect_WidthMapEntry& Entry) {
int fwidth = (int) Entry.Index() ;
float width = (float) TOMILLIMETER(Entry.Width()) ;

    if( fwidth ) {	/* Don't change Default index 0,One pixel Width */
	status = Xw_def_width(MyExtendedWidthMap,fwidth,width) ;

	if( !status ) {
	    ErrorMessag = Xw_get_error(&ErrorNumber,&ErrorGravity) ; 
	    if( ErrorGravity > 2 ) 
		Aspect_BadAccess::Raise (ErrorMessag) ;
	    else Xw_print_error() ;
	} 
    }

}

void Xw_WidthMap::SetEntries (const Handle(Aspect_WidthMap)& Widthmap) {
Standard_Integer size = Widthmap->Size() ;
Standard_Integer i ;

        if( !Xw_isdefine_widthmap(MyExtendedWidthMap) ) {
            ErrorMessag = Xw_get_error(&ErrorNumber,&ErrorGravity) ;
            if( ErrorGravity > 2 )
                Aspect_WidthMapDefinitionError::Raise (ErrorMessag) ;
            else Xw_print_error() ;
        }

        for( i=1 ; i<=size ; i++ ) {
            SetEntry(Widthmap->Entry(i)) ;
        }
}

void Xw_WidthMap::Destroy() {

        if( MyExtendedWidthMap ) {
            Xw_close_widthmap(MyExtendedWidthMap) ;
            MyExtendedWidthMap = NULL ;
        }
}

Standard_Integer Xw_WidthMap::FreeWidths( ) const {
Standard_Integer nwidth ;
int mwidth,uwidth,dwidth,fwidth ;

	status = Xw_get_widthmap_info(MyExtendedWidthMap,
				&mwidth,&uwidth,&dwidth,&fwidth) ;

	if( !status ) {
	    ErrorMessag = Xw_get_error(&ErrorNumber,&ErrorGravity) ; 
	    if( ErrorGravity > 2 ) 
		Aspect_BadAccess::Raise (ErrorMessag) ;
	    else Xw_print_error() ;
	} 

	nwidth = uwidth - dwidth ;

	return (nwidth) ;
}

Standard_Address Xw_WidthMap::ExtendedWidthMap () const {

        return (MyExtendedWidthMap);

}
