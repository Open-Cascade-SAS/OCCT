// File:	PDataStd_ExtStringArray.cxx
// Created:	Wed Jan 16 11:28:25 2002
// Author:	Michael PONIKAROV
//		<mpv@covox>


#include <PDataStd_ExtStringArray.ixx>

//=======================================================================
//function : PDataStd_ExtStringArray
//purpose  : 
//=======================================================================

PDataStd_ExtStringArray::PDataStd_ExtStringArray () { }

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void PDataStd_ExtStringArray::Init(const Standard_Integer lower,
				   const Standard_Integer upper)
{
  myValue = new PColStd_HArray1OfExtendedString(lower, upper);
}


//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================

void PDataStd_ExtStringArray::SetValue(const Standard_Integer index,
				       const Handle(PCollection_HExtendedString)& value)
{
  myValue->SetValue(index, value);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Handle(PCollection_HExtendedString) PDataStd_ExtStringArray::Value( const Standard_Integer index ) const
{
  return myValue->Value(index);
}

//=======================================================================
//function : Lower
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_ExtStringArray::Lower (void) const 
{
  return myValue->Lower();
}


//=======================================================================
//function : Upper
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_ExtStringArray::Upper (void) const 
{
  return myValue->Upper();
}
