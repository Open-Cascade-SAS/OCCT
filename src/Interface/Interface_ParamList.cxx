// File:      Interface_ParamList.cxx
// Created:   21.01.08 11:24
// Author:    Galina KULIKOVA
// Copyright: OpenCASCADE 2008

#include <Interface_ParamList.ixx>

//=======================================================================
//function : Interface_ParamList
//purpose  : 
//=======================================================================

Interface_ParamList::Interface_ParamList(const Standard_Integer theIncrement) :
        myVector (theIncrement)
{
  
}

//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================

void Interface_ParamList::SetValue(const Standard_Integer theIndex,const Interface_FileParameter& theValue) 
{
  Standard_Integer ind = theIndex-1;
  myVector.SetValue(ind,theValue);
}

 
//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

const Interface_FileParameter& Interface_ParamList::Value(const Standard_Integer theIndex) const
{
  Standard_Integer ind = theIndex-1;
  return myVector.Value(ind);
}

 
//=======================================================================
//function : ChangeValue
//purpose  : 
//=======================================================================

Interface_FileParameter& Interface_ParamList::ChangeValue(const Standard_Integer theIndex) 
{
  Standard_Integer ind = theIndex-1;
  if(ind >= myVector.Length())
  {
    Interface_FileParameter aFP;
    myVector.SetValue(ind,aFP);
  }    
  return myVector.ChangeValue(ind);
}

void Interface_ParamList::Clear()
{
  myVector.Clear();
}
