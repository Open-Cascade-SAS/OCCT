// Created on: 2009-06-17
// Created by: Sergey ZARITCHNY
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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


#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_Shell.hxx>
#include <BRepCheck_Wire.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <DNaming.hxx>
#include <DNaming_RevolutionDriver.hxx>
#include <Geom_Line.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <GProp_GProps.hxx>
#include <ModelDefinitions.hxx>
#include <Precision.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Real.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDF_Label.hxx>
#include <TDF_TagSource.hxx>
#include <TFunction_Function.hxx>
#include <TFunction_Logbook.hxx>
#include <TNaming.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DNaming_RevolutionDriver,TFunction_Driver)

// OCAF
//=======================================================================
//function : DNaming_RevolutionDriver
//purpose  : Constructor
//=======================================================================
DNaming_RevolutionDriver::DNaming_RevolutionDriver()
{}

//=======================================================================
//function : Validate
//purpose  : Validates labels of a function in <theLog>.
//=======================================================================
void DNaming_RevolutionDriver::Validate(Handle(TFunction_Logbook)&) const
{}

//=======================================================================
//function : MustExecute
//purpose  : Analyses in <theLog> if the loaded function must be executed
//=======================================================================
Standard_Boolean DNaming_RevolutionDriver::MustExecute(const Handle(TFunction_Logbook)&) const
{
  return Standard_True;
}

//=======================================================================
//function : Execute
//purpose  : Executes the function
//=======================================================================
Standard_Integer DNaming_RevolutionDriver::Execute(Handle(TFunction_Logbook)& theLog) const {
  Handle(TFunction_Function) aFunction;
  Label().FindAttribute(TFunction_Function::GetID(), aFunction);
  if(aFunction.IsNull()) return -1;

  // Save location
  Handle(TNaming_NamedShape) aPrevRevol = DNaming::GetFunctionResult(aFunction);
  TopLoc_Location aLocation;
  if (!aPrevRevol.IsNull() && !aPrevRevol->IsEmpty()) {
    aLocation = aPrevRevol->Get().Location();
  }

  //Basis for Revol
  Handle(TDataStd_UAttribute) aBasObject = DNaming::GetObjectArg(aFunction,REVOL_BASIS);
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
    aContextOfBasis = DNaming::GetAttachmentsContext(aBasObject); // a Context of Revolution basis
    if(aContextOfBasis.IsNull() || aContextOfBasis->IsEmpty()) {
      aFunction->SetFailure(WRONG_ARGUMENT);
      return -1;
    }
    anIsAttachment = Standard_True;
  }

