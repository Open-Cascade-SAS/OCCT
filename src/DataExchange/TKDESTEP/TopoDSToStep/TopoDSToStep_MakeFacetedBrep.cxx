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
#include <StepShape_FacetedBrep.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <StepVisual_TessellatedShell.hxx>
#include <StepVisual_TessellatedSolid.hxx>
#include <TCollection_HAsciiString.hxx>
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
TopoDSToStep_MakeFacetedBrep::TopoDSToStep_MakeFacetedBrep(
  const TopoDS_Shell&                        aShell,
  const occ::handle<Transfer_FinderProcess>& FP,
  const StepData_Factors&                    theLocalFactors,
  const Message_ProgressRange&               theProgress)
{
  done = false;
  if (aShell.Closed())
  {
    occ::handle<StepShape_TopologicalRepresentationItem> aItem;
    NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher>
                                    aMap;
    occ::handle<StepData_StepModel> aStepModel = occ::down_cast<StepData_StepModel>(FP->Model());
    const int aWriteTessGeom                   = aStepModel->InternalParameters.WriteTessellated;

    TopoDSToStep_Tool    aTool(aMap, true, aStepModel->InternalParameters.WriteSurfaceCurMode);
    TopoDSToStep_Builder StepB(aShell, aTool, FP, aWriteTessGeom, theLocalFactors, theProgress);
    if (theProgress.UserBreak())
      return;
    TopoDSToStep::AddResult(FP, aTool);

    if (StepB.IsDone())
    {
      aItem = StepB.Value();
      occ::handle<StepShape_ClosedShell> aCShell;
      aCShell                                     = occ::down_cast<StepShape_ClosedShell>(aItem);
      theFacetedBrep                              = new StepShape_FacetedBrep();
      occ::handle<TCollection_HAsciiString> aName = new TCollection_HAsciiString("");
      theFacetedBrep->Init(aName, aCShell);
      theTessellatedItem = StepB.TessellatedValue();
      done               = true;
    }
    else
    {
      done                                           = false;
      occ::handle<TransferBRep_ShapeMapper> errShape = new TransferBRep_ShapeMapper(aShell);
      FP->AddWarning(errShape, " Closed Shell not mapped to FacetedBrep");
    }
  }
  else
  {
    done                                           = false;
    occ::handle<TransferBRep_ShapeMapper> errShape = new TransferBRep_ShapeMapper(aShell);
    FP->AddWarning(errShape, " Shell not closed; not mapped to FacetedBrep");
  }
}

//=============================================================================
// Create a FacetedBrep of StepShape from a Solid of TopoDS containing
// only one closed shell
//=============================================================================

TopoDSToStep_MakeFacetedBrep::TopoDSToStep_MakeFacetedBrep(
  const TopoDS_Solid&                        aSolid,
  const occ::handle<Transfer_FinderProcess>& FP,
  const StepData_Factors&                    theLocalFactors,
  const Message_ProgressRange&               theProgress)
{
  done = false;

  // Looking for the Outer Shell
  TopoDS_Shell aOuterShell = BRepClass3d::OuterShell(aSolid);

  if (!aOuterShell.IsNull())
  {
    if (aOuterShell.Closed())
    {
      occ::handle<StepShape_TopologicalRepresentationItem> aItem;
      NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher>
                                      aMap;
      occ::handle<StepData_StepModel> aStepModel = occ::down_cast<StepData_StepModel>(FP->Model());
      const int aWriteTessGeom                   = aStepModel->InternalParameters.WriteTessellated;

      TopoDSToStep_Tool    aTool(aMap, true, aStepModel->InternalParameters.WriteSurfaceCurMode);
      TopoDSToStep_Builder StepB(aOuterShell,
                                 aTool,
                                 FP,
                                 aWriteTessGeom,
                                 theLocalFactors,
                                 theProgress);
      if (theProgress.UserBreak())
        return;
      TopoDSToStep::AddResult(FP, aTool);

      if (StepB.IsDone())
      {
        aItem = StepB.Value();
        occ::handle<StepShape_ClosedShell> aCShell;
        aCShell                                     = occ::down_cast<StepShape_ClosedShell>(aItem);
        theFacetedBrep                              = new StepShape_FacetedBrep();
        occ::handle<TCollection_HAsciiString> aName = new TCollection_HAsciiString("");
        theFacetedBrep->Init(aName, aCShell);
        done = true;
        if (!StepB.TessellatedValue())
        {
          occ::handle<StepVisual_TessellatedSolid> aTessSolid = new StepVisual_TessellatedSolid();
          occ::handle<StepVisual_TessellatedShell> aTessShell =
            occ::down_cast<StepVisual_TessellatedShell>(StepB.TessellatedValue());
          occ::handle<TCollection_HAsciiString> aTessName = new TCollection_HAsciiString("");
          occ::handle<NCollection_HArray1<occ::handle<StepVisual_TessellatedStructuredItem>>>
            anItems = new NCollection_HArray1<occ::handle<StepVisual_TessellatedStructuredItem>>(
              1,
              aTessShell->NbItems());
          for (int i = 1; i <= aTessShell->NbItems(); ++i)
          {
            anItems->SetValue(i, aTessShell->ItemsValue(i));
          }
          bool aHasGeomLink = !theFacetedBrep.IsNull();
          aTessSolid->Init(aTessName, anItems, aHasGeomLink, theFacetedBrep);
          theTessellatedItem = aTessSolid;
        }
      }
      else
      {
        done                                           = false;
        occ::handle<TransferBRep_ShapeMapper> errShape = new TransferBRep_ShapeMapper(aOuterShell);
        FP->AddWarning(errShape, " Closed Outer Shell from Solid not mapped to FacetedBrep");
      }
    }
    else
    {
      done                                           = false;
      occ::handle<TransferBRep_ShapeMapper> errShape = new TransferBRep_ShapeMapper(aOuterShell);
      FP->AddWarning(errShape, " Shell not closed; not mapped to FacetedBrep");
    }
  }
  else
  {
    done                                           = false;
    occ::handle<TransferBRep_ShapeMapper> errShape = new TransferBRep_ShapeMapper(aOuterShell);
    FP->AddWarning(errShape, " Solid contains no Outer Shell to be mapped to FacetedBrep");
  }
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const occ::handle<StepShape_FacetedBrep>& TopoDSToStep_MakeFacetedBrep::Value() const
{
  StdFail_NotDone_Raise_if(!done, "TopoDSToStep_MakeFacetedBrep::Value() - no result");
  return theFacetedBrep;
}

// ============================================================================
// Method  : TopoDSToStep_MakeFacetedBrep::TessellatedValue
// Purpose : Returns TessellatedItem as the optional result
// ============================================================================

const occ::handle<StepVisual_TessellatedItem>& TopoDSToStep_MakeFacetedBrep::TessellatedValue()
  const
{
  StdFail_NotDone_Raise_if(!done, "TopoDSToStep_MakeFacetedBrep::TessellatedValue() - no result");
  return theTessellatedItem;
}
