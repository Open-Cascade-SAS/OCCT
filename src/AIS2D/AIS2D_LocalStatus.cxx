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
