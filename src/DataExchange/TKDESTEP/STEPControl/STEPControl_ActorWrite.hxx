// Created on: 1995-01-31
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

#ifndef _STEPControl_ActorWrite_HeaderFile
#define _STEPControl_ActorWrite_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <STEPConstruct_ContextTool.hxx>
#include <Transfer_ActorOfFinderProcess.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <STEPControl_StepModelType.hxx>
class Transfer_Finder;
class Transfer_Binder;
class Transfer_FinderProcess;
class StepShape_ShapeDefinitionRepresentation;
class StepGeom_GeometricRepresentationItem;
class TopoDS_Shape;
class StepShape_NonManifoldSurfaceShapeRepresentation;

//! This class performs the transfer of a Shape from TopoDS
//! to AP203 or AP214 (CD2 or DIS)
class STEPControl_ActorWrite : public Transfer_ActorOfFinderProcess
{

public:
  Standard_EXPORT STEPControl_ActorWrite();

  Standard_EXPORT virtual bool Recognize(const occ::handle<Transfer_Finder>& start)
    override;

  Standard_EXPORT virtual occ::handle<Transfer_Binder> Transfer(
    const occ::handle<Transfer_Finder>&        start,
    const occ::handle<Transfer_FinderProcess>& FP,
    const Message_ProgressRange&          theProgress = Message_ProgressRange()) override;

  Standard_EXPORT occ::handle<Transfer_Binder> TransferSubShape(
    const occ::handle<Transfer_Finder>&                         start,
    const occ::handle<StepShape_ShapeDefinitionRepresentation>& SDR,
    occ::handle<StepGeom_GeometricRepresentationItem>&          AX1,
    const occ::handle<Transfer_FinderProcess>&                  FP,
    const StepData_Factors&                                theLocalFactors = StepData_Factors(),
    const occ::handle<NCollection_HSequence<TopoDS_Shape>>&               shapeGroup      = NULL,
    const bool                                 isManifold      = true,
    const Message_ProgressRange&                           theProgress = Message_ProgressRange());

  Standard_EXPORT occ::handle<Transfer_Binder> TransferShape(
    const occ::handle<Transfer_Finder>&                         start,
    const occ::handle<StepShape_ShapeDefinitionRepresentation>& SDR,
    const occ::handle<Transfer_FinderProcess>&                  FP,
    const StepData_Factors&                                theLocalFactors = StepData_Factors(),
    const occ::handle<NCollection_HSequence<TopoDS_Shape>>&               shapeGroup      = NULL,
    const bool                                 isManifold      = true,
    const Message_ProgressRange&                           theProgress = Message_ProgressRange());

  Standard_EXPORT occ::handle<Transfer_Binder> TransferCompound(
    const occ::handle<Transfer_Finder>&                         start,
    const occ::handle<StepShape_ShapeDefinitionRepresentation>& SDR,
    const occ::handle<Transfer_FinderProcess>&                  FP,
    const StepData_Factors&                                theLocalFactors = StepData_Factors(),
    const Message_ProgressRange&                           theProgress = Message_ProgressRange());

  Standard_EXPORT void SetMode(const STEPControl_StepModelType M);

  Standard_EXPORT STEPControl_StepModelType Mode() const;

  Standard_EXPORT void SetGroupMode(const int mode);

  Standard_EXPORT int GroupMode() const;

  Standard_EXPORT void SetTolerance(const double Tol);

  //! Customizable method to check whether shape S should
  //! be written as assembly or not
  //! Default implementation uses flag GroupMode and analyses
  //! the shape itself
  //! NOTE: this method can modify shape
  Standard_EXPORT virtual bool IsAssembly(const occ::handle<StepData_StepModel>& theModel,
                                                      TopoDS_Shape&                     S) const;

  DEFINE_STANDARD_RTTIEXT(STEPControl_ActorWrite, Transfer_ActorOfFinderProcess)

private:
  //! Non-manifold shapes are stored in NMSSR group
  //! (NON_MANIFOLD_SURFACE_SHAPE_REPRESENTATION).
  //! Use this method to get the corresponding NMSSR (or
  //! to create a new one if doesn't exist yet)
  //! (ssv; 13.11.2010)
  Standard_EXPORT occ::handle<StepShape_NonManifoldSurfaceShapeRepresentation> getNMSSRForGroup(
    const occ::handle<NCollection_HSequence<TopoDS_Shape>>& shapeGroup,
    const occ::handle<Transfer_FinderProcess>&    FP,
    bool&                        isNMSSRCreated) const;

  //! bind already written shared faces to STEP entity for non-manifold
  Standard_EXPORT void mergeInfoForNM(const occ::handle<Transfer_FinderProcess>& theFP,
                                      const occ::handle<Standard_Transient>&     theInfo) const;

  //! Gets sequence of vertices of all compounds level by recursive
  //! @param[in] theShape shape to iterate, checked for compound type and sub shapes vertex type
  //! @param[out] theVertices sequence of found vertices via recursively iterate of shape
  //! @return TRUE if one or more vertex was found and all shapes were compound or vertex
  bool separateShapeToSoloVertex(const TopoDS_Shape&       theShape,
                                             NCollection_Sequence<TopoDS_Shape>& theVertices);

private:
  int          mygroup;
  double             mytoler;
  STEPConstruct_ContextTool myContext;
};

#endif // _STEPControl_ActorWrite_HeaderFile
