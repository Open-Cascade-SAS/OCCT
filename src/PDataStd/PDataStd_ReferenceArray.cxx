// File:        PDataStd_ReferenceArray.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <PDataStd_ReferenceArray.ixx>

//=======================================================================
//function : PDataStd_ReferenceArray
//purpose  : 
//=======================================================================
PDataStd_ReferenceArray::PDataStd_ReferenceArray() 
{ 

}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void PDataStd_ReferenceArray::Init(const Standard_Integer lower,
				   const Standard_Integer upper)
{
  if (upper >= lower)
    myValue = new PColStd_HArray1OfExtendedString(lower, upper);
}

//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================
void PDataStd_ReferenceArray::SetValue(const Standard_Integer index, const Handle(PCollection_HExtendedString)& value)
{
  myValue->SetValue(index, value);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
Handle(PCollection_HExtendedString) PDataStd_ReferenceArray::Value( const Standard_Integer index ) const
{
  return myValue->Value(index);
}

//=======================================================================
//function : Lower
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_ReferenceArray::Lower (void) const 
{ 
  if (!myValue.IsNull())
    return myValue->Lower(); 
  return 0;
}

//=======================================================================
//function : Upper
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_ReferenceArray::Upper (void) const 
{ 
  if (!myValue.IsNull())
    return myValue->Upper(); 
  return -1;
}
