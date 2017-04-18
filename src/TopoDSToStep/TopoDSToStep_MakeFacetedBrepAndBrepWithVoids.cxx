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
#include <MoniTool_DataMapOfShapeTransient.hxx>
#include <StdFail_NotDone.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_FacetedBrepAndBrepWithVoids.hxx>
#include <StepShape_HArray1OfOrientedClosedShell.hxx>
#include <StepShape_OrientedClosedShell.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_SequenceOfTransient.hxx>
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
TopoDSToStep_MakeFacetedBrepAndBrepWithVoids::
  TopoDSToStep_MakeFacetedBrepAndBrepWithVoids(const TopoDS_Solid& aSolid,
				    const Handle(Transfer_FinderProcess)& FP)
{
  done = Standard_False;
  TopoDS_Iterator              It;
  MoniTool_DataMapOfShapeTransient   aMap;
  TColStd_SequenceOfTransient  S;
  TopoDS_Shell                 aOutShell;

  Handle(StepShape_TopologicalRepresentationItem) aItem;
  Handle(StepShape_ClosedShell)                   aOuter, aCShell;
  Handle(StepShape_OrientedClosedShell)           aOCShell;
  Handle(StepShape_HArray1OfOrientedClosedShell)  aVoids;

  aOutShell = BRepClass3d::OuterShell(aSolid);

  TopoDSToStep_Builder StepB;
  TopoDSToStep_Tool    aTool;

  if (!aOutShell.IsNull()) {
    It.Initialize(aSolid);
    for ( ; It.More(); It.Next() ) {
      if (It.Value().ShapeType() == TopAbs_SHELL) {
	TopoDS_Shell CurrentShell = TopoDS::Shell(It.Value());
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
	    Handle(TransferBRep_ShapeMapper) errShape =
	      new TransferBRep_ShapeMapper(CurrentShell);
	    FP->AddWarning(errShape," Shell from Solid not mapped to FacetedBrepAndBrepWithVoids");
	  }
	}
	else {
	  done = Standard_False;
	  Handle(TransferBRep_ShapeMapper) errShape =
	    new TransferBRep_ShapeMapper(CurrentShell);
	  FP->AddWarning(errShape," Shell from Solid not closed; not mapped to FacetedBrepAndBrepWithVoids");
	}
      }
    }
  }
  Standard_Integer N = S.Length();
  if ( N>=1 ) {
    aVoids = new StepShape_HArray1OfOrientedClosedShell(1,N);
    Handle(TCollection_HAsciiString) aName = 
      new TCollection_HAsciiString("");
    for ( Standard_Integer i=1; i<=N; i++ ) {
      aOCShell = new StepShape_OrientedClosedShell();
      aOCShell->Init(aName, Handle(StepShape_ClosedShell)::DownCast(S.Value(i)),
		     Standard_True);
      aVoids->SetValue(i, aOCShell);
    }
    theFacetedBrepAndBrepWithVoids = 
      new StepShape_FacetedBrepAndBrepWithVoids();
    theFacetedBrepAndBrepWithVoids->Init(aName, aOuter, aVoids);
    done = Standard_True;
  }
  else {
    done = Standard_False;
    Handle(TransferBRep_ShapeMapper) errShape =
      new TransferBRep_ShapeMapper(aSolid);
    FP->AddWarning(errShape," Solid contains no Shell to be mapped to FacetedBrepAndBrepWithVoids");
  }
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepShape_FacetedBrepAndBrepWithVoids) &
      TopoDSToStep_MakeFacetedBrepAndBrepWithVoids::Value() const
{
  StdFail_NotDone_Raise_if (!done, "TopoDSToStep_MakeFacetedBrepAndBrepWithVoids::Value() - no result");
  return theFacetedBrepAndBrepWithVoids;
}
