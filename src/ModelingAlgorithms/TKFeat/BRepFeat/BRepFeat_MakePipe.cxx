// Created on: 1996-09-03
// Created by: Jacques GOUSSARD
// Copyright (c) 1996-1999 Matra Datavision
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

#include <BRep_Tool.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepFeat_MakePipe.hxx>
#include <Geom_Curve.hxx>
#include <LocOpe.hxx>
#include <LocOpe_Pipe.hxx>
#include <Standard_ConstructionError.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>

#ifdef OCCT_DEBUG
extern bool BRepFeat_GettraceFEAT();
#endif

static void MajMap(const TopoDS_Shape&, // base
                   LocOpe_Pipe&,
                   NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&, // myMap
                   TopoDS_Shape&,                       // myFShape
                   TopoDS_Shape&);                      // myLShape

//=================================================================================================

void BRepFeat_MakePipe::Init(const TopoDS_Shape&    Sbase,
                             const TopoDS_Shape&    Pbase,
                             const TopoDS_Face&     Skface,
                             const TopoDS_Wire&     Spine,
                             const int Mode,
                             const bool Modify)
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakePipe::Init" << std::endl;
#endif
  mySbase = Sbase;
  BasisShapeValid();
  mySkface = Skface;
  SketchFaceValid();
  myPbase = Pbase;
  mySlface.Clear();
  mySpine = Spine;
  if (Mode == 0)
  {
    myFuse     = false;
    myJustFeat = false;
  }
  else if (Mode == 1)
  {
    myFuse     = true;
    myJustFeat = false;
  }
  else if (Mode == 2)
  {
    myFuse     = true;
    myJustFeat = true;
  }
  else
  {
  }
  myModify    = Modify;
  myJustGluer = false;

  //-------------- ifv
  // mySkface.Nullify();
  //-------------- ifv

  myShape.Nullify();
  myMap.Clear();
  myFShape.Nullify();
  myLShape.Nullify();
  TopExp_Explorer exp;
  for (exp.Init(mySbase, TopAbs_FACE); exp.More(); exp.Next())
  {
    NCollection_List<TopoDS_Shape> thelist;
    myMap.Bind(exp.Current(), thelist);
    myMap(exp.Current()).Append(exp.Current());
  }
#ifdef OCCT_DEBUG
  if (trc)
  {
    if (myJustFeat)
      std::cout << " Just Feature" << std::endl;
    if (myFuse)
      std::cout << " Fuse" << std::endl;
    if (!myFuse)
      std::cout << " Cut" << std::endl;
    if (!myModify)
      std::cout << " Modify = 0" << std::endl;
  }
#endif
}

//=================================================================================================

void BRepFeat_MakePipe::Add(const TopoDS_Edge& E, const TopoDS_Face& F)
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakePipe::Add(Edge,face)" << std::endl;
#endif
  TopExp_Explorer exp;
  for (exp.Init(mySbase, TopAbs_FACE); exp.More(); exp.Next())
  {
    if (exp.Current().IsSame(F))
    {
      break;
    }
  }
  if (!exp.More())
  {
    throw Standard_ConstructionError();
  }

  for (exp.Init(myPbase, TopAbs_EDGE); exp.More(); exp.Next())
  {
    if (exp.Current().IsSame(E))
    {
      break;
    }
  }
  if (!exp.More())
  {
    throw Standard_ConstructionError();
  }

  if (!mySlface.IsBound(F))
  {
    NCollection_List<TopoDS_Shape> thelist1;
    mySlface.Bind(F, thelist1);
  }
  NCollection_List<TopoDS_Shape>::Iterator itl(mySlface(F));
  for (; itl.More(); itl.Next())
  {
    if (itl.Value().IsSame(E))
    {
      break;
    }
  }
  if (!itl.More())
  {
    mySlface(F).Append(E);
  }
}

//=================================================================================================

void BRepFeat_MakePipe::Perform()
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakePipe::Perform()" << std::endl;
#endif
  mySFrom.Nullify();
  ShapeFromValid();
  mySUntil.Nullify();
  ShapeUntilValid();
  myGluedF.Clear();
  myPerfSelection = BRepFeat_NoSelection;
  PerfSelectionValid();
  TopoDS_Shape theBase = myPbase;
  LocOpe_Pipe  thePipe(mySpine, theBase);
  TopoDS_Shape VraiPipe = thePipe.Shape();
  MajMap(myPbase, thePipe, myMap, myFShape, myLShape);
  myGShape = VraiPipe;
  GeneratedShapeValid();

  GluedFacesValid();

  if (myGluedF.IsEmpty())
  {
    if (myFuse == 1)
    {
      BRepAlgoAPI_Fuse f(mySbase, myGShape);
      myShape = f.Shape();
      UpdateDescendants(f, myShape, false);
      Done();
    }
    else if (myFuse == 0)
    {
      BRepAlgoAPI_Cut c(mySbase, myGShape);
      myShape = c.Shape();
      UpdateDescendants(c, myShape, false);
      Done();
    }
    else
    {
      myShape = myGShape;
      Done();
    }
  }
  else
  {
    myFShape = thePipe.FirstShape();
    NCollection_Sequence<gp_Pnt> spt;
    LocOpe::SampleEdges(myFShape, spt);
    myCurves = thePipe.Curves(spt);
    myBCurve = thePipe.BarycCurve();
    GlobalPerform();
  }
}

