// Created on: 1994-06-24
// Created by: Frederic MAUPAS
// Copyright (c) 1994-1999 Matra Datavision
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

#include <TopoDS_Shape.hxx>
#include <Standard_Transient.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <Message_ProgressScope.hxx>
#include <StdFail_NotDone.hxx>
#include <StepData_Factors.hxx>
#include <StepData_StepModel.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_ConnectedFaceSet.hxx>
#include <StepShape_FaceSurface.hxx>
#include <StepShape_Face.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepShape_Shell.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepShape_OpenShell.hxx>
#include <StepShape_Shell.hxx>
#include <StepShape_ShellBasedSurfaceModel.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <StepVisual_TessellatedGeometricSet.hxx>
#include <StepVisual_TessellatedShell.hxx>
#include <StepVisual_TessellatedSolid.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDSToStep.hxx>
#include <TopoDSToStep_Builder.hxx>
#include <TopoDSToStep_MakeShellBasedSurfaceModel.hxx>
#include <TopoDSToStep_Tool.hxx>
#include <Transfer_FinderProcess.hxx>
#include <TransferBRep_ShapeMapper.hxx>

//=============================================================================
// Create a ShellBasedSurfaceModel of StepShape from a Face of TopoDS
//=============================================================================
TopoDSToStep_MakeShellBasedSurfaceModel::TopoDSToStep_MakeShellBasedSurfaceModel(
  const TopoDS_Face&                    aFace,
  const occ::handle<Transfer_FinderProcess>& FP,
  const StepData_Factors&               theLocalFactors,
  const Message_ProgressRange&          theProgress)
{
  done = false;
  NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher> aMap;

  occ::handle<StepData_StepModel> aStepModel       = occ::down_cast<StepData_StepModel>(FP->Model());
  int           aWriteTessGeom   = aStepModel->InternalParameters.WriteTessellated;
  const int     aWriteTessSchema = aStepModel->InternalParameters.WriteSchema;
  if (aWriteTessSchema != 5)
  {
    aWriteTessGeom                              = 0;
    occ::handle<TransferBRep_ShapeMapper> anErrShape = new TransferBRep_ShapeMapper(aFace);
    FP->AddWarning(anErrShape, " Tessellation can not be exported into not AP242");
  }

  TopoDSToStep_Tool aTool(aMap, false, aStepModel->InternalParameters.WriteSurfaceCurMode);
  TopoDSToStep_Builder StepB(aFace, aTool, FP, aWriteTessGeom, theLocalFactors, theProgress);
  if (theProgress.UserBreak())
    return;

  TopoDSToStep::AddResult(FP, aTool);

  if (StepB.IsDone())
  {
    occ::handle<StepShape_FaceSurface> aFS = occ::down_cast<StepShape_FaceSurface>(StepB.Value());
    if (!aFS.IsNull())
    {
      StepShape_Shell                 aShellSelect;
      occ::handle<StepShape_OpenShell>     aOpenShell = new StepShape_OpenShell();
      occ::handle<NCollection_HArray1<occ::handle<StepShape_Face>>> aCfsFaces  = new NCollection_HArray1<occ::handle<StepShape_Face>>(1, 1);
      aCfsFaces->SetValue(1, aFS);
      occ::handle<TCollection_HAsciiString> aName = new TCollection_HAsciiString("");
      aOpenShell->Init(aName, aCfsFaces);
      aShellSelect.SetValue(aOpenShell);
      occ::handle<NCollection_HArray1<StepShape_Shell>> aSbsmFaces = new NCollection_HArray1<StepShape_Shell>(1, 1);
      aSbsmFaces->SetValue(1, aShellSelect);
      theShellBasedSurfaceModel = new StepShape_ShellBasedSurfaceModel();
      theShellBasedSurfaceModel->Init(aName, aSbsmFaces);
    }
    theTessellatedItem = StepB.TessellatedValue();
    done               = true;
  }
  else
  {
    done                                      = false;
    occ::handle<TransferBRep_ShapeMapper> errShape = new TransferBRep_ShapeMapper(aFace);
    FP->AddWarning(errShape, " Single Face not mapped to ShellBasedSurfaceModel");
  }
}

