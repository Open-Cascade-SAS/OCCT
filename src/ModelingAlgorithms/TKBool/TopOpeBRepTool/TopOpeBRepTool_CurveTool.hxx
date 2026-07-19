// Created on: 1993-06-24
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

#ifndef _TopOpeBRepTool_CurveTool_HeaderFile
#define _TopOpeBRepTool_CurveTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRepTool_GeomTool.hxx>
#include <TopOpeBRepTool_OutCurveType.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>
class Geom_Curve;
class Geom2d_Curve;
class TopoDS_Shape;

class TopOpeBRepTool_CurveTool
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepTool_CurveTool();

  Standard_EXPORT TopOpeBRepTool_CurveTool(const TopOpeBRepTool_OutCurveType OCT);

  Standard_EXPORT TopOpeBRepTool_CurveTool(const TopOpeBRepTool_GeomTool& GT);

  Standard_EXPORT TopOpeBRepTool_GeomTool& ChangeGeomTool();

  Standard_EXPORT const TopOpeBRepTool_GeomTool& GetGeomTool() const;

  Standard_EXPORT void SetGeomTool(const TopOpeBRepTool_GeomTool& GT);

  //! Approximates curves.
  //! Returns False in the case of failure
  Standard_EXPORT bool MakeCurves(const double                     min,
                                  const double                     max,
                                  const occ::handle<Geom_Curve>&   C3D,
                                  const occ::handle<Geom2d_Curve>& PC1,
                                  const occ::handle<Geom2d_Curve>& PC2,
                                  const TopoDS_Shape&              S1,
                                  const TopoDS_Shape&              S2,
                                  occ::handle<Geom_Curve>&         C3DN,
                                  occ::handle<Geom2d_Curve>&       PC1N,
                                  occ::handle<Geom2d_Curve>&       PC2N,
                                  double&                          Tol3d,
                                  double&                          Tol2d) const;

  Standard_EXPORT static occ::handle<Geom_Curve> MakeBSpline1fromPnt(
    const NCollection_Array1<gp_Pnt>& P);

  Standard_EXPORT static occ::handle<Geom2d_Curve> MakeBSpline1fromPnt2d(
    const NCollection_Array1<gp_Pnt2d>& P);

  Standard_EXPORT static bool IsProjectable(const TopoDS_Shape&            S,
                                            const occ::handle<Geom_Curve>& C);

  Standard_EXPORT static occ::handle<Geom2d_Curve> MakePCurveOnFace(
    const TopoDS_Shape&            S,
    const occ::handle<Geom_Curve>& C,
    double&                        TolReached2d,
    const double                   first = 0.0,
    const double                   last  = 0.0);

protected:
  TopOpeBRepTool_GeomTool myGeomTool;
};

#endif // _TopOpeBRepTool_CurveTool_HeaderFile
