// Created on: 1996-09-04
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

#ifndef _Transfer_ActorOfTransientProcess_HeaderFile
#define _Transfer_ActorOfTransientProcess_HeaderFile

#include <ShapeProcess.hxx>
#include <Standard.hxx>
#include <Transfer_ActorOfProcessForTransient.hxx>
#include <XSAlgo_ShapeProcessor.hxx>

struct DE_ShapeFixParameters;
class Transfer_Binder;
class Standard_Transient;
class Transfer_ProcessForTransient;
class Transfer_TransientProcess;

class Transfer_ActorOfTransientProcess;
DEFINE_STANDARD_HANDLE(Transfer_ActorOfTransientProcess, Transfer_ActorOfProcessForTransient)

//! The original class was renamed. Compatibility only
class Transfer_ActorOfTransientProcess : public Transfer_ActorOfProcessForTransient
{
public:
  Standard_EXPORT Transfer_ActorOfTransientProcess();

  Standard_EXPORT virtual Handle(Transfer_Binder) Transferring(
    const Handle(Standard_Transient)&           start,
    const Handle(Transfer_ProcessForTransient)& TP,
    const Message_ProgressRange& theProgress = Message_ProgressRange()) Standard_OVERRIDE;

  Standard_EXPORT virtual Handle(Transfer_Binder) Transfer(
    const Handle(Standard_Transient)&        start,
    const Handle(Transfer_TransientProcess)& TP,
    const Message_ProgressRange&             theProgress = Message_ProgressRange());

  Standard_EXPORT virtual Handle(Standard_Transient) TransferTransient(
    const Handle(Standard_Transient)&        start,
    const Handle(Transfer_TransientProcess)& TP,
    const Message_ProgressRange&             theProgress = Message_ProgressRange());

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
  Standard_EXPORT void SetProcessingFlags(const ShapeProcess::OperationsFlags& theFlags);

  //! Returns flags defining operations to be performed on shapes.
  //! @return Pair: the flags defining operations to be performed on shapes and a boolean value that
  //! indicates
  //!         whether the flags were set.
  inline const XSAlgo_ShapeProcessor::ProcessingFlags& GetProcessingFlags() const
  {
    return myShapeProcFlags;
  }

  DEFINE_STANDARD_RTTIEXT(Transfer_ActorOfTransientProcess, Transfer_ActorOfProcessForTransient)

private:
  // clang-format off
  XSAlgo_ShapeProcessor::ParameterMap myShapeProcParams; //!< Parameters for shape processing.
  XSAlgo_ShapeProcessor::ProcessingFlags myShapeProcFlags; //!< Flags defining operations to be performed on shapes.
  // clang-format on
};

#endif // _Transfer_ActorOfTransientProcess_HeaderFile
