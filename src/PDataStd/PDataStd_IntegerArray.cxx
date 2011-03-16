// File:	PDataStd_IntegerArray.cxx
// Created:	Tue Jun  15 13:24:14 1999
// Author:	Sergey RUIN

#include <PDataStd_IntegerArray.ixx>

//=======================================================================
//function : PDataStd_IntegerArray
//purpose  : 
//=======================================================================

PDataStd_IntegerArray::PDataStd_IntegerArray () { }

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void PDataStd_IntegerArray::Init(const Standard_Integer lower,
			    const Standard_Integer upper)
{
  myValue = new PColStd_HArray1OfInteger(lower, upper);
}


//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================

void PDataStd_IntegerArray::SetValue(const Standard_Integer index, const Standard_Integer value)
{
  myValue->SetValue(index, value);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Integer PDataStd_IntegerArray::Value( const Standard_Integer index ) const
{
  return myValue->Value(index);
}

//=======================================================================
//function : Lower
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_IntegerArray::Lower (void) const 
{ return myValue->Lower(); }


//=======================================================================
//function : Upper
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_IntegerArray::Upper (void) const 
{ return myValue->Upper(); }
