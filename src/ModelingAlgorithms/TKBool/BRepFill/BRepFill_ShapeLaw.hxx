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
#include <TopTools_HArray1OfShape.hxx>
#include <BRepFill_SectionLaw.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
class Law_Function;
class TopoDS_Vertex;
class TopoDS_Wire;
class GeomFill_SectionLaw;
class TopoDS_Edge;

class BRepFill_ShapeLaw;
DEFINE_STANDARD_HANDLE(BRepFill_ShapeLaw, BRepFill_SectionLaw)

//! Build Section Law, with an Vertex, or an Wire
class BRepFill_ShapeLaw : public BRepFill_SectionLaw
{

public:
  //! Construct an constant Law
  Standard_EXPORT BRepFill_ShapeLaw(const TopoDS_Vertex&   V,
                                    const Standard_Boolean Build = Standard_True);

  //! Construct an constant Law
  Standard_EXPORT BRepFill_ShapeLaw(const TopoDS_Wire&     W,
                                    const Standard_Boolean Build = Standard_True);

  //! Construct an evolutive Law
  Standard_EXPORT BRepFill_ShapeLaw(const TopoDS_Wire&          W,
                                    const Handle(Law_Function)& L,
                                    const Standard_Boolean      Build = Standard_True);

  //! Say if the input shape is a  vertex.
  Standard_EXPORT virtual Standard_Boolean IsVertex() const Standard_OVERRIDE;

  //! Say if the Law is  Constant.
  Standard_EXPORT virtual Standard_Boolean IsConstant() const Standard_OVERRIDE;

  //! Give the law build on a concatenated section
  Standard_EXPORT virtual Handle(GeomFill_SectionLaw) ConcatenedLaw() const Standard_OVERRIDE;

  Standard_EXPORT virtual GeomAbs_Shape Continuity(const Standard_Integer Index,
                                                   const Standard_Real    TolAngular) const
    Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Real VertexTol(const Standard_Integer Index,
                                                  const Standard_Real    Param) const
    Standard_OVERRIDE;

  Standard_EXPORT virtual TopoDS_Vertex Vertex(const Standard_Integer Index,
                                               const Standard_Real Param) const Standard_OVERRIDE;

  Standard_EXPORT virtual void D0(const Standard_Real Param, TopoDS_Shape& S) Standard_OVERRIDE;

  const TopoDS_Edge& Edge(const Standard_Integer Index) const;

  DEFINE_STANDARD_RTTIEXT(BRepFill_ShapeLaw, BRepFill_SectionLaw)

protected:
  Standard_Boolean vertex;

private:
  Standard_EXPORT void Init(const Standard_Boolean B);

  TopoDS_Shape                    myShape;
  Handle(TopTools_HArray1OfShape) myEdges;
  Handle(Law_Function)            TheLaw;
};

#include <BRepFill_ShapeLaw.lxx>

#endif // _BRepFill_ShapeLaw_HeaderFile
