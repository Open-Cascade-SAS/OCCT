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


#include <Interface_Static.hxx>
#include <MoniTool_DataMapOfShapeTransient.hxx>
#include <Message_ProgressScope.hxx>
#include <StdFail_NotDone.hxx>
#include <StepData_Factors.hxx>
#include <StepData_StepModel.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_ConnectedFaceSet.hxx>
#include <StepShape_FaceSurface.hxx>
#include <StepShape_HArray1OfFace.hxx>
#include <StepShape_HArray1OfShell.hxx>
#include <StepShape_OpenShell.hxx>
#include <StepShape_Shell.hxx>
#include <StepShape_ShellBasedSurfaceModel.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <StepVisual_TessellatedGeometricSet.hxx>
#include <StepVisual_TessellatedShell.hxx>
#include <StepVisual_TessellatedSolid.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_SequenceOfTransient.hxx>
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
TopoDSToStep_MakeShellBasedSurfaceModel::
  TopoDSToStep_MakeShellBasedSurfaceModel(const TopoDS_Face& aFace,
                                          const Handle(Transfer_FinderProcess)& FP,
                                          const StepData_Factors& theLocalFactors,
                                          const Message_ProgressRange& theProgress)
{
  done = Standard_False;
  MoniTool_DataMapOfShapeTransient aMap;

  Handle(StepData_StepModel) aStepModel = Handle(StepData_StepModel)::DownCast(FP->Model());
  Standard_Integer aWriteTessGeom = aStepModel->InternalParameters.WriteTessellated;
  const Standard_Integer aWriteTessSchema = aStepModel->InternalParameters.WriteSchema;
  if (aWriteTessSchema != 5)
  {
    aWriteTessGeom = 0;
    Handle(TransferBRep_ShapeMapper) anErrShape =
      new TransferBRep_ShapeMapper(aFace);
    FP->AddWarning(anErrShape, " Tessellation can not be exported into not AP242");
  }

  TopoDSToStep_Tool    aTool(aMap, Standard_False, aStepModel->InternalParameters.WriteSurfaceCurMode);
  TopoDSToStep_Builder StepB(aFace, aTool, FP, aWriteTessGeom, theLocalFactors, theProgress);
  if (theProgress.UserBreak())
    return;

  TopoDSToStep::AddResult ( FP, aTool );

  if (StepB.IsDone())
  {
    Handle(StepShape_FaceSurface) aFS =
      Handle(StepShape_FaceSurface)::DownCast(StepB.Value());
    if (!aFS.IsNull()) 
    {
      StepShape_Shell aShellSelect;
      Handle(StepShape_OpenShell) aOpenShell
        = new StepShape_OpenShell();
      Handle(StepShape_HArray1OfFace) aCfsFaces =
        new StepShape_HArray1OfFace(1, 1);
      aCfsFaces->SetValue(1, aFS);
      Handle(TCollection_HAsciiString) aName =
        new TCollection_HAsciiString("");
      aOpenShell->Init(aName, aCfsFaces);
      aShellSelect.SetValue(aOpenShell);
      Handle(StepShape_HArray1OfShell) aSbsmFaces =
        new StepShape_HArray1OfShell(1, 1);
      aSbsmFaces->SetValue(1, aShellSelect);
      theShellBasedSurfaceModel = new StepShape_ShellBasedSurfaceModel();
      theShellBasedSurfaceModel->Init(aName, aSbsmFaces);
    }
    theTessellatedItem = StepB.TessellatedValue();
    done = Standard_True;
  }
  else 
  {
    done = Standard_False;
    Handle(TransferBRep_ShapeMapper) errShape =
      new TransferBRep_ShapeMapper(aFace);
    FP->AddWarning(errShape, " Single Face not mapped to ShellBasedSurfaceModel");
  }
}

