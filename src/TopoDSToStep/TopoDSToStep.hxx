// Created on: 1994-11-25
// Created by: Frederic MAUPAS
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _TopoDSToStep_HeaderFile
#define _TopoDSToStep_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDSToStep_BuilderError.hxx>
#include <TopoDSToStep_MakeFaceError.hxx>
#include <TopoDSToStep_MakeWireError.hxx>
#include <TopoDSToStep_MakeEdgeError.hxx>
#include <TopoDSToStep_MakeVertexError.hxx>
class TCollection_HAsciiString;
class Transfer_FinderProcess;
class TopoDS_Shape;
class Standard_Transient;
class TopoDSToStep_Tool;
class TopoDSToStep_Root;
class TopoDSToStep_MakeManifoldSolidBrep;
class TopoDSToStep_MakeBrepWithVoids;
class TopoDSToStep_MakeFacetedBrep;
class TopoDSToStep_MakeFacetedBrepAndBrepWithVoids;
class TopoDSToStep_MakeShellBasedSurfaceModel;
class TopoDSToStep_MakeGeometricCurveSet;
class TopoDSToStep_Builder;
class TopoDSToStep_WireframeBuilder;
class TopoDSToStep_Tool;
class TopoDSToStep_FacetedTool;
class TopoDSToStep_MakeStepFace;
class TopoDSToStep_MakeStepWire;
class TopoDSToStep_MakeStepEdge;
class TopoDSToStep_MakeStepVertex;


//! This package implements the mapping between CAS.CAD
//! Shape representation and AP214 Shape Representation.
//! The target schema is pms_c4 (a subset of AP214)
//!
//! How to use this Package :
//!
//! Entry point are context dependent. It can be :
//! MakeManifoldSolidBrep
//! MakeBrepWithVoids
//! MakeFacetedBrep
//! MakeFacetedBrepAndBrepWithVoids
//! MakeShellBasedSurfaceModel
//! Each of these classes call the Builder
//! The class tool centralizes some common informations.
class TopoDSToStep 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static Handle(TCollection_HAsciiString) DecodeBuilderError (const TopoDSToStep_BuilderError E);
  
  Standard_EXPORT static Handle(TCollection_HAsciiString) DecodeFaceError (const TopoDSToStep_MakeFaceError E);
  
  Standard_EXPORT static Handle(TCollection_HAsciiString) DecodeWireError (const TopoDSToStep_MakeWireError E);
  
  Standard_EXPORT static Handle(TCollection_HAsciiString) DecodeEdgeError (const TopoDSToStep_MakeEdgeError E);
  
  //! Returns a new shape without undirect surfaces.
  Standard_EXPORT static Handle(TCollection_HAsciiString) DecodeVertexError (const TopoDSToStep_MakeVertexError E);
  
  //! Adds an entity into the list of results (binders) for
  //! shape stored in FinderProcess
  Standard_EXPORT static void AddResult (const Handle(Transfer_FinderProcess)& FP, const TopoDS_Shape& Shape, const Handle(Standard_Transient)& entity);
  
  //! Adds all entities recorded in Tool into the map of results
  //! (binders) stored in FinderProcess
  Standard_EXPORT static void AddResult (const Handle(Transfer_FinderProcess)& FP, const TopoDSToStep_Tool& Tool);




protected:





private:




friend class TopoDSToStep_Root;
friend class TopoDSToStep_MakeManifoldSolidBrep;
friend class TopoDSToStep_MakeBrepWithVoids;
friend class TopoDSToStep_MakeFacetedBrep;
friend class TopoDSToStep_MakeFacetedBrepAndBrepWithVoids;
friend class TopoDSToStep_MakeShellBasedSurfaceModel;
friend class TopoDSToStep_MakeGeometricCurveSet;
friend class TopoDSToStep_Builder;
friend class TopoDSToStep_WireframeBuilder;
friend class TopoDSToStep_Tool;
friend class TopoDSToStep_FacetedTool;
friend class TopoDSToStep_MakeStepFace;
friend class TopoDSToStep_MakeStepWire;
friend class TopoDSToStep_MakeStepEdge;
friend class TopoDSToStep_MakeStepVertex;

};







#endif // _TopoDSToStep_HeaderFile
