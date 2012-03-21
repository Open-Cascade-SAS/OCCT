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


#include <Xw_TypeMap.ixx>
#include <Aspect_TypeOfLine.hxx>
#include <Aspect_LineStyle.hxx>
#include <Standard_ShortReal.hxx>
#include <TShort_Array1OfShortReal.hxx>
#include <TColQuantity_Array1OfLength.hxx>

// Routines C a declarer en extern
//extern "C" {
#include <Xw_Cextern.hxx>
//}

//-Static data definitions

static char *ErrorMessag ;
static int ErrorNumber ;
static int ErrorGravity ;
static XW_STATUS status ;

Xw_TypeMap::Xw_TypeMap () {
}

Xw_TypeMap::Xw_TypeMap (const Standard_CString Connexion) {

        MyExtendedDisplay = Xw_open_display((Standard_PCharacter)Connexion) ;

        if( !MyExtendedDisplay ) {
            ErrorMessag = Xw_get_error(&ErrorNumber,&ErrorGravity) ;
            if( ErrorGravity )
                Aspect_TypeMapDefinitionError::Raise (ErrorMessag) ;
            else Xw_print_error() ;
        }

	MyExtendedTypeMap = Xw_def_typemap(MyExtendedDisplay,(int)0) ;
	
	if( !Xw_isdefine_typemap(MyExtendedTypeMap) ) {
	    ErrorMessag = Xw_get_error(&ErrorNumber,&ErrorGravity) ; 
	    if( ErrorGravity ) 
		Aspect_TypeMapDefinitionError::Raise (ErrorMessag) ;
	    else Xw_print_error() ;
	} 
}

void Xw_TypeMap::SetEntry (const Aspect_TypeMapEntry& Entry) {
Aspect_LineStyle style = Entry.Type() ;
int length = (int) style.Length() ;
int ftype = (int) Entry.Index() ;
const TColQuantity_Array1OfLength& values(style.Values()) ;
TShort_Array1OfShortReal svalues(values.Lower(),values.Length()) ;
float *V = (float*) &svalues.Value(values.Lower()) ;
Standard_Integer i ;

    if( ftype ) {	/* Index 0 is default SOLID Line ,cann't be changed */
	for( i=values.Lower() ; i<=values.Upper() ; i++) 
		svalues(i) = TOMILLIMETER(Standard_ShortReal(values(i))) ;
	
	status = Xw_def_type(MyExtendedTypeMap,ftype,length,V) ;

	if( !status ) {
	    ErrorMessag = Xw_get_error(&ErrorNumber,&ErrorGravity) ; 
	    if( ErrorGravity > 2 ) 
		Aspect_BadAccess::Raise (ErrorMessag) ;
	    else Xw_print_error() ;
	} 
    }
}

void Xw_TypeMap::SetEntries (const Handle(Aspect_TypeMap)& Typemap) {
Standard_Integer size = Typemap->Size() ;
Standard_Integer i ;

        if( !Xw_isdefine_typemap(MyExtendedTypeMap) ) {
            ErrorMessag = Xw_get_error(&ErrorNumber,&ErrorGravity) ;
            if( ErrorGravity > 2 )
                Aspect_TypeMapDefinitionError::Raise (ErrorMessag) ;
            else Xw_print_error() ;
        }

        for( i=1 ; i<=size ; i++ ) {
            SetEntry(Typemap->Entry(i)) ;
        }
}

void Xw_TypeMap::Destroy() {

        if( MyExtendedTypeMap ) {
            Xw_close_typemap(MyExtendedTypeMap) ;
            MyExtendedTypeMap = NULL ;
        }
}

Standard_Integer Xw_TypeMap::FreeTypes( ) const {
Standard_Integer ntype ;
int mtype,utype,dtype,ftype ;

	status = Xw_get_typemap_info(MyExtendedTypeMap,
				&mtype,&utype,&dtype,&ftype) ;

	if( !status ) {
	    ErrorMessag = Xw_get_error(&ErrorNumber,&ErrorGravity) ; 
	    if( ErrorGravity > 2 ) 
		Aspect_BadAccess::Raise (ErrorMessag) ;
	    else Xw_print_error() ;
	} 

	ntype = utype - dtype ;

	return (ntype) ;
}

Standard_Address Xw_TypeMap::ExtendedTypeMap () const {

        return (MyExtendedTypeMap);

}