// Axis
  Handle(TDataStd_UAttribute) anAxObject = DNaming::GetObjectArg(aFunction,REVOL_AXIS);
  Handle(TNaming_NamedShape) anAxNS = DNaming::GetObjectValue(anAxObject);
  gp_Ax1 anAXIS;
  TopoDS_Shape aTopoDSAxis;
  if(anAxNS.IsNull() || anAxNS->IsEmpty()) {
    aFunction->SetFailure(WRONG_ARGUMENT);
    return -1;
  } else {
    aTopoDSAxis = anAxNS->Get();
    Standard_Boolean anAxisOK = Standard_False;
    if (!aTopoDSAxis.IsNull()) {
      if(aTopoDSAxis.ShapeType() == TopAbs_EDGE || aTopoDSAxis.ShapeType() == TopAbs_WIRE) {
	if (aTopoDSAxis.ShapeType() == TopAbs_WIRE) {
	  TopExp_Explorer anExplorer(aTopoDSAxis, TopAbs_EDGE);
	  aTopoDSAxis = anExplorer.Current();
	}
	const TopoDS_Edge& anEdge = TopoDS::Edge(aTopoDSAxis);
	Standard_Real aFirst, aLast;
	Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge,aFirst,aLast) ;
        if (aCurve->IsKind (STANDARD_TYPE(Geom_Line)) ) {
	  Handle(Geom_Line) aLine = Handle(Geom_Line)::DownCast(aCurve) ;
	  if(!aLine.IsNull()) {
	    anAXIS  = aLine->Position() ;
	    anAxisOK = Standard_True;
	  }
        }
      }
    }
    if(!anAxisOK) {
#ifdef OCCT_DEBUG
      cout<<"RevolutionDriver:: Axis is not correct"<<endl;
#endif
      aFunction->SetFailure(WRONG_ARGUMENT);
      return -1;
    }
  }

  if(aFunction->GetDriverGUID() == FULREVOL_GUID) {
    BRepPrimAPI_MakeRevol aMakeRevol(aBASIS, anAXIS, Standard_True);
    aMakeRevol.Build();
    if (!aMakeRevol.IsDone()) {
      aFunction->SetFailure(ALGO_FAILED);
      return -1;
    }
    const TopoDS_Shape aResult = aMakeRevol.Shape();
    BRepCheck_Analyzer aCheckAnalyzer(aResult);
    if (!aCheckAnalyzer.IsValid(aResult)) {
      aFunction->SetFailure(RESULT_NOT_VALID);
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
      LoadNamingDS(RESPOSITION(aFunction), aMakeRevol, aBASIS, aContextOfBasis->Get());
    else
      LoadNamingDS(RESPOSITION(aFunction), aMakeRevol, aBASIS, aBASIS);

  } else if(aFunction->GetDriverGUID() == SECREVOL_GUID) {
    Standard_Real anANGLE = DNaming::GetReal(aFunction,REVOL_ANGLE)->Get();
    if(anANGLE <= Precision::Confusion()) {
      aFunction->SetFailure(WRONG_ARGUMENT);
      return -1;
    }
    // Reverse
    Standard_Integer aRev = DNaming::GetInteger(aFunction,REVOL_REV)->Get();
    if(aRev) anAXIS.Reverse();

    BRepPrimAPI_MakeRevol aMakeRevol(aBASIS, anAXIS, anANGLE, Standard_True);
    aMakeRevol.Build();
    if (!aMakeRevol.IsDone()) {
      aFunction->SetFailure(ALGO_FAILED);
      return -1;
    }
    const TopoDS_Shape aResult = aMakeRevol.Shape();
    BRepCheck_Analyzer aCheckAnalyzer(aResult);
    if (!aCheckAnalyzer.IsValid(aResult)) {
      aFunction->SetFailure(RESULT_NOT_VALID);
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
      LoadNamingDS(RESPOSITION(aFunction), aMakeRevol, aBASIS, aContextOfBasis->Get());
    else
      LoadNamingDS(RESPOSITION(aFunction), aMakeRevol, aBASIS, aBASIS);

  } else {
    aFunction->SetFailure(UNSUPPORTED_FUNCTION);
    return -1;
  }

  // restore location
  if(!aLocation.IsIdentity())
    TNaming::Displace(RESPOSITION(aFunction), aLocation, Standard_True);

  theLog->SetValid(RESPOSITION(aFunction),Standard_True);
  aFunction->SetFailure(DONE);
  return 0;
}

//=======================================================================
static void LoadSeamEdge (BRepPrimAPI_MakeRevol&        mkRevol,
			  const TDF_Label&              ResultLabel,
			  const TopoDS_Shape&           ShapeIn)

{
  TopTools_MapOfShape View;
  TopExp_Explorer ShapeExplorer (ShapeIn, TopAbs_EDGE);
  Standard_Boolean isFound(Standard_False);
  for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
    const TopoDS_Shape& Root = ShapeExplorer.Current ();
    if (!View.Add(Root)) continue;
    const TopTools_ListOfShape& Shapes = mkRevol.Generated (Root);
    TopTools_ListIteratorOfListOfShape ShapesIterator (Shapes);
    for (;ShapesIterator.More (); ShapesIterator.Next ()) {
      TopoDS_Shape newShape = ShapesIterator.Value ();
      if(newShape.ShapeType() != TopAbs_FACE)
	continue;
      if (!Root.IsSame (newShape)) {
	TopExp_Explorer exp(newShape, TopAbs_EDGE);
	for (;exp.More();exp.Next()) {
	  if(BRep_Tool::IsClosed (TopoDS::Edge(exp.Current()), TopoDS::Face(newShape))) {
	    TNaming_Builder Builder(ResultLabel.NewChild());
	    Builder.Generated (exp.Current());
	    isFound = Standard_True;
	    break;
	  }
	}
	if(isFound) {isFound = Standard_False;break;}
      }
    }
  }
}

