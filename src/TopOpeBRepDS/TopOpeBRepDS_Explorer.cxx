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
#include <Standard_NoSuchObject.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepDS_Explorer.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>

//=======================================================================
//function : TopOpeBRepDS_Explorer
//purpose  : 
//=======================================================================
TopOpeBRepDS_Explorer::TopOpeBRepDS_Explorer()
:myT(TopAbs_SHAPE),myI(1),myN(0),myB(Standard_False),myFK(Standard_True)
{
}

//=======================================================================
//function : TopOpeBRepDS_Explorer
//purpose  : 
//=======================================================================
TopOpeBRepDS_Explorer::TopOpeBRepDS_Explorer
(const Handle(TopOpeBRepDS_HDataStructure)& HDS,const TopAbs_ShapeEnum T,const Standard_Boolean FK)
{
  Init(HDS,T,FK);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void TopOpeBRepDS_Explorer::Init
(const Handle(TopOpeBRepDS_HDataStructure)& HDS,const TopAbs_ShapeEnum T,const Standard_Boolean FK)
{
  myI = 1; myN = 0; myB = Standard_False; myFK = Standard_True; myT = T;
  myHDS = HDS; if (myHDS.IsNull()) return;
  myN = myHDS->NbShapes(); myFK = FK;
  Find();
}


//=======================================================================
//function : Type
//purpose  : 
//=======================================================================
TopAbs_ShapeEnum TopOpeBRepDS_Explorer::Type() const
{
  return myT;
}

//=======================================================================
//function : Find
//purpose  : 
//=======================================================================
void TopOpeBRepDS_Explorer::Find()
{
  Standard_Boolean found = Standard_False;
  const TopOpeBRepDS_DataStructure& BDS = myHDS->DS();
  while ( (myI <= myN) && (!found) ) {
    Standard_Boolean b = BDS.KeepShape(myI,myFK);
    if (b) {
      const TopoDS_Shape& s = BDS.Shape(myI,Standard_False);
      TopAbs_ShapeEnum t = s.ShapeType();
      if ( t == myT || myT == TopAbs_SHAPE ) found = Standard_True;
      else myI++;
    }
    else myI++;
  }
  myB = found;
}

//=======================================================================
//function : More
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepDS_Explorer::More() const
{
  return myB;
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================
void TopOpeBRepDS_Explorer::Next()
{
  Standard_NoMoreObject_Raise_if(!myB,"TopOpeBRepDS_Explorer::Next");
  myI++;
  Find();
}

//=======================================================================
//function : Current
//purpose  : 
//=======================================================================
const TopoDS_Shape& TopOpeBRepDS_Explorer::Current() const
{
  Standard_NoSuchObject_Raise_if(!More(),"TopOpeBRepDS_Explorer::Current");
  return myHDS->Shape(myI);
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRepDS_Explorer::Index() const
{
  Standard_NoSuchObject_Raise_if(!More(),"TopOpeBRepDS_Explorer::Index");
  return myI;
}

//=======================================================================
//function : Face
//purpose  : 
//=======================================================================
const TopoDS_Face& TopOpeBRepDS_Explorer::Face() const
{
  Standard_NoSuchObject_Raise_if(!More(),"TopOpeBRepDS_Explorer::Face");
  const TopoDS_Shape& s = Current();
  const TopoDS_Face& f = TopoDS::Face(s);
  return f;
}

//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================
const TopoDS_Edge& TopOpeBRepDS_Explorer::Edge() const
{
  Standard_NoSuchObject_Raise_if(!More(),"TopOpeBRepDS_Explorer::Edge");
  const TopoDS_Shape& s = Current();
  const TopoDS_Edge& e = TopoDS::Edge(s);
  return e;
}

//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================
const TopoDS_Vertex& TopOpeBRepDS_Explorer::Vertex() const
{
  Standard_NoSuchObject_Raise_if(!More(),"TopOpeBRepDS_Explorer::Vertex");
  const TopoDS_Shape& s = Current();
  const TopoDS_Vertex& v = TopoDS::Vertex(s);
  return v;
}
