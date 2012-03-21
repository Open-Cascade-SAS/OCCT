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


#include <Xw_MarkMap.ixx>
#include <Aspect_TypeOfMarker.hxx>
#include <Aspect_MarkerStyle.hxx>
#include <Standard_ShortReal.hxx>
#include <TShort_Array1OfShortReal.hxx>
#include <TColStd_Array1OfBoolean.hxx>

// Routines C a declarer en extern
//extern "C" {
#include <Xw_Cextern.hxx>
//}

//-Static data definitions

static char *ErrorMessag ;
static int ErrorNumber ;
static int ErrorGravity ;
static XW_STATUS status ;

Xw_MarkMap::Xw_MarkMap () {
}

Xw_MarkMap::Xw_MarkMap (const Standard_CString Connexion) {

        MyExtendedDisplay = Xw_open_display((Standard_PCharacter)Connexion) ;

        if( !MyExtendedDisplay ) {
            ErrorMessag = Xw_get_error(&ErrorNumber,&ErrorGravity) ;
            if( ErrorGravity )
                Aspect_MarkMapDefinitionError::Raise (ErrorMessag) ;
            else Xw_print_error() ;
        }

	MyExtendedMarkMap = Xw_def_markmap(MyExtendedDisplay,(int)0) ;
	
	if( !Xw_isdefine_markmap(MyExtendedMarkMap) ) {
	    ErrorMessag = Xw_get_error(&ErrorNumber,&ErrorGravity) ; 
	    if( ErrorGravity ) 
		Aspect_MarkMapDefinitionError::Raise (ErrorMessag) ;
	    else Xw_print_error() ;
	} 
}

void Xw_MarkMap::SetEntry (const Aspect_MarkMapEntry& anEntry) {
Aspect_MarkerStyle style = anEntry.Style() ;
int length = (int) style.Length() ;
int index = (int) anEntry.Index() ;
const TShort_Array1OfShortReal& xvalues(style.XValues()) ;
const TShort_Array1OfShortReal& yvalues(style.YValues()) ;
const TColStd_Array1OfBoolean&  svalues(style.SValues()) ;
float *X = (float*) &xvalues.Value(xvalues.Lower());
float *Y = (float*) &yvalues.Value(yvalues.Lower());
int *S = (int*) &svalues.Value(svalues.Lower());
Standard_Integer i;

    for( i=1 ; i<=length ; i++ ) {
//	Standard_Real xcoord,ycoord;
//	Standard_Boolean state = style.Values(i,xcoord,ycoord);
    }

    if( index ) {	/* Index 0 is default POINT Marker ,cann't be changed */
	status = Xw_def_marker(MyExtendedMarkMap,index,length,S,X,Y) ;

	if( !status ) {
	    ErrorMessag = Xw_get_error(&ErrorNumber,&ErrorGravity) ; 
	    if( ErrorGravity > 2 ) 
		Aspect_BadAccess::Raise (ErrorMessag) ;
	    else Xw_print_error() ;
	} 
    }
}

void Xw_MarkMap::SetEntries (const Handle(Aspect_MarkMap)& Markmap) {
Standard_Integer size = Markmap->Size() ;
Standard_Integer i ;

        if( !Xw_isdefine_markmap(MyExtendedMarkMap) ) {
            ErrorMessag = Xw_get_error(&ErrorNumber,&ErrorGravity) ;
            if( ErrorGravity > 2 )
                Aspect_MarkMapDefinitionError::Raise (ErrorMessag) ;
            else Xw_print_error() ;
        }

        for( i=1 ; i<=size ; i++ ) {
            SetEntry(Markmap->Entry(i)) ;
        }
}

void Xw_MarkMap::Destroy() {

        if( MyExtendedMarkMap ) {
            Xw_close_markmap(MyExtendedMarkMap) ;
            MyExtendedMarkMap = NULL ;
        }
}

Standard_Integer Xw_MarkMap::FreeMarkers( ) const {
Standard_Integer ntype ;
int mtype,utype,dtype,ftype ;

	status = Xw_get_markmap_info(MyExtendedMarkMap,
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

Standard_Address Xw_MarkMap::ExtendedMarkMap () const {

        return (MyExtendedMarkMap);

}
