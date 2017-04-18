// Created on: 1993-07-23
// Created by: Martine LANGLOIS
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


#include <BRepClass3d.hxx>
#include <StdFail_NotDone.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_FacetedBrep.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDSToStep.hxx>
#include <TopoDSToStep_Builder.hxx>
#include <TopoDSToStep_MakeFacetedBrep.hxx>
#include <TopoDSToStep_Tool.hxx>
#include <Transfer_FinderProcess.hxx>
#include <TransferBRep_ShapeMapper.hxx>

//=============================================================================
// Create a FacetedBrep of StepShape from a Shell of TopoDS
//=============================================================================
TopoDSToStep_MakeFacetedBrep::
  TopoDSToStep_MakeFacetedBrep(const TopoDS_Shell& aShell,
				    const Handle(Transfer_FinderProcess)& FP)
{
  done = Standard_False;
  if (aShell.Closed()) {
    Handle(StepShape_TopologicalRepresentationItem) aItem;
    MoniTool_DataMapOfShapeTransient aMap;
    
    TopoDSToStep_Tool    aTool(aMap, Standard_True);
    TopoDSToStep_Builder StepB(aShell, aTool, FP);
    TopoDSToStep::AddResult ( FP, aTool );

    if (StepB.IsDone()) {
      aItem = StepB.Value();
      Handle(StepShape_ClosedShell) aCShell;
      aCShell = Handle(StepShape_ClosedShell)::DownCast(aItem);
      theFacetedBrep = new StepShape_FacetedBrep();
      Handle(TCollection_HAsciiString) aName = 
	new TCollection_HAsciiString("");
      theFacetedBrep->Init(aName, aCShell);
      done = Standard_True;
    }
    else {
      done = Standard_False;
      Handle(TransferBRep_ShapeMapper) errShape =
	new TransferBRep_ShapeMapper(aShell);
      FP->AddWarning(errShape," Closed Shell not mapped to FacetedBrep");
    }
  }
  else {
    done = Standard_False;
    Handle(TransferBRep_ShapeMapper) errShape =
      new TransferBRep_ShapeMapper(aShell);
    FP->AddWarning(errShape," Shell not closed; not mapped to FacetedBrep");
  }
}

//=============================================================================
// Create a FacetedBrep of StepShape from a Solid of TopoDS containing
// only one closed shell
//=============================================================================

TopoDSToStep_MakeFacetedBrep::
  TopoDSToStep_MakeFacetedBrep(const TopoDS_Solid& aSolid,
				    const Handle(Transfer_FinderProcess)& FP)
{
  done = Standard_False;

  // Looking for the Outer Shell
  TopoDS_Shell aOuterShell = BRepClass3d::OuterShell(aSolid);

  if (!aOuterShell.IsNull()) {
    if (aOuterShell.Closed()) {
      Handle(StepShape_TopologicalRepresentationItem) aItem;
      MoniTool_DataMapOfShapeTransient aMap;
      
      TopoDSToStep_Tool    aTool(aMap, Standard_True);
      TopoDSToStep_Builder StepB(aOuterShell, aTool, FP);
      TopoDSToStep::AddResult ( FP, aTool );
      
      if (StepB.IsDone()) {
	aItem = StepB.Value();
	Handle(StepShape_ClosedShell) aCShell;
	aCShell = Handle(StepShape_ClosedShell)::DownCast(aItem);
	theFacetedBrep = new StepShape_FacetedBrep();
	Handle(TCollection_HAsciiString) aName = 
	  new TCollection_HAsciiString("");
	theFacetedBrep->Init(aName, aCShell);
	done = Standard_True;
      }
      else {
	done = Standard_False;
	Handle(TransferBRep_ShapeMapper) errShape =
	  new TransferBRep_ShapeMapper(aOuterShell);
	FP->AddWarning(errShape," Closed Outer Shell from Solid not mapped to FacetedBrep");
      }
    }
    else {
      done = Standard_False; 
      Handle(TransferBRep_ShapeMapper) errShape =
	new TransferBRep_ShapeMapper(aOuterShell);
      FP->AddWarning(errShape," Shell not closed; not mapped to FacetedBrep");
    }
  }
  else {
    done = Standard_False; 
    Handle(TransferBRep_ShapeMapper) errShape =
      new TransferBRep_ShapeMapper(aOuterShell);
    FP->AddWarning(errShape," Solid contains no Outer Shell to be mapped to FacetedBrep");
  }
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepShape_FacetedBrep) &
      TopoDSToStep_MakeFacetedBrep::Value() const
{
  StdFail_NotDone_Raise_if (!done, "TopoDSToStep_MakeFacetedBrep::Value() - no result");
  return theFacetedBrep;
}