//=======================================================================
static Standard_Boolean HasDangle(const TopoDS_Shape& ShapeIn)
{
  if (ShapeIn.ShapeType() == TopAbs_SOLID)
    return Standard_False;
  else if (ShapeIn.ShapeType() == TopAbs_SHELL) {
    Handle(BRepCheck_Shell) aCheck = new BRepCheck_Shell(TopoDS::Shell(ShapeIn));
    return aCheck->Closed() != BRepCheck_NoError;
  } else if (ShapeIn.ShapeType() == TopAbs_FACE ||
             ShapeIn.ShapeType() == TopAbs_WIRE ||
             ShapeIn.ShapeType() == TopAbs_EDGE ||
             ShapeIn.ShapeType() == TopAbs_VERTEX)
    return Standard_True;
  return Standard_False;
}
//=======================================================================
static void BuildAtomicMap(const TopoDS_Shape& S, TopTools_MapOfShape& M)
{
  if(S.ShapeType() == TopAbs_COMPOUND || S.ShapeType() == TopAbs_COMPSOLID) {
    TopoDS_Iterator it(S);
    for(;it.More();it.Next()) {
      if(it.Value().ShapeType() > TopAbs_COMPSOLID)
        M.Add(it.Value());
      else
        BuildAtomicMap(it.Value(), M);
    }
  } else
    M.Add(S);
}
//=======================================================================
//function : HasDangleShapes
//purpose  :
//=======================================================================

