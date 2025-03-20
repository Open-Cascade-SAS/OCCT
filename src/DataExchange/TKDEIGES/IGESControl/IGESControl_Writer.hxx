// Created on: 1996-01-30
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

#ifndef _IGESControl_Writer_HeaderFile
#define _IGESControl_Writer_HeaderFile

#include <ShapeProcess.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <IGESData_BasicEditor.hxx>
#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>
#include <Standard_OStream.hxx>
#include <Message_ProgressRange.hxx>
#include <XSAlgo_ShapeProcessor.hxx>

struct DE_ShapeFixParameters;
class Transfer_FinderProcess;
class IGESData_IGESModel;
class TopoDS_Shape;
class Standard_Transient;
class IGESData_IGESEntity;

//! This class creates and writes
//! IGES files from CAS.CADE models. An IGES file can be written to
//! an existing IGES file or to a new one.
//! The translation can be performed in one or several
//! operations. Each translation operation
//! outputs a distinct root entity in the IGES file.
//! To write an IGES file it is possible to use the following sequence:
//! To modify the IGES file header or to change translation
//! parameters it is necessary to use class Interface_Static (see
//! IGESParameters and GeneralParameters).
class IGESControl_Writer
{
public:
  DEFINE_STANDARD_ALLOC

public:
  //! Creates a writer object with the
  //! default unit (millimeters) and write mode (Face).
  //! IGESControl_Writer (const Standard_CString unit,
  //! const Standard_Integer modecr = 0);
  Standard_EXPORT IGESControl_Writer();

  //! Creates a writer with given
  //! values for units and for write mode.
  //! theUnit may be any unit that is accepted by the IGES standard.
  //! By default, it is the millimeter.
  //! theModecr defines the write mode and may be:
  //! - 0: Faces (default)
  //! - 1: BRep.
  Standard_EXPORT IGESControl_Writer(const Standard_CString theUnit,
                                     const Standard_Integer theModecr = 0);

  //! Creates a writer object with the
  //! prepared IGES model theModel in write mode.
  //! theModecr defines the write mode and may be:
  //! - 0: Faces (default)
  //! - 1: BRep.
  Standard_EXPORT IGESControl_Writer(const Handle(IGESData_IGESModel)& theModel,
                                     const Standard_Integer            theModecr = 0);

  //! Returns the IGES model to be written in output.
  const Handle(IGESData_IGESModel)& Model() const { return myModel; }

  const Handle(Transfer_FinderProcess)& TransferProcess() const { return myTP; }

  //! Returns/Sets the TransferProcess : it contains final results
  //! and if some, check messages
  void SetTransferProcess(const Handle(Transfer_FinderProcess)& TP) { myTP = TP; }

  //! Translates a Shape to IGES Entities and adds them to the model
  //! Returns True if done, False if Shape not suitable for IGES or null
  Standard_EXPORT Standard_Boolean
    AddShape(const TopoDS_Shape&          sh,
             const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Translates a Geometry (Surface or Curve) to IGES Entities and
  //! adds them to the model
  //! Returns True if done, False if geom is neither a Surface or
  //! a Curve suitable for IGES or is null
  Standard_EXPORT Standard_Boolean AddGeom(const Handle(Standard_Transient)& geom);

  //! Adds an IGES entity (and the ones it references) to the model
  Standard_EXPORT Standard_Boolean AddEntity(const Handle(IGESData_IGESEntity)& ent);

  //! Computes the entities found in
  //! the model, which is ready to be written.
  //! This contrasts with the default computation of headers only.
  Standard_EXPORT void ComputeModel();

  //! Computes then writes the model to an OStream
  //! Returns True when done, false in case of error
  Standard_EXPORT Standard_Boolean Write(Standard_OStream&      S,
                                         const Standard_Boolean fnes = Standard_False);

  //! Prepares and writes an IGES model
  //! either to an OStream, S or to a file name,CString.
  //! Returns True if the operation was performed correctly and
  //! False if an error occurred (for instance,
  //! if the processor could not create the file).
  Standard_EXPORT Standard_Boolean Write(const Standard_CString file,
                                         const Standard_Boolean fnes = Standard_False);

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
  inline const XSAlgo_ShapeProcessor::ParameterMap& GetShapeFixParameters() const
  {
    return myShapeProcParams;
  }

  //! Sets flags defining operations to be performed on shapes.
  //! @param theFlags The flags defining operations to be performed on shapes.
  Standard_EXPORT void SetShapeProcessFlags(const ShapeProcess::OperationsFlags& theFlags);

  //! Returns flags defining operations to be performed on shapes.
  //! @return The flags defining operations to be performed on shapes.
  inline const ShapeProcess::OperationsFlags& GetShapeProcessFlags() const
  {
    return myShapeProcFlags.first;
  }

private:
  //! If parameters haven't yet been provided, initializes them with default values
  //! provided by GetDefaultShapeFixParameters() method.
  void InitializeMissingParameters();

private:
  Handle(Transfer_FinderProcess) myTP;
  Handle(IGESData_IGESModel)     myModel;
  IGESData_BasicEditor           myEditor;
  Standard_Integer               myWriteMode;
  Standard_Boolean               myIsComputed;
  // clang-format off
  XSAlgo_ShapeProcessor::ParameterMap myShapeProcParams;   //!< Parameters for shape processing.
  XSAlgo_ShapeProcessor::ProcessingFlags myShapeProcFlags; //!< Flags defining operations to be performed on shapes.
  // clang-format on
};

#endif // _IGESControl_Writer_HeaderFile
