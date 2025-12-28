// Created on: 1998-12-29
// Created by: Joelle CHAUVET
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

#ifndef _BRepFill_NSections_HeaderFile
#define _BRepFill_NSections_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>
#include <gp_Trsf.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_Sequence.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <BRepFill_SectionLaw.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
class Geom_BSplineSurface;
class GeomFill_SectionLaw;
class TopoDS_Vertex;
class TopoDS_Shape;

//! Build Section Law, with N Sections
class BRepFill_NSections : public BRepFill_SectionLaw
{

public:
  //! Construct
  Standard_EXPORT BRepFill_NSections(const NCollection_Sequence<TopoDS_Shape>& S,
                                     const bool          Build = true);

  //! Construct
  Standard_EXPORT BRepFill_NSections(const NCollection_Sequence<TopoDS_Shape>& S,
                                     const NCollection_Sequence<gp_Trsf>&  Trsfs,
                                     const NCollection_Sequence<double>&   P,
                                     const double             VF,
                                     const double             VL,
                                     const bool          Build = true);

  //! Say if the input shape is a vertex.
  Standard_EXPORT virtual bool IsVertex() const override;

  //! Say if the Law is Constant.
  Standard_EXPORT virtual bool IsConstant() const override;

  //! Give the law build on a concatenated section
  Standard_EXPORT virtual occ::handle<GeomFill_SectionLaw> ConcatenedLaw() const override;

  Standard_EXPORT virtual GeomAbs_Shape Continuity(const int Index,
                                                   const double    TolAngular) const
    override;

  Standard_EXPORT virtual double VertexTol(const int Index,
                                                  const double    Param) const
    override;

  Standard_EXPORT virtual TopoDS_Vertex Vertex(const int Index,
                                               const double Param) const override;

  Standard_EXPORT virtual void D0(const double Param, TopoDS_Shape& S) override;

  DEFINE_STANDARD_RTTIEXT(BRepFill_NSections, BRepFill_SectionLaw)

private:
  Standard_EXPORT void Init(const NCollection_Sequence<double>& P, const bool B);

  double                   VFirst;
  double                   VLast;
  NCollection_Sequence<TopoDS_Shape>        myShapes;
  NCollection_Sequence<gp_Trsf>         myTrsfs;
  NCollection_Sequence<double>          myParams;
  occ::handle<NCollection_HArray2<TopoDS_Shape>> myEdges;
  occ::handle<Geom_BSplineSurface>     mySurface;
};

#endif // _BRepFill_NSections_HeaderFile
