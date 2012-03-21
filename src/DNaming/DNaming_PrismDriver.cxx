// Created on: 2009-06-16
// Created by: Sergey ZARITCHNY
// Copyright (c) 2009-2012 OPEN CASCADE SAS
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



#include <DNaming_PrismDriver.ixx>
// OCC
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax2.hxx>
#include <Geom_Line.hxx>

#include <BRep_Tool.hxx>
#include <BRepGProp.hxx>
#include <BRepCheck_Shell.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopLoc_Location.hxx>
#include <TopExp_Explorer.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <BRepCheck_Wire.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Solid.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Real.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>

// OCAF
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TNaming.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TFunction_Function.hxx>
#include <TFunction_Logbook.hxx>
#include <TDF_Label.hxx>
#include <TDF_TagSource.hxx>

#include <DNaming.hxx>
#include <ModelDefinitions.hxx>

//=======================================================================
//function : DNaming_PrismDriver
//purpose  : Constructor
//=======================================================================
DNaming_PrismDriver::DNaming_PrismDriver()
{}

//=======================================================================
//function : Validate
//purpose  : Validates labels of a function in <theLog>.
//=======================================================================
void DNaming_PrismDriver::Validate(TFunction_Logbook& theLog) const
{}

//=======================================================================
//function : MustExecute
//purpose  : Analyses in <theLog> if the loaded function must be executed
//=======================================================================
Standard_Boolean DNaming_PrismDriver::MustExecute(const TFunction_Logbook& theLog) const
{
  return Standard_True;
}

#ifdef OCC_DEB
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
//purpose  : Executes the function 
//=======================================================================
Standard_Integer DNaming_PrismDriver::Execute(TFunction_Logbook& theLog) const {
  Handle(TFunction_Function) aFunction;
  Label().FindAttribute(TFunction_Function::GetID(), aFunction);
  if(aFunction.IsNull()) return -1;

  // Save location
  Handle(TNaming_NamedShape) aPrevPrism = DNaming::GetFunctionResult(aFunction);
  TopLoc_Location aLocation;
  if (!aPrevPrism.IsNull() && !aPrevPrism->IsEmpty()) {
    aLocation = aPrevPrism->Get().Location();
  }

  //Basis for IPrism
  Handle(TDataStd_UAttribute) aBasObject = DNaming::GetObjectArg(aFunction,PRISM_BASIS);
  Handle(TNaming_NamedShape) aBasisNS = DNaming::GetObjectValue(aBasObject);
  if(aBasisNS.IsNull() || aBasisNS->IsEmpty()) {
    aFunction->SetFailure(WRONG_ARGUMENT);
    return -1;
  }
 
  const TopoDS_Shape& aBasis = aBasisNS->Get(); 
  TopoDS_Shape aBASIS;
  if(aBasis.ShapeType() == TopAbs_WIRE) {
    Handle(BRepCheck_Wire) aCheck = new BRepCheck_Wire(TopoDS::Wire(aBasis));
    if(aCheck->Closed(Standard_True) == BRepCheck_NoError) {
      BRepBuilderAPI_MakeFace aMaker (TopoDS::Wire(aBasis), Standard_True); //Makes planar face
      if(aMaker.IsDone()) 
	aBASIS = aMaker.Face();//aMaker.Face();           
    }
  } else if(aBasis.ShapeType() == TopAbs_FACE)
    aBASIS = aBasis;
  if(aBASIS.IsNull()) {
    aFunction->SetFailure(WRONG_ARGUMENT);
    return -1;
  }


  Handle(TNaming_NamedShape) aContextOfBasis;
  Standard_Boolean anIsAttachment = Standard_False;
  if(DNaming::IsAttachment(aBasObject)) {
    aContextOfBasis = DNaming::GetAttachmentsContext(aBasObject); // a Context of Prism basis
    if(aContextOfBasis.IsNull() || aContextOfBasis->IsEmpty()) {
      aFunction->SetFailure(WRONG_ARGUMENT);
      return -1;
    }
    anIsAttachment = Standard_True;
  }

// Height
  Standard_Real aHeight = DNaming::GetReal(aFunction,PRISM_HEIGHT)->Get();
  if(aHeight <= Precision::Confusion()) {
    aFunction->SetFailure(WRONG_ARGUMENT);
    return -1;
  }

  // Direction
  gp_Ax1 anAxis;  
  DNaming::ComputeSweepDir(aBasis, anAxis);
    
  // Reverse
  Standard_Integer aRev = DNaming::GetInteger(aFunction,PRISM_DIR)->Get();
  if(aRev) anAxis.Reverse();

  // Calculate Vec - direction of extrusion
  gp_Vec aVEC(anAxis.Direction());
  aVEC = aVEC*aHeight;
  
  BRepPrimAPI_MakePrism aMakePrism(aBASIS, aVEC, Standard_True);  
  aMakePrism.Build();
  if (!aMakePrism.IsDone()) {
    aFunction->SetFailure(ALGO_FAILED);
    return -1;
  }
  
  const TopoDS_Shape& aResult = aMakePrism.Shape();
  BRepCheck_Analyzer aCheck(aResult);
  if (!aCheck.IsValid(aResult)) {
    aFunction->SetFailure(RESULT_NOT_VALID);
#ifdef OCC_DEB
    Standard_CString aFileName = "PrismResult.brep";
    Write(aResult, aFileName);
#endif
    return -1;
  }
  Standard_Boolean aVol = Standard_False;

  if(aResult.ShapeType() == TopAbs_SOLID) aVol = Standard_True;
  else if(aResult.ShapeType() == TopAbs_SHELL) {
    Handle(BRepCheck_Shell) aCheck = new BRepCheck_Shell(TopoDS::Shell(aResult));
    if(aCheck->Closed() == BRepCheck_NoError) 
      aVol = Standard_True;
  } 

  if(aVol) {
    GProp_GProps aGProp;
    BRepGProp::VolumeProperties(aResult, aGProp);
    if(aGProp.Mass() <= Precision::Confusion()) {
      aFunction->SetFailure(RESULT_NOT_VALID);
      return -1;
    }
  }

  // Naming  
  if(anIsAttachment)
    LoadNamingDS(RESPOSITION(aFunction), aMakePrism, aBASIS, aContextOfBasis->Get());
  else
    LoadNamingDS(RESPOSITION(aFunction), aMakePrism, aBASIS, aBASIS);

  // restore location
  if(!aLocation.IsIdentity()) 
    TNaming::Displace(RESPOSITION(aFunction), aLocation, Standard_True);

  theLog.SetValid(RESPOSITION(aFunction),Standard_True);  
  aFunction->SetFailure(DONE);
  return 0;
}

