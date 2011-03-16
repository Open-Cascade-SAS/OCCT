// File:        PDataStd_RealList.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <PDataStd_RealList.ixx>

//=======================================================================
//function : PDataStd_RealList
//purpose  : 
//=======================================================================
PDataStd_RealList::PDataStd_RealList() 
{ 

}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void PDataStd_RealList::Init(const Standard_Integer lower,
			     const Standard_Integer upper)
{
  if (upper >= lower)
    myValue = new PColStd_HArray1OfReal(lower, upper);
}

//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================
void PDataStd_RealList::SetValue(const Standard_Integer index, const Standard_Real value)
{
  myValue->SetValue(index, value);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
Standard_Real PDataStd_RealList::Value( const Standard_Integer index ) const
{
  return myValue->Value(index);
}

//=======================================================================
//function : Lower
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_RealList::Lower (void) const 
{ 
  if (!myValue.IsNull())
    return myValue->Lower(); 
  return 0;
}

//=======================================================================
//function : Upper
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_RealList::Upper (void) const 
{ 
  if (!myValue.IsNull())
    return myValue->Upper(); 
  return -1;
}
