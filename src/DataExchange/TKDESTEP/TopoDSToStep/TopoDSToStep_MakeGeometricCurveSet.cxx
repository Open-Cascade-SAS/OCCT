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

#include <TopoDS_Shape.hxx>
#include <Standard_Transient.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <StdFail_NotDone.hxx>
#include <StepData_Factors.hxx>
#include <StepData_StepModel.hxx>
#include <StepShape_GeometricCurveSet.hxx>
#include <StepShape_GeometricSetSelect.hxx>
#include <StepShape_GeometricSetSelect.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDSToStep.hxx>
#include <TopoDSToStep_MakeGeometricCurveSet.hxx>
#include <TopoDSToStep_Tool.hxx>
#include <TopoDSToStep_WireframeBuilder.hxx>
#include <Transfer_FinderProcess.hxx>

//=============================================================================
// Create a GeometricCurveSet of StepShape from a Shape of TopoDS
//=============================================================================
TopoDSToStep_MakeGeometricCurveSet::TopoDSToStep_MakeGeometricCurveSet(
  const TopoDS_Shape&                   aShape,
  const occ::handle<Transfer_FinderProcess>& FP,
  const StepData_Factors&               theLocalFactors)
{
  done = false;
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> itemList;
  NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher>     aMap;
  occ::handle<StepData_StepModel> aStepModel = occ::down_cast<StepData_StepModel>(FP->Model());
  TopoDSToStep_Tool aTool(aMap, false, aStepModel->InternalParameters.WriteSurfaceCurMode);
  TopoDSToStep_WireframeBuilder wirefB(aShape, aTool, theLocalFactors);
  TopoDSToStep::AddResult(FP, aTool);

  occ::handle<StepShape_GeometricCurveSet> aGCSet = new StepShape_GeometricCurveSet;
  occ::handle<TCollection_HAsciiString>    empty  = new TCollection_HAsciiString("");
  if (wirefB.IsDone())
  {
    itemList                = wirefB.Value();
    int nbItem = itemList->Length();
    if (nbItem > 0)
    {
      occ::handle<NCollection_HArray1<StepShape_GeometricSetSelect>> aGSS =
        new NCollection_HArray1<StepShape_GeometricSetSelect>(1, nbItem);

      for (int i = 1; i <= nbItem; i++)
      {
        StepShape_GeometricSetSelect select;
        select.SetValue(itemList->Value(i));
        aGSS->SetValue(i, select);
      }
      aGCSet->SetName(empty);
      aGCSet->SetElements(aGSS);
      theGeometricCurveSet = aGCSet;
      done                 = true;
    }
  }
}

const occ::handle<StepShape_GeometricCurveSet>& TopoDSToStep_MakeGeometricCurveSet::Value() const
{
  StdFail_NotDone_Raise_if(!done, "TopoDSToStep_MakeGeometricCurveSet::Value() - no result");
  return theGeometricCurveSet;
}

// const occ::handle<StepShape_GeometricCurveSet>& TopoDSToStep_MakeGeometricCurveSet::Operator() const
//{
//   return Value();
// }

// TopoDSToStep_MakeGeometricCurveSet::operator occ::handle<StepShape_GeometricCurveSet> () const
//{
//   return Value();
// }
