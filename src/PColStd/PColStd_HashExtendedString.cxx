
//-Copyright:	 Matra Datavision 1992,1993
//-Version: 

//-History:
//  Version	Date         Purpose
//              01/04/93     Creation   

//-Language     C++

//-Declarations
#include <Standard_Integer.hxx>
#include <PColStd_HashExtendedString.ixx>
#include <Standard_RangeError.hxx>
#include <PCollection_HExtendedString.hxx>

//=======================================================================
// Function : Create 
//=======================================================================

PColStd_HashExtendedString::PColStd_HashExtendedString()
{}

//=======================================================================
// Function : HashCode
//=======================================================================

Standard_Integer PColStd_HashExtendedString::HashCode
  (const Handle(PCollection_HExtendedString) &MyKey,
   const Standard_Integer Upper) const
{
  Standard_Integer total=0;

  if ( Upper < 1 ){
    Standard_RangeError::
      Raise("Try to apply HasCode method with negative or null argument.");
  }
  for (Standard_Integer I = 1 ; I <= MyKey->Length() ; I++ ) {
    total = total + (Standard_Integer) (MyKey->Value(I));
  }
  total = ::HashCode( total , Upper ) ;

  return total;
}

//=======================================================================
// Function : Compare
//=======================================================================

Standard_Boolean PColStd_HashExtendedString::Compare
  (const Handle(PCollection_HExtendedString) &One,
   const Handle(PCollection_HExtendedString) &Two) const
{
  return (One->IsSameString(Two));
}
