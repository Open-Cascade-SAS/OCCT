// Created on: 1995-03-17
// Created by: Dieter THIEMANN
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <TopoDSToStep_MakeGeometricCurveSet.ixx>
#include <TopoDSToStep.hxx>
#include <TopoDSToStep_Builder.hxx>
#include <TopoDSToStep_WireframeBuilder.hxx>
#include <TopoDSToStep_Tool.hxx>
#include <MoniTool_DataMapOfShapeTransient.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>

#include <StepShape_GeometricSetSelect.hxx>
#include <StepShape_HArray1OfGeometricSetSelect.hxx>

#include <TColStd_HSequenceOfTransient.hxx>
#include <StdFail_NotDone.hxx>
#include <Transfer_FinderProcess.hxx>
#include <TransferBRep_ShapeMapper.hxx>

#include <TCollection_HAsciiString.hxx>

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
  StdFail_NotDone_Raise_if(!done,"");
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


