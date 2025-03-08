// Created on: 1996-07-08
// Created by: Christian CAILLET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _STEPControl_Writer_HeaderFile
#define _STEPControl_Writer_HeaderFile

#include <ShapeProcess.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Real.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <DESTEP_Parameters.hxx>
#include <STEPControl_StepModelType.hxx>
#include <Standard_Integer.hxx>
#include <Message_ProgressRange.hxx>
#include <XSAlgo_ShapeProcessor.hxx>

#include <unordered_map>

struct DE_ShapeFixParameters;
class XSControl_WorkSession;
class StepData_StepModel;
class TopoDS_Shape;
class Transfer_ActorOfFinderProcess;

//! This class creates and writes
//! STEP files from Open CASCADE models. A STEP file can be
//! written to an existing STEP file or to a new one.
//! Translation can be performed in one or several operations. Each
//! translation operation outputs a distinct root entity in the STEP file.
class STEPControl_Writer
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a Writer from scratch
  Standard_EXPORT STEPControl_Writer();

  //! Creates a Writer from an already existing Session
  //! If <scratch> is True (D), clears already recorded data
  Standard_EXPORT STEPControl_Writer(const Handle(XSControl_WorkSession)& WS,
                                     const Standard_Boolean               scratch = Standard_True);

  //! Sets a length-measure value that
  //! will be written to uncertainty-measure-with-unit
  //! when the next shape is translated.
  Standard_EXPORT void SetTolerance(const Standard_Real Tol);

  //! Unsets the tolerance formerly forced by SetTolerance
  Standard_EXPORT void UnsetTolerance();

  //! Sets a specific session to <me>
  Standard_EXPORT void SetWS(const Handle(XSControl_WorkSession)& WS,
                             const Standard_Boolean               scratch = Standard_True);

  //! Returns the session used in <me>
  Standard_EXPORT Handle(XSControl_WorkSession) WS() const;

  //! Returns the produced model. Produces a new one if not yet done
  //! or if <newone> is True
  //! This method allows for instance to edit product or header
  //! data before writing.
  Standard_EXPORT Handle(StepData_StepModel) Model(const Standard_Boolean newone = Standard_False);

  //! Translates shape sh to a STEP
  //! entity. mode defines the STEP entity type to be output:
  //! - STEPControlStd_AsIs translates a shape to its highest possible
  //! STEP representation.
  //! - STEPControlStd_ManifoldSolidBrep translates a shape to a STEP
  //! manifold_solid_brep or brep_with_voids entity.
  //! - STEPControlStd_FacetedBrep translates a shape into a STEP
  //! faceted_brep entity.
  //! - STEPControlStd_ShellBasedSurfaceModel translates a shape into a STEP
  //! shell_based_surface_model entity.
  //! - STEPControlStd_GeometricCurveSet translates a shape into a STEP
  //! geometric_curve_set entity.
  Standard_EXPORT IFSelect_ReturnStatus
    Transfer(const TopoDS_Shape&             sh,
             const STEPControl_StepModelType mode,
             const Standard_Boolean          compgraph   = Standard_True,
             const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Translates shape sh to a STEP entity
  Standard_EXPORT IFSelect_ReturnStatus
    Transfer(const TopoDS_Shape&             sh,
             const STEPControl_StepModelType mode,
             const DESTEP_Parameters&        theParams,
             const Standard_Boolean          compgraph   = Standard_True,
             const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Writes a STEP model in the file identified by filename.
  Standard_EXPORT IFSelect_ReturnStatus Write(const Standard_CString theFileName);

  //! Writes a STEP model in the std::ostream.
  Standard_EXPORT IFSelect_ReturnStatus WriteStream(std::ostream& theOStream);

  //! Displays the statistics for the
  //! last translation. what defines the kind of statistics that are displayed:
  //! - 0 gives general statistics   (number of translated roots,
  //! number of warnings, number of   fail messages),
  //! - 1 gives root results,
  //! - 2 gives statistics for all checked entities,
  //! - 3 gives the list of translated entities,
  //! - 4 gives warning and fail messages,
  //! - 5 gives fail messages only.
  //! mode is used according to the use of what. If what is 0, mode is
  //! ignored. If what is 1, 2 or 3, mode defines the following:
  //! - 0 lists the numbers of STEP entities in a STEP model,
  //! - 1 gives the number, identifier, type and result type for each
  //! STEP entity and/or its status (fail, warning, etc.),
  //! - 2 gives maximum information for each STEP entity (i.e. checks),
  //! - 3 gives the number of entities by the type of a STEP entity,
  //! - 4 gives the number of of STEP entities per result type and/or status,
  //! - 5 gives the number of pairs (STEP or result type and status),
  //! - 6 gives the number of pairs (STEP or result type and status)
  //! AND the list of entity numbers in the STEP model.
  Standard_EXPORT void PrintStatsTransfer(const Standard_Integer what,
                                          const Standard_Integer mode = 0) const;

  //! Sets parameters for shape processing.
  //! @param theParameters the parameters for shape processing.
  Standard_EXPORT void SetShapeFixParameters(
    const XSAlgo_ShapeProcessor::ParameterMap& theParameters);

  //! Sets parameters for shape processing.
  //! Parameters are moved from the input map.
  //! @param theParameters the parameters for shape processing.
  Standard_EXPORT void SetShapeFixParameters(XSAlgo_ShapeProcessor::ParameterMap&& theParameters);

  //! Sets parameters for shape processing.
  //! Parameters from @p theParameters are copied to the internal map.
  //! Parameters from @p theAdditionalParameters are copied to the internal map
  //! if they are not present in @p theParameters.
  //! @param theParameters the parameters for shape processing.
  //! @param theAdditionalParameters the additional parameters for shape processing.
  Standard_EXPORT void SetShapeFixParameters(
    const DE_ShapeFixParameters&               theParameters,
    const XSAlgo_ShapeProcessor::ParameterMap& theAdditionalParameters = {});

  //! Returns parameters for shape processing that was set by SetParameters() method.
  //! @return the parameters for shape processing. Empty map if no parameters were set.
  Standard_EXPORT const XSAlgo_ShapeProcessor::ParameterMap& GetShapeFixParameters() const;

  //! Sets flags defining operations to be performed on shapes.
  //! @param theFlags The flags defining operations to be performed on shapes.
  Standard_EXPORT void SetShapeProcessFlags(const ShapeProcess::OperationsFlags& theFlags);

  //! Returns flags defining operations to be performed on shapes.
  //! @return Pair of values defining operations to be performed on shapes and a boolean value
  //!         that indicates whether the flags were set.
  Standard_EXPORT const XSAlgo_ShapeProcessor::ProcessingFlags& GetShapeProcessFlags() const;

private:
  //! Returns the Actor for the Transfer of an Entity.
  //! This Actor is used by the Writer to perform the Transfer.
  //! @return the Actor for the Transfer of an Entity. May be nullptr.
  Handle(Transfer_ActorOfFinderProcess) GetActor() const;

  //! If parameters haven't yet been provided, initializes them with default values
  //! provided by GetDefaultShapeFixParameters() method.
  void InitializeMissingParameters();

private:
  Handle(XSControl_WorkSession) thesession;
};

#endif // _STEPControl_Writer_HeaderFile
