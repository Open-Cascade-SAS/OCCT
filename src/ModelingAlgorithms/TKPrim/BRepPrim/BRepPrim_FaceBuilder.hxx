// Created on: 1993-06-23
// Created by: Remi LEQUETTE
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

#ifndef _BRepPrim_FaceBuilder_HeaderFile
#define _BRepPrim_FaceBuilder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <Standard_Integer.hxx>
class BRep_Builder;
class Geom_Surface;

//! The  FaceBuilder is an algorithm   to build a BRep
//! Face from a Geom Surface.
//!
//! The  face covers  the  whole surface or  the  area
//! delimited by UMin, UMax, VMin, VMax
class BRepPrim_FaceBuilder
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepPrim_FaceBuilder();

  Standard_EXPORT BRepPrim_FaceBuilder(const BRep_Builder& B, const Handle(Geom_Surface)& S);

  Standard_EXPORT BRepPrim_FaceBuilder(const BRep_Builder&         B,
                                       const Handle(Geom_Surface)& S,
                                       const Standard_Real         UMin,
                                       const Standard_Real         UMax,
                                       const Standard_Real         VMin,
                                       const Standard_Real         VMax);

  Standard_EXPORT void Init(const BRep_Builder& B, const Handle(Geom_Surface)& S);

  Standard_EXPORT void Init(const BRep_Builder&         B,
                            const Handle(Geom_Surface)& S,
                            const Standard_Real         UMin,
                            const Standard_Real         UMax,
                            const Standard_Real         VMin,
                            const Standard_Real         VMax);

  Standard_EXPORT const TopoDS_Face& Face() const;
  Standard_EXPORT                    operator TopoDS_Face();

  //! Returns the edge of index <I>
  //! 1 - Edge VMin
  //! 2 - Edge UMax
  //! 3 - Edge VMax
  //! 4 - Edge UMin
  Standard_EXPORT const TopoDS_Edge& Edge(const Standard_Integer I) const;

  //! Returns the vertex of index <I>
  //! 1 - Vertex UMin,VMin
  //! 2 - Vertex UMax,VMin
  //! 3 - Vertex UMax,VMax
  //! 4 - Vertex UMin,VMax
  Standard_EXPORT const TopoDS_Vertex& Vertex(const Standard_Integer I) const;

protected:
private:
  TopoDS_Vertex myVertex[4];
  TopoDS_Edge   myEdges[4];
  TopoDS_Face   myFace;
};

#endif // _BRepPrim_FaceBuilder_HeaderFile
