// File:	PDataStd_RealArray.cxx
// Created:	Tue Jun  15 13:24:14 1999
// Author:	Sergey RUIN
//Copyright:	Matra Datavision 1999
#include <PDataStd_RealArray.ixx>

//=======================================================================
//function : PDataStd_RealArray
//purpose  : 
//=======================================================================

PDataStd_RealArray::PDataStd_RealArray () { }

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void PDataStd_RealArray::Init(const Standard_Integer lower,
			    const Standard_Integer upper)
{
  myValue = new PColStd_HArray1OfReal(lower, upper);
}


//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================

void PDataStd_RealArray::SetValue(const Standard_Integer index, const Standard_Real value)
{
  myValue->SetValue(index, value);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Real PDataStd_RealArray::Value( const Standard_Integer index ) const
{
  return myValue->Value(index);
}

//=======================================================================
//function : Lower
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_RealArray::Lower (void) const 
{ return myValue->Lower(); }


//=======================================================================
//function : Upper
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_RealArray::Upper (void) const 
{ return myValue->Upper(); }
