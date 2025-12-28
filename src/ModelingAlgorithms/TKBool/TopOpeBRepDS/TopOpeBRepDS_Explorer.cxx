// Created on: 1999-01-05
// Created by: Jean Yves LEBEY
// Copyright (c) 1999-1999 Matra Datavision
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

#define No_Standard_NoMoreObject
#define No_Standard_NoSuchObject

#include <Standard_NoMoreObject.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepDS_Explorer.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>

//=================================================================================================

TopOpeBRepDS_Explorer::TopOpeBRepDS_Explorer()
    : myT(TopAbs_SHAPE),
      myI(1),
      myN(0),
      myB(false),
      myFK(true)
{
}

//=================================================================================================

TopOpeBRepDS_Explorer::TopOpeBRepDS_Explorer(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                             const TopAbs_ShapeEnum                     T,
                                             const bool                     FK)
{
  Init(HDS, T, FK);
}

//=================================================================================================

void TopOpeBRepDS_Explorer::Init(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                 const TopAbs_ShapeEnum                     T,
                                 const bool                     FK)
{
  myI   = 1;
  myN   = 0;
  myB   = false;
  myFK  = true;
  myT   = T;
  myHDS = HDS;
  if (myHDS.IsNull())
    return;
  myN  = myHDS->NbShapes();
  myFK = FK;
  Find();
}

//=================================================================================================

TopAbs_ShapeEnum TopOpeBRepDS_Explorer::Type() const
{
  return myT;
}

//=================================================================================================

void TopOpeBRepDS_Explorer::Find()
{
  bool                  found = false;
  const TopOpeBRepDS_DataStructure& BDS   = myHDS->DS();
  while ((myI <= myN) && (!found))
  {
    bool b = BDS.KeepShape(myI, myFK);
    if (b)
    {
      const TopoDS_Shape& s = BDS.Shape(myI, false);
      TopAbs_ShapeEnum    t = s.ShapeType();
      if (t == myT || myT == TopAbs_SHAPE)
        found = true;
      else
        myI++;
    }
    else
      myI++;
  }
  myB = found;
}

//=================================================================================================

bool TopOpeBRepDS_Explorer::More() const
{
  return myB;
}

//=================================================================================================

void TopOpeBRepDS_Explorer::Next()
{
  Standard_NoMoreObject_Raise_if(!myB, "TopOpeBRepDS_Explorer::Next");
  myI++;
  Find();
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRepDS_Explorer::Current() const
{
  Standard_NoSuchObject_Raise_if(!More(), "TopOpeBRepDS_Explorer::Current");
  return myHDS->Shape(myI);
}

//=================================================================================================

int TopOpeBRepDS_Explorer::Index() const
{
  Standard_NoSuchObject_Raise_if(!More(), "TopOpeBRepDS_Explorer::Index");
  return myI;
}

//=================================================================================================

const TopoDS_Face& TopOpeBRepDS_Explorer::Face() const
{
  Standard_NoSuchObject_Raise_if(!More(), "TopOpeBRepDS_Explorer::Face");
  const TopoDS_Shape& s = Current();
  const TopoDS_Face&  f = TopoDS::Face(s);
  return f;
}

//=================================================================================================

const TopoDS_Edge& TopOpeBRepDS_Explorer::Edge() const
{
  Standard_NoSuchObject_Raise_if(!More(), "TopOpeBRepDS_Explorer::Edge");
  const TopoDS_Shape& s = Current();
  const TopoDS_Edge&  e = TopoDS::Edge(s);
  return e;
}

//=================================================================================================

const TopoDS_Vertex& TopOpeBRepDS_Explorer::Vertex() const
{
  Standard_NoSuchObject_Raise_if(!More(), "TopOpeBRepDS_Explorer::Vertex");
  const TopoDS_Shape&  s = Current();
  const TopoDS_Vertex& v = TopoDS::Vertex(s);
  return v;
}
