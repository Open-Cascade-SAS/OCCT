// Created on: 1999-03-09
// Created by: Roman LYGIN
// Copyright (c) 1999 Matra Datavision
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

#ifndef _ShapeCustom_TrsfModification_HeaderFile
#define _ShapeCustom_TrsfModification_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BRepTools_TrsfModification.hxx>
class gp_Trsf;
class TopoDS_Face;
class Geom_Surface;
class TopLoc_Location;
class TopoDS_Edge;
class Geom_Curve;
class TopoDS_Vertex;
class gp_Pnt;
class Geom2d_Curve;

//! Complements BRepTools_TrsfModification to provide reversible
//! scaling regarding tolerances.
//! Uses actual tolerances (attached to the shapes) not ones
//! returned by BRep_Tool::Tolerance to work with tolerances
//! lower than Precision::Confusion.
class ShapeCustom_TrsfModification : public BRepTools_TrsfModification
{

public:
  //! Empty constructor
  Standard_EXPORT ShapeCustom_TrsfModification(const gp_Trsf& T);

  //! Calls inherited method.
  //! Sets <Tol> as actual tolerance of <F> multiplied with scale
  //! factor.
  Standard_EXPORT bool NewSurface(const TopoDS_Face&         F,
                                  occ::handle<Geom_Surface>& S,
                                  TopLoc_Location&           L,
                                  double&                    Tol,
                                  bool&                      RevWires,
                                  bool&                      RevFace) override;

  //! Calls inherited method.
  //! Sets <Tol> as actual tolerance of <E> multiplied with scale
  //! factor.
  Standard_EXPORT bool NewCurve(const TopoDS_Edge&       E,
                                occ::handle<Geom_Curve>& C,
                                TopLoc_Location&         L,
                                double&                  Tol) override;

  //! Calls inherited method.
  //! Sets <Tol> as actual tolerance of <V> multiplied with scale
  //! factor.
  Standard_EXPORT bool NewPoint(const TopoDS_Vertex& V, gp_Pnt& P, double& Tol) override;

  //! Calls inherited method.
  //! Sets <Tol> as actual tolerance of <E> multiplied with scale
  //! factor.
  Standard_EXPORT bool NewCurve2d(const TopoDS_Edge&         E,
                                  const TopoDS_Face&         F,
                                  const TopoDS_Edge&         NewE,
                                  const TopoDS_Face&         NewF,
                                  occ::handle<Geom2d_Curve>& C,
                                  double&                    Tol) override;

  //! Calls inherited method.
  //! Sets <Tol> as actual tolerance of <V> multiplied with scale
  //! factor.
  Standard_EXPORT bool NewParameter(const TopoDS_Vertex& V,
                                    const TopoDS_Edge&   E,
                                    double&              P,
                                    double&              Tol) override;

  DEFINE_STANDARD_RTTIEXT(ShapeCustom_TrsfModification, BRepTools_TrsfModification)
};

#endif // _ShapeCustom_TrsfModification_HeaderFile
