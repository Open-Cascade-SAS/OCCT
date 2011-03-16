// File:	PDataStd_IntegerArray_1.cxx
// Created:	Wed Mar 26 18:23:45 2008
// Author:	Sergey ZARITCHNY
//		<sergey.zaritchny@opencascade.com>
// Copyright:   Open CasCade SA 2008

#include <PDataStd_IntegerArray_1.ixx>

//=======================================================================
//function : PDataStd_IntegerArray_1
//purpose  : 
//=======================================================================

PDataStd_IntegerArray_1::PDataStd_IntegerArray_1 () { }

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void PDataStd_IntegerArray_1::Init(const Standard_Integer lower,
			    const Standard_Integer upper)
{
  myValue = new PColStd_HArray1OfInteger(lower, upper);
}


//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================

void PDataStd_IntegerArray_1::SetValue(const Standard_Integer index, 
				       const Standard_Integer value)
{
  myValue->SetValue(index, value);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Integer PDataStd_IntegerArray_1::Value
                                   ( const Standard_Integer index ) const
{
  return myValue->Value(index);
}

//=======================================================================
//function : Lower
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_IntegerArray_1::Lower (void) const 
{ return myValue->Lower(); }


//=======================================================================
//function : Upper
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_IntegerArray_1::Upper (void) const 
{ return myValue->Upper(); }

//=======================================================================
//function : SetDelta
//purpose  : 
//=======================================================================
void PDataStd_IntegerArray_1::SetDelta(const Standard_Boolean delta)
{
  myDelta = delta;
}

//=======================================================================
//function : GetDelta
//purpose  : 
//=======================================================================
Standard_Boolean PDataStd_IntegerArray_1::GetDelta() const
{
  return myDelta;
}
