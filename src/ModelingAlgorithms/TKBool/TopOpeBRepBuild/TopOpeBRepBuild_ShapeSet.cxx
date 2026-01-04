// Created on: 1993-06-17
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

#include <BRepCheck_Analyzer.hxx>
#include <Standard_ProgramError.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_ShapeSet.hxx>

#ifdef OCCT_DEBUG
extern bool TopOpeBRepBuild_GettraceCHK();
extern bool TopOpeBRepBuild_GettraceCHKOK();
extern bool TopOpeBRepBuild_GettraceCHKNOK();

Standard_EXPORT void debaddss() {}

Standard_IMPORT TopOpeBRepBuild_Builder* LOCAL_PBUILDER_DEB;
#endif

//=================================================================================================

TopOpeBRepBuild_ShapeSet::TopOpeBRepBuild_ShapeSet(const TopAbs_ShapeEnum SubShapeType,
                                                   const bool             checkshape)
    : mySubShapeType(SubShapeType),
      myCheckShape(checkshape)
{
  if (SubShapeType == TopAbs_EDGE)
    myShapeType = TopAbs_FACE;
  else if (SubShapeType == TopAbs_VERTEX)
    myShapeType = TopAbs_EDGE;
  else
    throw Standard_ProgramError("ShapeSet : bad ShapeType");
  myDEBNumber = 0;

  myCheckShape = false; // temporary NYI
}

TopOpeBRepBuild_ShapeSet::~TopOpeBRepBuild_ShapeSet() = default;

//=================================================================================================

void TopOpeBRepBuild_ShapeSet::AddShape(const TopoDS_Shape& S)
{
  bool chk = CheckShape(S);
#ifdef OCCT_DEBUG
  DumpCheck(std::cout, " AddShape", S, chk);
#endif

  if (!chk)
    return;
  ProcessAddShape(S);
}

//=================================================================================================

void TopOpeBRepBuild_ShapeSet::AddStartElement(const TopoDS_Shape& S)
{
  bool chk = CheckShape(S);
#ifdef OCCT_DEBUG
  DumpCheck(std::cout, " AddStartElement", S, chk);
#endif

  if (!chk)
    return;
  ProcessAddStartElement(S);
}

//=================================================================================================

void TopOpeBRepBuild_ShapeSet::AddElement(const TopoDS_Shape& S)
{
  bool chk = CheckShape(S);
#ifdef OCCT_DEBUG
  DumpCheck(std::cout, " AddElement", S, chk);
#endif

  if (!chk)
    return;
  ProcessAddElement(S);
}

//=================================================================================================

void TopOpeBRepBuild_ShapeSet::ProcessAddShape(const TopoDS_Shape& S)
{
  if (!myOMSH.Contains(S))
  {
    myOMSH.Add(S);
    myShapes.Append(S);
  }
}

//=================================================================================================

void TopOpeBRepBuild_ShapeSet::ProcessAddStartElement(const TopoDS_Shape& S)
{
  if (!myOMSS.Contains(S))
  {
    myOMSS.Add(S);
    myStartShapes.Append(S);
    ProcessAddElement(S);
  }
}

//=================================================================================================

