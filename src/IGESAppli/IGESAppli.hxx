// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen (Anand NATRAJAN)
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

#ifndef _IGESAppli_HeaderFile
#define _IGESAppli_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class IGESAppli_Protocol;


//! This package represents collection of miscellaneous
//! entities from IGES
class IGESAppli 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Prepares dynamic data (Protocol, Modules) for this package
  Standard_EXPORT static void Init();
  
  //! Returns the Protocol for this Package
  Standard_EXPORT static Handle(IGESAppli_Protocol) Protocol();




protected:





private:




friend class IGESAppli_Node;
friend class IGESAppli_FiniteElement;
friend class IGESAppli_NodalDisplAndRot;
friend class IGESAppli_NodalResults;
friend class IGESAppli_ElementResults;
friend class IGESAppli_Flow;
friend class IGESAppli_PipingFlow;
friend class IGESAppli_RegionRestriction;
friend class IGESAppli_LevelFunction;
friend class IGESAppli_LineWidening;
friend class IGESAppli_DrilledHole;
friend class IGESAppli_ReferenceDesignator;
friend class IGESAppli_PinNumber;
friend class IGESAppli_PartNumber;
friend class IGESAppli_FlowLineSpec;
friend class IGESAppli_LevelToPWBLayerMap;
friend class IGESAppli_PWBArtworkStackup;
friend class IGESAppli_PWBDrilledHole;
friend class IGESAppli_NodalConstraint;
friend class IGESAppli_ToolNode;
friend class IGESAppli_ToolFiniteElement;
friend class IGESAppli_ToolNodalDisplAndRot;
friend class IGESAppli_ToolNodalResults;
friend class IGESAppli_ToolElementResults;
friend class IGESAppli_ToolFlow;
friend class IGESAppli_ToolPipingFlow;
friend class IGESAppli_ToolRegionRestriction;
friend class IGESAppli_ToolLevelFunction;
friend class IGESAppli_ToolLineWidening;
friend class IGESAppli_ToolDrilledHole;
friend class IGESAppli_ToolReferenceDesignator;
friend class IGESAppli_ToolPinNumber;
friend class IGESAppli_ToolPartNumber;
friend class IGESAppli_ToolFlowLineSpec;
friend class IGESAppli_ToolLevelToPWBLayerMap;
friend class IGESAppli_ToolPWBArtworkStackup;
friend class IGESAppli_ToolPWBDrilledHole;
friend class IGESAppli_ToolNodalConstraint;
friend class IGESAppli_Protocol;
friend class IGESAppli_ReadWriteModule;
friend class IGESAppli_GeneralModule;
friend class IGESAppli_SpecificModule;

};







#endif // _IGESAppli_HeaderFile
