// Created on: 1993-10-11
// Created by: Christophe MARION
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

#ifndef _HLRBRep_HLRToShape_HeaderFile
#define _HLRBRep_HLRToShape_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <Standard_Integer.hxx>
#include <HLRBRep_TypeOfResultingEdge.hxx>
class HLRBRep_Algo;
class TopoDS_Shape;
class HLRBRep_Data;
class HLRBRep_EdgeData;

//! A framework for filtering the computation
//! results of an HLRBRep_Algo algorithm by extraction.
//! From the results calculated by the algorithm on
//! a shape, a filter returns the type of edge you
//! want to identify. You can choose any of the following types of output:
//! -   visible sharp edges
//! -   hidden sharp edges
//! -   visible smooth edges
//! -   hidden smooth edges
//! -   visible sewn edges
//! -   hidden sewn edges
//! -   visible outline edges
//! -   hidden outline edges.
//! -   visible isoparameters and
//! -   hidden isoparameters.
//! Sharp edges present a C0 continuity (non G1).
//! Smooth edges present a G1 continuity (non G2).
//! Sewn edges present a C2 continuity.
//! The result is composed of 2D edges in the
//! projection plane of the view which the
//! algorithm has worked with. These 2D edges
//! are not included in the data structure of the visualized shape.
//! In order to obtain a complete image, you must
//! combine the shapes given by each of the chosen filters.
//! The construction of the shape does not call a
//! new computation of the algorithm, but only
//! reads its internal results.
//! The methods of this shape are almost identic to those of the HLRBrep_PolyHLRToShape class.
class HLRBRep_HLRToShape
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs a framework for filtering the
  //! results of the HLRBRep_Algo algorithm, A.
  //! Use the extraction filters to obtain the results you want for A.
  Standard_EXPORT HLRBRep_HLRToShape(const Handle(HLRBRep_Algo)& A);

  TopoDS_Shape VCompound();

  TopoDS_Shape VCompound(const TopoDS_Shape& S);

  TopoDS_Shape Rg1LineVCompound();

  TopoDS_Shape Rg1LineVCompound(const TopoDS_Shape& S);

  TopoDS_Shape RgNLineVCompound();

  TopoDS_Shape RgNLineVCompound(const TopoDS_Shape& S);

  TopoDS_Shape OutLineVCompound();

  TopoDS_Shape OutLineVCompound3d();

  TopoDS_Shape OutLineVCompound(const TopoDS_Shape& S);

  TopoDS_Shape IsoLineVCompound();

  TopoDS_Shape IsoLineVCompound(const TopoDS_Shape& S);

  TopoDS_Shape HCompound();

  TopoDS_Shape HCompound(const TopoDS_Shape& S);

  TopoDS_Shape Rg1LineHCompound();

  TopoDS_Shape Rg1LineHCompound(const TopoDS_Shape& S);

  TopoDS_Shape RgNLineHCompound();

  TopoDS_Shape RgNLineHCompound(const TopoDS_Shape& S);

  TopoDS_Shape OutLineHCompound();

  TopoDS_Shape OutLineHCompound(const TopoDS_Shape& S);

  TopoDS_Shape IsoLineHCompound();

  TopoDS_Shape IsoLineHCompound(const TopoDS_Shape& S);

  //! Returns compound of resulting edges
  //! of required type and visibility,
  //! taking into account the kind of space
  //! (2d or 3d)
  TopoDS_Shape CompoundOfEdges(const HLRBRep_TypeOfResultingEdge type,
                               const Standard_Boolean            visible,
                               const Standard_Boolean            In3d);

  //! For specified shape
  //! returns compound of resulting edges
  //! of required type and visibility,
  //! taking into account the kind of space
  //! (2d or 3d)
  TopoDS_Shape CompoundOfEdges(const TopoDS_Shape&               S,
                               const HLRBRep_TypeOfResultingEdge type,
                               const Standard_Boolean            visible,
                               const Standard_Boolean            In3d);

protected:
private:
  Standard_EXPORT TopoDS_Shape InternalCompound(const Standard_Integer typ,
                                                const Standard_Boolean visible,
                                                const TopoDS_Shape&    S,
                                                const Standard_Boolean In3d = Standard_False);

  Standard_EXPORT void DrawFace(const Standard_Boolean visible,
                                const Standard_Integer typ,
                                const Standard_Integer iface,
                                Handle(HLRBRep_Data)&  DS,
                                TopoDS_Shape&          Result,
                                Standard_Boolean&      added,
                                const Standard_Boolean In3d = Standard_False) const;

  Standard_EXPORT void DrawEdge(const Standard_Boolean visible,
                                const Standard_Boolean inFace,
                                const Standard_Integer typ,
                                HLRBRep_EdgeData&      ed,
                                TopoDS_Shape&          Result,
                                Standard_Boolean&      added,
                                const Standard_Boolean In3d = Standard_False) const;

  Handle(HLRBRep_Algo) myAlgo;
};

#include <HLRBRep_HLRToShape.lxx>

#endif // _HLRBRep_HLRToShape_HeaderFile
