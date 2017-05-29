// Created on: 2017-06-27
// Created by: Andrey Betenev
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <TopoDS_AlertWithShape.hxx>

#include <Message_Msg.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TopoDS_AlertWithShape,Message_Alert)

//=======================================================================
//function : TopoDS_AlertWithShape
//purpose  : 
//=======================================================================

TopoDS_AlertWithShape::TopoDS_AlertWithShape (const TopoDS_Shape& theShape)
{
  myShape = theShape;
}

//=======================================================================
//function : SupportsMerge
//purpose  : 
//=======================================================================

Standard_Boolean TopoDS_AlertWithShape::SupportsMerge () const
{
  return Standard_False;
}
  
//=======================================================================
//function : Merge
//purpose  : 
//=======================================================================

Standard_Boolean TopoDS_AlertWithShape::Merge (const Handle(Message_Alert)& /*theTarget*/)
{
  return Standard_False;
//  Handle(TopoDS_AlertWithShape) aTarget = Handle(TopoDS_AlertWithShape)::DownCast (theTarget);
//  return aTarget->GetShape() == myShape;
}
