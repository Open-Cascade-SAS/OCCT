// Created on: 1993-07-23
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <TopoDSToStep_MakeFacetedBrep.ixx>
#include <StdFail_NotDone.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <TopoDSToStep.hxx>
#include <TopoDSToStep_Builder.hxx>
#include <TopoDSToStep_Tool.hxx>
#include <TopoDS_Iterator.hxx>
#include <BRepTools.hxx>
#include <Transfer_FinderProcess.hxx>
#include <TransferBRep_ShapeMapper.hxx>
#include <TCollection_HAsciiString.hxx>

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
  TopoDS_Shell aOuterShell = BRepTools::OuterShell(aSolid);

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
  StdFail_NotDone_Raise_if(!done,"");
  return theFacetedBrep;
}