Standard_Boolean HasDangleShapes(const TopoDS_Shape& ShapeIn) {
  if (ShapeIn.ShapeType() == TopAbs_COMPOUND || ShapeIn.ShapeType() == TopAbs_COMPSOLID) {
    TopTools_MapOfShape M;
    BuildAtomicMap(ShapeIn, M);
    TopTools_MapIteratorOfMapOfShape it(M);
    for (; it.More(); it.Next())
      if (HasDangle(it.Key())) return Standard_True;
  } else
    return HasDangle(ShapeIn);
  return Standard_False;
}
//=======================================================================
//function : LoadAndName
//purpose  :
//=======================================================================
void DNaming_RevolutionDriver::LoadNamingDS (const TDF_Label& theResultLabel,
					BRepPrimAPI_MakeRevol& MS,
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

 // is full
  TopoDS_Shape StartShape = MS.FirstShape();
  TopoDS_Shape EndShape = MS.LastShape();
  Standard_Boolean isFull(Standard_False);
  if (!StartShape.IsNull() && !EndShape.IsNull())
    isFull = StartShape.IsEqual(EndShape);

  Standard_Boolean hasDangle = HasDangleShapes(MS.Shape());
  Standard_Boolean isBasisClosed(Standard_True);
  TopoDS_Vertex Vfirst, Vlast;
  if(Basis.ShapeType() == TopAbs_WIRE) {
    Handle(BRepCheck_Wire) aCheck = new BRepCheck_Wire(TopoDS::Wire(Basis));
    if(aCheck->Closed() != BRepCheck_NoError) {
      isBasisClosed = Standard_False; //open
      TopExp::Vertices(TopoDS::Wire(Basis), Vfirst, Vlast);
    }
  } else if(Basis.ShapeType() == TopAbs_EDGE) {
    BRepBuilderAPI_MakeWire aMakeWire;
    aMakeWire.Add(TopoDS::Edge(Basis));
    if(aMakeWire.IsDone()) {
      Handle(BRepCheck_Wire) aCheck = new BRepCheck_Wire(aMakeWire.Wire());
      if(aCheck->Closed() != BRepCheck_NoError) {//check for circle case
	isBasisClosed = Standard_False; //open
	TopExp::Vertices(TopoDS::Edge(Basis), Vfirst, Vlast);
      }
    }
  }
  if(isFull) {
    //seam edge
    LoadSeamEdge(MS, theResultLabel, Basis);

    if(hasDangle) {
      if(!isBasisClosed) {
      //dangle edges
	const TopTools_ListOfShape& Shapes = MS.Generated (Vfirst);
	TopTools_ListIteratorOfListOfShape it (Shapes);
	for (;it.More (); it.Next ()) {
	  if(!BRep_Tool::Degenerated(TopoDS::Edge(it.Value()))) {
	    TNaming_Builder aBuilder(theResultLabel.NewChild());
	    aBuilder.Generated (Vfirst, it.Value());
	  }
#ifdef OCCT_DEBUG
	  else {
	    if(MS.HasDegenerated())
	      cout <<"mkRevol has degenerated" <<endl;
	    cout << "BRep_Tool found degenerated edge (from Vfirst) TS = " << it.Value().TShape().get() <<endl;
	  }
#endif
	}


	const TopTools_ListOfShape& Shapes2 = MS.Generated (Vlast);
	it.Initialize(Shapes2);
	for (;it.More (); it.Next ()) {
	  if(!BRep_Tool::Degenerated(TopoDS::Edge(it.Value()))) {
	    TNaming_Builder aBuilder(theResultLabel.NewChild());
	    aBuilder.Generated (Vlast, it.Value());
	  }
#ifdef OCCT_DEBUG
	  else {
	    if(MS.HasDegenerated())
	      cout <<"mkRevol has degenerated" <<endl;
	    cout << "BRep_Tool found degenerated edge (from Vlast) TS = " << it.Value().TShape().get() <<endl;
	  }
#endif
	}
      }
    }
  } else {//if(!isFull)
    //Insert start shape
    if (!StartShape.IsNull()) {
      if (StartShape.ShapeType() != TopAbs_COMPOUND) {
	TNaming_Builder StartBuilder(theResultLabel.NewChild());
	if (SubShapes.IsBound(StartShape)) {
	  StartShape = SubShapes(StartShape);
	}
	StartBuilder.Generated(StartShape);
	if(StartShape.ShapeType() != TopAbs_FACE) {
	  TopoDS_Iterator it(StartShape);
	  for (; it.More(); it.Next()) {
	    TNaming_Builder aBuilder(theResultLabel.NewChild());
	    aBuilder.Generated(it.Value());
	  }
	}
      } else {
	TopoDS_Iterator itr(StartShape);
	for (; itr.More(); itr.Next()) {
	  TNaming_Builder StartBuilder(theResultLabel.NewChild());
	  StartBuilder.Generated(itr.Value());
	}
      }
    }


  //Insert end shape
    if (!EndShape.IsNull()) {
      if (EndShape.ShapeType() != TopAbs_COMPOUND) {
	TNaming_Builder EndBuilder(theResultLabel.NewChild());
	if (SubShapes.IsBound(EndShape)) {
	  EndShape = SubShapes(EndShape);
	}
	EndBuilder.Generated(EndShape);
	if(EndShape.ShapeType() != TopAbs_FACE) {
	  TopoDS_Iterator it(EndShape);
	  for (; it.More(); it.Next()) {
	    TNaming_Builder aBuilder(theResultLabel.NewChild());
	    aBuilder.Generated(it.Value());
	  }
	}
      } else {
	TopoDS_Iterator itr(EndShape);
	for (; itr.More(); itr.Next()) {
	  TNaming_Builder EndBuilder(theResultLabel.NewChild());
	  EndBuilder.Generated(itr.Value());
	}
      }
    }
    if(hasDangle) {
      if(!isBasisClosed) {
      //dangle edges
	const TopTools_ListOfShape& Shapes = MS.Generated (Vfirst);
	TopTools_ListIteratorOfListOfShape it (Shapes);
	for (;it.More (); it.Next ()) {
	  if(!BRep_Tool::Degenerated(TopoDS::Edge(it.Value()))) {
	    TNaming_Builder aBuilder(theResultLabel.NewChild());
	    aBuilder.Generated (Vfirst, it.Value());
	  }
#ifdef OCCT_DEBUG
	  else {
	    if(MS.HasDegenerated())
	      cout <<"mkRevol has degenerated" <<endl;
	    cout << "BRep_Tool found degenerated edge (from Vfirst) TS = " << it.Value().TShape().get() <<endl;
	  }
#endif
	}

	const TopTools_ListOfShape& Shapes2 = MS.Generated (Vlast);
	it.Initialize(Shapes2);
	for (;it.More (); it.Next ()) {
	  if(!BRep_Tool::Degenerated(TopoDS::Edge(it.Value()))) {
	    TNaming_Builder aBuilder(theResultLabel.NewChild());
	    aBuilder.Generated (Vlast, it.Value());
	  }
#ifdef OCCT_DEBUG
	  else {
	    if(MS.HasDegenerated())
	      cout <<"mkRevol has degenerated" <<endl;
	    cout << "BRep_Tool found degenerated edge (from Vlast) TS = " << it.Value().TShape().get() <<endl;
	  }
#endif
	}
      }
    }
  }
}

