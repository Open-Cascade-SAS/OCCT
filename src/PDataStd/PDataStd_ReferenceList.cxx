// File:        PDataStd_ReferenceList.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <PDataStd_ReferenceList.ixx>

//=======================================================================
//function : PDataStd_ReferenceList
//purpose  : 
//=======================================================================
PDataStd_ReferenceList::PDataStd_ReferenceList() 
{ 

}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void PDataStd_ReferenceList::Init(const Standard_Integer lower,
				  const Standard_Integer upper)
{
  if (upper >= lower)
    myValue = new PColStd_HArray1OfExtendedString(lower, upper);
}

//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================
void PDataStd_ReferenceList::SetValue(const Standard_Integer index, const Handle(PCollection_HExtendedString)& value)
{
  myValue->SetValue(index, value);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
Handle(PCollection_HExtendedString) PDataStd_ReferenceList::Value( const Standard_Integer index ) const
{
  return myValue->Value(index);
}

//=======================================================================
//function : Lower
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_ReferenceList::Lower (void) const 
{ 
  if (!myValue.IsNull())
    return myValue->Lower(); 
  return 0;
}

//=======================================================================
//function : Upper
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_ReferenceList::Upper (void) const 
{ 
  if (!myValue.IsNull())
    return myValue->Upper(); 
  return -1;
}
