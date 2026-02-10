// Created on: 1992-05-27
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _BRep_TEdge_HeaderFile
#define _BRep_TEdge_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_TEdge.hxx>
class BRep_Curve3D;
class BRep_Polygon3D;
class TopoDS_TShape;

//! The TEdge from BRep is inherited from the TEdge
//! from TopoDS. It contains the geometric data.
//!
//! The TEdge contains a:
//!
//! * tolerance.
//! * same parameter flag.
//! * same range flag.
//! * Degenerated flag.
//! * list of curve representation.
class BRep_TEdge : public TopoDS_TEdge
{

public:
  //! Creates an empty TEdge.
  Standard_EXPORT BRep_TEdge();

  double Tolerance() const;

  void Tolerance(const double T);

  //! Sets the tolerance to the max of <T> and the
  //! current tolerance.
  void UpdateTolerance(const double T);

  Standard_EXPORT bool SameParameter() const;

  Standard_EXPORT void SameParameter(const bool S);

  Standard_EXPORT bool SameRange() const;

  Standard_EXPORT void SameRange(const bool S);

  Standard_EXPORT bool Degenerated() const;

  Standard_EXPORT void Degenerated(const bool S);

  const NCollection_List<occ::handle<BRep_CurveRepresentation>>& Curves() const;

  NCollection_List<occ::handle<BRep_CurveRepresentation>>& ChangeCurves();

  //! Returns the cached 3D curve representation, or null handle if none.
  const occ::handle<BRep_Curve3D>& Curve3D() const;

  //! Sets the cached 3D curve representation.
  void Curve3D(const occ::handle<BRep_Curve3D>& theCurve);

  //! Returns the cached 3D polygon representation, or null handle if none.
  const occ::handle<BRep_Polygon3D>& Polygon3D() const;

  //! Sets the cached 3D polygon representation.
  void Polygon3D(const occ::handle<BRep_Polygon3D>& thePolygon);

  //! Returns a copy of the TShape with no sub-shapes.
  Standard_EXPORT occ::handle<TopoDS_TShape> EmptyCopy() const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(BRep_TEdge, TopoDS_TEdge)

private:
  double                                                  myTolerance;
  int                                                     myFlags;
  NCollection_List<occ::handle<BRep_CurveRepresentation>> myCurves;
  occ::handle<BRep_Curve3D>                               myCurve3D;
  occ::handle<BRep_Polygon3D>                             myPolygon3D;
};

#include <BRep_TEdge.lxx>

#endif // _BRep_TEdge_HeaderFile
