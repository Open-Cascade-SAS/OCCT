// Created on: 1993-05-07
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

#include <TopOpeBRep_ShapeIntersector2d.hxx>
#include <TopOpeBRepTool_box.hxx>
#include <TopOpeBRepTool_HBoxTool.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRep_GettraceSI();
extern Standard_Boolean TopOpeBRep_GetcontextFFOR();
#endif

//=================================================================================================

TopOpeBRep_ShapeIntersector2d::TopOpeBRep_ShapeIntersector2d()
{
  Reset();
  myHBoxTool = FBOX_GetHBoxTool();
  myFaceScanner.ChangeBoxSort().SetHBoxTool(myHBoxTool);
  myEdgeScanner.ChangeBoxSort().SetHBoxTool(myHBoxTool);
}

//=================================================================================================

void TopOpeBRep_ShapeIntersector2d::Reset()
{
  myIntersectionDone = Standard_False;

  myFFDone   = Standard_False;
  myEEFFDone = Standard_False;

  myFFInit   = Standard_False;
  myEEFFInit = Standard_False;
}

//=================================================================================================

void TopOpeBRep_ShapeIntersector2d::Init(const TopoDS_Shape& S1, const TopoDS_Shape& S2)
{
  Reset();
  myShape1 = S1;
  myShape2 = S2;
  myHBoxTool->Clear();
}

//=================================================================================================

