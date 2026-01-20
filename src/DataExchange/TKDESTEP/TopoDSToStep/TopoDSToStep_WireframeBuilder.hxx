// Created on: 1995-03-17
// Created by: Dieter THIEMANN
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _TopoDSToStep_WireframeBuilder_HeaderFile
#define _TopoDSToStep_WireframeBuilder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <TopoDSToStep_BuilderError.hxx>
#include <TopoDSToStep_Root.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Transient.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
class TopoDS_Shape;
class TopoDSToStep_Tool;
class Transfer_FinderProcess;
class TopoDS_Edge;
class TopoDS_Face;

//! This builder Class provides services to build
//! a ProSTEP Wireframemodel from a Cas.Cad BRep.
class TopoDSToStep_WireframeBuilder : public TopoDSToStep_Root
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopoDSToStep_WireframeBuilder();

  Standard_EXPORT TopoDSToStep_WireframeBuilder(
    const TopoDS_Shape&     S,
    TopoDSToStep_Tool&      T,
    const StepData_Factors& theLocalFactors = StepData_Factors());

  Standard_EXPORT void Init(const TopoDS_Shape&     S,
                            TopoDSToStep_Tool&      T,
                            const StepData_Factors& theLocalFactors = StepData_Factors());

  Standard_EXPORT TopoDSToStep_BuilderError Error() const;

  Standard_EXPORT const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& Value() const;

  //! Extraction of Trimmed Curves from TopoDS_Edge for the
  //! Creation of a GeometricallyBoundedWireframeRepresentation
  Standard_EXPORT bool
    GetTrimmedCurveFromEdge(const TopoDS_Edge&                    E,
                            const TopoDS_Face&                    F,
                            NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher>&     M,
                            occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& L,
                            const StepData_Factors& theLocalFactors = StepData_Factors()) const;

  //! Extraction of Trimmed Curves from TopoDS_Face for the
  //! Creation of a GeometricallyBoundedWireframeRepresentation
  Standard_EXPORT bool
    GetTrimmedCurveFromFace(const TopoDS_Face&                    F,
                            NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher>&     M,
                            occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& L,
                            const StepData_Factors& theLocalFactors = StepData_Factors()) const;

  //! Extraction of Trimmed Curves from any TopoDS_Shape for the
  //! Creation of a GeometricallyBoundedWireframeRepresentation
  Standard_EXPORT bool
    GetTrimmedCurveFromShape(const TopoDS_Shape&                   S,
                             NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher>&     M,
                             occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& L,
                             const StepData_Factors& theLocalFactors = StepData_Factors()) const;

private:
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> myResult;
  TopoDSToStep_BuilderError            myError;
};

#endif // _TopoDSToStep_WireframeBuilder_HeaderFile
