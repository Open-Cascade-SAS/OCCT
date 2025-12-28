// Created on: 1998-08-17
// Created by: Philippe MANGIN
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

#ifndef _BRepFill_ShapeLaw_HeaderFile
#define _BRepFill_ShapeLaw_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <BRepFill_SectionLaw.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
class Law_Function;
class TopoDS_Vertex;
class TopoDS_Wire;
class GeomFill_SectionLaw;
class TopoDS_Edge;

//! Build Section Law, with an Vertex, or an Wire
class BRepFill_ShapeLaw : public BRepFill_SectionLaw
{

public:
  //! Construct an constant Law
  Standard_EXPORT BRepFill_ShapeLaw(const TopoDS_Vertex&   V,
                                    const bool Build = true);

  //! Construct an constant Law
  Standard_EXPORT BRepFill_ShapeLaw(const TopoDS_Wire&     W,
                                    const bool Build = true);

  //! Construct an evolutive Law
  Standard_EXPORT BRepFill_ShapeLaw(const TopoDS_Wire&          W,
                                    const occ::handle<Law_Function>& L,
                                    const bool      Build = true);

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

  const TopoDS_Edge& Edge(const int Index) const;

  DEFINE_STANDARD_RTTIEXT(BRepFill_ShapeLaw, BRepFill_SectionLaw)

protected:
  bool vertex;

private:
  Standard_EXPORT void Init(const bool B);

  TopoDS_Shape                    myShape;
  occ::handle<NCollection_HArray1<TopoDS_Shape>> myEdges;
  occ::handle<Law_Function>            TheLaw;
};

#include <BRepFill_ShapeLaw.lxx>

#endif // _BRepFill_ShapeLaw_HeaderFile
