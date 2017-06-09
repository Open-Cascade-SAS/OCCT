// Created on: 1995-03-08
// Created by: Bruno DUMORTIER
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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepLProp_SLProps.hxx>
#include <BRepPrimAPI_MakeHalfSpace.hxx>
#include <gp.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <StdFail_NotDone.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>

//=======================================================================
//function : getNormalOnFace
//purpose  : 
//=======================================================================

static gp_Dir getNormalOnFace(const TopoDS_Face& theFace,
                              const Standard_Real theU,
                              const Standard_Real theV)
{
  Standard_Real aPrec = gp::Resolution();
  BRepLProp_SLProps aProps(BRepAdaptor_Surface(theFace), theU, theV, 2, aPrec);
  gp_Dir aNormal = aProps.Normal();
  if (theFace.Orientation() == TopAbs_REVERSED)
    aNormal.Reverse();
  return aNormal;
}

//=======================================================================
//function : getNormalFromEdge
//purpose  : Get average normal at the point with the given parameter on the edge
//=======================================================================

static Standard_Boolean  getNormalFromEdge(const TopoDS_Shape& theShape,
                                           const TopoDS_Edge& theEdge,
                                           const Standard_Real thePar,
                                           gp_Dir& theNormal)
{
  gp_XYZ aSum;
  TopExp_Explorer ex(theShape, TopAbs_FACE);
  for (; ex.More(); ex.Next()) {
    const TopoDS_Face& aF = TopoDS::Face(ex.Current());
    TopExp_Explorer ex1(aF, TopAbs_EDGE);
    for (; ex1.More(); ex1.Next()) {
      if (ex1.Current().IsSame(theEdge)) {
        Standard_Real f, l;
        Handle(Geom2d_Curve) aC2d = BRep_Tool::CurveOnSurface(theEdge, aF, f, l);
        gp_Pnt2d aP2d = aC2d->Value(thePar);
        gp_Dir aNorm = getNormalOnFace(aF, aP2d.X(), aP2d.Y());
        aSum += aNorm.XYZ();
      }
    }
  }
  if (aSum.SquareModulus() > gp::Resolution()) {
    theNormal = aSum;
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : getNormalFromVertex
//purpose  : Get average normal at the point of the vertex
//=======================================================================

static Standard_Boolean  getNormalFromVertex(const TopoDS_Shape& theShape,
                                             const TopoDS_Vertex& theVer,
                                             gp_Dir& theNormal)
{
  gp_XYZ aSum;
  TopExp_Explorer ex(theShape, TopAbs_FACE);
  for (; ex.More(); ex.Next()) {
    const TopoDS_Face& aF = TopoDS::Face(ex.Current());
    TopExp_Explorer ex1(aF, TopAbs_VERTEX);
    for (; ex1.More(); ex1.Next()) {
      if (ex1.Current().IsSame(theVer)) {
        gp_Pnt2d aP2d = BRep_Tool::Parameters(theVer, aF);
        gp_Dir aNorm = getNormalOnFace(aF, aP2d.X(), aP2d.Y());
        aSum += aNorm.XYZ();
      }
    }
  }
  if (aSum.SquareModulus() > gp::Resolution()) {
    theNormal = aSum;
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : FindExtrema
//purpose  : This finction is called to find the nearest normal projection
//           of a point <aPnt> on a shape <aShape>.
//           1) return true if extrema is found.
//           2) Set in:
//             - theMinPnt : The solution point
//             - theNormal : The normal direction to the shape at projection point
//=======================================================================
static Standard_Boolean FindExtrema(const gp_Pnt&        thePnt,
                                    const TopoDS_Shape&  theShape,
                                    gp_Pnt&              theMinPnt,
                                    gp_Dir&              theNormal)
{
  TopoDS_Vertex aRefVertex = BRepBuilderAPI_MakeVertex(thePnt);
  
  BRepExtrema_DistShapeShape ext(aRefVertex, theShape);
  
  if (!ext.IsDone() || ext.NbSolution() == 0)
    return Standard_False;

  // the point projection exist
  Standard_Integer nbext = ext.NbSolution();
  // try to find a projection on face
  for (Standard_Integer iext = 1; iext <= nbext; iext++) {
    if (ext.SupportTypeShape2(iext) == BRepExtrema_IsInFace) {
      TopoDS_Face aF = TopoDS::Face(ext.SupportOnShape2(iext));
      theMinPnt = ext.PointOnShape2(iext);
      Standard_Real aU, aV;
      ext.ParOnFaceS2(iext, aU, aV);
      theNormal = getNormalOnFace(aF, aU, aV);
      return Standard_True;
    }
  }

  // if not found then take any edge or vertex solution
  for (Standard_Integer iext = 1; iext <= nbext; iext++) {
    if (ext.SupportTypeShape2(iext) == BRepExtrema_IsOnEdge) {
      theMinPnt = ext.PointOnShape2(iext);
      Standard_Real aPar;
      ext.ParOnEdgeS2(iext, aPar);
      TopoDS_Edge aE = TopoDS::Edge(ext.SupportOnShape2(iext));
      if (getNormalFromEdge(theShape, aE, aPar, theNormal))
        return Standard_True;
    }
    else if (ext.SupportTypeShape2(iext) == BRepExtrema_IsVertex) {
      theMinPnt = ext.PointOnShape2(iext);
      TopoDS_Vertex aV = TopoDS::Vertex(ext.SupportOnShape2(iext));
      if (getNormalFromVertex(theShape, aV, theNormal))
        return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : isOutside
//purpose  : 
//=======================================================================

static Standard_Boolean isOutside(const gp_Pnt&      thePnt,
                                  const gp_Pnt&      thePonF,
                                  const gp_Dir&      theNormal)
{
  gp_Dir anOppRef(thePnt.XYZ() - thePonF.XYZ());
  Standard_Real aSca = theNormal * anOppRef;
  // outside if same directions
  return aSca > 0.;
}

//=======================================================================
//function : BRepPrimAPI_MakeHalfSpace
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeHalfSpace::BRepPrimAPI_MakeHalfSpace(const TopoDS_Face& theFace,
                                                     const gp_Pnt&      theRefPnt)
{
  // Set the flag is <IsDone> to False.
  NotDone();

  TopoDS_Shell aShell;

  gp_Pnt aMinPnt;
  gp_Dir aNormal;
  if (FindExtrema(theRefPnt, theFace, aMinPnt, aNormal)) {
    Standard_Boolean toReverse = isOutside(theRefPnt, aMinPnt, aNormal);

    // Construction of the open solid.
    BRep_Builder().MakeShell(aShell);
    BRep_Builder().Add(aShell, theFace);
    BRep_Builder().MakeSolid(mySolid);
    if (toReverse) {
      aShell.Reverse();
    }
    BRep_Builder().Add(mySolid, aShell);
    Done();
  }
}


//=======================================================================
//function : BRepPrimAPI_MakeHalfSpace
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeHalfSpace::BRepPrimAPI_MakeHalfSpace(const TopoDS_Shell& theShell,
                                                     const gp_Pnt&       theRefPnt)
{
  // Set the flag is <IsDone> to False.
  NotDone();

  // Find the point of the skin closest to the reference point.
  gp_Pnt aMinPnt;
  gp_Dir aNormal;
  if (FindExtrema(theRefPnt, theShell, aMinPnt, aNormal)) {
    Standard_Boolean toReverse = isOutside(theRefPnt, aMinPnt, aNormal);

    // Construction of the open solid.
    TopoDS_Shell aShell = theShell;
    BRep_Builder().MakeSolid(mySolid);
    if (toReverse) {
      aShell.Reverse();
    }
    BRep_Builder().Add(mySolid, aShell);
    Done();
  }
}


//=======================================================================
//function : Solid
//purpose  : 
//=======================================================================

const TopoDS_Solid& BRepPrimAPI_MakeHalfSpace::Solid() const
{
  StdFail_NotDone_Raise_if( !IsDone(), "BRepPrimAPI_MakeHalfSpace::Solid");
  return mySolid;
}



//=======================================================================
//function : TopoDS_Solid
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeHalfSpace::operator TopoDS_Solid() const
{
  return Solid();
}
