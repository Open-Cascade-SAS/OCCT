// Created on: 1998-01-07
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

#ifndef _BRepFill_SectionLaw_HeaderFile
#define _BRepFill_SectionLaw_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <GeomFill_SectionLaw.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Standard_Transient.hxx>
#include <GeomAbs_Shape.hxx>
class GeomFill_SectionLaw;
class TopoDS_Vertex;
class TopoDS_Shape;
class TopoDS_Wire;
class TopoDS_Edge;

//! Build Section Law, with an Vertex, or an Wire
class BRepFill_SectionLaw : public Standard_Transient
{

public:
  Standard_EXPORT int NbLaw() const;

  Standard_EXPORT const occ::handle<GeomFill_SectionLaw>& Law(const int Index) const;

  Standard_EXPORT int IndexOfEdge(const TopoDS_Shape& anEdge) const;

  Standard_EXPORT virtual bool IsConstant() const = 0;

  Standard_EXPORT bool IsUClosed() const;

  Standard_EXPORT bool IsVClosed() const;

  Standard_EXPORT bool IsDone() const;

  //! Say if the input shape is a vertex.
  Standard_EXPORT virtual bool IsVertex() const = 0;

  Standard_EXPORT virtual occ::handle<GeomFill_SectionLaw> ConcatenedLaw() const = 0;

  Standard_EXPORT virtual GeomAbs_Shape Continuity(const int    Index,
                                                   const double TolAngular) const = 0;

  Standard_EXPORT virtual double VertexTol(const int Index, const double Param) const = 0;

  Standard_EXPORT virtual TopoDS_Vertex Vertex(const int Index, const double Param) const = 0;

  Standard_EXPORT virtual void D0(const double U, TopoDS_Shape& S) = 0;

  Standard_EXPORT void Init(const TopoDS_Wire& W);

  Standard_EXPORT TopoDS_Edge CurrentEdge();

  DEFINE_STANDARD_RTTIEXT(BRepFill_SectionLaw, Standard_Transient)

protected:
  occ::handle<NCollection_HArray1<occ::handle<GeomFill_SectionLaw>>> myLaws;
  bool                                                               uclosed;
  bool                                                               vclosed;
  bool                                                               myDone;
  NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>    myIndices;

private:
  BRepTools_WireExplorer myIterator;
};

#endif // _BRepFill_SectionLaw_HeaderFile
