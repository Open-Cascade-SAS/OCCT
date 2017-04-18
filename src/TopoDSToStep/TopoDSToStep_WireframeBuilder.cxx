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

//:S4134 abv 10.03.99: working methods moved from package TopoDSToGBWire
//:j1 modified by abv 22 Oct 98: CSR BUC60401
// - unused parts of code dropped
// - fixed trimming of circles and ellipses (radians used instead of degrees)
//szv#4 S4163

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepTools.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomToStep_MakeCartesianPoint.hxx>
#include <GeomToStep_MakeCurve.hxx>
#include <GeomToStep_MakeLine.hxx>
#include <gp_Vec.hxx>
#include <MoniTool_DataMapOfShapeTransient.hxx>
#include <Precision.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_NullObject.hxx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_HArray1OfTrimmingSelect.hxx>
#include <StepGeom_Line.hxx>
#include <StepGeom_SeamCurve.hxx>
#include <StepGeom_SurfaceCurve.hxx>
#include <StepGeom_TrimmedCurve.hxx>
#include <StepGeom_TrimmingSelect.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_SequenceOfTransient.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDSToStep_Tool.hxx>
#include <TopoDSToStep_WireframeBuilder.hxx>
#include <Transfer_FinderProcess.hxx>
#include <TransferBRep_ShapeMapper.hxx>

// ============================================================================
// Method  : TopoDSToStep_Builder::TopoDSToStep_Builder
// Purpose :
// ============================================================================
TopoDSToStep_WireframeBuilder::TopoDSToStep_WireframeBuilder()
{
  done = Standard_False;
}

// ============================================================================
// Method  : TopoDSToStep_Builder::TopoDSToStep_Builder
// Purpose :
// ============================================================================

 TopoDSToStep_WireframeBuilder::TopoDSToStep_WireframeBuilder(const TopoDS_Shape& aShape, TopoDSToStep_Tool& aTool, const Handle(Transfer_FinderProcess)& FP)
{
  done = Standard_False;
  Init(aShape, aTool, FP);
}

void TopoDSToStep_WireframeBuilder::Init(const TopoDS_Shape& aShape, TopoDSToStep_Tool& /* T */, const Handle(Transfer_FinderProcess)& /* FP */)
{
  Handle(TColStd_HSequenceOfTransient) itemList =
    new TColStd_HSequenceOfTransient();
  MoniTool_DataMapOfShapeTransient aPmsMap;
  done = GetTrimmedCurveFromShape(aShape, aPmsMap, itemList);
  myResult = itemList;
}

// ============================================================================
// Method  : TopoDSToStep_Builder::Error
// Purpose :
// ============================================================================

TopoDSToStep_BuilderError TopoDSToStep_WireframeBuilder::Error() const 
{
	return myError;
}

// ============================================================================
// Method  : TopoDSToStep_Builder::Value
// Purpose :
// ============================================================================

const Handle(TColStd_HSequenceOfTransient)& TopoDSToStep_WireframeBuilder::Value() const 
{
  StdFail_NotDone_Raise_if (!done, "TopoDSToStep_WireframeBuilder::Value() - no result");
  return myResult;
}


// ============================================================================
//:S4134: abv 10 Mar 99: the methods below moved from package TopoDSToGBWire

#define Nbpt 23

