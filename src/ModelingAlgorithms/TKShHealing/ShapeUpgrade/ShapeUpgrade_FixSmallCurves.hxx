// Created on: 2000-06-07
// Created by: Galina KULIKOVA
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _ShapeUpgrade_FixSmallCurves_HeaderFile
#define _ShapeUpgrade_FixSmallCurves_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <ShapeUpgrade_Tool.hxx>
#include <Standard_Real.hxx>
#include <ShapeExtend_Status.hxx>
class ShapeUpgrade_SplitCurve3d;
class ShapeUpgrade_SplitCurve2d;
class Geom_Curve;
class Geom2d_Curve;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

class ShapeUpgrade_FixSmallCurves : public ShapeUpgrade_Tool
{

public:
  Standard_EXPORT ShapeUpgrade_FixSmallCurves();

  Standard_EXPORT void Init(const TopoDS_Edge& theEdge, const TopoDS_Face& theFace);

  Standard_EXPORT virtual bool Approx(occ::handle<Geom_Curve>&   Curve3d,
                                      occ::handle<Geom2d_Curve>& Curve2d,
                                      occ::handle<Geom2d_Curve>& Curve2dR,
                                      double&                    First,
                                      double&                    Last);

  //! Sets the tool for splitting 3D curves.
  Standard_EXPORT void SetSplitCurve3dTool(
    const occ::handle<ShapeUpgrade_SplitCurve3d>& splitCurve3dTool);

  //! Sets the tool for splitting pcurves.
  Standard_EXPORT void SetSplitCurve2dTool(
    const occ::handle<ShapeUpgrade_SplitCurve2d>& splitCurve2dTool);

  //! Queries the status of last call to Perform
  //! OK   :
  //! DONE1:
  //! DONE2:
  //! FAIL1:
  Standard_EXPORT bool Status(const ShapeExtend_Status status) const;

  DEFINE_STANDARD_RTTIEXT(ShapeUpgrade_FixSmallCurves, ShapeUpgrade_Tool)

protected:
  Standard_EXPORT virtual occ::handle<ShapeUpgrade_SplitCurve3d> GetSplitCurve3dTool() const;

  //! Returns the tool for splitting pcurves.
  Standard_EXPORT virtual occ::handle<ShapeUpgrade_SplitCurve2d> GetSplitCurve2dTool() const;

  int                                    myStatus;
  occ::handle<ShapeUpgrade_SplitCurve3d> mySplitCurve3dTool;
  occ::handle<ShapeUpgrade_SplitCurve2d> mySplitCurve2dTool;
  TopoDS_Edge                            myEdge;
  TopoDS_Face                            myFace;
};

#endif // _ShapeUpgrade_FixSmallCurves_HeaderFile
