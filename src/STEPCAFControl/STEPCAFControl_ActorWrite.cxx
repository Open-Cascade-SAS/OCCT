// Created on: 2000-10-05
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <STEPCAFControl_ActorWrite.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(STEPCAFControl_ActorWrite, STEPControl_ActorWrite)

//=================================================================================================

STEPCAFControl_ActorWrite::STEPCAFControl_ActorWrite()
    : myStdMode(Standard_True)
{
}

//=================================================================================================

void STEPCAFControl_ActorWrite::SetStdMode(const Standard_Boolean stdmode)
{
  myStdMode = stdmode;
  if (myStdMode)
    ClearMap();
}

//=================================================================================================

void STEPCAFControl_ActorWrite::ClearMap()
{
  myMap.Clear();
}

//=================================================================================================

void STEPCAFControl_ActorWrite::RegisterAssembly(const TopoDS_Shape& S)
{
  if (!myStdMode && S.ShapeType() == TopAbs_COMPOUND)
    myMap.Add(S);
}

//=================================================================================================

Standard_Boolean STEPCAFControl_ActorWrite::IsAssembly(const Handle(StepData_StepModel)& theModel,
                                                       TopoDS_Shape&                     S) const
{
  if (myStdMode)
    return STEPControl_ActorWrite::IsAssembly(theModel, S);
  return myMap.Contains(S);
}
