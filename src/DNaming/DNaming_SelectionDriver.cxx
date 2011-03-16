// File:	DNaming_SelectionDriver.cxx
// Created:	Tue May  5 10:42:07 2009
// Author:	Sergey ZARITCHNY <sergey.zaritchny@opencascade.com>
// Copyright:	Open  CasCade SA 2009 


#include <DNaming_SelectionDriver.ixx>
#include <TopAbs.hxx>
#include <TopoDS_Shape.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelMap.hxx>
#include <TFunction_Logbook.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Selector.hxx>
#include <TNaming_Tool.hxx>
#include <TFunction_Function.hxx>
#include <TDataStd_Integer.hxx>
#include <ModelDefinitions.hxx>

//#define SEL_DEB 1
#ifdef SEL_DEB
#include <TDF_Tool.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>
#endif
//=======================================================================
//function : DNaming_SelectionDriver
//purpose  : Constructor
//=======================================================================
DNaming_SelectionDriver::DNaming_SelectionDriver()
{}

//=======================================================================
//function : Validate
//purpose  : Validates labels of a function in <theLog>.
//=======================================================================
void DNaming_SelectionDriver::Validate(TFunction_Logbook& ) const
{}

//=======================================================================
//function : MustExecute
//purpose  : Analyse in <theLog> if the loaded function must be
//           executed (i.e.arguments are modified) or not.
//=======================================================================
Standard_Boolean DNaming_SelectionDriver::MustExecute(const TFunction_Logbook& ) const {
  return Standard_True;
}

#ifdef SEL_DEB
#include <BRepTools.hxx>
static void Write(const TopoDS_Shape& shape,
		      const Standard_CString filename) 
{
  ofstream save;
  save.open(filename);
  save << "DBRep_DrawableShape" << endl << endl;
  if(!shape.IsNull()) BRepTools::Write(shape, save);
  save.close();
}
#endif
//=======================================================================
//function : Execute
//purpose  : Execute the function and push in <theLog> the impacted
//           labels (see method SetImpacted).
//=======================================================================
#include <TNaming_ListOfNamedShape.hxx>
#include <TDF_AttributeMap.hxx>
#include <TDF_IDFilter.hxx>
#include <TDF_Tool.hxx>
#include <TDF_MapIteratorOfAttributeMap.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_ChildIterator.hxx>
static void CollectLabels(const TDF_Label& theLabel, TDF_LabelMap& theMap) {
  TDF_AttributeMap outRefs;
  TDF_Tool::OutReferences(theLabel, outRefs);
  for (TDF_MapIteratorOfAttributeMap itr(outRefs); itr.More(); itr.Next()) {
    if (itr.Key()->DynamicType() == STANDARD_TYPE(TNaming_NamedShape)) {
      theMap.Add(itr.Key()->Label());
      CollectLabels(itr.Key()->Label(), theMap);
      TCollection_AsciiString anEntry;
      TDF_Tool::Entry(itr.Key()->Label(), anEntry);
      cout << "NS refered Label = " << anEntry << endl;	
    }
  }
}
Standard_Integer DNaming_SelectionDriver::Execute(TFunction_Logbook& theLog) const
{
  Handle(TFunction_Function) aFunction;
  Label().FindAttribute(TFunction_Function::GetID(),aFunction);
  if(aFunction.IsNull()) return -1;


  TDF_Label aRLabel = RESPOSITION(aFunction);
  if(aRLabel.IsNull()) return -1;

  Standard_Boolean aIsWire = Standard_False;
  TopAbs_ShapeEnum aPrevShapeType = TopAbs_SHAPE;
  Handle(TNaming_NamedShape) aNShape; 
  if(aRLabel.FindAttribute(TNaming_NamedShape::GetID(),aNShape)) {
    if(!aNShape.IsNull() && !aNShape->IsEmpty()) {
      aPrevShapeType = aNShape->Get().ShapeType();
      if (aPrevShapeType == TopAbs_WIRE) aIsWire = Standard_True;      
    }
  }

  TNaming_Selector aSelector(aRLabel);

  TDF_LabelMap aMap;
  aMap = theLog.ChangeValid();
#ifdef SEL_DEB
  cout <<"#E_DNaming_SelectionDriver:: Valid Label Map:"<<endl;
  TDF_MapIteratorOfLabelMap anItr(aMap);
  for (; anItr.More(); anItr.Next()) {
    const TDF_Label& aLabel = anItr.Key();
    TCollection_AsciiString anEntry;
    TDF_Tool::Entry(aLabel, anEntry);
    cout << "\tLabel = " << anEntry << endl;
  }
#endif
//***
//  TDF_IDFilter aFilterForReferers;
//  aFilterForReferers.Keep(TNaming_NamedShape::GetID());
//  TDF_IDFilter aFilterForReferences;
//  aFilterForReferences.Keep(TNaming_NamedShape::GetID());
//   TDF_LabelMap aMap1;
//   CollectLabels(aLabel, aMap1);  
//  TDF_Tool::OutReferences(aLabel, /*aFilterForReferers, aFilterForReferences, */outRefs);
//***

  if(aSelector.Solve(aMap)) {
    theLog.SetValid(aRLabel);    
    Handle(TNaming_NamedShape) aNS;
    if(!aRLabel.FindAttribute(TNaming_NamedShape::GetID(),aNS)) {
      cout <<"%%%WARNING: DNaming_SelectionDriver::NamedShape is not found"<<endl;
    }
    else {
      if(aNS.IsNull()) {
	cout << "%%%WARNING: DNaming_SelectionDriver::NamedShape is NULL" <<endl;
      } else 
	if(aNS->IsEmpty()) {
	  cout << "%%%WARNING: DNaming_SelectionDriver::NamedShape is EMPTY on Label = ";
	  aNS->Label().EntryDump(cout); cout << endl;
	  
	} else {
#ifdef SEL_DEB
	  Write(aNS->Get(), "Selection_Result.brep");
	  Handle(TopoDS_TShape) aTS = aNS->Get().TShape();
	  cout << "TShape = " << (Standard_Address)aTS <<endl;
#endif 
	  if(aIsWire && aNS->Get().ShapeType()== TopAbs_COMPOUND) {
	    TopoDS_Shape aWireShape;
	    TNaming_Tool::FindShape(aMap, aMap, aNS, aWireShape);
	    TNaming_Builder aBuilder(aRLabel);
	    aBuilder.Select(aWireShape, aWireShape);
	    aFunction->SetFailure(DONE);
	  } else
	    if(aPrevShapeType == aNS->Get().ShapeType()) {
	      aFunction->SetFailure(DONE);
	    } else {
#ifdef SEL_DEB	    
	      cout <<"%%%WARNING: DNaming_SelectionDriver::Execute: The Shape after solving changed type = "<<aNS->Get().ShapeType()<< endl;
#endif
	      aFunction->SetFailure(DONE);
	    }
	}
    }
  }
  else {
    aFunction->SetFailure(NOTDONE);
    cout << "%%%WARNING: DNaming_SelectionDriver::Execute: Selection is Not solved !!!" << endl;
    return 1;
  }
  return 0;
}