static Handle(StepGeom_TrimmedCurve) MakeTrimmedCurve (const Handle(StepGeom_Curve) &C,
						       const Handle(StepGeom_CartesianPoint) P1, 
						       const Handle(StepGeom_CartesianPoint) P2, 
						       Standard_Real trim1,
						       Standard_Real trim2,
						       Standard_Boolean sense)
{
  Handle(StepGeom_HArray1OfTrimmingSelect) aSTS1 =
    new StepGeom_HArray1OfTrimmingSelect(1,2);
  StepGeom_TrimmingSelect tSel;
  tSel.SetValue(P1);
  aSTS1->SetValue(1,tSel);
  tSel.SetParameterValue(trim1);
  aSTS1->SetValue(2,tSel);
    
  Handle(StepGeom_HArray1OfTrimmingSelect) aSTS2 =
    new StepGeom_HArray1OfTrimmingSelect(1,2);
  tSel.SetValue(P2);
  aSTS2->SetValue(1,tSel);
  tSel.SetParameterValue(trim2);
  aSTS2->SetValue(2,tSel);
    
  Handle(TCollection_HAsciiString) empty = 
      new TCollection_HAsciiString("");
  Handle(StepGeom_TrimmedCurve) pmsTC = new StepGeom_TrimmedCurve;
  pmsTC->Init(empty,C,aSTS1,aSTS2,sense,StepGeom_tpParameter);
  return pmsTC;
}
  
