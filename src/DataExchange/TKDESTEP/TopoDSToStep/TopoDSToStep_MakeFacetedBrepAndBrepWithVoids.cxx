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
#include <TopoDS_Shape.hxx>
#include <Standard_Transient.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <StdFail_NotDone.hxx>
#include <StepData_Factors.hxx>
#include <StepData_StepModel.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_FacetedBrepAndBrepWithVoids.hxx>
#include <StepShape_OrientedClosedShell.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <StepVisual_TessellatedStructuredItem.hxx>
#include <StepVisual_TessellatedShell.hxx>
#include <StepVisual_TessellatedSolid.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Sequence.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDSToStep.hxx>
#include <TopoDSToStep_Builder.hxx>
#include <TopoDSToStep_MakeFacetedBrepAndBrepWithVoids.hxx>
#include <TopoDSToStep_Tool.hxx>
#include <Transfer_FinderProcess.hxx>
#include <TransferBRep_ShapeMapper.hxx>

//=============================================================================
// Create a FacetedBrepAndBrepWithVoids of StepShape from a Solid of TopoDS
// containing more than one closed shell
//=============================================================================
TopoDSToStep_MakeFacetedBrepAndBrepWithVoids::TopoDSToStep_MakeFacetedBrepAndBrepWithVoids(
  const TopoDS_Solid&                        aSolid,
  const occ::handle<Transfer_FinderProcess>& FP,
  const StepData_Factors&                    theLocalFactors,
  const Message_ProgressRange&               theProgress)
{
  done = false;
  TopoDS_Iterator                                                                             It;
  NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher> aMap;
  NCollection_Sequence<occ::handle<Standard_Transient>>                                       S;
  TopoDS_Shell aOutShell;

  occ::handle<StepShape_TopologicalRepresentationItem>                         aItem;
  occ::handle<StepShape_ClosedShell>                                           aOuter, aCShell;
  occ::handle<StepShape_OrientedClosedShell>                                   aOCShell;
  occ::handle<NCollection_HArray1<occ::handle<StepShape_OrientedClosedShell>>> aVoids;
  NCollection_Sequence<occ::handle<Standard_Transient>>                        aTessShells;

  aOutShell = BRepClass3d::OuterShell(aSolid);

  occ::handle<StepData_StepModel> aStepModel = occ::down_cast<StepData_StepModel>(FP->Model());
  TopoDSToStep_Builder            StepB;
  TopoDSToStep_Tool               aTool(aStepModel);

  if (!aOutShell.IsNull())
  {
    int nbshapes = 0;
    for (It.Initialize(aSolid); It.More(); It.Next())
      if (It.Value().ShapeType() == TopAbs_SHELL)
        nbshapes++;
    Message_ProgressScope aPS(theProgress, NULL, nbshapes);
    for (It.Initialize(aSolid); It.More() && aPS.More(); It.Next())
    {
      if (It.Value().ShapeType() == TopAbs_SHELL)
      {
        Message_ProgressRange aRange       = aPS.Next();
        TopoDS_Shell          CurrentShell = TopoDS::Shell(It.Value());
        if (It.Value().Closed())
        {

          aTool.Init(aMap, false, aStepModel->InternalParameters.WriteSurfaceCurMode);
          StepB.Init(CurrentShell, aTool, FP, false, theLocalFactors, aRange);
          TopoDSToStep::AddResult(FP, aTool);

          if (StepB.IsDone())
          {
            aCShell = occ::down_cast<StepShape_ClosedShell>(StepB.Value());
            if (aOutShell.IsEqual(It.Value()))
              aOuter = aCShell;
            else
              S.Append(aCShell);
            occ::handle<StepVisual_TessellatedItem> aTessShell = StepB.TessellatedValue();
            if (!aTessShell.IsNull())
            {
              aTessShells.Append(aTessShell);
            }
          }
          else
          {
            occ::handle<TransferBRep_ShapeMapper> errShape =
              new TransferBRep_ShapeMapper(CurrentShell);
            FP->AddWarning(errShape, " Shell from Solid not mapped to FacetedBrepAndBrepWithVoids");
          }
        }
        else
        {
          done = false;
          occ::handle<TransferBRep_ShapeMapper> errShape =
            new TransferBRep_ShapeMapper(CurrentShell);
          FP->AddWarning(errShape,
                         " Shell from Solid not closed; not mapped to FacetedBrepAndBrepWithVoids");
        }
      }
    }
    if (!aPS.More())
      return;
  }
  int N = S.Length();
  if (N >= 1)
  {
    aVoids = new NCollection_HArray1<occ::handle<StepShape_OrientedClosedShell>>(1, N);
    occ::handle<TCollection_HAsciiString> aName = new TCollection_HAsciiString("");
    for (int i = 1; i <= N; i++)
    {
      aOCShell = new StepShape_OrientedClosedShell();
      aOCShell->Init(aName, occ::down_cast<StepShape_ClosedShell>(S.Value(i)), true);
      aVoids->SetValue(i, aOCShell);
    }
    theFacetedBrepAndBrepWithVoids = new StepShape_FacetedBrepAndBrepWithVoids();
    theFacetedBrepAndBrepWithVoids->Init(aName, aOuter, aVoids);

    if (!aTessShells.IsEmpty())
    {
      occ::handle<StepVisual_TessellatedSolid> aTessSolid = new StepVisual_TessellatedSolid();
      occ::handle<TCollection_HAsciiString>    aTessName  = new TCollection_HAsciiString("");
      int                                      aNbItems   = 0;
      for (NCollection_Sequence<occ::handle<Standard_Transient>>::Iterator anIt(aTessShells);
           anIt.More();
           anIt.Next())
      {
        occ::handle<StepVisual_TessellatedShell> aTessShell =
          occ::down_cast<StepVisual_TessellatedShell>(anIt.Value());
        aNbItems += aTessShell->NbItems();
      }
      occ::handle<NCollection_HArray1<occ::handle<StepVisual_TessellatedStructuredItem>>> anItems =
        new NCollection_HArray1<occ::handle<StepVisual_TessellatedStructuredItem>>(1, aNbItems);
      for (NCollection_Sequence<occ::handle<Standard_Transient>>::Iterator anIt(aTessShells);
           anIt.More();
           anIt.Next())
      {
        occ::handle<StepVisual_TessellatedShell> aTessShell =
          occ::down_cast<StepVisual_TessellatedShell>(anIt.Value());
        for (int i = 1; i <= aTessShell->NbItems(); ++i)
        {
          anItems->SetValue(i, aTessShell->ItemsValue(i));
        }
      }
      bool aHasGeomLink = !theFacetedBrepAndBrepWithVoids.IsNull();
      aTessSolid->Init(aTessName, anItems, aHasGeomLink, theFacetedBrepAndBrepWithVoids);
      theTessellatedItem = aTessSolid;
    }

    done = true;
  }
  else
  {
    done                                           = false;
    occ::handle<TransferBRep_ShapeMapper> errShape = new TransferBRep_ShapeMapper(aSolid);
    FP->AddWarning(errShape,
                   " Solid contains no Shell to be mapped to FacetedBrepAndBrepWithVoids");
  }
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const occ::handle<StepShape_FacetedBrepAndBrepWithVoids>&
  TopoDSToStep_MakeFacetedBrepAndBrepWithVoids::Value() const
{
  StdFail_NotDone_Raise_if(!done,
                           "TopoDSToStep_MakeFacetedBrepAndBrepWithVoids::Value() - no result");
  return theFacetedBrepAndBrepWithVoids;
}

// ============================================================================
// Method  : TopoDSToStep_MakeFacetedBrepAndBrepWithVoids::TessellatedValue
// Purpose : Returns TessellatedItem as the optional result
// ============================================================================

const occ::handle<StepVisual_TessellatedItem>& TopoDSToStep_MakeFacetedBrepAndBrepWithVoids::
  TessellatedValue() const
{
  StdFail_NotDone_Raise_if(
    !done,
    "TopoDSToStep_MakeFacetedBrepAndBrepWithVoids::TessellatedValue() - no result");
  return theTessellatedItem;
}
