//File:	        TopoDSToStep_MakeGeometricCurveSet.cxx
//Created:	Fri Mar 17 10:54:55 1995
//Author:	Dieter THIEMANN
//		<dth@cinox>
//Copyright:    Matra Datavision 1995

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


