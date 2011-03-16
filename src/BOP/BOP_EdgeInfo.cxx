// File:	BOP_EdgeInfo.cxx
// Created:	Mon Apr  9 10:29:58 2001
// Author:	Peter KURNEV
//		<pkv@irinox>

#include <BOP_EdgeInfo.ixx>

//=======================================================================
// function: BOP_EdgeInfo::BOP_EdgeInfo
// purpose: 
//=======================================================================
  BOP_EdgeInfo::BOP_EdgeInfo() 
:
  myPassed(Standard_False),
  myInFlag(Standard_False),
  myAngle (-1.)
{}

//=======================================================================
// function: SetEdge
// purpose: 
//=======================================================================
  void BOP_EdgeInfo::SetEdge(const TopoDS_Edge& anEdge)
{
  myEdge=anEdge;
}

//=======================================================================
// function: SetPassed
// purpose: 
//=======================================================================
  void BOP_EdgeInfo::SetPassed(const Standard_Boolean aFlag)
{
  myPassed=aFlag;
}
//=======================================================================
// function: SetInFlag
// purpose: 
//=======================================================================
  void BOP_EdgeInfo::SetInFlag(const Standard_Boolean aFlag)
{
  myInFlag=aFlag;
}

//=======================================================================
// function: SetAngle
// purpose: 
//=======================================================================
  void BOP_EdgeInfo::SetAngle(const Standard_Real anAngle)
{
  myAngle=anAngle;
}

//=======================================================================
// function: Edge
// purpose: 
//=======================================================================
  const TopoDS_Edge& BOP_EdgeInfo::Edge()const
{
  return myEdge;
}

//=======================================================================
// function: Passed
// purpose: 
//=======================================================================
  Standard_Boolean BOP_EdgeInfo::Passed()const 
{
  return myPassed;
}
//=======================================================================
// function: IsIn
// purpose: 
//=======================================================================
  Standard_Boolean BOP_EdgeInfo::IsIn()const 
{
  return myInFlag;
}

//=======================================================================
// function: Angle
// purpose: 
//=======================================================================
  Standard_Real BOP_EdgeInfo::Angle()const 
{
  return myAngle;
}