//=============================================================================
// Create a ShellBasedSurfaceModel of StepShape from a Shell of TopoDS
//=============================================================================
TopoDSToStep_MakeShellBasedSurfaceModel::TopoDSToStep_MakeShellBasedSurfaceModel(
  const TopoDS_Shell&                   aShell,
  const occ::handle<Transfer_FinderProcess>& FP,
  const StepData_Factors&               theLocalFactors,
  const Message_ProgressRange&          theProgress)
{
  done = false;
  StepShape_Shell                  aShellSelect;
  occ::handle<NCollection_HArray1<StepShape_Shell>> aSbsmBoundary;
  occ::handle<StepShape_OpenShell>      aOpenShell;
  occ::handle<StepShape_ClosedShell>    aClosedShell;
  NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher> aMap;

  occ::handle<StepData_StepModel> aStepModel       = occ::down_cast<StepData_StepModel>(FP->Model());
  int           aWriteTessGeom   = aStepModel->InternalParameters.WriteTessellated;
  const int     aWriteTessSchema = aStepModel->InternalParameters.WriteSchema;
  if (aWriteTessSchema != 5)
  {
    aWriteTessGeom                              = 0;
    occ::handle<TransferBRep_ShapeMapper> anErrShape = new TransferBRep_ShapeMapper(aShell);
    FP->AddWarning(anErrShape, " Tessellation can not be exported into not AP242");
  }

  TopoDSToStep_Tool aTool(aMap, false, aStepModel->InternalParameters.WriteSurfaceCurMode);
  TopoDSToStep_Builder StepB(aShell, aTool, FP, aWriteTessGeom, theLocalFactors, theProgress);
  if (theProgress.UserBreak())
    return;
  // TopoDSToStep::AddResult ( FP, aTool );

  if (StepB.IsDone())
  {
    if (!StepB.Value().IsNull())
    {
      aSbsmBoundary = new NCollection_HArray1<StepShape_Shell>(1, 1);
      if (aShell.Closed())
      {
        aClosedShell = occ::down_cast<StepShape_ClosedShell>(StepB.Value());
        aShellSelect.SetValue(aClosedShell);
      }
      else
      {
        aOpenShell = occ::down_cast<StepShape_OpenShell>(StepB.Value());
        aShellSelect.SetValue(aOpenShell);
      }
      aSbsmBoundary->SetValue(1, aShellSelect);
      theShellBasedSurfaceModel              = new StepShape_ShellBasedSurfaceModel();
      occ::handle<TCollection_HAsciiString> aName = new TCollection_HAsciiString("");
      theShellBasedSurfaceModel->Init(aName, aSbsmBoundary);
      TopoDSToStep::AddResult(FP, aShell, theShellBasedSurfaceModel);
    }
    theTessellatedItem = StepB.TessellatedValue();
    done               = true;
  }
  else
  {
    done                                      = false;
    occ::handle<TransferBRep_ShapeMapper> errShape = new TransferBRep_ShapeMapper(aShell);
    FP->AddWarning(errShape, " Shell not mapped to ShellBasedSurfaceModel");
  }

  TopoDSToStep::AddResult(FP, aTool);
}

//=============================================================================
// Create a ShellBasedSurfaceModel of StepShape from a Solid of TopoDS
//=============================================================================

