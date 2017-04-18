// Created on: 1994-11-25
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


#include <Message_ProgressIndicator.hxx>
#include <StdFail_NotDone.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_ConnectedFaceSet.hxx>
#include <StepShape_Face.hxx>
#include <StepShape_FaceSurface.hxx>
#include <StepShape_HArray1OfFace.hxx>
#include <StepShape_OpenShell.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_SequenceOfTransient.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDSToStep_Builder.hxx>
#include <TopoDSToStep_MakeStepFace.hxx>
#include <TopoDSToStep_Tool.hxx>
#include <Transfer_FinderProcess.hxx>
#include <TransferBRep_ShapeMapper.hxx>

// ============================================================================
// Method  : TopoDSToStep_Builder::TopoDSToStep_Builder
// Purpose :
// ============================================================================
TopoDSToStep_Builder::TopoDSToStep_Builder()
{
  done = Standard_False;
}

// ============================================================================
// Method  : TopoDSToStep_Builder::TopoDSToStep_Builder
// Purpose :
// ============================================================================

TopoDSToStep_Builder::TopoDSToStep_Builder
(const TopoDS_Shape& aShape,
 TopoDSToStep_Tool& aTool, const Handle(Transfer_FinderProcess)& FP)
{
  done = Standard_False;
  Init(aShape, aTool, FP);
}

// ============================================================================
// Method  : TopoDSToStep_Builder::Init
// Purpose :
// ============================================================================

void TopoDSToStep_Builder::Init(const TopoDS_Shape& aShape,
                                TopoDSToStep_Tool& myTool,
                                const Handle(Transfer_FinderProcess)& FP)
{
  
   if (myTool.IsBound(aShape)) {
    myError  = TopoDSToStep_BuilderDone;
    done     = Standard_True;
    myResult = myTool.Find(aShape);
    return;
  }

  Handle(Message_ProgressIndicator) progress = FP->GetProgress();

  switch (aShape.ShapeType()) 
    {      
    case TopAbs_SHELL: 
      {	
	TopoDS_Shell myShell = TopoDS::Shell(aShape);	  
	myTool.SetCurrentShell(myShell);
	
	Handle(StepShape_FaceSurface)                   FS;
	Handle(StepShape_TopologicalRepresentationItem) Fpms;
	TColStd_SequenceOfTransient                 mySeq;
	
//	const TopoDS_Shell ForwardShell = 
//	  TopoDS::Shell(myShell.Oriented(TopAbs_FORWARD));

//	TopExp_Explorer myExp(ForwardShell, TopAbs_FACE);
//  CKY  9-DEC-1997 (PRO9824 et consorts)
//   Pour passer les orientations : ELLES SONT DONNEES EN RELATIF
//   Donc, pour SHELL, on doit l ecrire en direct en STEP (pas le choix)
//   -> il faut repercuter en dessous, donc explorer le Shell TEL QUEL
//   Pour FACE WIRE, d une part on ECRIT SON ORIENTATION relative au contenant
//     (puisqu on peut), d autre part on EXPLORE EN FORWARD : ainsi les
//     orientations des sous-shapes sont relatives a leur contenant immediat
//     et la recombinaison en lecture est sans malice
//  Il reste ici et la du code relatif a "en Faceted on combine differemment"
//  -> reste encore du menage a faire



	TopExp_Explorer myExp(myShell, TopAbs_FACE);

	TopoDSToStep_MakeStepFace MkFace;

	for (;myExp.More();myExp.Next()) {

	  const TopoDS_Face Face = TopoDS::Face(myExp.Current());
    
    MkFace.Init(Face, myTool, FP);

	  if (MkFace.IsDone()) {
	    FS = Handle(StepShape_FaceSurface)::DownCast(MkFace.Value());
	    Fpms = FS;
	    mySeq.Append(Fpms);
	  }
	  else {
	    // MakeFace Error Handling : warning only
//	    cout << "Warning : one Face has not been mapped" << endl;
//	  Handle(TransferBRep_ShapeMapper) errShape =
//	    new TransferBRep_ShapeMapper(Face);
//	    FP->AddWarning(errShape, " a Face from a Shell has not been mapped");
	  }
    if (!progress.IsNull()) progress->Increment();
	}

	Standard_Integer nbFaces = mySeq.Length();
	if ( nbFaces >= 1) {
	  Handle(StepShape_HArray1OfFace) aSet = 
	    new StepShape_HArray1OfFace(1,nbFaces);
	  for (Standard_Integer i=1; i<=nbFaces; i++ ) {
	    aSet->SetValue(i, Handle(StepShape_Face)::DownCast(mySeq.Value(i)));
	  }
	  Handle(StepShape_ConnectedFaceSet) CFSpms;
	  if (myShell.Closed())
	    CFSpms = new StepShape_ClosedShell();
	  else
	    CFSpms = new StepShape_OpenShell();
	  Handle(TCollection_HAsciiString) aName = 
	    new TCollection_HAsciiString("");
	  CFSpms->Init(aName, aSet);

	  // --------------------------------------------------------------
	  // To add later : if not facetted context & shell is reversed
	  //                then shall create an oriented_shell with
	  //                orientation flag to false.
	  // --------------------------------------------------------------

	  myTool.Bind(aShape, CFSpms);
	  myResult = CFSpms;
	  done     = Standard_True;
	}
	else {
	  // Builder Error handling;
	  myError = TopoDSToStep_NoFaceMapped;
	  done    = Standard_False;
	}
	break;
      }
      
    case TopAbs_FACE:
      {
	const TopoDS_Face Face = TopoDS::Face(aShape);	  

	Handle(StepShape_FaceSurface)                   FS;
	Handle(StepShape_TopologicalRepresentationItem) Fpms;

	TopoDSToStep_MakeStepFace MkFace(Face, myTool, FP);

	if (MkFace.IsDone()) {
	  FS = Handle(StepShape_FaceSurface)::DownCast(MkFace.Value());
	  Fpms = FS;
	  myResult = Fpms;
	  myError = TopoDSToStep_BuilderDone;
	  done = Standard_True;
	}
	else {
	  // MakeFace Error Handling : Face not Mapped
	  myError = TopoDSToStep_BuilderOther;
//	  Handle(TransferBRep_ShapeMapper) errShape =
//	    new TransferBRep_ShapeMapper(Face);
//	  FP->AddWarning(errShape, " the Face has not been mapped");
	  done = Standard_False;
	}
        if (!progress.IsNull()) progress->Increment();
	break;
      }
    default: break;
    }
}

// ============================================================================
// Method  : TopoDSToStep_Builder::Value
// Purpose :
// ============================================================================

const Handle(StepShape_TopologicalRepresentationItem)& 
TopoDSToStep_Builder::Value() const 
{
  StdFail_NotDone_Raise_if (!done, "TopoDSToStep_Builder::Value() - no result");
  return myResult;
}

// ============================================================================
// Method  : TopoDSToStep_Builder::Error
// Purpose :
// ============================================================================

TopoDSToStep_BuilderError TopoDSToStep_Builder::Error() const 
{
  return myError;
}

