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
#include <MoniTool_DataMapOfShapeTransient.hxx>
#include <StdFail_NotDone.hxx>
#include <StepData_Factors.hxx>
#include <StepData_StepModel.hxx>
#include <StepShape_BrepWithVoids.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_HArray1OfOrientedClosedShell.hxx>
#include <StepShape_OpenShell.hxx>
#include <StepShape_OrientedClosedShell.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <StepVisual_HArray1OfTessellatedStructuredItem.hxx>
#include <StepVisual_TessellatedShell.hxx>
#include <StepVisual_TessellatedSolid.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_SequenceOfTransient.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDSToStep.hxx>
#include <TopoDSToStep_Builder.hxx>
#include <TopoDSToStep_MakeBrepWithVoids.hxx>
#include <TopoDSToStep_Tool.hxx>
#include <Transfer_FinderProcess.hxx>
#include <TransferBRep_ShapeMapper.hxx>

//:d7
#ifdef OCCT_DEBUG
#define DEBUG
#endif

//=============================================================================
// Create a BrepWithVoids of StepShape from a Solid of TopoDS containing
// more than one closed shell 
//=============================================================================

TopoDSToStep_MakeBrepWithVoids::
  TopoDSToStep_MakeBrepWithVoids(const TopoDS_Solid& aSolid,
                                 const Handle(Transfer_FinderProcess)& FP,
                                 const StepData_Factors& theLocalFactors,
                                 const Message_ProgressRange& theProgress)
{
  done = Standard_False;
  TopoDS_Iterator                  It;
  MoniTool_DataMapOfShapeTransient aMap;
  TColStd_SequenceOfTransient      S;
  TopoDS_Shell                     aOutShell;

  Handle(StepShape_TopologicalRepresentationItem) aItem;
  Handle(StepShape_ClosedShell)                   aOuter, aCShell;
  Handle(StepShape_OrientedClosedShell)           aOCShell;
  Handle(StepShape_HArray1OfOrientedClosedShell)  aVoids;
  TColStd_SequenceOfTransient                     aTessShells;

  Handle(StepData_StepModel) aStepModel = Handle(StepData_StepModel)::DownCast(FP->Model());
  const Standard_Integer aWriteTessGeom = aStepModel->InternalParameters.WriteTessellated;

  try 
  {
    // BRepClass3d::OuterShell requires geometry when the solid has voids
    aOutShell = BRepClass3d::OuterShell(aSolid);
  }
  catch (...)
  {
  }

  TopoDSToStep_Builder StepB;
  TopoDSToStep_Tool    aTool(aStepModel);

  Standard_Integer nbshapes = 0;
  for (It.Initialize(aSolid); It.More(); It.Next())
    if (It.Value().ShapeType() == TopAbs_SHELL)
      nbshapes++;
  Message_ProgressScope aPS(theProgress, NULL, nbshapes);
  for (It.Initialize(aSolid); It.More() && aPS.More(); It.Next())
  {
    if (It.Value().ShapeType() == TopAbs_SHELL) 
    {
      TopoDS_Shell CurrentShell = TopoDS::Shell(It.Value());
      if (!aOutShell.IsNull() && !aOutShell.IsEqual(CurrentShell)) //:e0 abv 25 Mar 98: voids should be reversed according to EXPRESS for ABSR
        CurrentShell.Reverse();
      //:d7 abv 16 Mar 98: try to treat 'open' shells as closed since flag 
      // IsClosed() is often incorrect (taken from MakeManifoldSolid(Solid))
      aTool.Init(aMap, Standard_False, aStepModel->InternalParameters.WriteSurfaceCurMode);
      StepB.Init(CurrentShell, aTool, FP, aWriteTessGeom, theLocalFactors, aPS.Next());
      TopoDSToStep::AddResult(FP, aTool);
      if (StepB.IsDone()) 
      {
        aCShell = Handle(StepShape_ClosedShell)::DownCast(StepB.Value());
        // si OPEN on le force a CLOSED mais que c est une honte !
        if (aCShell.IsNull()) 
        {
          Handle(StepShape_OpenShell) aOShell = Handle(StepShape_OpenShell)::DownCast(StepB.Value());
          if (!aOShell.IsNull()) 
          {
            aCShell = new StepShape_ClosedShell;
            aCShell->Init(aOShell->Name(), aOShell->CfsFaces());
#ifdef OCCT_DEBUG
            std::cout << "Warning: MakeBRepWithVoids: Open shell in the solid; treated as closed" << std::endl;
#endif
          }
        }
        Handle(StepVisual_TessellatedItem) aTessShell = StepB.TessellatedValue();
        if (!aTessShell.IsNull()) 
        {
          aTessShells.Append(aTessShell);
        }
        if (!aOutShell.IsNull() && !aCShell.IsNull()) 
        {
          if (aOutShell.IsEqual(It.Value()))
            aOuter = aCShell;
          else
            S.Append(aCShell);
        }
#ifdef OCCT_DEBUG
        else std::cout << "*** MakeBRepWithVoids : Shell not mapped" << std::endl;
#endif
      }
#ifdef OCCT_DEBUG
      else std::cout << "Shell not mapped" << std::endl;
#endif
      /* //:d7
        if (It.Value().Closed()) {
          aTool.Init(aMap, Standard_False);
          StepB.Init(CurrentShell, aTool, FP);
          TopoDSToStep::AddResult ( FP, aTool );
          if (StepB.IsDone()) {
            aCShell = Handle(StepShape_ClosedShell)::DownCast(StepB.Value());
            if ( aOutShell.IsEqual(It.Value()) )
              aOuter = aCShell;
            else
              S.Append(aCShell);
          }
          else {
      #ifdef OCCT_DEBUG
            std::cout << "Shell not mapped" << std::endl;
      #endif
            done = Standard_False;
          }
        }
        else {
          // Error Handling : the Shape is not closed
          done = Standard_False;
        }
      */
    }
  }
  if (!aPS.More())
    return;

  Standard_Integer N = S.Length();
  if (N >= 1) 
  {
    Handle(TCollection_HAsciiString) aName =
      new TCollection_HAsciiString("");
    aVoids = new StepShape_HArray1OfOrientedClosedShell(1, N);
    for (Standard_Integer i = 1; i <= N; i++) {
      aOCShell = new StepShape_OrientedClosedShell;
      // Warning : the Oriented Shell Orientation is not always
      //           TRUE.
      //           Shall check the TopoDS_Shell orientation.
      // => if the Shell is reversed, shall create an OrientedShell.
      aOCShell->Init(aName, Handle(StepShape_ClosedShell)::DownCast(S.Value(i)),
        Standard_False); //:e0
 //:e0			 Standard_True);
      aVoids->SetValue(i, aOCShell);
    }

    theBrepWithVoids = new StepShape_BrepWithVoids();
    theBrepWithVoids->Init(aName, aOuter, aVoids);

    done = Standard_True;
  }

  if (!aTessShells.IsEmpty()) 
  {
    Handle(StepVisual_TessellatedSolid) aTessSolid = new StepVisual_TessellatedSolid();
    Handle(TCollection_HAsciiString) aTessName = new TCollection_HAsciiString("");
    Standard_Integer aNbItems = 0;
    for (TColStd_SequenceOfTransient::Iterator anIt(aTessShells); anIt.More(); anIt.Next()) 
    {
      Handle(StepVisual_TessellatedShell) aTessShell = Handle(StepVisual_TessellatedShell)::DownCast(anIt.Value());
      aNbItems += aTessShell->NbItems();
    }
    Handle(StepVisual_HArray1OfTessellatedStructuredItem) anItems
      = new StepVisual_HArray1OfTessellatedStructuredItem(1, aNbItems);
    Standard_Integer j = 1;
    for (TColStd_SequenceOfTransient::Iterator anIt(aTessShells); anIt.More(); anIt.Next()) 
    {
      Handle(StepVisual_TessellatedShell) aTessShell = Handle(StepVisual_TessellatedShell)::DownCast(anIt.Value());
      for (Standard_Integer i = 1; i <= aTessShell->NbItems(); ++i) {
        anItems->SetValue(j++, aTessShell->ItemsValue(i));
      }
    }
    Standard_Boolean aHasGeomLink = !theBrepWithVoids.IsNull();
    aTessSolid->Init(aTessName, anItems, aHasGeomLink, theBrepWithVoids);
    theTessellatedItem = aTessSolid;

    done = Standard_True;
  }
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepShape_BrepWithVoids) &
      TopoDSToStep_MakeBrepWithVoids::Value() const
{
  StdFail_NotDone_Raise_if (!done, "TopoDSToStep_MakeBrepWithVoids::Value() - no result");
  return theBrepWithVoids;
}

// ============================================================================
// Method  : TopoDSToStep_MakeBrepWithVoids::TessellatedValue
// Purpose : Returns TessellatedItem as the optional result
// ============================================================================

const Handle(StepVisual_TessellatedItem) &
TopoDSToStep_MakeBrepWithVoids::TessellatedValue() const
{
  StdFail_NotDone_Raise_if(!done, "TopoDSToStep_MakeBrepWithVoids::TessellatedValue() - no result");
  return theTessellatedItem;
}
