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

#ifndef _Transfer_ActorOfFinderProcess_HeaderFile
#define _Transfer_ActorOfFinderProcess_HeaderFile

#include <Standard.hxx>
#include <ShapeProcess.hxx>
#include <Transfer_ActorOfProcessForFinder.hxx>

#include <unordered_map>

struct DE_ShapeFixParameters;
class Transfer_Binder;
class Transfer_Finder;
class Transfer_ProcessForFinder;
class Transfer_FinderProcess;
class Standard_Transient;

class Transfer_ActorOfFinderProcess;
DEFINE_STANDARD_HANDLE(Transfer_ActorOfFinderProcess, Transfer_ActorOfProcessForFinder)

//! The original class was renamed. Compatibility only
//!
//! ModeTrans : a simple way of transmitting a transfer mode from
//! a user. To be interpreted for each norm
class Transfer_ActorOfFinderProcess : public Transfer_ActorOfProcessForFinder
{
public:
  using ParameterMap = std::unordered_map<std::string, std::string>;
  // Flags defining operations to be performed on shapes. Since there is no std::optional in C++11,
  // we use a pair. The first element is the flags, the second element is a boolean value that
  // indicates whether the flags were set.
  using ProcessingFlags = std::pair<ShapeProcess::OperationsFlags, bool>;

public:
  Standard_EXPORT Transfer_ActorOfFinderProcess();

  //! Returns the Transfer Mode, modifiable
  Standard_EXPORT Standard_Integer& ModeTrans();

  Standard_EXPORT virtual Handle(Transfer_Binder) Transferring(
    const Handle(Transfer_Finder)&           start,
    const Handle(Transfer_ProcessForFinder)& TP,
    const Message_ProgressRange& theProgress = Message_ProgressRange()) Standard_OVERRIDE;

  Standard_EXPORT virtual Handle(Transfer_Binder) Transfer(
    const Handle(Transfer_Finder)&        start,
    const Handle(Transfer_FinderProcess)& TP,
    const Message_ProgressRange&          theProgress = Message_ProgressRange());

  Standard_EXPORT virtual Handle(Standard_Transient) TransferTransient(
    const Handle(Standard_Transient)&     start,
    const Handle(Transfer_FinderProcess)& TP,
    const Message_ProgressRange&          theProgress = Message_ProgressRange());

  //! Sets parameters for shape processing.
  //! @param theParameters the parameters for shape processing.
  Standard_EXPORT void SetShapeFixParameters(const ParameterMap& theParameters);

  //! Sets parameters for shape processing.
  //! Parameters are moved from the input map.
  //! @param theParameters the parameters for shape processing.
  Standard_EXPORT void SetShapeFixParameters(ParameterMap&& theParameters);

  //! Sets parameters for shape processing.
  //! Parameters from @p theParameters are copied to the internal map.
  //! Parameters from @p theAdditionalParameters are copied to the internal map
  //! if they are not present in @p theParameters.
  //! @param theParameters the parameters for shape processing.
  //! @param theAdditionalParameters the additional parameters for shape processing.
  Standard_EXPORT void SetShapeFixParameters(const DE_ShapeFixParameters& theParameters,
                                             const ParameterMap& theAdditionalParameters = {});

  //! Returns parameters for shape processing that was set by SetParameters() method.
  //! @return the parameters for shape processing. Empty map if no parameters were set.
  inline const ParameterMap& GetShapeFixParameters() const { return myShapeProcParams; }

  //! Sets flags defining operations to be performed on shapes.
  //! @param theFlags The flags defining operations to be performed on shapes.
  Standard_EXPORT void SetShapeProcessFlags(const ShapeProcess::OperationsFlags& theFlags);

  //! Returns flags defining operations to be performed on shapes.
  //! @return Pair of values defining operations to be performed on shapes and a boolean value
  //!         that indicates whether the flags were set.
  inline const ProcessingFlags& GetShapeProcessFlags() const { return myShapeProcFlags; }

  DEFINE_STANDARD_RTTIEXT(Transfer_ActorOfFinderProcess, Transfer_ActorOfProcessForFinder)

protected:
  Standard_Integer themodetrans;

private:
  ParameterMap    myShapeProcParams; //!< Parameters for shape processing.
  ProcessingFlags myShapeProcFlags;  //!< Flags defining operations to be performed on shapes.
};

#endif // _Transfer_ActorOfFinderProcess_HeaderFile
