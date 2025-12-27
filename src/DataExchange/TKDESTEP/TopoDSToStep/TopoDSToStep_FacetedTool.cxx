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
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDSToStep_FacetedTool.hxx>

//=================================================================================================

TopoDSToStep_FacetedError TopoDSToStep_FacetedTool::CheckTopoDSShape(const TopoDS_Shape& aShape)
{
  TopExp_Explorer FaceExp, EdgeExp;
  FaceExp.Init(aShape, TopAbs_FACE);
  while (FaceExp.More())
  {
    const TopoDS_Face aFace = TopoDS::Face(FaceExp.Current());
    FaceExp.Next();

    occ::handle<Geom_Surface> Su = BRep_Tool::Surface(aFace);

    if (Su->IsKind(STANDARD_TYPE(Geom_Plane)))
    {
      // OK -> no further check
    }
    else if (Su->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
    {
      occ::handle<Geom_BSplineSurface> aBsplS = occ::down_cast<Geom_BSplineSurface>(Su);
      int            uDeg, vDeg, nUPol, nVPol;
      uDeg = aBsplS->UDegree();
      if (uDeg == 1)
      {
        vDeg = aBsplS->VDegree();
        if (vDeg == 1)
        {
          nUPol = aBsplS->NbUPoles();
          nVPol = aBsplS->NbVPoles();
          if (nUPol != 2 || nVPol != 2)
          {
            return TopoDSToStep_SurfaceNotPlane;
          }
        }
        else
        {
          // Degree in v != 1
          return TopoDSToStep_SurfaceNotPlane;
        }
      }
      else
      {
        // Degree in u != 1
        return TopoDSToStep_SurfaceNotPlane;
      }
    }
    else if (Su->IsKind(STANDARD_TYPE(Geom_BezierSurface)))
    {
      occ::handle<Geom_BezierSurface> aBzS = occ::down_cast<Geom_BezierSurface>(Su);
      int           uDeg, vDeg, nUPol, nVPol;
      uDeg = aBzS->UDegree();
      if (uDeg == 1)
      {
        vDeg = aBzS->VDegree();
        if (vDeg == 1)
        {
          nUPol = aBzS->NbUPoles();
          nVPol = aBzS->NbVPoles();
          if (nUPol != 2 || nVPol != 2)
          {
            return TopoDSToStep_SurfaceNotPlane;
          }
        }
        else
        {
          // Degree in v != 1
          return TopoDSToStep_SurfaceNotPlane;
        }
      }
      else
      {
        // Degree in u != 1
        return TopoDSToStep_SurfaceNotPlane;
      }
    }
    else
    {
      // the surface is neither a Plane nor a flat BSpline or Beziersurface
      return TopoDSToStep_SurfaceNotPlane;
    }

    // surface is flat; now check, if the PCurves are linear

    EdgeExp.Init(aFace, TopAbs_EDGE);
    while (EdgeExp.More())
    {
      const TopoDS_Edge anEdge = TopoDS::Edge(EdgeExp.Current());
      EdgeExp.Next();

      double        cf, cl;
      occ::handle<Geom2d_Curve> C2d = BRep_Tool::CurveOnSurface(anEdge, aFace, cf, cl);

      if (C2d->IsKind(STANDARD_TYPE(Geom2d_Line)))
      {
        return TopoDSToStep_FacetedDone;
      }
      else if (C2d->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
      {
        occ::handle<Geom2d_BSplineCurve> aBspl2d = occ::down_cast<Geom2d_BSplineCurve>(C2d);
        if ((aBspl2d->Degree() != 1) || (aBspl2d->NbPoles() != 2))
        {
          return TopoDSToStep_PCurveNotLinear;
        }
      }
      else if (C2d->IsKind(STANDARD_TYPE(Geom2d_BezierCurve)))
      {
        occ::handle<Geom2d_BezierCurve> aBzC2d = occ::down_cast<Geom2d_BezierCurve>(C2d);
        if ((aBzC2d->Degree() != 1) || (aBzC2d->NbPoles() != 2))
        {
          return TopoDSToStep_PCurveNotLinear;
        }
      }
      else
      {
        return TopoDSToStep_PCurveNotLinear;
      }
    } // end while (EdgeExp.More())
  } // end while (FaceExp.More())

  return TopoDSToStep_FacetedDone;
}
