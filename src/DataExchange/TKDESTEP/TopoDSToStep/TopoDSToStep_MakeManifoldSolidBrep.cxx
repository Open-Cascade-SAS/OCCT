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
#include <Message_ProgressScope.hxx>
#include <StdFail_NotDone.hxx>
#include <StepData_Factors.hxx>
#include <StepData_StepModel.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_Face.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
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

static std::pair<occ::handle<StepShape_ManifoldSolidBrep>, occ::handle<StepVisual_TessellatedItem>>
  MakeManifoldSolidBrep(const TopoDS_Shell&                   aShell,
                        const occ::handle<Transfer_FinderProcess>& FP,
                        const StepData_Factors&               theLocalFactors,
                        const Message_ProgressRange&          theProgress)
{
  occ::handle<StepShape_ManifoldSolidBrep> theManifoldSolidBrep;
  occ::handle<StepVisual_TessellatedItem>  aTessItem;

  NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher> aMap;
  occ::handle<StepData_StepModel>       aStepModel = occ::down_cast<StepData_StepModel>(FP->Model());
  TopoDSToStep_Tool aTool(aMap, false, aStepModel->InternalParameters.WriteSurfaceCurMode);
  const int aWriteTessGeom = aStepModel->InternalParameters.WriteTessellated;

  TopoDSToStep_Builder StepB(aShell, aTool, FP, aWriteTessGeom, theLocalFactors, theProgress);
  if (theProgress.UserBreak())
    return std::make_pair(theManifoldSolidBrep, aTessItem);

  TopoDSToStep::AddResult(FP, aTool);

  if (StepB.IsDone())
  {
    aTessItem                             = StepB.TessellatedValue();
    occ::handle<StepShape_ClosedShell> aCShell = occ::down_cast<StepShape_ClosedShell>(StepB.Value());
    // si OPEN on le force a CLOSED mais que c est une honte !
    if (aCShell.IsNull())
    {
      occ::handle<StepShape_OpenShell> aOShell = occ::down_cast<StepShape_OpenShell>(StepB.Value());
      if (aOShell.IsNull())
        return std::make_pair(theManifoldSolidBrep, aTessItem);
      else
      {
        aCShell = new StepShape_ClosedShell;
        aCShell->Init(aOShell->Name(), aOShell->CfsFaces());
      }
    }
    theManifoldSolidBrep                   = new StepShape_ManifoldSolidBrep();
    occ::handle<TCollection_HAsciiString> aName = new TCollection_HAsciiString("");
    theManifoldSolidBrep->Init(aName, aCShell);
  }

  return std::make_pair(theManifoldSolidBrep, aTessItem);
}

//=============================================================================
// Create a ManifoldSolidBrep of StepShape from a Shell of TopoDS
//=============================================================================

TopoDSToStep_MakeManifoldSolidBrep::TopoDSToStep_MakeManifoldSolidBrep(
  const TopoDS_Shell&                   aShell,
  const occ::handle<Transfer_FinderProcess>& FP,
  const StepData_Factors&               theLocalFactors,
  const Message_ProgressRange&          theProgress)
{
  std::pair<occ::handle<StepShape_ManifoldSolidBrep>, occ::handle<StepVisual_TessellatedItem>> aResult =
    MakeManifoldSolidBrep(aShell, FP, theLocalFactors, theProgress);
  done = !aResult.first.IsNull() || !aResult.second.IsNull();
  if (done)
  {
    theManifoldSolidBrep = aResult.first;
    theTessellatedItem   = aResult.second;
  }
  if (!done && !theProgress.UserBreak())
  {
    occ::handle<TransferBRep_ShapeMapper> errShape = new TransferBRep_ShapeMapper(aShell);
    FP->AddWarning(errShape, " Closed Shell not mapped to ManifoldSolidBrep");
  }
}

//=============================================================================
// Create a ManifoldSolidBrep of StepShape from a Solid of TopoDS containing
// only one closed shell
//=============================================================================

TopoDSToStep_MakeManifoldSolidBrep::TopoDSToStep_MakeManifoldSolidBrep(
  const TopoDS_Solid&                   aSolid,
  const occ::handle<Transfer_FinderProcess>& FP,
  const StepData_Factors&               theLocalFactors,
  const Message_ProgressRange&          theProgress)
{
  TopoDS_Shell aOuterShell = BRepClass3d::OuterShell(aSolid);
  if (!aOuterShell.IsNull())
  {
    std::pair<occ::handle<StepShape_ManifoldSolidBrep>, occ::handle<StepVisual_TessellatedItem>> aResult =
      MakeManifoldSolidBrep(aOuterShell, FP, theLocalFactors, theProgress);
    done = !aResult.first.IsNull() || !aResult.second.IsNull();
    if (done)
    {
      theManifoldSolidBrep = aResult.first;
      if (!aResult.second.IsNull())
      {
        occ::handle<StepVisual_TessellatedSolid> aTessSolid = new StepVisual_TessellatedSolid();
        occ::handle<StepVisual_TessellatedShell> aTessShell =
          occ::down_cast<StepVisual_TessellatedShell>(aResult.second);
        occ::handle<TCollection_HAsciiString> aName = new TCollection_HAsciiString("");
        occ::handle<NCollection_HArray1<occ::handle<StepVisual_TessellatedStructuredItem>>> anItems =
          new NCollection_HArray1<occ::handle<StepVisual_TessellatedStructuredItem>>(1, aTessShell->NbItems());
        for (int i = 1; i <= aTessShell->NbItems(); ++i)
        {
          anItems->SetValue(i, aTessShell->ItemsValue(i));
        }
        bool aHasGeomLink = !theManifoldSolidBrep.IsNull();
        aTessSolid->Init(aName, anItems, aHasGeomLink, theManifoldSolidBrep);
        theTessellatedItem = aTessSolid;
      }
    }
    if (!done && !theProgress.UserBreak())
    {
      occ::handle<TransferBRep_ShapeMapper> errShape = new TransferBRep_ShapeMapper(aOuterShell);
      FP->AddWarning(errShape, " Outer Shell of Solid not mapped to ManifoldSolidBrep");
    }
  }
  else
  {
    occ::handle<TransferBRep_ShapeMapper> errShape = new TransferBRep_ShapeMapper(aOuterShell);
    FP->AddWarning(errShape, " Outer Shell is null; not mapped to ManifoldSolidBrep ");
    done = false;
  }
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const occ::handle<StepShape_ManifoldSolidBrep>& TopoDSToStep_MakeManifoldSolidBrep::Value() const
{
  StdFail_NotDone_Raise_if(!done, "TopoDSToStep_MakeManifoldSolidBrep::Value() - no result");
  return theManifoldSolidBrep;
}

// ============================================================================
// Method  : TopoDSToStep_MakeManifoldSolidBrep::TessellatedValue
// Purpose : Returns TessellatedItem as the optional result
// ============================================================================

const occ::handle<StepVisual_TessellatedItem>& TopoDSToStep_MakeManifoldSolidBrep::TessellatedValue()
  const
{
  StdFail_NotDone_Raise_if(!done,
                           "TopoDSToStep_MakeManifoldSolidBrep::TessellatedValue() - no result");
  return theTessellatedItem;
}
