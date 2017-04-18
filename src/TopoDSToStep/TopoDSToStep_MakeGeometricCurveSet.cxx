// Created on: 1995-03-17
// Created by: Dieter THIEMANN
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


#include <MoniTool_DataMapOfShapeTransient.hxx>
#include <StdFail_NotDone.hxx>
#include <StepShape_GeometricCurveSet.hxx>
#include <StepShape_GeometricSetSelect.hxx>
#include <StepShape_HArray1OfGeometricSetSelect.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDSToStep.hxx>
#include <TopoDSToStep_Builder.hxx>
#include <TopoDSToStep_MakeGeometricCurveSet.hxx>
#include <TopoDSToStep_Tool.hxx>
#include <TopoDSToStep_WireframeBuilder.hxx>
#include <Transfer_FinderProcess.hxx>
#include <TransferBRep_ShapeMapper.hxx>

//=============================================================================
// Create a GeometricCurveSet of StepShape from a Shape of TopoDS
//=============================================================================
TopoDSToStep_MakeGeometricCurveSet::TopoDSToStep_MakeGeometricCurveSet(
                                    const TopoDS_Shape& aShape,
				    const Handle(Transfer_FinderProcess)& FP)
{
  done = Standard_False;
  Handle(TColStd_HSequenceOfTransient)  itemList;
  MoniTool_DataMapOfShapeTransient      aMap;
  TopoDSToStep_Tool                aTool (aMap, Standard_False);
  TopoDSToStep_WireframeBuilder    wirefB (aShape, aTool, FP);
  TopoDSToStep::AddResult ( FP, aTool );

  Handle(StepShape_GeometricCurveSet) aGCSet =
    new StepShape_GeometricCurveSet;
  Handle(TCollection_HAsciiString) empty = new TCollection_HAsciiString("");
  if (wirefB.IsDone()) {
    itemList = wirefB.Value();
    Standard_Integer nbItem = itemList->Length();
    if (nbItem > 0) {
      Handle(StepShape_HArray1OfGeometricSetSelect) aGSS =
	new StepShape_HArray1OfGeometricSetSelect(1,nbItem);

      for (Standard_Integer i=1; i<=nbItem; i++) {
	StepShape_GeometricSetSelect select;
	select.SetValue(itemList->Value(i));
	aGSS->SetValue(i,select);
      }
      aGCSet->SetName(empty);
      aGCSet->SetElements(aGSS);
      theGeometricCurveSet = aGCSet;
      done = Standard_True;
    }
  }
}

const Handle(StepShape_GeometricCurveSet)& TopoDSToStep_MakeGeometricCurveSet::Value() const 
{
  StdFail_NotDone_Raise_if (!done, "TopoDSToStep_MakeGeometricCurveSet::Value() - no result");
  return theGeometricCurveSet;
}

//const Handle(StepShape_GeometricCurveSet)& TopoDSToStep_MakeGeometricCurveSet::Operator() const 
//{
//  return Value();
//}

//TopoDSToStep_MakeGeometricCurveSet::operator Handle(StepShape_GeometricCurveSet) () const 
//{
//  return Value();
//}


