// Created on: 1995-12-19
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
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

#include <TopOpeBRepBuild_Loop.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TopOpeBRepBuild_Loop, Standard_Transient)

//=================================================================================================

TopOpeBRepBuild_Loop::TopOpeBRepBuild_Loop(const TopoDS_Shape& S)
    : myIsShape(Standard_True),
      myShape(S),
      myBlockIterator(0, 0)
{
}

//=================================================================================================

TopOpeBRepBuild_Loop::TopOpeBRepBuild_Loop(const TopOpeBRepBuild_BlockIterator& BI)
    : myIsShape(Standard_False),
      myBlockIterator(BI)
{
}

//=================================================================================================

Standard_Boolean TopOpeBRepBuild_Loop::IsShape() const
{
  return myIsShape;
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRepBuild_Loop::Shape() const
{
  return myShape;
}

//=================================================================================================

const TopOpeBRepBuild_BlockIterator& TopOpeBRepBuild_Loop::BlockIterator() const
{
  return myBlockIterator;
}

//=================================================================================================

void TopOpeBRepBuild_Loop::Dump() const {}
