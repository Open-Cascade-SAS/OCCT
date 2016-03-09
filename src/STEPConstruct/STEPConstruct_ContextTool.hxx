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

#include <TColStd_SequenceOfInteger.hxx>
#include <STEPConstruct_AP203Context.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
class StepBasic_ApplicationProtocolDefinition;
class StepGeom_Axis2Placement3d;
class StepData_StepModel;
class TCollection_HAsciiString;
class STEPConstruct_AP203Context;
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
  
  Standard_EXPORT STEPConstruct_ContextTool(const Handle(StepData_StepModel)& aStepModel);
  
  //! Initialize ApplicationProtocolDefinition by the first
  //! entity of that type found in the model
  Standard_EXPORT void SetModel (const Handle(StepData_StepModel)& aStepModel);
  
  Standard_EXPORT Handle(StepBasic_ApplicationProtocolDefinition) GetAPD();
  
  Standard_EXPORT void AddAPD (const Standard_Boolean enforce = Standard_False);
  
  //! Returns True if APD.schema_name is config_control_design
  Standard_EXPORT Standard_Boolean IsAP203() const;
  
  //! Returns True if APD.schema_name is automotive_design
  Standard_EXPORT Standard_Boolean IsAP214() const;

  //! Returns True if APD.schema_name is ap242_managed_model_based_3d_engineering
  Standard_EXPORT Standard_Boolean IsAP242() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) GetACstatus();
  
  Standard_EXPORT Handle(TCollection_HAsciiString) GetACschemaName();
  
  Standard_EXPORT Standard_Integer GetACyear();
  
  Standard_EXPORT Handle(TCollection_HAsciiString) GetACname();
  
  Standard_EXPORT void SetACstatus (const Handle(TCollection_HAsciiString)& status);
  
  Standard_EXPORT void SetACschemaName (const Handle(TCollection_HAsciiString)& schemaName);
  
  Standard_EXPORT void SetACyear (const Standard_Integer year);
  
  Standard_EXPORT void SetACname (const Handle(TCollection_HAsciiString)& name);
  
  //! Returns a default axis placement
  Standard_EXPORT Handle(StepGeom_Axis2Placement3d) GetDefaultAxis();
  
  //! Returns tool which maintains context specific for AP203
  Standard_EXPORT STEPConstruct_AP203Context& AP203Context();
  
  //! Returns current assembly level
  Standard_EXPORT Standard_Integer Level() const;
  
  Standard_EXPORT void NextLevel();
  
  Standard_EXPORT void PrevLevel();
  
  //! Changes current assembly level
  Standard_EXPORT void SetLevel (const Standard_Integer lev);
  
  //! Returns current index of assembly component on current level
  Standard_EXPORT Standard_Integer Index() const;
  
  Standard_EXPORT void NextIndex();
  
  Standard_EXPORT void PrevIndex();
  
  //! Changes current index of assembly component on current level
  Standard_EXPORT void SetIndex (const Standard_Integer ind);
  
  //! Generates a product name basing on write.step.product.name
  //! parameter and current position in the assembly structure
  Standard_EXPORT Handle(TCollection_HAsciiString) GetProductName() const;
  
  //! Produces and returns a full list of root entities required
  //! for part identified by SDRTool (including SDR itself)
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GetRootsForPart (const STEPConstruct_Part& SDRTool);
  
  //! Produces and returns a full list of root entities required
  //! for assembly link identified by assembly (including NAUO and CDSR)
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GetRootsForAssemblyLink (const STEPConstruct_Assembly& assembly);




protected:





private:



  TColStd_SequenceOfInteger myLevel;
  Handle(StepBasic_ApplicationProtocolDefinition) theAPD;
  STEPConstruct_AP203Context theAP203;
  Handle(StepGeom_Axis2Placement3d) myAxis;


};







#endif // _STEPConstruct_ContextTool_HeaderFile
