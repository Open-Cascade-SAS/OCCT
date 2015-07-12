// Created on: 1993-01-21
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#define No_Standard_NoSuchObject


#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================
void TopoDS_Iterator::Initialize(const TopoDS_Shape& S,
                                 const Standard_Boolean cumOri,
                                 const Standard_Boolean cumLoc)
{
  if (cumLoc)
    myLocation = S.Location();
  else
    myLocation.Identity();
  if (cumOri)
    myOrientation = S.Orientation();
  else
    myOrientation = TopAbs_FORWARD;
  myShapes.Initialize(S.TShape()->Shapes());
  if (More()) {
    myShape = myShapes.Value();
    myShape.Orientation(TopAbs::Compose(myOrientation,myShape.Orientation()));
    //modified by NIZNHY-PKV Fri Jan 16 07:42:30 2009f
    if (!myLocation.IsIdentity())
    //modified by NIZNHY-PKV Fri Jan 16 07:42:37 2009t
      myShape.Move(myLocation);
  }
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TopoDS_Iterator::Next()
{
  myShapes.Next();
  if (More()) {
    myShape = myShapes.Value();
    myShape.Orientation(TopAbs::Compose(myOrientation,myShape.Orientation()));
    //modified by NIZNHY-PKV Fri Jan 16 07:42:30 2009f
    if (!myLocation.IsIdentity())
    //modified by NIZNHY-PKV Fri Jan 16 07:42:37 2009t
      myShape.Move(myLocation);
  }
}