//=============================================================================
// Create a ShellBasedSurfaceModel of StepShape from a Shell of TopoDS
//=============================================================================
TopoDSToStep_MakeShellBasedSurfaceModel::
  TopoDSToStep_MakeShellBasedSurfaceModel(const TopoDS_Shell& aShell,
                                          const Handle(Transfer_FinderProcess)& FP,
                                          const StepData_Factors& theLocalFactors,
                                          const Message_ProgressRange& theProgress)
{
  done = Standard_False;
  StepShape_Shell                                 aShellSelect;
  Handle(StepShape_HArray1OfShell)                aSbsmBoundary;
  Handle(StepShape_OpenShell)                     aOpenShell;
  Handle(StepShape_ClosedShell)                   aClosedShell;
  MoniTool_DataMapOfShapeTransient                aMap;

  Handle(StepData_StepModel) aStepModel = Handle(StepData_StepModel)::DownCast(FP->Model());
  Standard_Integer aWriteTessGeom = aStepModel->InternalParameters.WriteTessellated;
  const Standard_Integer aWriteTessSchema = aStepModel->InternalParameters.WriteSchema;
  if (aWriteTessSchema != 5)
  {
    aWriteTessGeom = 0;
    Handle(TransferBRep_ShapeMapper) anErrShape =
      new TransferBRep_ShapeMapper(aShell);
    FP->AddWarning(anErrShape, " Tessellation can not be exported into not AP242");
  }

  TopoDSToStep_Tool    aTool(aMap, Standard_False, aStepModel->InternalParameters.WriteSurfaceCurMode);
  TopoDSToStep_Builder StepB(aShell, aTool, FP, aWriteTessGeom, theLocalFactors, theProgress);
  if (theProgress.UserBreak())
    return;
  //TopoDSToStep::AddResult ( FP, aTool );

  if (StepB.IsDone()) {
    if (!StepB.Value().IsNull()) {
      aSbsmBoundary = new StepShape_HArray1OfShell(1, 1);
      if (aShell.Closed()) {
        aClosedShell = Handle(StepShape_ClosedShell)::DownCast(StepB.Value());
        aShellSelect.SetValue(aClosedShell);
      }
      else {
        aOpenShell = Handle(StepShape_OpenShell)::DownCast(StepB.Value());
        aShellSelect.SetValue(aOpenShell);
      }
      aSbsmBoundary->SetValue(1, aShellSelect);
      theShellBasedSurfaceModel = new StepShape_ShellBasedSurfaceModel();
      Handle(TCollection_HAsciiString) aName =
        new TCollection_HAsciiString("");
      theShellBasedSurfaceModel->Init(aName, aSbsmBoundary);
      TopoDSToStep::AddResult(FP, aShell, theShellBasedSurfaceModel);
    }
    theTessellatedItem = StepB.TessellatedValue();
    done = Standard_True;
  }
  else {
    done = Standard_False;
    Handle(TransferBRep_ShapeMapper) errShape =
      new TransferBRep_ShapeMapper(aShell);
    FP->AddWarning(errShape, " Shell not mapped to ShellBasedSurfaceModel");
  }

  TopoDSToStep::AddResult ( FP, aTool );
}

//=============================================================================
// Create a ShellBasedSurfaceModel of StepShape from a Solid of TopoDS
//=============================================================================

