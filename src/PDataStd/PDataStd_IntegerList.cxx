// File:        PDataStd_IntegerList.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <PDataStd_IntegerList.ixx>

//=======================================================================
//function : PDataStd_IntegerList
//purpose  : 
//=======================================================================
PDataStd_IntegerList::PDataStd_IntegerList() 
{ 

}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void PDataStd_IntegerList::Init(const Standard_Integer lower,
				const Standard_Integer upper)
{
  if (upper >= lower)
    myValue = new PColStd_HArray1OfInteger(lower, upper);
}

//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================
void PDataStd_IntegerList::SetValue(const Standard_Integer index, const Standard_Integer value)
{
  myValue->SetValue(index, value);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_IntegerList::Value( const Standard_Integer index ) const
{
  return myValue->Value(index);
}

//=======================================================================
//function : Lower
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_IntegerList::Lower (void) const 
{ 
  if (!myValue.IsNull())
    return myValue->Lower(); 
  return 0;
}

//=======================================================================
//function : Upper
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_IntegerList::Upper (void) const 
{ 
  if (!myValue.IsNull())
    return myValue->Upper(); 
  return -1;
}
