// Created on: 1998-01-22
// Created by: Sergey ZARITCHNY
// Copyright (c) 1998-1999 Matra Datavision
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

#include <PrsDim_EllipseRadiusDimension.hxx>

#include <PrsDim.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(PrsDim_EllipseRadiusDimension, PrsDim_Relation)

//=================================================================================================

PrsDim_EllipseRadiusDimension::PrsDim_EllipseRadiusDimension(
  const TopoDS_Shape&               aShape,
  const TCollection_ExtendedString& aText)

{
  myFShape = aShape;
  myText   = aText;
  //  ComputeGeometry( );
}

//=================================================================================================

void PrsDim_EllipseRadiusDimension::ComputeGeometry()
{

  switch (myFShape.ShapeType())
  {
    case TopAbs_FACE: {
      // compute one face case
      ComputeFaceGeometry();
      break;
    }
    case TopAbs_EDGE: {
      ComputeEdgeGeometry();
      break;
    }
    default:
      break;
  }
  while (myFirstPar > 2 * M_PI)
    myFirstPar -= 2 * M_PI;
  while (myLastPar > 2 * M_PI)
    myLastPar -= 2 * M_PI;
  while (myFirstPar < 0.0)
    myFirstPar += 2 * M_PI;
  while (myLastPar < 0.0)
    myLastPar += 2 * M_PI;
}

//=================================================================================================

void PrsDim_EllipseRadiusDimension::ComputeFaceGeometry()
{

  gp_Pln                    aPln;
  occ::handle<Geom_Surface> aBasisSurf;
  PrsDim_KindOfSurface      aSurfType;
  double                    Offset;
  PrsDim::GetPlaneFromFace(TopoDS::Face(myFShape), aPln, aBasisSurf, aSurfType, Offset);

  if (aSurfType == PrsDim_KOS_Plane)
    ComputePlanarFaceGeometry();
  else
    ComputeCylFaceGeometry(aSurfType, aBasisSurf, Offset);
}

//=======================================================================
// function : ComputeCylFaceGeometry
// purpose  : defines Ellipse and plane of dimension
//=======================================================================

void PrsDim_EllipseRadiusDimension::ComputeCylFaceGeometry(
  const PrsDim_KindOfSurface       aSurfType,
  const occ::handle<Geom_Surface>& aBasisSurf,
  const double                     Offset)
{

  BRepAdaptor_Surface surf1(TopoDS::Face(myFShape));
  double              vFirst, vLast;
  vFirst      = surf1.FirstVParameter();
  vLast       = surf1.LastVParameter();
  double vMid = (vFirst + vLast) * 0.5;
  gp_Pln aPlane;
  gp_Ax1 Axis;
  //  double Param;
  if (aSurfType == PrsDim_KOS_Extrusion)
  {
    Axis.SetDirection((occ::down_cast<Geom_SurfaceOfLinearExtrusion>(aBasisSurf))->Direction());
    Axis.SetLocation(
      gp_Pnt((occ::down_cast<Geom_SurfaceOfLinearExtrusion>(aBasisSurf))->Direction().XYZ()));

    aPlane.SetAxis(Axis);
    aPlane.SetLocation(myEllipse.Location());
    myPlane = new Geom_Plane(aPlane);

    occ::handle<Geom_Curve> aCurve;
    aCurve = aBasisSurf->VIso(vMid);
    if (aCurve->DynamicType() == STANDARD_TYPE(Geom_Ellipse))
    {
      myEllipse = occ::down_cast<Geom_Ellipse>(aCurve)->Elips(); // gp_Elips
      myIsAnArc = false;
    }
    else if (aCurve->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve))
    {
      occ::handle<Geom_TrimmedCurve> tCurve = occ::down_cast<Geom_TrimmedCurve>(aCurve);
      aCurve                                = tCurve->BasisCurve();
      myFirstPar                            = tCurve->FirstParameter();
      myLastPar                             = tCurve->LastParameter();
      myIsAnArc                             = true;
      if (aCurve->DynamicType() == STANDARD_TYPE(Geom_Ellipse))
      {
        myEllipse = occ::down_cast<Geom_Ellipse>(aCurve)->Elips(); // gp_Elips
      }
    }
    else
    {
      throw Standard_ConstructionError("PrsDim:: Not expected type of surface");
      return;
    }

    // Offset

    if (surf1.GetType() == GeomAbs_OffsetSurface)
    {
      if (Offset < 0.0 && std::abs(Offset) > myEllipse.MinorRadius())
      {
        throw Standard_ConstructionError(
          "PrsDim:: Absolute value of negative offset is larger than MinorRadius");
        return;
      }

      myOffsetCurve  = new Geom_OffsetCurve(new Geom_Ellipse(myEllipse),
                                           Offset,
                                           myPlane->Pln().Axis().Direction());
      myOffset       = Offset;
      myIsOffset     = true;
      gp_Elips elips = myEllipse;
      double   Val   = Offset + elips.MajorRadius(); // simulation
      myEllipse.SetMajorRadius(Val);
      Val = Offset + elips.MinorRadius();
      myEllipse.SetMinorRadius(Val);
    }
    else
      myIsOffset = false;
  }
}

