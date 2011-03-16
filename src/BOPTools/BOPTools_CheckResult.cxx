// File:      BOPTools_CheckResult.cxx
// Created:   02.09.04 16:26:31
// Author:    Oleg FEDYAEV
// Copyright: Open Cascade 2003

#include <BOPTools_CheckResult.ixx>

//=======================================================================
//function : BOPTools_CheckResult()
//purpose  : 
//=======================================================================

BOPTools_CheckResult::BOPTools_CheckResult() : myStatus(BOPTools_CHKUNKNOWN), myGeometry(0)
{
}

//=======================================================================
//function : AddShape()
//purpose  : 
//=======================================================================

void BOPTools_CheckResult::AddShape(const TopoDS_Shape & TheShape)
{
  myShapes.Append(TheShape);
}

//=======================================================================
//function : GetShapes()
//purpose  : 
//=======================================================================

const TopTools_ListOfShape & BOPTools_CheckResult::GetShapes() const
{
  return myShapes;
}

//=======================================================================
//function : SetCheckStatus()
//purpose  : 
//=======================================================================

void BOPTools_CheckResult::SetCheckStatus(const BOPTools_CheckStatus TheStatus)
{
  myStatus = TheStatus;
}

//=======================================================================
//function : GetCheckStatus()
//purpose  : 
//=======================================================================

BOPTools_CheckStatus BOPTools_CheckResult::GetCheckStatus() const
{
  return myStatus;
}

//=======================================================================
//function : SetGeometry()
//purpose  : 
//=======================================================================

void BOPTools_CheckResult::SetInterferenceGeometry(const Handle(Geom_Geometry)& TheGeometry)
{
  myGeometry = TheGeometry;
}

//=======================================================================
//function : GetGeometry()
//purpose  : 
//=======================================================================

const Handle(Geom_Geometry)& BOPTools_CheckResult::GetInterferenceGeometry() const 
{
  return myGeometry;
}
