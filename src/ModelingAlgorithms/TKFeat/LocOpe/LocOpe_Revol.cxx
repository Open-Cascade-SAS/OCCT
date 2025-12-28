// Created on: 1996-09-04
// Created by: Olga PILLOT
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

#include <BRepLib_MakeVertex.hxx>
#include <BRepSweep_Revol.hxx>
#include <BRepTools_Modifier.hxx>
#include <BRepTools_TrsfModification.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Circ.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <LocOpe.hxx>
#include <LocOpe_BuildShape.hxx>
#include <LocOpe_Revol.hxx>
#include <Precision.hxx>
#include <StdFail_NotDone.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>

static bool FindCircle(const gp_Ax1&, const gp_Pnt&, gp_Circ&);

//=================================================================================================

LocOpe_Revol::LocOpe_Revol()
    : myAngle(0.0),
      myAngTra(0.0),
      myIsTrans(false),
      myDone(false)
{
}

//=================================================================================================

void LocOpe_Revol::Perform(const TopoDS_Shape& Base, const gp_Ax1& Axis, const double Angle)
{
  myMap.Clear();
  myFirstShape.Nullify();
  myLastShape.Nullify();
  myBase.Nullify();
  myRes.Nullify();
  myBase    = Base;
  myAngle   = Angle;
  myAxis    = Axis;
  myAngTra  = 0.;
  myIsTrans = false;
  IntPerf();
}

//=================================================================================================

void LocOpe_Revol::Perform(const TopoDS_Shape& Base,
                           const gp_Ax1&       Axis,
                           const double        Angle,
                           const double        angledec)
{
  myMap.Clear();
  myFirstShape.Nullify();
  myLastShape.Nullify();
  myBase.Nullify();
  myRes.Nullify();
  myBase    = Base;
  myAngle   = Angle;
  myAxis    = Axis;
  myAngTra  = angledec;
  myIsTrans = true;
  IntPerf();
}

//=================================================================================================

void LocOpe_Revol::IntPerf()
{
  TopoDS_Shape       theBase = myBase;
  BRepTools_Modifier Modif;
  if (myIsTrans)
  {
    gp_Trsf T;
    T.SetRotation(myAxis, myAngTra);
    occ::handle<BRepTools_TrsfModification> modbase = new BRepTools_TrsfModification(T);
    Modif.Init(theBase);
    Modif.Perform(modbase);
    theBase = Modif.ModifiedShape(theBase);
  }

  BRepSweep_Revol theRevol(theBase, myAxis, myAngle);

  myFirstShape = theRevol.FirstShape();
  myLastShape  = theRevol.LastShape();

  TopExp_Explorer exp;
  if (theBase.ShapeType() == TopAbs_FACE)
  {
    for (exp.Init(theBase, TopAbs_EDGE); exp.More(); exp.Next())
    {
      const TopoDS_Edge& edg = TopoDS::Edge(exp.Current());
      if (!myMap.IsBound(edg))
      {
        NCollection_List<TopoDS_Shape> thelist;
        myMap.Bind(edg, thelist);
        TopoDS_Shape desc = theRevol.Shape(edg);
        if (!desc.IsNull())
        {
          myMap(edg).Append(desc);
        }
      }
    }
    myRes = theRevol.Shape();
  }

  else
  {
    // Cas base != FACE
    NCollection_IndexedDataMap<TopoDS_Shape,
                               NCollection_List<TopoDS_Shape>,
                               TopTools_ShapeMapHasher>
      theEFMap;
    TopExp::MapShapesAndAncestors(theBase, TopAbs_EDGE, TopAbs_FACE, theEFMap);
    NCollection_List<TopoDS_Shape> lfaces;
    bool                           toremove = false;
    for (int i = 1; i <= theEFMap.Extent(); i++)
    {
      const TopoDS_Shape&            edg = theEFMap.FindKey(i);
      NCollection_List<TopoDS_Shape> thelist1;
      myMap.Bind(edg, thelist1);
      TopoDS_Shape desc = theRevol.Shape(edg);
      if (!desc.IsNull())
      {
        if (theEFMap(i).Extent() >= 2)
        {
          toremove = true;
        }
        else
        {
          myMap(edg).Append(desc);
          lfaces.Append(desc);
        }
      }
    }
    if (toremove)
    {
      // Rajouter les faces de FirstShape et LastShape
      for (exp.Init(myFirstShape, TopAbs_FACE); exp.More(); exp.Next())
      {
        lfaces.Append(exp.Current());
      }
      for (exp.Init(myLastShape, TopAbs_FACE); exp.More(); exp.Next())
      {
        lfaces.Append(exp.Current());
      }

      LocOpe_BuildShape BS(lfaces);
      myRes = BS.Shape();
    }
    else
    {
      for (exp.Init(theBase, TopAbs_EDGE); exp.More(); exp.Next())
      {
        const TopoDS_Edge& edg = TopoDS::Edge(exp.Current());
        if (!myMap.IsBound(edg))
        {
          NCollection_List<TopoDS_Shape> thelist2;
          myMap.Bind(edg, thelist2);
          TopoDS_Shape desc = theRevol.Shape(edg);
          if (!desc.IsNull())
          {
            myMap(edg).Append(desc);
          }
        }
      }
      myRes = theRevol.Shape();
    }
  }

  if (myIsTrans)
  {
    // m-a-j des descendants
    TopExp_Explorer anExp;
    for (anExp.Init(myBase, TopAbs_EDGE); anExp.More(); anExp.Next())
    {
      const TopoDS_Edge& edg    = TopoDS::Edge(anExp.Current());
      const TopoDS_Edge& edgbis = TopoDS::Edge(Modif.ModifiedShape(edg));
      if (!edgbis.IsSame(edg) && myMap.IsBound(edgbis))
      {
        myMap.Bind(edg, myMap(edgbis));
        myMap.UnBind(edgbis);
      }
    }
  }
  myDone = true;
}

