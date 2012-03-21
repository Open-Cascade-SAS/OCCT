// Created on: 2008-01-21
// Created by: Galina KULIKOVA
// Copyright (c) 2008-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
