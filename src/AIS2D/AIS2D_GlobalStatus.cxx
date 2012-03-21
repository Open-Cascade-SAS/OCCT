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

#include <AIS2D_GlobalStatus.ixx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

static TColStd_ListIteratorOfListOfInteger It;


AIS2D_GlobalStatus::AIS2D_GlobalStatus():
  
   myStatus( AIS2D_DS_None ),
   myDispModes(),
   mySelModes(),
   myLayerIndex( 0 ),
   myIsHighl( Standard_False ),
   myHiCol( Quantity_NOC_WHITE ),
   mySubInt( Standard_False )
{
}

AIS2D_GlobalStatus::AIS2D_GlobalStatus( 
				   const AIS2D_DisplayStatus DS,
				   const Standard_Integer DMode,
				   const Standard_Integer SMode,
				   const Standard_Boolean /*isHighlight*/,
				   const Quantity_NameOfColor aHighlCol,
				   const Standard_Integer aLayerIndex ):
   myStatus( DS ),
   myLayerIndex( aLayerIndex ),
   myIsHighl( Standard_False ),
   myHiCol( aHighlCol ),
   mySubInt( Standard_False )
{
   myDispModes.Append( DMode );
   mySelModes.Append( SMode );
}


void AIS2D_GlobalStatus::RemoveDisplayMode( const Standard_Integer aMode ) {

  for ( It.Initialize( myDispModes ); It.More(); It.Next() )
    if ( It.Value() == aMode ) { 
		myDispModes.Remove( It );
	    return;
	}
}

void AIS2D_GlobalStatus::RemoveSelectionMode( const Standard_Integer aMode) {
  
  for ( It.Initialize( mySelModes ); It.More(); It.Next() ) 
    if ( It.Value() == aMode ) { 
		mySelModes.Remove( It );
		return;
	}
}

void AIS2D_GlobalStatus::ClearSelectionModes() {
  mySelModes.Clear();
}

void AIS2D_GlobalStatus::AddSelectionMode( const Standard_Integer aMode ) {
  if ( !IsSModeIn( aMode ) )
	mySelModes.Append( aMode );
}

Standard_Boolean AIS2D_GlobalStatus::IsDModeIn( const Standard_Integer aMode) const {
  for ( It.Initialize( myDispModes ); It.More(); It.Next() )
    if ( It.Value() == aMode ) return Standard_True;
  return Standard_False;
}

Standard_Boolean AIS2D_GlobalStatus::IsSModeIn( const Standard_Integer aMode) const {
  for ( It.Initialize( mySelModes ); It.More(); It.Next() )
    if ( It.Value() == aMode ) return Standard_True;
  return Standard_False;
}

void AIS2D_GlobalStatus::AddDisplayMode( const Standard_Integer aMode ) {
  if ( !IsDModeIn( aMode ) ) 
	 myDispModes.Append( aMode );
}