void TopOpeBRep_ShapeIntersector2d::SetIntersectionDone()
{
  myIntersectionDone = (myFFDone || myEEFFDone);
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRep_ShapeIntersector2d::CurrentGeomShape(
  const Standard_Integer Index) const
{
  if (myIntersectionDone)
  {
    if (myFFDone)
    {
      if (Index == 1)
        return myFaceScanner.Current();
      else if (Index == 2)
        return myFaceExplorer.Current();
    }
    else if (myEEFFDone)
    {
      if (Index == 1)
        return myEdgeScanner.Current();
      else if (Index == 2)
        return myEdgeExplorer.Current();
    }
  }

  throw Standard_ProgramError("CurrentGeomShape : no intersection 2d");
}

//=================================================================================================

void TopOpeBRep_ShapeIntersector2d::InitIntersection(const TopoDS_Shape& S1, const TopoDS_Shape& S2)
{
  Init(S1, S2);
  InitFFIntersection();
}

//=================================================================================================

Standard_Boolean TopOpeBRep_ShapeIntersector2d::MoreIntersection() const
{
  Standard_Boolean res = myIntersectionDone;

#ifdef OCCT_DEBUG
  if (TopOpeBRep_GettraceSI() && res)
  {
    if (myFFDone)
      std::cout << "FF : ";
    else if (myEEFFDone)
      std::cout << "    EE : ";
    DumpCurrent(1);
    DumpCurrent(2);
  }
#endif

  return res;
}

//=================================================================================================

#ifdef OCCT_DEBUG
void TopOpeBRep_ShapeIntersector2d::DumpCurrent(const Standard_Integer K) const
{
  if (myFFDone)
  {
    if (K == 1)
      myFaceScanner.DumpCurrent(std::cout);
    else if (K == 2)
      myFaceExplorer.DumpCurrent(std::cout);
  }
  else if (myEEFFDone)
  {
    if (K == 1)
      myEdgeScanner.DumpCurrent(std::cout);
    else if (K == 2)
      myEdgeExplorer.DumpCurrent(std::cout);
  }
#else
void TopOpeBRep_ShapeIntersector2d::DumpCurrent(const Standard_Integer) const
{
#endif
}

//=================================================================================================

#ifdef OCCT_DEBUG
Standard_Integer TopOpeBRep_ShapeIntersector2d::Index(const Standard_Integer K) const
{
  Standard_Integer i = 0;

  if (myFFDone)
  {
    if (K == 1)
      i = myFaceScanner.Index();
    else if (K == 2)
      i = myFaceExplorer.Index();
  }
  else if (myEEFFDone)
  {
    if (K == 1)
      i = myEdgeScanner.Index();
    else if (K == 2)
      i = myEdgeExplorer.Index();
  }

  return i;
}
#else
Standard_Integer TopOpeBRep_ShapeIntersector2d::Index(const Standard_Integer) const
{
  return 0;
}
#endif

//=================================================================================================

void TopOpeBRep_ShapeIntersector2d::NextIntersection()
{
  myIntersectionDone = Standard_False;

  if (myFFDone)
  {
    // precedant etat du More() : 2 faces
    myFFDone = Standard_False;
    InitEEFFIntersection();
    FindEEFFIntersection();
    if (!myIntersectionDone)
    {
      NextFFCouple();
      FindFFIntersection();
    }
  }
  else if (myEEFFDone)
  {
    NextEEFFCouple();
    FindEEFFIntersection();
    if (!myIntersectionDone)
    {
      NextFFCouple();
      FindFFIntersection();
    }
  }

  if (!myIntersectionDone)
  {
    InitFFIntersection();
  }
}

// ========
// FFFFFFFF
// ========

//=================================================================================================

void TopOpeBRep_ShapeIntersector2d::InitFFIntersection()
{
  if (!myFFInit)
  {
    TopAbs_ShapeEnum tscann = TopAbs_FACE;
    TopAbs_ShapeEnum texplo = TopAbs_FACE;
    myFaceScanner.Clear();
    myFaceScanner.AddBoxesMakeCOB(myShape1, tscann);
    myFaceExplorer.Init(myShape2, texplo);
    myFaceScanner.Init(myFaceExplorer);
    FindFFIntersection();
  }
  myFFInit = Standard_True;
}

//=================================================================================================

void TopOpeBRep_ShapeIntersector2d::FindFFIntersection()
{
  myFFDone = Standard_False;
  //  myFFSameDomain = Standard_False;

  if (MoreFFCouple())
  {

    // The two candidate intersecting GeomShapes GS1,GS2 and their types t1,t2
    const TopoDS_Shape& GS1 = myFaceScanner.Current();
    const TopoDS_Shape& GS2 = myFaceExplorer.Current();

#ifdef OCCT_DEBUG
    if (TopOpeBRep_GettraceSI())
    {
      std::cout << "?? FF : ";
      myFaceScanner.DumpCurrent(std::cout);
      myFaceExplorer.DumpCurrent(std::cout);
      std::cout << std::endl;
    }
#endif

    const TopOpeBRepTool_BoxSort& BS = myFaceScanner.BoxSort();
    BS.Box(GS1);
    BS.Box(GS2);
    myFFDone = Standard_True;
  }

  SetIntersectionDone();
}

//=================================================================================================

Standard_Boolean TopOpeBRep_ShapeIntersector2d::MoreFFCouple() const
{
  Standard_Boolean more1 = myFaceScanner.More();
  Standard_Boolean more2 = myFaceExplorer.More();
  return (more1 && more2);
}

//=================================================================================================

void TopOpeBRep_ShapeIntersector2d::NextFFCouple()
{
  myFaceScanner.Next();
  Standard_Boolean b1, b2;

  b1 = (!myFaceScanner.More());
  b2 = (myFaceExplorer.More());
  while (b1 && b2)
  {
    myFaceExplorer.Next();
    myFaceScanner.Init(myFaceExplorer);
    b1 = (!myFaceScanner.More());
    b2 = (myFaceExplorer.More());
  }
}

// ========
// EEFFEEFF
// ========

//=================================================================================================

void TopOpeBRep_ShapeIntersector2d::InitEEFFIntersection()
{
  // prepare exploration of the edges of the two current SameDomain faces
  TopoDS_Shape face1 = myFaceScanner.Current();  // -26-08-96
  TopoDS_Shape face2 = myFaceExplorer.Current(); // -26-08-96

#ifdef OCCT_DEBUG
  if (TopOpeBRep_GetcontextFFOR())
  {
    face1.Orientation(TopAbs_FORWARD); //-05/07
    face2.Orientation(TopAbs_FORWARD); //-05/07
    std::cout << "ctx : InitEEFFIntersection : faces FORWARD" << std::endl;
  }
#endif

  myEEIntersector.SetFaces(face1, face2);

  TopAbs_ShapeEnum tscann = TopAbs_EDGE;
  TopAbs_ShapeEnum texplo = TopAbs_EDGE;
  myEdgeScanner.Clear();
  myEdgeScanner.AddBoxesMakeCOB(face1, tscann);
  myEdgeExplorer.Init(face2, texplo);
  myEdgeScanner.Init(myEdgeExplorer);

  myEEFFInit = Standard_True;
}

//=================================================================================================

void TopOpeBRep_ShapeIntersector2d::FindEEFFIntersection()
{
  myEEFFDone = Standard_False;
  while (MoreEEFFCouple())
  {
    const TopoDS_Shape& GS1 = myEdgeScanner.Current();
    const TopoDS_Shape& GS2 = myEdgeExplorer.Current();
    myEEIntersector.Perform(GS1, GS2);

#ifdef OCCT_DEBUG
    if (TopOpeBRep_GettraceSI() && myEEIntersector.IsEmpty())
    {
      std::cout << "    EE : ";
      myEdgeScanner.DumpCurrent(std::cout);
      myEdgeExplorer.DumpCurrent(std::cout);
      std::cout << "(EE of FF SameDomain)";
      std::cout << " : EMPTY INTERSECTION";
      std::cout << std::endl;
    }
#endif

    myEEFFDone = !(myEEIntersector.IsEmpty());
    if (myEEFFDone)
      break;
    else
      NextEEFFCouple();
  }
  SetIntersectionDone();
}

//=================================================================================================

Standard_Boolean TopOpeBRep_ShapeIntersector2d::MoreEEFFCouple() const
{
  Standard_Boolean more1 = myEdgeScanner.More();
  Standard_Boolean more2 = myEdgeExplorer.More();
  return (more1 && more2);
}

//=================================================================================================

void TopOpeBRep_ShapeIntersector2d::NextEEFFCouple()
{
  myEdgeScanner.Next();
  while (!myEdgeScanner.More() && myEdgeExplorer.More())
  {
    myEdgeExplorer.Next();
    myEdgeScanner.Init(myEdgeExplorer);
  }
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRep_ShapeIntersector2d::Shape(const Standard_Integer Index) const
{
  if (Index == 1)
    return myShape1;
  else if (Index == 2)
    return myShape2;

  throw Standard_ProgramError("ShapeIntersector : no shape");
}

//=================================================================================================

TopOpeBRep_EdgesIntersector& TopOpeBRep_ShapeIntersector2d::ChangeEdgesIntersector()
{
  return myEEIntersector;
}
