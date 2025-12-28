// Created on: 1993-07-07
// Created by: Jean Yves LEBEY
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

#include <TopOpeBRep_ShapeScanner.hxx>
#include <TopOpeBRepTool_BoxSort.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_State.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_List.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>

//=================================================================================================

TopOpeBRep_ShapeScanner::TopOpeBRep_ShapeScanner() {}

//=================================================================================================

void TopOpeBRep_ShapeScanner::Clear()
{
  myBoxSort.Clear();
}

//=================================================================================================

void TopOpeBRep_ShapeScanner::AddBoxesMakeCOB(const TopoDS_Shape&    S,
                                              const TopAbs_ShapeEnum TS,
                                              const TopAbs_ShapeEnum TA)
{
  myBoxSort.AddBoxesMakeCOB(S, TS, TA);
}

//=================================================================================================

void TopOpeBRep_ShapeScanner::Init(TopOpeBRepTool_ShapeExplorer& E)
{
  NCollection_List<int> anEmptyList;

  myListIterator.Initialize(anEmptyList);

  for (; E.More(); E.Next())
  {
    const TopoDS_Shape& cur = E.Current();
    //    TopAbs_ShapeEnum t = cur.ShapeType();
    Init(cur);
    bool b = More();
    if (b)
      break;
  }
}

//=================================================================================================

void TopOpeBRep_ShapeScanner::Init(const TopoDS_Shape& E)
{
  myListIterator = myBoxSort.Compare(E);
}

//=================================================================================================

bool TopOpeBRep_ShapeScanner::More() const
{
  bool b = myListIterator.More();
  return b;
}

//=================================================================================================

void TopOpeBRep_ShapeScanner::Next()
{
  myListIterator.Next();
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRep_ShapeScanner::Current() const
{
  const TopoDS_Shape& E = myBoxSort.TouchedShape(myListIterator);
  return E;
}

//=================================================================================================

const TopOpeBRepTool_BoxSort& TopOpeBRep_ShapeScanner::BoxSort() const
{
  return myBoxSort;
}

//=================================================================================================

TopOpeBRepTool_BoxSort& TopOpeBRep_ShapeScanner::ChangeBoxSort()
{
  return myBoxSort;
}

//=================================================================================================

int TopOpeBRep_ShapeScanner::Index() const
{
  int n = 0;
  if (myListIterator.More())
    n = myListIterator.Value();
  return n;
}

//=================================================================================================

Standard_OStream& TopOpeBRep_ShapeScanner::DumpCurrent(Standard_OStream& OS) const
{
#ifdef OCCT_DEBUG
  if (More())
  {
    const TopoDS_Shape& S = Current();
    TopAbs_ShapeEnum    T = S.ShapeType();
    TopAbs_Orientation  O = S.Orientation();
    int                 I = Index();
    TopAbs::Print(T, std::cout);
    std::cout << "(" << I << ",";
    TopAbs::Print(O, std::cout);
    std::cout << ") ";
  }
#endif
  return OS;
}
