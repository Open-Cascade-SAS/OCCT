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

#ifndef _BRep_TVertex_HeaderFile
#define _BRep_TVertex_HeaderFile

#include <Standard.hxx>

#include <gp_Pnt.hxx>
#include <Standard_Real.hxx>
#include <BRep_PointRepresentation.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_TVertex.hxx>
class TopoDS_TShape;

//! The TVertex from BRep inherits from the TVertex
//! from TopoDS. It contains the geometric data.
//!
//! The TVertex contains a 3d point, location and a tolerance.
class BRep_TVertex : public TopoDS_TVertex
{

public:
  Standard_EXPORT BRep_TVertex();

  double Tolerance() const;

  void Tolerance(const double T);

  //! Sets the tolerance to the max of <T> and the
  //! current tolerance.
  void UpdateTolerance(const double T);

  const gp_Pnt& Pnt() const;

  void Pnt(const gp_Pnt& P);

  const NCollection_List<occ::handle<BRep_PointRepresentation>>& Points() const;

  NCollection_List<occ::handle<BRep_PointRepresentation>>& ChangePoints();

  //! Returns a copy of the TShape with no sub-shapes.
  Standard_EXPORT occ::handle<TopoDS_TShape> EmptyCopy() const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream,
                                        int               theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(BRep_TVertex, TopoDS_TVertex)

private:
  gp_Pnt                                                  myPnt;
  double                                                  myTolerance;
  NCollection_List<occ::handle<BRep_PointRepresentation>> myPoints;
};

#include <BRep_TVertex.lxx>

#endif // _BRep_TVertex_HeaderFile