Standard_Boolean TopoDSToStep_WireframeBuilder::
  GetTrimmedCurveFromEdge(const TopoDS_Edge& theEdge, 
			  const TopoDS_Face& aFace, 
			  MoniTool_DataMapOfShapeTransient& aMap, 
			  Handle(TColStd_HSequenceOfTransient)& curveList) const
{
  if (theEdge.Orientation() == TopAbs_INTERNAL  ||
      theEdge.Orientation() == TopAbs_EXTERNAL ) {
#ifdef OCCT_DEBUG
    cout <<"Warning: TopoDSToStep_WireframeBuilder::GetTrimmedCurveFromEdge: Edge is internal or external; dropped" << endl;
#endif
    return Standard_False;
  }
  //szv#4:S4163:12Mar99 SGI warns
  TopoDS_Shape sh = theEdge.Oriented(TopAbs_FORWARD);
  TopoDS_Edge anEdge = TopoDS::Edge ( sh );

  // resulting curve
  Handle(StepGeom_Curve) Gpms;
  
  if ( aMap.IsBound(anEdge)) {
    Gpms = Handle(StepGeom_Curve)::DownCast ( aMap.Find(anEdge) );
    if ( Gpms.IsNull() ) return Standard_False;
//??    curveList->Append(Gpms);
    return Standard_True;
  }

  BRepAdaptor_Curve CA;
  try {
    OCC_CATCH_SIGNALS
    CA.Initialize (anEdge);
  }
  catch (Standard_NullObject) {
    return Standard_False;
  }

  // Vertices
  TopoDS_Vertex Vfirst, Vlast;
  Handle(StepGeom_CartesianPoint) pmsP1, pmsP2; 
  for (TopoDS_Iterator It(anEdge);It.More();It.Next()) {
    // Translates the Edge Vertices
    TopoDS_Vertex V = TopoDS::Vertex(It.Value());
    gp_Pnt gpP = BRep_Tool::Pnt(V);
    if ( V.Orientation() == TopAbs_FORWARD ) {
      Vfirst = V;
      // 1.point for trimming
      GeomToStep_MakeCartesianPoint gtpP(gpP);
      pmsP1 = gtpP.Value();
    }
    if ( V.Orientation() == TopAbs_REVERSED ) {
      Vlast = V;
      // 2.point for trimming
      GeomToStep_MakeCartesianPoint gtpP(gpP);
      pmsP2 = gtpP.Value();
    }
  }
    
  // ---------------------------------------
  // Translate 3D representation of the Edge
  // ---------------------------------------
    
    
  // Handle(Geom_Curve) C = CA.Curve().Curve();

  // UPDATE FMA 26-02-96 
  // General remark : this full code should be deaply reworked
  //                  Too many objects are not used !

  Standard_Real First, Last;
  Handle(Geom_Curve) C = BRep_Tool::Curve(anEdge, First, Last); 
  if ( ! C.IsNull() ) {
    if (C->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
      C = Handle(Geom_TrimmedCurve)::DownCast(C)->BasisCurve();
    GeomToStep_MakeCurve  gtpC(C);

    if(!gtpC.IsDone())
      return Standard_False;

    Handle(StepGeom_Curve) pmsC = gtpC.Value();

    // trim the curve
    Standard_Real trim1 = CA.FirstParameter();
    Standard_Real trim2 = CA.LastParameter();
/* //:j1 abv 22 Oct 98: radians are used in the produced STEP file (at least by default)
   if(C->IsKind(STANDARD_TYPE(Geom_Circle)) ||
       C->IsKind(STANDARD_TYPE(Geom_Ellipse))) {
      Standard_Real fact = 180. / M_PI;
      trim1 = trim1 * fact;
      trim2 = trim2 * fact;
    }
*/
    Gpms = MakeTrimmedCurve (pmsC, pmsP1, pmsP2, trim1, trim2, Standard_True );
//			     (anEdge.Orientation() == TopAbs_FORWARD));
  }
  else {

    // -------------------------
    // a 3D Curve is constructed
    // -------------------------

    Standard_Boolean iaplan = Standard_False;
    if ( ! aFace.IsNull() ) {
      Standard_Real cf, cl;
      Handle(Geom2d_Curve) C2d = BRep_Tool::CurveOnSurface(anEdge, aFace, cf, cl);
      Handle(Geom_Surface) S = BRep_Tool::Surface(aFace);
      if (S->IsKind(STANDARD_TYPE(Geom_Plane)) &&
	  C2d->IsKind(STANDARD_TYPE(Geom2d_Line))) iaplan = Standard_True;
    }

    // to be modified : cf and cl are the topological trimming prameter
    // these are computed after ! (U1 and U2) -> cf and cl instead
    if (iaplan) {
      gp_Pnt Pnt1 = CA.Value(CA.FirstParameter()), Pnt2 = CA.Value(CA.LastParameter());
      gp_Vec V ( Pnt1, Pnt2 );
      Standard_Real length = V.Magnitude();
      if ( length >= Precision::Confusion() ) {
	Handle(Geom_Line) L = new Geom_Line(Pnt1, gp_Dir(V));
	GeomToStep_MakeLine gtpL(L);
	Gpms = gtpL.Value();
	Gpms = MakeTrimmedCurve (gtpL.Value(), pmsP1, pmsP2, 0, length, Standard_True );
//				 (anEdge.Orientation() == TopAbs_FORWARD));
      }
#ifdef OCCT_DEBUG
      else cout << "Warning: TopoDSToStep_WireframeBuilder::GetTrimmedCurveFromEdge: Null-length curve not mapped" << endl;
#endif
    }
    else {
      TColgp_Array1OfPnt Points(1,Nbpt);
      TColStd_Array1OfReal Knots(1,Nbpt);
      TColStd_Array1OfInteger Mult(1,Nbpt);
      Standard_Real U1 = CA.FirstParameter();
      Standard_Real U2 = CA.LastParameter();
      for ( Standard_Integer i=1; i<=Nbpt; i++ ) {
	Standard_Real U = U1 + (i-1)*(U2 - U1)/(Nbpt - 1);
	gp_Pnt P = CA.Value(U);
	Points.SetValue(i,P);
	Knots.SetValue(i,U);
	Mult.SetValue(i,1);
      }
      Points.SetValue(1, BRep_Tool::Pnt(Vfirst));
      Points.SetValue(Nbpt, BRep_Tool::Pnt(Vlast));
      Mult.SetValue(1,2);
      Mult.SetValue(Nbpt,2);
      Handle(Geom_Curve) Bs = 
	new Geom_BSplineCurve(Points, Knots, Mult, 1);
      GeomToStep_MakeCurve gtpC(Bs);
      Gpms = gtpC.Value();
    }
  }
  if( Gpms.IsNull() ) return Standard_False;

  aMap.Bind(anEdge, Gpms);
  curveList->Append(Gpms);
  return Standard_True;
}


Standard_Boolean TopoDSToStep_WireframeBuilder::
  GetTrimmedCurveFromFace(const TopoDS_Face& aFace, 
			  MoniTool_DataMapOfShapeTransient& aMap, 
			  Handle(TColStd_HSequenceOfTransient)& aCurveList) const
{
  TopoDS_Shape curShape;
  TopoDS_Edge  curEdge;
  TopExp_Explorer  exp;
  Standard_Boolean result = Standard_False; //szv#4:S4163:12Mar99 `done` hid one from this, initialisation needed

  for (exp.Init(aFace,TopAbs_EDGE); exp.More(); exp.Next()){
    curShape = exp.Current();
    curEdge  = TopoDS::Edge(curShape);
    if (GetTrimmedCurveFromEdge(curEdge, aFace, aMap, aCurveList)) result = Standard_True;
  }
  return result;
}

Standard_Boolean TopoDSToStep_WireframeBuilder::
  GetTrimmedCurveFromShape(const TopoDS_Shape& aShape, 
			   MoniTool_DataMapOfShapeTransient& aMap,  
			   Handle(TColStd_HSequenceOfTransient)& aCurveList) const
{
  TopoDS_Iterator  It;
  Standard_Boolean result = Standard_False; //szv#4:S4163:12Mar99 `done` hid one from this, initialisation needed

  //szv#4:S4163:12Mar99 optimized
  switch (aShape.ShapeType()) {
    case TopAbs_EDGE : {
      const TopoDS_Edge& curEdge = TopoDS::Edge(aShape);
      TopoDS_Face nulFace;
      result = GetTrimmedCurveFromEdge(curEdge, nulFace, aMap, aCurveList);
      break;
    }
    case TopAbs_WIRE : {
      TopoDS_Face nulFace;
      TopoDS_Shape curShape;
      TopoDS_Edge  curEdge;
      TopExp_Explorer  exp;

      for (exp.Init(aShape,TopAbs_EDGE); exp.More(); exp.Next()){
	curShape = exp.Current();
	curEdge  = TopoDS::Edge(curShape);
	if (GetTrimmedCurveFromEdge(curEdge, nulFace, aMap, aCurveList)) result = Standard_True;
      }
      break;
    }
    case TopAbs_FACE : {
      const TopoDS_Face& curFace = TopoDS::Face(aShape);
      result = GetTrimmedCurveFromFace(curFace, aMap, aCurveList);
      break;
    }
    case TopAbs_SHELL : {
      TopoDS_Shell Sh = TopoDS::Shell(aShape);	  
      It.Initialize(Sh);
      for (;It.More();It.Next()) {
	TopoDS_Face curFace = TopoDS::Face(It.Value());
	if (GetTrimmedCurveFromFace(curFace, aMap, aCurveList)) result = Standard_True;
#ifdef OCCT_DEBUG
	if(!result) {
	  cout << "ERROR extracting trimmedCurve from Face" << endl;
	  //BRepTools::Dump(curFace,cout);  cout<<endl;
	}
#endif
      }
      break;
    }
    case TopAbs_SOLID : {
      It.Initialize(aShape);
      for (;It.More();It.Next()) {
	if  (It.Value().ShapeType() == TopAbs_SHELL) {
	  if (GetTrimmedCurveFromShape(It.Value(), aMap, aCurveList)) result = Standard_True;
	}
      } 
      break;
    }
    case TopAbs_COMPOUND : {
      It.Initialize(aShape);
      for (;It.More();It.Next()) {
/*	  if  ((It.Value().ShapeType() == TopAbs_SHELL) ||
	       (It.Value().ShapeType() == TopAbs_COMPOUND)) {
	    result = GetTrimmedCurveFromShape(It.Value(), aMap, aCurveList);
	    break;
	  }
	  else if (It.Value().ShapeType() == TopAbs_FACE) {
	    result = GetTrimmedCurveFromFace(TopoDS::Face(It.Value()), aMap, aCurveList);
	    break;
	  } */
	if (GetTrimmedCurveFromShape(It.Value(), aMap, aCurveList)) result = Standard_True;
      }
      break;
    }
    default : break;
  }
  return result;
}
