// Created on: 2001-02-08
// Created by: Peter KURNEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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


#include <BOPTools_Pave.ixx>

//=======================================================================
// function: BOPTools_Pave::BOPTools_Pave
// purpose: 
//=======================================================================
BOPTools_Pave::BOPTools_Pave() 
  :
  myParam(0.),
  myIndex(0),
  myType(BooleanOperations_UnknownInterference),
  myInterf(0)
{}
//=======================================================================
// function: BOPTools_Pave::BOPTools_Pave
// purpose: 
//=======================================================================
  BOPTools_Pave::BOPTools_Pave(const Standard_Integer anIndex,
			       const Standard_Real aParam, 
			       const BooleanOperations_KindOfInterference aType)
  :
  myParam(aParam),
  myIndex(anIndex),
  myType(aType),
  myInterf(0)
{}
//=======================================================================
// function: SetParam
// purpose: 
//=======================================================================
  void BOPTools_Pave::SetParam(const Standard_Real aParam)
{
  myParam=aParam;
}
//=======================================================================
// function: SetIndex
// purpose: 
//=======================================================================
  void BOPTools_Pave::SetIndex(const Standard_Integer anIndex)
{
  myIndex=anIndex;
}
//=======================================================================
// function: SetType
// purpose: 
//=======================================================================
  void BOPTools_Pave::SetType(const BooleanOperations_KindOfInterference aType)
{
  myType=aType;
}
//=======================================================================
// function: SetInterference
// purpose: 
//=======================================================================
  void BOPTools_Pave::SetInterference(const Standard_Integer anIndex)
{
  myInterf=anIndex;
}
//=======================================================================
// function: Param
// purpose: 
//=======================================================================
  Standard_Real BOPTools_Pave::Param() const
{
  return myParam;
}
//=======================================================================
// function: Index
// purpose: 
//=======================================================================
  Standard_Integer BOPTools_Pave::Index() const
{
  return myIndex;
}
//=======================================================================
// function: Type
// purpose: 
//=======================================================================
  BooleanOperations_KindOfInterference BOPTools_Pave::Type() const
{
  return myType;
}
//=======================================================================
// function: Interference
// purpose: 
//=======================================================================
  Standard_Integer BOPTools_Pave::Interference() const
{
  return myInterf;
}
//=======================================================================
// function: IsEqual
// purpose: 
//=======================================================================
  Standard_Boolean BOPTools_Pave::IsEqual(const BOPTools_Pave& Other) const
{
  Standard_Integer anIndOther;
  Standard_Real    aParamOther, dt=1.e-14;
  
  anIndOther=Other.Index();
  if (anIndOther!=myIndex) {
    return Standard_False;
  }
  
  aParamOther=Other.Param();
  if (fabs (aParamOther-myParam) > dt){
    return Standard_False;
  }
   return Standard_True;
}