TopoDSToStep_MakeShellBasedSurfaceModel::TopoDSToStep_MakeShellBasedSurfaceModel(
  const TopoDS_Solid&                   aSolid,
  const occ::handle<Transfer_FinderProcess>& FP,
  const StepData_Factors&               theLocalFactors,
  const Message_ProgressRange&          theProgress)
{
  done = false;
  StepShape_Shell                  aShellSelect;
  occ::handle<NCollection_HArray1<StepShape_Shell>> aSbsmBoundary;
  occ::handle<StepShape_OpenShell>      aOpenShell;
  occ::handle<StepShape_ClosedShell>    aClosedShell;
  TopoDS_Iterator                  It;
  TopoDS_Shell                     aShell;
  NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher> aMap;
  NCollection_Sequence<occ::handle<Standard_Transient>>      S;
  NCollection_Sequence<occ::handle<Standard_Transient>>      aTessShells;

  occ::handle<StepData_StepModel> aStepModel       = occ::down_cast<StepData_StepModel>(FP->Model());
  int           aWriteTessGeom   = aStepModel->InternalParameters.WriteTessellated;
  const int     aWriteTessSchema = aStepModel->InternalParameters.WriteSchema;
  if (aWriteTessSchema != 5)
  {
    aWriteTessGeom                              = 0;
    occ::handle<TransferBRep_ShapeMapper> anErrShape = new TransferBRep_ShapeMapper(aShell);
    FP->AddWarning(anErrShape, " Tessellation can not be exported into not AP242");
  }

  int nbshapes = 0;
  for (It.Initialize(aSolid); It.More(); It.Next())
    if (It.Value().ShapeType() == TopAbs_SHELL)
      nbshapes++;
  Message_ProgressScope aPS(theProgress, NULL, nbshapes);
  for (It.Initialize(aSolid); It.More() && aPS.More(); It.Next())
  {
    if (It.Value().ShapeType() == TopAbs_SHELL)
    {
      aShell = TopoDS::Shell(It.Value());

      TopoDSToStep_Tool    aTool(aMap,
                              false,
                              aStepModel->InternalParameters.WriteSurfaceCurMode);
      TopoDSToStep_Builder StepB(aShell, aTool, FP, aWriteTessGeom, theLocalFactors, aPS.Next());
      TopoDSToStep::AddResult(FP, aTool);

      if (StepB.IsDone())
      {
        if (!StepB.Value().IsNull())
        {
          S.Append(StepB.Value());
        }
        occ::handle<StepVisual_TessellatedItem> aTessShell = StepB.TessellatedValue();
        if (!aTessShell.IsNull())
        {
          aTessShells.Append(aTessShell);
        }
      }
      else
      {
        occ::handle<TransferBRep_ShapeMapper> errShape = new TransferBRep_ShapeMapper(aShell);
        FP->AddWarning(errShape, " Shell from Solid not mapped to ShellBasedSurfaceModel");
      }
    }
  }
  if (!aPS.More())
    return;
  int N = S.Length();
  if (N >= 1)
  {
    aSbsmBoundary = new NCollection_HArray1<StepShape_Shell>(1, N);
    for (int i = 1; i <= N; i++)
    {
      aOpenShell = occ::down_cast<StepShape_OpenShell>(S.Value(i));
      if (!aOpenShell.IsNull())
      {
        aShellSelect.SetValue(aOpenShell);
      }
      else
      {
        aClosedShell = occ::down_cast<StepShape_ClosedShell>(S.Value(i));
        aShellSelect.SetValue(aClosedShell);
      }
      aSbsmBoundary->SetValue(i, aShellSelect);
    }

    theShellBasedSurfaceModel              = new StepShape_ShellBasedSurfaceModel();
    occ::handle<TCollection_HAsciiString> aName = new TCollection_HAsciiString("");
    theShellBasedSurfaceModel->Init(aName, aSbsmBoundary);

    if (!aTessShells.IsEmpty())
    {
      occ::handle<StepVisual_TessellatedGeometricSet> aTessGS = new StepVisual_TessellatedGeometricSet();
      occ::handle<TCollection_HAsciiString>           aTessName = new TCollection_HAsciiString("");
      NCollection_Handle<NCollection_Array1<occ::handle<StepVisual_TessellatedItem>>> anItems =
        new NCollection_Array1<occ::handle<StepVisual_TessellatedItem>>(1, aTessShells.Length());
      int i = 1;
      for (NCollection_Sequence<occ::handle<Standard_Transient>>::Iterator anIt(aTessShells); anIt.More(); anIt.Next(), ++i)
      {
        occ::handle<StepVisual_TessellatedShell> aTessShell =
          occ::down_cast<StepVisual_TessellatedShell>(anIt.Value());
        anItems->SetValue(i, aTessShell);
      }
      aTessGS->Init(aTessName, anItems);
    }

    done = true;
  }
  else
  {
    done                                      = false;
    occ::handle<TransferBRep_ShapeMapper> errShape = new TransferBRep_ShapeMapper(aSolid);
    FP->AddWarning(errShape, " Solid contains no Shell to be mapped to ShellBasedSurfaceModel");
  }
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const occ::handle<StepShape_ShellBasedSurfaceModel>& TopoDSToStep_MakeShellBasedSurfaceModel::Value()
  const
{
  StdFail_NotDone_Raise_if(!done, "TopoDSToStep_MakeShellBasedSurfaceModel::Value() - no result");
  return theShellBasedSurfaceModel;
}

// ============================================================================
// Method  : TopoDSToStep_MakeShellBasedSurfaceModel::TessellatedValue
// Purpose : Returns TessellatedItem as the optional result
// ============================================================================

const occ::handle<StepVisual_TessellatedItem>& TopoDSToStep_MakeShellBasedSurfaceModel::
  TessellatedValue() const
{
  StdFail_NotDone_Raise_if(
    !done,
    "TopoDSToStep_MakeShellBasedSurfaceModel::TessellatedValue() - no result");
  return theTessellatedItem;
}