//=======================================================================
//function : LoadAndName
//purpose  : 
//=======================================================================
void DNaming_PrismDriver::LoadNamingDS (const TDF_Label& theResultLabel, 
					   BRepPrimAPI_MakePrism& MS,
					   const TopoDS_Shape& Basis,
					   const TopoDS_Shape& Context
					   ) const 
{

  TopTools_DataMapOfShapeShape SubShapes;
  for (TopExp_Explorer Exp(MS.Shape(),TopAbs_FACE); Exp.More(); Exp.Next()) {
    SubShapes.Bind(Exp.Current(),Exp.Current());
  }
  
  Handle(TDF_TagSource) Tagger = TDF_TagSource::Set(theResultLabel);
  if (Tagger.IsNull()) return;
  Tagger->Set(0);

  TNaming_Builder Builder (theResultLabel);
  if(Basis.IsEqual(Context))
    Builder.Generated(MS.Shape());
  else
    Builder.Generated(Context, MS.Shape());
 
  //Insert lateral face : Face from Edge
  TNaming_Builder  LateralFaceBuilder(theResultLabel.NewChild());   
  DNaming::LoadAndOrientGeneratedShapes(MS, Basis, TopAbs_EDGE, LateralFaceBuilder, SubShapes);

  Standard_Boolean makeTopBottom = Standard_True;
  if (Basis.ShapeType() == TopAbs_COMPOUND) {
    TopoDS_Iterator itr(Basis);
    if (itr.More() && itr.Value().ShapeType() == TopAbs_WIRE) makeTopBottom = Standard_False;
  } else if (Basis.ShapeType() == TopAbs_WIRE) {
    makeTopBottom = Standard_False;
  }
  if (makeTopBottom) {
    //Insert bottom face
    TopoDS_Shape BottomFace = MS.FirstShape();
    if (!BottomFace.IsNull()) {
      if (BottomFace.ShapeType() != TopAbs_COMPOUND) {
	TNaming_Builder BottomBuilder(theResultLabel.NewChild());  //2
	if (SubShapes.IsBound(BottomFace)) {
	  BottomFace = SubShapes(BottomFace);
	}
	BottomBuilder.Generated(BottomFace);
      } else {
	TopoDS_Iterator itr(BottomFace);
	for (; itr.More(); itr.Next()) {
	  TNaming_Builder BottomBuilder(theResultLabel.NewChild());  
	  BottomBuilder.Generated(itr.Value());
	}
      }
    }
    
    //Insert top face
    TopoDS_Shape TopFace = MS.LastShape();
    if (!TopFace.IsNull()) {
      if (TopFace.ShapeType() != TopAbs_COMPOUND) {
	TNaming_Builder TopBuilder(theResultLabel.NewChild()); //3
	if (SubShapes.IsBound(TopFace)) {
	  TopFace = SubShapes(TopFace);
	}
	TopBuilder.Generated(TopFace);
      } else {
	TopoDS_Iterator itr(TopFace);
	for (; itr.More(); itr.Next()) {
	  TNaming_Builder TopBuilder(theResultLabel.NewChild());  
	  TopBuilder.Generated(itr.Value());
	}
      }
    }
  }   
}
