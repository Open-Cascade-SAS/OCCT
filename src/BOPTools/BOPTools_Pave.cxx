// File:	BOPTools_Pave.cxx
// Created:	Thu Feb  8 12:32:40 2001
// Author:	Peter KURNEV
//		<pkv@irinox>

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