TopoDSToStep_MakeShellBasedSurfaceModel::
  TopoDSToStep_MakeShellBasedSurfaceModel(const TopoDS_Solid& aSolid,
                                          const Handle(Transfer_FinderProcess)& FP,
                                          const StepData_Factors& theLocalFactors,
                                          const Message_ProgressRange& theProgress)
{
  done = Standard_False;
  StepShape_Shell                  aShellSelect;
  Handle(StepShape_HArray1OfShell) aSbsmBoundary;
  Handle(StepShape_OpenShell)      aOpenShell;
  Handle(StepShape_ClosedShell)    aClosedShell;
  TopoDS_Iterator                  It;
  TopoDS_Shell                     aShell;
  MoniTool_DataMapOfShapeTransient aMap;
  TColStd_SequenceOfTransient      S;
  TColStd_SequenceOfTransient      aTessShells;

  Handle(StepData_StepModel) aStepModel = Handle(StepData_StepModel)::DownCast(FP->Model());
  Standard_Integer aWriteTessGeom = aStepModel->InternalParameters.WriteTessellated;
  const Standard_Integer aWriteTessSchema = aStepModel->InternalParameters.WriteSchema;
  if (aWriteTessSchema != 5)
  {
    aWriteTessGeom = 0;
    Handle(TransferBRep_ShapeMapper) anErrShape =
      new TransferBRep_ShapeMapper(aShell);
    FP->AddWarning(anErrShape, " Tessellation can not be exported into not AP242");
  }

  Standard_Integer nbshapes = 0;
  for (It.Initialize(aSolid); It.More(); It.Next())
    if (It.Value().ShapeType() == TopAbs_SHELL)
      nbshapes++;
  Message_ProgressScope aPS(theProgress, NULL, nbshapes);
  for (It.Initialize(aSolid); It.More() && aPS.More(); It.Next())
  {
    if (It.Value().ShapeType() == TopAbs_SHELL) {
      aShell = TopoDS::Shell(It.Value());

      TopoDSToStep_Tool    aTool(aMap, Standard_False, aStepModel->InternalParameters.WriteSurfaceCurMode);
      TopoDSToStep_Builder StepB(aShell, aTool, FP, aWriteTessGeom, theLocalFactors, aPS.Next());
      TopoDSToStep::AddResult ( FP, aTool );

      if (StepB.IsDone()) {
        if (!StepB.Value().IsNull()) {
          S.Append(StepB.Value());
        }
        Handle(StepVisual_TessellatedItem) aTessShell = StepB.TessellatedValue();
        if (!aTessShell.IsNull()) 
        {
          aTessShells.Append(aTessShell);
        }
      }
      else {
        Handle(TransferBRep_ShapeMapper) errShape =
          new TransferBRep_ShapeMapper(aShell);
        FP->AddWarning(errShape, " Shell from Solid not mapped to ShellBasedSurfaceModel");
      }
    }
  }
  if (!aPS.More())
    return;
  Standard_Integer N = S.Length();
  if ( N >= 1) {
    aSbsmBoundary = new StepShape_HArray1OfShell(1,N);
    for (Standard_Integer i=1; i<=N; i++) {
      aOpenShell = Handle(StepShape_OpenShell)::DownCast(S.Value(i));
      if (!aOpenShell.IsNull()) {
        aShellSelect.SetValue(aOpenShell);
      }
      else {
        aClosedShell = Handle(StepShape_ClosedShell)::DownCast(S.Value(i));
        aShellSelect.SetValue(aClosedShell);
      }
      aSbsmBoundary->SetValue(i,aShellSelect);
    }
    
    theShellBasedSurfaceModel = new StepShape_ShellBasedSurfaceModel();
    Handle(TCollection_HAsciiString) aName = 
      new TCollection_HAsciiString("");
    theShellBasedSurfaceModel->Init(aName,aSbsmBoundary);

    if (!aTessShells.IsEmpty()) 
    {
      Handle(StepVisual_TessellatedGeometricSet) aTessGS = new StepVisual_TessellatedGeometricSet();
      Handle(TCollection_HAsciiString) aTessName = new TCollection_HAsciiString("");
      NCollection_Handle<StepVisual_Array1OfTessellatedItem> anItems
        = new StepVisual_Array1OfTessellatedItem(1, aTessShells.Length());
      Standard_Integer i = 1;
      for (TColStd_SequenceOfTransient::Iterator anIt(aTessShells); anIt.More(); anIt.Next(), ++i) 
      {
        Handle(StepVisual_TessellatedShell) aTessShell = Handle(StepVisual_TessellatedShell)::DownCast(anIt.Value());
        anItems->SetValue(i, aTessShell);
      }
      aTessGS->Init(aTessName, anItems);
    }

    done = Standard_True;
  }
  else {
    done = Standard_False;
    Handle(TransferBRep_ShapeMapper) errShape =
      new TransferBRep_ShapeMapper(aSolid);
    FP->AddWarning(errShape," Solid contains no Shell to be mapped to ShellBasedSurfaceModel");
  }
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepShape_ShellBasedSurfaceModel) &
      TopoDSToStep_MakeShellBasedSurfaceModel::Value() const
{
  StdFail_NotDone_Raise_if (!done, "TopoDSToStep_MakeShellBasedSurfaceModel::Value() - no result");
  return theShellBasedSurfaceModel;
}

// ============================================================================
// Method  : TopoDSToStep_MakeShellBasedSurfaceModel::TessellatedValue
// Purpose : Returns TessellatedItem as the optional result
// ============================================================================

const Handle(StepVisual_TessellatedItem)&
TopoDSToStep_MakeShellBasedSurfaceModel::TessellatedValue() const
{
  StdFail_NotDone_Raise_if(!done, "TopoDSToStep_MakeShellBasedSurfaceModel::TessellatedValue() - no result");
  return theTessellatedItem;
}