//=================================================================================================

const TopoDS_Shape& LocOpe_Revol::Shape() const
{
  if (!myDone)
  {
    throw StdFail_NotDone();
  }
  return myRes;
}

//=================================================================================================

const TopoDS_Shape& LocOpe_Revol::FirstShape() const
{
  return myFirstShape;
}

//=================================================================================================

const TopoDS_Shape& LocOpe_Revol::LastShape() const
{
  return myLastShape;
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& LocOpe_Revol::Shapes(const TopoDS_Shape& S) const
{
  return myMap(S);
}

//=================================================================================================

void LocOpe_Revol::Curves(NCollection_Sequence<occ::handle<Geom_Curve>>& Scurves) const
{
  Scurves.Clear();
  NCollection_Sequence<gp_Pnt> spt;
  LocOpe::SampleEdges(myFirstShape, spt);
  for (int jj = 1; jj <= spt.Length(); jj++)
  {
    const gp_Pnt& pvt = spt(jj);
    gp_Circ       CAX;
    if (FindCircle(myAxis, pvt, CAX))
    {
      gp_Ax2                   A2 = CAX.Position();
      double                   r  = CAX.Radius();
      occ::handle<Geom_Circle> Ci = new Geom_Circle(A2, r);
      Scurves.Append(Ci);
    }
  }
}

//=================================================================================================

occ::handle<Geom_Curve> LocOpe_Revol::BarycCurve() const
{
  gp_Pnt                       bar(0., 0., 0.);
  NCollection_Sequence<gp_Pnt> spt;
  LocOpe::SampleEdges(myFirstShape, spt);
  for (int jj = 1; jj <= spt.Length(); jj++)
  {
    const gp_Pnt& pvt = spt(jj);
    bar.ChangeCoord() += pvt.XYZ();
  }
  bar.ChangeCoord().Divide(spt.Length());
  gp_Circ                  CAX;
  occ::handle<Geom_Circle> theCi;
  if (FindCircle(myAxis, bar, CAX))
  {
    gp_Ax2 A2 = CAX.Position();
    double r  = CAX.Radius();
    theCi     = new Geom_Circle(A2, r);
  }
  return theCi;
}

//=================================================================================================

static bool FindCircle(const gp_Ax1& Ax, const gp_Pnt& Pt, gp_Circ& Ci)
{

  const gp_Dir& Dax = Ax.Direction();
  gp_Vec        OP(Ax.Location(), Pt);

  double prm = OP.Dot(Dax);

  gp_Pnt prj(Ax.Location().XYZ().Added(prm * Dax.XYZ()));
  gp_Vec axx(prj, Pt);
  double Radius = axx.Magnitude();
  if (Radius < Precision::Confusion())
  {
    return false;
  }
  Ci.SetRadius(Radius);
  Ci.SetPosition(gp_Ax2(prj, Dax, axx));
  return true;
}
