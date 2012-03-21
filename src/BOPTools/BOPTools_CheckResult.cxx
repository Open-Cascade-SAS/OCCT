// Created on: 2004-09-02
// Created by: Oleg FEDYAEV
// Copyright (c) 2004-2012 OPEN CASCADE SAS
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
