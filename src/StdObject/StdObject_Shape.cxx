// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <StdObject_Shape.hxx>
#include <StdObjMgt_ReadData.hxx>


//=======================================================================
//function : Import
//purpose  : Import transient object from the persistent data
//=======================================================================
TopoDS_Shape StdObject_Shape::Import() const
{
  TopoDS_Shape aShape;

  if (myTShape)
    aShape.TShape (myTShape->Import());

  aShape.Location (myLocation.Import());
  aShape.Orientation (static_cast<TopAbs_Orientation> (myOrient));

  return aShape;
}

//=======================================================================
//function : PChildren
//purpose  : 
//=======================================================================
void StdObject_Shape::PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  theChildren.Append(myTShape);
  myLocation.PChildren(theChildren);
}