void TopOpeBRepBuild_ShapeSet::ProcessAddElement(const TopoDS_Shape& S)
{
  if (!myOMES.Contains(S))
  {
    myOMES.Add(S);
    NCollection_List<TopoDS_Shape> Lemp;
    TopOpeBRepTool_ShapeExplorer   Ex(S, mySubShapeType);
    for (; Ex.More(); Ex.Next())
    {
      const TopoDS_Shape& subshape = Ex.Current();
      bool                b        = (!mySubShapeMap.Contains(subshape));
      if (b)
        mySubShapeMap.Add(subshape, Lemp);
      mySubShapeMap.ChangeFromKey(subshape).Append(S);
    }
  }
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& TopOpeBRepBuild_ShapeSet::StartElements() const
{
  return myStartShapes;
}

//=================================================================================================

void TopOpeBRepBuild_ShapeSet::InitShapes()
{
  myShapesIter.Initialize(myShapes);
}

//=================================================================================================

bool TopOpeBRepBuild_ShapeSet::MoreShapes() const
{
  bool b = myShapesIter.More();
  return b;
}

//=================================================================================================

void TopOpeBRepBuild_ShapeSet::NextShape()
{
  myShapesIter.Next();
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRepBuild_ShapeSet::Shape() const
{
  const TopoDS_Shape& S = myShapesIter.Value();
  return S;
}

//=================================================================================================

void TopOpeBRepBuild_ShapeSet::InitStartElements()
{
  myStartShapesIter.Initialize(myStartShapes);
}

//=================================================================================================

bool TopOpeBRepBuild_ShapeSet::MoreStartElements() const
{
  bool b = myStartShapesIter.More();
  return b;
}

//=================================================================================================

void TopOpeBRepBuild_ShapeSet::NextStartElement()
{
  myStartShapesIter.Next();
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRepBuild_ShapeSet::StartElement() const
{
  const TopoDS_Shape& S = myStartShapesIter.Value();
  return S;
}

//=================================================================================================

void TopOpeBRepBuild_ShapeSet::InitNeighbours(const TopoDS_Shape& S)
{
  mySubShapeExplorer.Init(S, mySubShapeType);
  myCurrentShape = S;
  FindNeighbours();
}

//=================================================================================================

bool TopOpeBRepBuild_ShapeSet::MoreNeighbours()
{
  bool b = myIncidentShapesIter.More();
  return b;
}

//=================================================================================================

void TopOpeBRepBuild_ShapeSet::NextNeighbour()
{
  myIncidentShapesIter.Next();
  bool noisimore = !myIncidentShapesIter.More();
  if (noisimore)
  {
    bool ssemore = mySubShapeExplorer.More();
    if (ssemore)
    {
      mySubShapeExplorer.Next();
      FindNeighbours();
    }
  }
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRepBuild_ShapeSet::Neighbour() const
{
  const TopoDS_Shape& S = myIncidentShapesIter.Value();
  return S;
}

//=================================================================================================

NCollection_List<TopoDS_Shape>& TopOpeBRepBuild_ShapeSet::ChangeStartShapes()
{
  return myStartShapes;
}

//=================================================================================================

void TopOpeBRepBuild_ShapeSet::FindNeighbours()
{
  while (mySubShapeExplorer.More())
  {

    // l = list of edges neighbour of edge myCurrentShape through
    // the vertex mySubShapeExplorer.Current(), which is a vertex of the
    // edge myCurrentShape.
    const TopoDS_Shape&                   V = mySubShapeExplorer.Current();
    const NCollection_List<TopoDS_Shape>& l = MakeNeighboursList(myCurrentShape, V);

    // myIncidentShapesIter iterates on the neighbour edges of the edge
    // given as InitNeighbours() argument (this edge has been stored
    // in the field myCurrentShape).

    myIncidentShapesIter.Initialize(l);
    if (myIncidentShapesIter.More())
      break;
    else
      mySubShapeExplorer.Next();
  }
}

//=======================================================================
// function : MakeNeighboursList
// purpose  : // (Earg = Edge, Varg = Vertex) to find connected to Earg by Varg
//=======================================================================
const NCollection_List<TopoDS_Shape>& TopOpeBRepBuild_ShapeSet::MakeNeighboursList(
  const TopoDS_Shape& /*Earg*/,
  const TopoDS_Shape& Varg)
{
  const NCollection_List<TopoDS_Shape>& l = mySubShapeMap.FindFromKey(Varg);
  return l;
}

//=================================================================================================

int TopOpeBRepBuild_ShapeSet::MaxNumberSubShape(const TopoDS_Shape& Shape)
{
  int                                      i, m = 0;
  TopOpeBRepTool_ShapeExplorer             SE(Shape, mySubShapeType);
  NCollection_List<TopoDS_Shape>::Iterator LI;
  while (SE.More())
  {
    const TopoDS_Shape& SubShape = SE.Current();
    if (!mySubShapeMap.Contains(SubShape))
    {
      SE.Next();
      continue;
    }
    const NCollection_List<TopoDS_Shape>& l = mySubShapeMap.FindFromKey(SubShape);
    LI.Initialize(l);
    for (i = 0; LI.More(); LI.Next(), i++)
    {
    }
    m = std::max(m, i);
    SE.Next();
  }
  return m;
}

//=================================================================================================

void TopOpeBRepBuild_ShapeSet::CheckShape(const bool checkshape)
{
  myCheckShape = checkshape;

#ifdef OCCT_DEBUG
  if (TopOpeBRepBuild_GettraceCHK() && !myCheckShape)
  {
    DumpName(std::cout, "no checkshape set on ");
    std::cout << std::endl;
  }
#endif
}

//=================================================================================================

bool TopOpeBRepBuild_ShapeSet::CheckShape() const
{
  return myCheckShape;
}

//=================================================================================================

bool TopOpeBRepBuild_ShapeSet::CheckShape(const TopoDS_Shape& S, const bool checkgeom)
{
  if (!myCheckShape)
    return true;

  BRepCheck_Analyzer ana(S, checkgeom);
  bool               val = ana.IsValid();
  return val;
}

//=================================================================================================

void TopOpeBRepBuild_ShapeSet::DumpName(Standard_OStream&              OS,
                                        const TCollection_AsciiString& str) const
{
  OS << str << "(" << myDEBName << "," << myDEBNumber << ")";
}

//=================================================================================================

#ifdef OCCT_DEBUG
void TopOpeBRepBuild_ShapeSet::DumpCheck(Standard_OStream&              OS,
                                         const TCollection_AsciiString& str,
                                         const TopoDS_Shape&            S,
                                         const bool                     chk
#else
void TopOpeBRepBuild_ShapeSet::DumpCheck(Standard_OStream&,
                                         const TCollection_AsciiString&,
                                         const TopoDS_Shape&,
                                         const bool
#endif
) const
{
  if (!myCheckShape)
    return;

#ifdef OCCT_DEBUG
  TopAbs_ShapeEnum t = S.ShapeType();
  if (!chk)
  {
    if (TopOpeBRepBuild_GettraceCHK() || TopOpeBRepBuild_GettraceCHKNOK())
    {
      DumpName(OS, "*********************** ");
      OS << str << " ";
      TopAbs::Print(t, OS);
      OS << " : incorrect" << std::endl;
    }
  }
  else
  {
    if (TopOpeBRepBuild_GettraceCHK() || TopOpeBRepBuild_GettraceCHKOK())
    {
      DumpName(OS, "");
      OS << str << " ";
      TopAbs::Print(t, OS);
      OS << " : correct" << std::endl;
    }
  }
  if (!chk)
    debaddss();
#endif
}

//=================================================================================================

void TopOpeBRepBuild_ShapeSet::DumpSS() {}

//=================================================================================================

void TopOpeBRepBuild_ShapeSet::DumpBB() {}

//=================================================================================================

void TopOpeBRepBuild_ShapeSet::DEBName(const TCollection_AsciiString& N)
{
  myDEBName = N;
}

//=================================================================================================

const TCollection_AsciiString& TopOpeBRepBuild_ShapeSet::DEBName() const
{
  return myDEBName;
}

//=================================================================================================

void TopOpeBRepBuild_ShapeSet::DEBNumber(const int I)
{
  myDEBNumber = I;
}

//=================================================================================================

int TopOpeBRepBuild_ShapeSet::DEBNumber() const
{
  return myDEBNumber;
}

//=================================================================================================

TCollection_AsciiString TopOpeBRepBuild_ShapeSet::SName(const TopoDS_Shape&,
                                                        const TCollection_AsciiString&,
                                                        const TCollection_AsciiString&) const
{
  TCollection_AsciiString str;
  return str;
}

//=================================================================================================

TCollection_AsciiString TopOpeBRepBuild_ShapeSet::SNameori(const TopoDS_Shape&,
                                                           const TCollection_AsciiString&,
                                                           const TCollection_AsciiString&) const
{
  TCollection_AsciiString str;
  return str;
}

//=================================================================================================

TCollection_AsciiString TopOpeBRepBuild_ShapeSet::SName(const NCollection_List<TopoDS_Shape>&,
                                                        const TCollection_AsciiString&,
                                                        const TCollection_AsciiString&) const
{
  TCollection_AsciiString str;
  return str;
}

//=================================================================================================

TCollection_AsciiString TopOpeBRepBuild_ShapeSet::SNameori(
  const NCollection_List<TopoDS_Shape>& /*L*/,
  const TCollection_AsciiString& /*sb*/,
  const TCollection_AsciiString& /*sa*/) const
{
  TCollection_AsciiString str;
  return str;
}