//=================================================================================================

void BRepFeat_MakePipe::Perform(const TopoDS_Shape& Until)
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakePipe::Perform(Until)" << std::endl;
#endif
  if (Until.IsNull())
  {
    throw Standard_ConstructionError();
  }
  TopExp_Explorer exp(Until, TopAbs_FACE);
  if (!exp.More())
  {
    throw Standard_ConstructionError();
  }
  myGluedF.Clear();
  myPerfSelection = BRepFeat_SelectionU;
  PerfSelectionValid();
  mySFrom.Nullify();
  ShapeFromValid();
  mySUntil = Until;
  TransformShapeFU(1);
  ShapeUntilValid();
  LocOpe_Pipe  thePipe(mySpine, myPbase);
  TopoDS_Shape VraiTuyau = thePipe.Shape();
  MajMap(myPbase, thePipe, myMap, myFShape, myLShape);
  myGShape = VraiTuyau;
  GeneratedShapeValid();

  GluedFacesValid();

  myFShape = thePipe.FirstShape();
  NCollection_Sequence<gp_Pnt> spt;
  LocOpe::SampleEdges(myFShape, spt);
  myCurves = thePipe.Curves(spt);
  myBCurve = thePipe.BarycCurve();
  GlobalPerform();
}

//=======================================================================
// function : Perform
// purpose  : between From and Until
//=======================================================================

void BRepFeat_MakePipe::Perform(const TopoDS_Shape& From, const TopoDS_Shape& Until)
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakePipe::Perform(From,Until)" << std::endl;
#endif
  if (From.IsNull() || Until.IsNull())
  {
    throw Standard_ConstructionError();
  }
  if (!mySkface.IsNull())
  {
    if (From.IsSame(mySkface))
    {
      Perform(Until);
      return;
    }
    else if (Until.IsSame(mySkface))
    {
      Perform(From);
      return;
    }
  }
  myGluedF.Clear();
  myPerfSelection = BRepFeat_SelectionFU;
  PerfSelectionValid();
  TopExp_Explorer exp(From, TopAbs_FACE);
  if (!exp.More())
  {
    throw Standard_ConstructionError();
  }
  exp.Init(Until, TopAbs_FACE);
  if (!exp.More())
  {
    throw Standard_ConstructionError();
  }
  mySFrom = From;
  TransformShapeFU(0);
  ShapeFromValid();
  mySUntil = Until;
  TransformShapeFU(1);
  ShapeUntilValid();
  LocOpe_Pipe  thePipe(mySpine, myPbase);
  TopoDS_Shape VraiTuyau = thePipe.Shape();
  MajMap(myPbase, thePipe, myMap, myFShape, myLShape);
  myGShape = VraiTuyau;
  GeneratedShapeValid();

  //	mySbase, myPbase, mySlface, thePipe, myGluedF);
  GluedFacesValid();

  myFShape = thePipe.FirstShape();
  NCollection_Sequence<gp_Pnt> spt;
  LocOpe::SampleEdges(myFShape, spt);
  myCurves = thePipe.Curves(spt);
  myBCurve = thePipe.BarycCurve();
  GlobalPerform();
}

//=======================================================================
// function : Curves
// purpose  : curves parallel to the generating wire of the pipe
//=======================================================================

void BRepFeat_MakePipe::Curves(NCollection_Sequence<occ::handle<Geom_Curve>>& scur)
{
  scur = myCurves;
}

//=======================================================================
// function : BarycCurve
// purpose  : pass through the center of mass
//=======================================================================

occ::handle<Geom_Curve> BRepFeat_MakePipe::BarycCurve()
{
  return myBCurve;
}

//=======================================================================
// function : MajMap
// purpose  : management of descendants
//=======================================================================

static void MajMap(const TopoDS_Shape&                 theB,
                   LocOpe_Pipe&                        theP,
                   NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theMap,    // myMap
                   TopoDS_Shape&                       theFShape, // myFShape
                   TopoDS_Shape&                       theLShape)                       // myLShape
{
  TopExp_Explorer exp(theP.FirstShape(), TopAbs_WIRE);
  if (exp.More())
  {
    theFShape = exp.Current();
    NCollection_List<TopoDS_Shape> thelist2;
    theMap.Bind(theFShape, thelist2);
    for (exp.Init(theP.FirstShape(), TopAbs_FACE); exp.More(); exp.Next())
    {
      theMap(theFShape).Append(exp.Current());
    }
  }

  exp.Init(theP.LastShape(), TopAbs_WIRE);
  if (exp.More())
  {
    theLShape = exp.Current();
    NCollection_List<TopoDS_Shape> thelist3;
    theMap.Bind(theLShape, thelist3);
    for (exp.Init(theP.LastShape(), TopAbs_FACE); exp.More(); exp.Next())
    {
      theMap(theLShape).Append(exp.Current());
    }
  }

  for (exp.Init(theB, TopAbs_EDGE); exp.More(); exp.Next())
  {
    if (!theMap.IsBound(exp.Current()))
    {
      NCollection_List<TopoDS_Shape> thelist4;
      theMap.Bind(exp.Current(), thelist4);
      theMap(exp.Current()) = theP.Shapes(exp.Current());
    }
  }
}
