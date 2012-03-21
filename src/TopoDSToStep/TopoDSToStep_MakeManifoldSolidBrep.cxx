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


#include <TopoDSToStep_MakeManifoldSolidBrep.ixx>
#include <StdFail_NotDone.hxx>
#include <TopoDSToStep.hxx>
#include <TopoDSToStep_Builder.hxx>
#include <TopoDSToStep_Tool.hxx>
#include <BRepTools.hxx>
#include <Transfer_FinderProcess.hxx>
#include <TransferBRep_ShapeMapper.hxx>
#include <TCollection_HAsciiString.hxx>

#include <StepShape_ClosedShell.hxx>
#include <StepShape_OpenShell.hxx>
#include <StepShape_HArray1OfFace.hxx>

static Handle(StepShape_ManifoldSolidBrep) MakeManifoldSolidBrep (const TopoDS_Shell& aShell,
								  const Handle(Transfer_FinderProcess)& FP)
{
  Handle(StepShape_ManifoldSolidBrep) theManifoldSolidBrep;

  MoniTool_DataMapOfShapeTransient aMap;
  TopoDSToStep_Tool aTool(aMap, Standard_False);
  
  TopoDSToStep_Builder StepB(aShell, aTool, FP);

  TopoDSToStep::AddResult(FP, aTool);

  if (StepB.IsDone()) {
    Handle(StepShape_ClosedShell) aCShell = Handle(StepShape_ClosedShell)::DownCast(StepB.Value());
    // si OPEN on le force a CLOSED mais que c est une honte !
    if (aCShell.IsNull()) {
      Handle(StepShape_OpenShell) aOShell = Handle(StepShape_OpenShell)::DownCast(StepB.Value());
      if (aOShell.IsNull()) return theManifoldSolidBrep;
      else {
	aCShell = new StepShape_ClosedShell;
	aCShell->Init(aOShell->Name(),aOShell->CfsFaces());
      }
    }
    theManifoldSolidBrep = new StepShape_ManifoldSolidBrep();
    Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString("");
    theManifoldSolidBrep->Init(aName, aCShell);
  }

  return theManifoldSolidBrep;
}

//=============================================================================
// Create a ManifoldSolidBrep of StepShape from a Shell of TopoDS
//=============================================================================

TopoDSToStep_MakeManifoldSolidBrep::
  TopoDSToStep_MakeManifoldSolidBrep(const TopoDS_Shell& aShell,
				     const Handle(Transfer_FinderProcess)& FP)
{
  theManifoldSolidBrep = MakeManifoldSolidBrep(aShell, FP);
  done = !theManifoldSolidBrep.IsNull();
  if (!done) {
    Handle(TransferBRep_ShapeMapper) errShape = new TransferBRep_ShapeMapper(aShell);
    FP->AddWarning(errShape, " Closed Shell not mapped to ManifoldSolidBrep");
  }
}

//=============================================================================
// Create a ManifoldSolidBrep of StepShape from a Solid of TopoDS containing
// only one closed shell
//=============================================================================

TopoDSToStep_MakeManifoldSolidBrep::
  TopoDSToStep_MakeManifoldSolidBrep(const TopoDS_Solid& aSolid,
				     const Handle(Transfer_FinderProcess)& FP)
{
  TopoDS_Shell aOuterShell = BRepTools::OuterShell(aSolid);
  if (!aOuterShell.IsNull()) {

    theManifoldSolidBrep = MakeManifoldSolidBrep(aOuterShell, FP);
    done = !theManifoldSolidBrep.IsNull();
    if (!done) {
      Handle(TransferBRep_ShapeMapper) errShape = new TransferBRep_ShapeMapper(aOuterShell);
      FP->AddWarning(errShape, " Outer Shell of Solid not mapped to ManifoldSolidBrep");
    }
  }
  else {
    Handle(TransferBRep_ShapeMapper) errShape = new TransferBRep_ShapeMapper(aOuterShell);
    FP->AddWarning(errShape, " Outer Shell is null; not mapped to ManifoldSolidBrep ");
    done = Standard_False;
  }
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepShape_ManifoldSolidBrep) &
      TopoDSToStep_MakeManifoldSolidBrep::Value() const
{
  StdFail_NotDone_Raise_if(!done,"");
  return theManifoldSolidBrep;
}