//=================================================================================================

void PrsDim_EllipseRadiusDimension::ComputePlanarFaceGeometry()
{

  bool            find = false;
  gp_Pnt          ptfirst, ptend;
  TopExp_Explorer ExploEd(TopoDS::Face(myFShape), TopAbs_EDGE);
  for (; ExploEd.More(); ExploEd.Next())
  {
    TopoDS_Edge               curedge = TopoDS::Edge(ExploEd.Current());
    occ::handle<Geom_Curve>   curv;
    occ::handle<Geom_Ellipse> ellips;
    if (PrsDim::ComputeGeometry(curedge, curv, ptfirst, ptend))
    {
      if (curv->DynamicType() == STANDARD_TYPE(Geom_Ellipse))
      {
        ellips = occ::down_cast<Geom_Ellipse>(curv);
        if (!ellips.IsNull())
        {
          myEllipse = ellips->Elips();
          find      = true;
          break;
        }
      }
    }
  }
  if (!find)
  {
    throw Standard_ConstructionError("PrsDim:: Curve is not an ellipsee or is Null");
    return;
  }

  if (!ptfirst.IsEqual(ptend, Precision::Confusion()))
  {
    myIsAnArc  = true;
    myFirstPar = ElCLib::Parameter(myEllipse, ptfirst);
    myLastPar  = ElCLib::Parameter(myEllipse, ptend);
  }
  else
    myIsAnArc = false;

  BRepAdaptor_Surface surfAlgo(TopoDS::Face(myFShape));
  myPlane = new Geom_Plane(surfAlgo.Plane());
}

//=================================================================================================

void PrsDim_EllipseRadiusDimension::ComputeEdgeGeometry()
{
  gp_Pnt                  ptfirst, ptend;
  occ::handle<Geom_Curve> curv;
  if (!PrsDim::ComputeGeometry(TopoDS::Edge(myFShape), curv, ptfirst, ptend))
    return;

  occ::handle<Geom_Ellipse> elips = occ::down_cast<Geom_Ellipse>(curv);
  if (elips.IsNull())
    return;

  myEllipse = elips->Elips();
  gp_Pln aPlane;
  aPlane.SetPosition(gp_Ax3(myEllipse.Position()));
  myPlane = new Geom_Plane(aPlane);

  if (ptfirst.IsEqual(ptend, Precision::Confusion()))
  {
    myIsAnArc = false;
  }
  else
  {
    myIsAnArc  = true;
    myFirstPar = ElCLib::Parameter(myEllipse, ptfirst);
    myLastPar  = ElCLib::Parameter(myEllipse, ptend);
  }
}
