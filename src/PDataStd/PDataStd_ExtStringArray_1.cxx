// File:	PDataStd_ExtStringArray_1.cxx
// Created:	Thu Mar 27 17:12:47 2008
// Author:	Sergey ZARITCHNY
//		<sergey.zaritchny@opencascade.com>
// Copyright:   Open CasCade SA 2008


#include <PDataStd_ExtStringArray_1.ixx>

//=======================================================================
//function : PDataStd_ExtStringArray_1
//purpose  : 
//=======================================================================

PDataStd_ExtStringArray_1::PDataStd_ExtStringArray_1 () { }

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void PDataStd_ExtStringArray_1::Init(const Standard_Integer lower,
				   const Standard_Integer upper)
{
  myValue = new PColStd_HArray1OfExtendedString(lower, upper);
}


//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================

void PDataStd_ExtStringArray_1::SetValue(const Standard_Integer index,
				       const Handle(PCollection_HExtendedString)& value)
{
  myValue->SetValue(index, value);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Handle(PCollection_HExtendedString) PDataStd_ExtStringArray_1::Value( const Standard_Integer index ) const
{
  return myValue->Value(index);
}

//=======================================================================
//function : Lower
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_ExtStringArray_1::Lower (void) const 
{
  return myValue->Lower();
}


//=======================================================================
//function : Upper
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_ExtStringArray_1::Upper (void) const 
{
  return myValue->Upper();
}

//=======================================================================
//function : SetDelta
//purpose  : 
//=======================================================================
void PDataStd_ExtStringArray_1::SetDelta(const Standard_Boolean delta)
{
  myDelta = delta;
}

//=======================================================================
//function : GetDelta
//purpose  : 
//=======================================================================
Standard_Boolean PDataStd_ExtStringArray_1::GetDelta() const
{
  return myDelta;
}

