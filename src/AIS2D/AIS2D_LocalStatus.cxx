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

#include <AIS2D_LocalStatus.ixx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

AIS2D_LocalStatus::AIS2D_LocalStatus( const Standard_Boolean      isTemporary,
									  const Standard_Boolean      Decompose,
									  const Standard_Integer      DMode,
									  const Standard_Integer      SMode,
									  const AIS2D_TypeOfDetection HMode,
									  const Standard_Boolean      SubIntensity,
									  const Quantity_NameOfColor  HighlCol ):

    myDecomposition( Decompose ),
    myIsTemporary( isTemporary ),
    myDMode( DMode ),
    myFirstDisplay( Standard_False),
    myHMode( HMode ),
	mySModes(),
    mySubIntensity( SubIntensity ),
    myHighlCol( HighlCol )
	    						
{
   if ( SMode != -1 ) mySModes.Append( SMode );

}

Standard_Boolean AIS2D_LocalStatus::IsActivated(const Standard_Integer aSelMode) const {
  
 TColStd_ListIteratorOfListOfInteger It( mySModes );
 for ( ; It.More(); It.Next() )
   if ( It.Value() == aSelMode )
       return Standard_True;
 return Standard_False;
}

Standard_Boolean AIS2D_LocalStatus::IsSelModeIn(const Standard_Integer aMode) const {
  
  TColStd_ListIteratorOfListOfInteger It( mySModes );	  
  for ( ; It.More(); It.Next() ) 
    if( It.Value() == aMode )
      return Standard_True;
  return Standard_False;
}

 void AIS2D_LocalStatus::AddSelectionMode(const Standard_Integer aMode) {
	   
  if( IsSelModeIn( aMode ) ) return;

  if( aMode != -1 )
    mySModes.Append( aMode );
  else
    mySModes.Clear();
}

void AIS2D_LocalStatus::RemoveSelectionMode(const Standard_Integer aMode) {

  TColStd_ListIteratorOfListOfInteger It(mySModes);
  for ( ; It.More(); It.Next() ) 
    if ( It.Value() == aMode ) {
	     mySModes.Remove( It );
	     return;
     }
}

void AIS2D_LocalStatus::ClearSelectionModes() {
	 mySModes.Clear();
}

void AIS2D_LocalStatus::SubIntensityOff() {
  mySubIntensity = Standard_False;
}

void AIS2D_LocalStatus::SetPreviousState(const Handle(Standard_Transient)& aStatus) {
	 myPrevState = aStatus;
}
