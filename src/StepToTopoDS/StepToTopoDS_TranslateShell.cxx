// Created on: 1995-01-03
// Created by: Frederic MAUPAS
// Copyright (c) 1995-1999 Matra Datavision
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

//:   gka 09.04.99: S4136: improving tolerance management

#include <BRep_Builder.hxx>
#include <Message_ProgressScope.hxx>
#include <StdFail_NotDone.hxx>
#include <StepShape_ConnectedFaceSet.hxx>
#include <StepShape_FaceSurface.hxx>
#include <StepToTopoDS_NMTool.hxx>
#include <StepToTopoDS_Tool.hxx>
#include <StepToTopoDS_TranslateFace.hxx>
#include <StepToTopoDS_TranslateShell.hxx>
#include <StepVisual_TessellatedShell.hxx>
#include <StepVisual_TriangulatedFace.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep_ShapeBinder.hxx>

// ============================================================================
// Method  : StepToTopoDS_TranslateShell::StepToTopoDS_TranslateShell
// Purpose : Empty Constructor
// ============================================================================
StepToTopoDS_TranslateShell::StepToTopoDS_TranslateShell()
: myError(StepToTopoDS_TranslateShellOther)
{
  done = Standard_False;
}

// ============================================================================
// Method  : Init
// Purpose : Init with a ConnectedFaceSet and a Tool
// ============================================================================

void StepToTopoDS_TranslateShell::Init
(const Handle(StepShape_ConnectedFaceSet)& CFS,
 StepToTopoDS_Tool& aTool,
 StepToTopoDS_NMTool& NMTool,
 const Message_ProgressRange& theProgress)
{
  //bug15697
  if(CFS.IsNull())
    return;
  
  if (!aTool.IsBound(CFS)) {

    BRep_Builder B;
    Handle(Transfer_TransientProcess) TP = aTool.TransientProcess();

    Standard_Integer NbFc = CFS->NbCfsFaces();
    TopoDS_Shell Sh;
    B.MakeShell(Sh);
    TopoDS_Face F;
    TopoDS_Shape S;
    Handle(StepShape_Face) StepFace;

    StepToTopoDS_TranslateFace myTranFace;
    myTranFace.SetPrecision(Precision()); //gka
    myTranFace.SetMaxTol(MaxTol());

    Message_ProgressScope PS ( theProgress, "Face", NbFc);
    for (Standard_Integer i = 1; i <= NbFc && PS.More(); i++, PS.Next()) {
#ifdef OCCT_DEBUG
      std::cout << "Processing Face : " << i << std::endl;
#endif
      StepFace = CFS->CfsFacesValue(i);
      Handle(StepShape_FaceSurface) theFS =
        Handle(StepShape_FaceSurface)::DownCast(StepFace);
      if (!theFS.IsNull()) {
        myTranFace.Init(theFS, aTool, NMTool);
        if (myTranFace.IsDone()) {
          S = myTranFace.Value();
          F = TopoDS::Face(S);
          B.Add(Sh, F);
        }
        else { // Warning only + add FaceSurface file Identifier
          TP->AddWarning(theFS, " a Face from Shell not mapped to TopoDS");
        }
      }
      else { // Warning : add identifier
        TP->AddWarning(StepFace, " Face is not of FaceSurface Type; not mapped to TopoDS");
      }
    }
    Sh.Closed (BRep_Tool::IsClosed (Sh));
    myResult = Sh;
    aTool.Bind(CFS, myResult);
    myError  = StepToTopoDS_TranslateShellDone;
    done     = Standard_True;
  }
  else {
    myResult = TopoDS::Shell(aTool.Find(CFS));
    myError  = StepToTopoDS_TranslateShellDone;
    done     = Standard_True;
  }
}

// ============================================================================
// Method  : Init
// Purpose : Init with a ConnectedFaceSet and a Tool
// ============================================================================

void StepToTopoDS_TranslateShell::Init(const Handle(StepVisual_TessellatedShell)& theTSh,
                                       StepToTopoDS_Tool& theTool,
                                       StepToTopoDS_NMTool& theNMTool,
                                       const Standard_Boolean theReadTessellatedWhenNoBRepOnly,
                                       Standard_Boolean& theHasGeom,
                                       const Message_ProgressRange& theProgress)
{
  if (theTSh.IsNull())
    return;

  BRep_Builder aB;
  TopoDS_Shell aSh;

  Standard_Integer aNb = theTSh->NbItems();
  Message_ProgressScope aPS(theProgress, "Face", theTSh->HasTopologicalLink() ? aNb + 1 : aNb);

  Handle(Transfer_TransientProcess) aTP = theTool.TransientProcess();

  if (theTSh->HasTopologicalLink()) 
  {
    Handle(TransferBRep_ShapeBinder) aBinder
      = Handle(TransferBRep_ShapeBinder)::DownCast(aTP->Find(theTSh->TopologicalLink()));
    if (!aBinder.IsNull()) 
    {
      aSh = aBinder->Shell();
      theHasGeom = Standard_True;
    }
  }

  Standard_Boolean aNewShell = Standard_False;
  if (aSh.IsNull()) 
  {
    aB.MakeShell(aSh);
    aNewShell = Standard_True;
    theHasGeom = Standard_False;
  }

  StepToTopoDS_TranslateFace aTranTF;
  aTranTF.SetPrecision(Precision());
  aTranTF.SetMaxTol(MaxTol());

  for (Standard_Integer i = 1; i <= aNb && aPS.More(); i++, aPS.Next()) 
  {
#ifdef OCCT_DEBUG
    std::cout << "Processing Face : " << i << std::endl;
#endif
    Handle(StepVisual_TessellatedStructuredItem) anItem = theTSh->ItemsValue(i);
    if (anItem->IsKind(STANDARD_TYPE(StepVisual_TessellatedFace))) 
    {
      Handle(StepVisual_TessellatedFace) aTFace = Handle(StepVisual_TessellatedFace)::DownCast(anItem);
      Standard_Boolean aHasFaceGeom = Standard_False;
      aTranTF.Init(aTFace, theTool, theNMTool, theReadTessellatedWhenNoBRepOnly, aHasFaceGeom);
      if (aTranTF.IsDone()) 
      {
        if (aNewShell) 
        {
          aB.Add(aSh, TopoDS::Face(aTranTF.Value()));
        }
        theHasGeom &= aHasFaceGeom;
      }
      else 
      {
        aTP->AddWarning(anItem, " Triangulated face if not mapped to TopoDS");
      }
    }
    else 
    {
      aTP->AddWarning(anItem, " Face is not of TriangulatedFace Type; not mapped to TopoDS");
    }
  }

  aSh.Closed(BRep_Tool::IsClosed(aSh));
  myResult = aSh;
  myError = StepToTopoDS_TranslateShellDone;
  done = Standard_True;
}

// ============================================================================
// Method  : Value
// Purpose : Return the mapped Shape
// ============================================================================

const TopoDS_Shape& StepToTopoDS_TranslateShell::Value() const 
{
  StdFail_NotDone_Raise_if (!done, "StepToTopoDS_TranslateShell::Value() - no result");
  return myResult;
}

// ============================================================================
// Method  : Error
// Purpose : Return the TranslateShell Error code
// ============================================================================

StepToTopoDS_TranslateShellError StepToTopoDS_TranslateShell::Error() const
{
  return myError;
}

