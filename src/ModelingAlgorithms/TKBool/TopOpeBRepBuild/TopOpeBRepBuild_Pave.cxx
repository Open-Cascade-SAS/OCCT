// Created on: 1994-11-14
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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

#include <TopOpeBRepBuild_Pave.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TopOpeBRepBuild_Pave, TopOpeBRepBuild_Loop)

//=================================================================================================

TopOpeBRepBuild_Pave::TopOpeBRepBuild_Pave(const TopoDS_Shape&    V,
                                           const Standard_Real    P,
                                           const Standard_Boolean B)
    : TopOpeBRepBuild_Loop(V),
      myVertex(V),
      myParam(P),
      myIsShape(B),
      myHasSameDomain(Standard_False),
      myIntType(TopOpeBRepDS_FACE)
{
}

//=================================================================================================

void TopOpeBRepBuild_Pave::HasSameDomain(const Standard_Boolean B)
{
  myHasSameDomain = B;
}

//=================================================================================================

void TopOpeBRepBuild_Pave::SameDomain(const TopoDS_Shape& VSD)
{
  mySameDomain = VSD;
}

//=================================================================================================

Standard_Boolean TopOpeBRepBuild_Pave::HasSameDomain() const
{
  return myHasSameDomain;
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRepBuild_Pave::SameDomain() const
{
  return mySameDomain;
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRepBuild_Pave::Vertex() const
{
  return myVertex;
}

//=================================================================================================

TopoDS_Shape& TopOpeBRepBuild_Pave::ChangeVertex()
{
  return myVertex;
}

//=================================================================================================

Standard_Real TopOpeBRepBuild_Pave::Parameter() const
{
  return myParam;
}

// modified by NIZHNY-MZV  Mon Feb 21 14:11:40 2000
//=================================================================================================

void TopOpeBRepBuild_Pave::Parameter(const Standard_Real Par)
{
  myParam = Par;
}

//=================================================================================================

Standard_Boolean TopOpeBRepBuild_Pave::IsShape() const
{
  return myIsShape;
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRepBuild_Pave::Shape() const
{
  return myVertex;
}

//=================================================================================================

void TopOpeBRepBuild_Pave::Dump() const
{
#ifdef OCCT_DEBUG
  std::cout << Parameter() << " ";
  TopAbs::Print(Vertex().Orientation(), std::cout);
#endif
}

// modified by NIZHNY-MZV  Mon Feb 21 14:27:48 2000
//=================================================================================================

TopOpeBRepDS_Kind& TopOpeBRepBuild_Pave::InterferenceType()
{
  return myIntType;
}
