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
#include <Interface_Static.hxx>
#include <Message_ProgressScope.hxx>
#include <StdFail_NotDone.hxx>
#include <StepData_Factors.hxx>
#include <StepData_StepModel.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_HArray1OfFace.hxx>
#include <StepShape_ManifoldSolidBrep.hxx>
#include <StepShape_OpenShell.hxx>
#include <StepVisual_TessellatedShell.hxx>
#include <StepVisual_TessellatedSolid.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDSToStep.hxx>
#include <TopoDSToStep_Builder.hxx>
#include <TopoDSToStep_MakeManifoldSolidBrep.hxx>
#include <TopoDSToStep_Tool.hxx>
#include <Transfer_FinderProcess.hxx>
#include <TransferBRep_ShapeMapper.hxx>

static std::pair<Handle(StepShape_ManifoldSolidBrep), Handle(StepVisual_TessellatedItem)>
MakeManifoldSolidBrep(const TopoDS_Shell& aShell,
                      const Handle(Transfer_FinderProcess)& FP,
                      const StepData_Factors& theLocalFactors,
                      const Message_ProgressRange& theProgress)
{
  Handle(StepShape_ManifoldSolidBrep) theManifoldSolidBrep;
  Handle(StepVisual_TessellatedItem) aTessItem;

  MoniTool_DataMapOfShapeTransient aMap;
  Handle(StepData_StepModel) aStepModel = Handle(StepData_StepModel)::DownCast(FP->Model());
  TopoDSToStep_Tool aTool(aMap, Standard_False, aStepModel->InternalParameters.WriteSurfaceCurMode);
  const Standard_Integer aWriteTessGeom = aStepModel->InternalParameters.WriteTessellated;

  TopoDSToStep_Builder StepB(aShell, aTool, FP, aWriteTessGeom, theLocalFactors, theProgress);
  if (theProgress.UserBreak())
    return std::make_pair(theManifoldSolidBrep, aTessItem);

  TopoDSToStep::AddResult(FP, aTool);

  if (StepB.IsDone()) {
    aTessItem = StepB.TessellatedValue();
    Handle(StepShape_ClosedShell) aCShell = Handle(StepShape_ClosedShell)::DownCast(StepB.Value());
    // si OPEN on le force a CLOSED mais que c est une honte !
    if (aCShell.IsNull()) {
      Handle(StepShape_OpenShell) aOShell = Handle(StepShape_OpenShell)::DownCast(StepB.Value());
      if (aOShell.IsNull()) return std::make_pair(theManifoldSolidBrep, aTessItem);
      else {
        aCShell = new StepShape_ClosedShell;
        aCShell->Init(aOShell->Name(), aOShell->CfsFaces());
      }
    }
    theManifoldSolidBrep = new StepShape_ManifoldSolidBrep();
    Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString("");
    theManifoldSolidBrep->Init(aName, aCShell);
  }

  return std::make_pair(theManifoldSolidBrep, aTessItem);
}

//=============================================================================
// Create a ManifoldSolidBrep of StepShape from a Shell of TopoDS
//=============================================================================

TopoDSToStep_MakeManifoldSolidBrep::
  TopoDSToStep_MakeManifoldSolidBrep(const TopoDS_Shell& aShell,
                                     const Handle(Transfer_FinderProcess)& FP,
                                     const StepData_Factors& theLocalFactors,
                                     const Message_ProgressRange& theProgress)
{
  std::pair<Handle(StepShape_ManifoldSolidBrep), Handle(StepVisual_TessellatedItem)>
    aResult = MakeManifoldSolidBrep(aShell, FP, theLocalFactors, theProgress);
  done = !aResult.first.IsNull() || !aResult.second.IsNull();
  if (done) 
  {
    theManifoldSolidBrep = aResult.first;
    theTessellatedItem = aResult.second;
  }
  if (!done && !theProgress.UserBreak()) 
  {
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
                                     const Handle(Transfer_FinderProcess)& FP,
                                     const StepData_Factors& theLocalFactors,
                                     const Message_ProgressRange& theProgress)
{
  TopoDS_Shell aOuterShell = BRepClass3d::OuterShell(aSolid);
  if (!aOuterShell.IsNull())
  {
    std::pair<Handle(StepShape_ManifoldSolidBrep), Handle(StepVisual_TessellatedItem)>
      aResult = MakeManifoldSolidBrep(aOuterShell, FP, theLocalFactors, theProgress);
    done = !aResult.first.IsNull() || !aResult.second.IsNull();
    if (done) 
    {
      theManifoldSolidBrep = aResult.first;
      if (!aResult.second.IsNull()) {
        Handle(StepVisual_TessellatedSolid) aTessSolid = new StepVisual_TessellatedSolid();
        Handle(StepVisual_TessellatedShell) aTessShell = Handle(StepVisual_TessellatedShell)::DownCast(aResult.second);
        Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString("");
        Handle(StepVisual_HArray1OfTessellatedStructuredItem) anItems
          = new StepVisual_HArray1OfTessellatedStructuredItem(1, aTessShell->NbItems());
        for (Standard_Integer i = 1; i <= aTessShell->NbItems(); ++i) 
        {
          anItems->SetValue(i, aTessShell->ItemsValue(i));
        }
        Standard_Boolean aHasGeomLink = !theManifoldSolidBrep.IsNull();
        aTessSolid->Init(aName, anItems, aHasGeomLink, theManifoldSolidBrep);
        theTessellatedItem = aTessSolid;
      }
    }
    if (!done && !theProgress.UserBreak()) 
    {
      Handle(TransferBRep_ShapeMapper) errShape = new TransferBRep_ShapeMapper(aOuterShell);
      FP->AddWarning(errShape, " Outer Shell of Solid not mapped to ManifoldSolidBrep");
    }
  }
  else
  {
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
  StdFail_NotDone_Raise_if (!done, "TopoDSToStep_MakeManifoldSolidBrep::Value() - no result");
  return theManifoldSolidBrep;
}

// ============================================================================
// Method  : TopoDSToStep_MakeManifoldSolidBrep::TessellatedValue
// Purpose : Returns TessellatedItem as the optional result
// ============================================================================

const Handle(StepVisual_TessellatedItem)& 
TopoDSToStep_MakeManifoldSolidBrep::TessellatedValue() const
{
  StdFail_NotDone_Raise_if(!done, "TopoDSToStep_MakeManifoldSolidBrep::TessellatedValue() - no result");
  return theTessellatedItem;
}
