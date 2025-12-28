// Created on: 1993-09-14
// Created by: Frederic MAUPAS
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

#ifndef _STEPConstruct_ContextTool_HeaderFile
#define _STEPConstruct_ContextTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <NCollection_Sequence.hxx>
#include <STEPConstruct_AP203Context.hxx>
#include <StepData_Factors.hxx>
#include <StepData_StepModel.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_HSequence.hxx>
class StepBasic_ApplicationProtocolDefinition;
class StepGeom_Axis2Placement3d;
class TCollection_HAsciiString;
class STEPConstruct_Part;
class STEPConstruct_Assembly;

//! Maintains global context tool for writing.
//! Gives access to Product Definition Context (one per Model)
//! Maintains ApplicationProtocolDefinition entity (common for all
//! products)
//! Also maintains context specific for AP203 and provides set of
//! methods to work with various STEP constructs as required
//! by Actor
class STEPConstruct_ContextTool
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT STEPConstruct_ContextTool();

  Standard_EXPORT STEPConstruct_ContextTool(const occ::handle<StepData_StepModel>& aStepModel);

  //! Initialize ApplicationProtocolDefinition by the first
  //! entity of that type found in the model
  Standard_EXPORT void SetModel(const occ::handle<StepData_StepModel>& aStepModel);

  Standard_EXPORT void SetGlobalFactor(const StepData_Factors& theGlobalFactor);

  Standard_EXPORT occ::handle<StepBasic_ApplicationProtocolDefinition> GetAPD();

  Standard_EXPORT void AddAPD(const bool enforce = false);

  //! Returns True if APD.schema_name is config_control_design
  Standard_EXPORT bool IsAP203() const;

  //! Returns True if APD.schema_name is automotive_design
  Standard_EXPORT bool IsAP214() const;

  //! Returns True if APD.schema_name is ap242_managed_model_based_3d_engineering
  Standard_EXPORT bool IsAP242() const;

  Standard_EXPORT occ::handle<TCollection_HAsciiString> GetACstatus();

  Standard_EXPORT occ::handle<TCollection_HAsciiString> GetACschemaName();

  Standard_EXPORT int GetACyear();

  Standard_EXPORT occ::handle<TCollection_HAsciiString> GetACname();

  Standard_EXPORT void SetACstatus(const occ::handle<TCollection_HAsciiString>& status);

  Standard_EXPORT void SetACschemaName(const occ::handle<TCollection_HAsciiString>& schemaName);

  Standard_EXPORT void SetACyear(const int year);

  Standard_EXPORT void SetACname(const occ::handle<TCollection_HAsciiString>& name);

  //! Returns a default axis placement
  Standard_EXPORT occ::handle<StepGeom_Axis2Placement3d> GetDefaultAxis();

  //! Returns tool which maintains context specific for AP203
  Standard_EXPORT STEPConstruct_AP203Context& AP203Context();

  //! Returns current assembly level
  Standard_EXPORT int Level() const;

  Standard_EXPORT void NextLevel();

  Standard_EXPORT void PrevLevel();

  //! Changes current assembly level
  Standard_EXPORT void SetLevel(const int lev);

  //! Returns current index of assembly component on current level
  Standard_EXPORT int Index() const;

  Standard_EXPORT void NextIndex();

  Standard_EXPORT void PrevIndex();

  //! Changes current index of assembly component on current level
  Standard_EXPORT void SetIndex(const int ind);

  //! Generates a product name basing on write.step.product.name
  //! parameter and current position in the assembly structure
  Standard_EXPORT occ::handle<TCollection_HAsciiString> GetProductName() const;

  //! Produces and returns a full list of root entities required
  //! for part identified by SDRTool (including SDR itself)
  Standard_EXPORT occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>
                  GetRootsForPart(const STEPConstruct_Part& SDRTool);

  //! Produces and returns a full list of root entities required
  //! for assembly link identified by assembly (including NAUO and CDSR)
  Standard_EXPORT occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>
                  GetRootsForAssemblyLink(const STEPConstruct_Assembly& assembly);

private:
  NCollection_Sequence<int>                            myLevel;
  occ::handle<StepBasic_ApplicationProtocolDefinition> theAPD;
  STEPConstruct_AP203Context                           theAP203;
  occ::handle<StepGeom_Axis2Placement3d>               myAxis;
  StepData_Factors                                     myGlobalFactor;
  DESTEP_Parameters::WriteMode_StepSchema              mySchema;
  TCollection_AsciiString                              myProductName;
};

#endif // _STEPConstruct_ContextTool_HeaderFile
