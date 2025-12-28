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
#include <StepShape_BrepWithVoids.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_OrientedClosedShell.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepShape_OpenShell.hxx>
#include <StepShape_OrientedClosedShell.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <StepVisual_TessellatedStructuredItem.hxx>
#include <NCollection_HArray1.hxx>
#include <StepVisual_TessellatedShell.hxx>
#include <StepVisual_TessellatedSolid.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
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

//: d7
#ifdef OCCT_DEBUG
  #define DEBUG
#endif

//=============================================================================
// Create a BrepWithVoids of StepShape from a Solid of TopoDS containing
// more than one closed shell
//=============================================================================

TopoDSToStep_MakeBrepWithVoids::TopoDSToStep_MakeBrepWithVoids(
  const TopoDS_Solid&                   aSolid,
  const occ::handle<Transfer_FinderProcess>& FP,
  const StepData_Factors&               theLocalFactors,
  const Message_ProgressRange&          theProgress)
{
  done = false;
  TopoDS_Iterator                  It;
  NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher> aMap;
  NCollection_Sequence<occ::handle<Standard_Transient>>      S;
  TopoDS_Shell                     aOutShell;

  occ::handle<StepShape_TopologicalRepresentationItem> aItem;
  occ::handle<StepShape_ClosedShell>                   aOuter, aCShell;
  occ::handle<StepShape_OrientedClosedShell>           aOCShell;
  occ::handle<NCollection_HArray1<occ::handle<StepShape_OrientedClosedShell>>>  aVoids;
  NCollection_Sequence<occ::handle<Standard_Transient>>                     aTessShells;

  occ::handle<StepData_StepModel> aStepModel     = occ::down_cast<StepData_StepModel>(FP->Model());
  const int     aWriteTessGeom = aStepModel->InternalParameters.WriteTessellated;

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

  int nbshapes = 0;
  for (It.Initialize(aSolid); It.More(); It.Next())
    if (It.Value().ShapeType() == TopAbs_SHELL)
      nbshapes++;
  Message_ProgressScope aPS(theProgress, NULL, nbshapes);
  for (It.Initialize(aSolid); It.More() && aPS.More(); It.Next())
  {
    if (It.Value().ShapeType() == TopAbs_SHELL)
    {
      TopoDS_Shell CurrentShell = TopoDS::Shell(It.Value());
      if (!aOutShell.IsNull()
          && !aOutShell.IsEqual(CurrentShell)) //: e0 abv 25 Mar 98: voids should be reversed
                                               //: according to EXPRESS for ABSR
        CurrentShell.Reverse();
      //: d7 abv 16 Mar 98: try to treat 'open' shells as closed since flag
      // IsClosed() is often incorrect (taken from MakeManifoldSolid(Solid))
      aTool.Init(aMap, false, aStepModel->InternalParameters.WriteSurfaceCurMode);
      StepB.Init(CurrentShell, aTool, FP, aWriteTessGeom, theLocalFactors, aPS.Next());
      TopoDSToStep::AddResult(FP, aTool);
      if (StepB.IsDone())
      {
        aCShell = occ::down_cast<StepShape_ClosedShell>(StepB.Value());
        // si OPEN on le force a CLOSED mais que c est une honte !
        if (aCShell.IsNull())
        {
          occ::handle<StepShape_OpenShell> aOShell =
            occ::down_cast<StepShape_OpenShell>(StepB.Value());
          if (!aOShell.IsNull())
          {
            aCShell = new StepShape_ClosedShell;
            aCShell->Init(aOShell->Name(), aOShell->CfsFaces());
#ifdef OCCT_DEBUG
            std::cout << "Warning: MakeBRepWithVoids: Open shell in the solid; treated as closed"
                      << std::endl;
#endif
          }
        }
        occ::handle<StepVisual_TessellatedItem> aTessShell = StepB.TessellatedValue();
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
        else
          std::cout << "*** MakeBRepWithVoids : Shell not mapped" << std::endl;
#endif
      }
#ifdef OCCT_DEBUG
      else
        std::cout << "Shell not mapped" << std::endl;
#endif
      /* //:d7
        if (It.Value().Closed()) {
          aTool.Init(aMap, false);
          StepB.Init(CurrentShell, aTool, FP);
          TopoDSToStep::AddResult ( FP, aTool );
          if (StepB.IsDone()) {
            aCShell = occ::down_cast<StepShape_ClosedShell>(StepB.Value());
            if ( aOutShell.IsEqual(It.Value()) )
              aOuter = aCShell;
            else
              S.Append(aCShell);
          }
          else {
      #ifdef OCCT_DEBUG
            std::cout << "Shell not mapped" << std::endl;
      #endif
            done = false;
          }
        }
        else {
          // Error Handling : the Shape is not closed
          done = false;
        }
      */
    }
  }
  if (!aPS.More())
    return;

  int N = S.Length();
  if (N >= 1)
  {
    occ::handle<TCollection_HAsciiString> aName = new TCollection_HAsciiString("");
    aVoids                                 = new NCollection_HArray1<occ::handle<StepShape_OrientedClosedShell>>(1, N);
    for (int i = 1; i <= N; i++)
    {
      aOCShell = new StepShape_OrientedClosedShell;
      // Warning : the Oriented Shell Orientation is not always
      //           TRUE.
      //           Shall check the TopoDS_Shell orientation.
      // => if the Shell is reversed, shall create an OrientedShell.
      aOCShell->Init(aName,
                     occ::down_cast<StepShape_ClosedShell>(S.Value(i)),
                     false); //: e0
      //: e0			 true);
      aVoids->SetValue(i, aOCShell);
    }

    theBrepWithVoids = new StepShape_BrepWithVoids();
    theBrepWithVoids->Init(aName, aOuter, aVoids);

    done = true;
  }

  if (!aTessShells.IsEmpty())
  {
    occ::handle<StepVisual_TessellatedSolid> aTessSolid = new StepVisual_TessellatedSolid();
    occ::handle<TCollection_HAsciiString>    aTessName  = new TCollection_HAsciiString("");
    int                    aNbItems   = 0;
    for (NCollection_Sequence<occ::handle<Standard_Transient>>::Iterator anIt(aTessShells); anIt.More(); anIt.Next())
    {
      occ::handle<StepVisual_TessellatedShell> aTessShell =
        occ::down_cast<StepVisual_TessellatedShell>(anIt.Value());
      aNbItems += aTessShell->NbItems();
    }
    occ::handle<NCollection_HArray1<occ::handle<StepVisual_TessellatedStructuredItem>>> anItems =
      new NCollection_HArray1<occ::handle<StepVisual_TessellatedStructuredItem>>(1, aNbItems);
    int j = 1;
    for (NCollection_Sequence<occ::handle<Standard_Transient>>::Iterator anIt(aTessShells); anIt.More(); anIt.Next())
    {
      occ::handle<StepVisual_TessellatedShell> aTessShell =
        occ::down_cast<StepVisual_TessellatedShell>(anIt.Value());
      for (int i = 1; i <= aTessShell->NbItems(); ++i)
      {
        anItems->SetValue(j++, aTessShell->ItemsValue(i));
      }
    }
    bool aHasGeomLink = !theBrepWithVoids.IsNull();
    aTessSolid->Init(aTessName, anItems, aHasGeomLink, theBrepWithVoids);
    theTessellatedItem = aTessSolid;

    done = true;
  }
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const occ::handle<StepShape_BrepWithVoids>& TopoDSToStep_MakeBrepWithVoids::Value() const
{
  StdFail_NotDone_Raise_if(!done, "TopoDSToStep_MakeBrepWithVoids::Value() - no result");
  return theBrepWithVoids;
}

// ============================================================================
// Method  : TopoDSToStep_MakeBrepWithVoids::TessellatedValue
// Purpose : Returns TessellatedItem as the optional result
// ============================================================================

const occ::handle<StepVisual_TessellatedItem>& TopoDSToStep_MakeBrepWithVoids::TessellatedValue() const
{
  StdFail_NotDone_Raise_if(!done, "TopoDSToStep_MakeBrepWithVoids::TessellatedValue() - no result");
  return theTessellatedItem;
}
