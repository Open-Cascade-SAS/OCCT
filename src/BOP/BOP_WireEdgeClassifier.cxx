// File:	BOP_WireEdgeClassifier.cxx
// Created:	Thu Jun 17 18:52:50 1993
// Author:	Jean Yves LEBEY
//		<jyl@zerox>
//     modified by PKV
// ... and nobody played synthesizer .

//  Modified by skv - Thu Jul  6 12:01:17 2006 OCC12627

#include <BOP_WireEdgeClassifier.ixx>

#include <Standard_ProgramError.hxx>

#include <Precision.hxx>

#include <gp_Pnt.hxx>
#include <gp_Vec2d.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>

#include <GeomAdaptor_Surface.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>

#include <TopLoc_Location.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <BRep_Tool.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve2d.hxx>

#include <IntTools_Tools.hxx>

#include <BOPTools_Tools2D.hxx>
#include <BOPTools_Tools3D.hxx>
#include <IntTools_FClass2d.hxx>



static
  Standard_Real MinStepIn2d(const TopoDS_Edge& aE,
			    const TopoDS_Face& aF);

//=======================================================================
//function : BOP_WireEdgeClassifier::BOP_WireEdgeClassifier
//purpose  : 
//=======================================================================
  BOP_WireEdgeClassifier::BOP_WireEdgeClassifier (const TopoDS_Face& F,
						  const BOP_BlockBuilder& BB)
:
  BOP_CompositeClassifier(BB)
{
  myBCEdge.Face() = F;
}

//=======================================================================
//function : Compare
//purpose  : 
//=======================================================================
  TopAbs_State BOP_WireEdgeClassifier::Compare (const Handle(BOP_Loop)& L1,
						const Handle(BOP_Loop)& L2)
{ 
  TopAbs_State state = TopAbs_UNKNOWN;

  Standard_Boolean isshape1, isshape2, yena1 ;

  isshape1 = L1->IsShape();
  isshape2 = L2->IsShape();

  if      ( isshape2 && isshape1 )  { 
    // L1 is Shape , L2 is Shape
    const TopoDS_Shape& s1 = L1->Shape();
    const TopoDS_Shape& s2 = L2->Shape();
    state = CompareShapes(s1,s2);
  }

  else if ( isshape2 && !isshape1 ) { 
    // L1 is Block , L2 is Shape
    BOP_BlockIterator Bit1 = L1->BlockIterator();
    Bit1.Initialize();
    yena1 = Bit1.More();
    while (yena1) {
      const TopoDS_Shape& s1 = 
	((BOP_BlockBuilder*)myBlockBuilder)->Element(Bit1);
      const TopoDS_Shape& s2 = L2->Shape();
      state = CompareElementToShape(s1,s2);
      yena1 = Standard_False;
      if (state == TopAbs_UNKNOWN) { 
	if (Bit1.More()) Bit1.Next();
	yena1 = Bit1.More();
      }
    }
  }
  
  else if ( !isshape2 && isshape1 ) { 
    // L1 is Shape , L2 is Block
    const TopoDS_Shape& s1 = L1->Shape();
    ResetShape(s1);
    BOP_BlockIterator Bit2 = L2->BlockIterator();
    for (Bit2.Initialize(); Bit2.More(); Bit2.Next()) {
      const TopoDS_Shape& s2 = 
	((BOP_BlockBuilder*)myBlockBuilder)->Element(Bit2);
      CompareElement(s2);
    }
    state = State();
  }
  else if ( !isshape2 && !isshape1 ) { 
    // L1 is Block , L2 is Block
    if (state == TopAbs_UNKNOWN) {      
      BOP_BlockIterator Bit1 = L1->BlockIterator();
      Bit1.Initialize();
      yena1 = Bit1.More();
      while (yena1) {
	const TopoDS_Shape& s1 = 
	  ((BOP_BlockBuilder*)myBlockBuilder)->Element(Bit1);
	ResetElement(s1);
	BOP_BlockIterator Bit2 = L2->BlockIterator();
	for (Bit2.Initialize(); Bit2.More(); Bit2.Next()) {
	  const TopoDS_Shape& s2 = 
	    ((BOP_BlockBuilder*)myBlockBuilder)->Element(Bit2);
	  CompareElement(s2);
	}
	state = State();
	yena1 = Standard_False;
	if (state == TopAbs_UNKNOWN) { 
	  if (Bit1.More()) Bit1.Next();
	  yena1 = Bit1.More();
	}
      }
    } // if (state == TopAbs_UNKNOWN)

    if (state == TopAbs_UNKNOWN) {
      //
      TopoDS_Shape s1 = LoopToShape(L1); 
      if (s1.IsNull()) {
	return state;
      }
      TopoDS_Shape s2 = LoopToShape(L2); 
      if (s2.IsNull()) {
	return state;
      }
    } // if (state == TopAbs_UNKNOWN)

  } // else if ( !isshape2 && !isshape1 )
  return state;
}

//=======================================================================
//function : LoopToShape
//purpose  : 
//=======================================================================
  TopoDS_Shape BOP_WireEdgeClassifier::LoopToShape(const Handle(BOP_Loop)& L)
{
  myShape.Nullify();

  BOP_BlockIterator Bit = L->BlockIterator();
  Bit.Initialize();
  if ( !Bit.More() ) {
    return myShape;
  }

  TopoDS_Shape aLocalShape = myBCEdge.Face();
  const TopoDS_Face& F1 = TopoDS::Face(aLocalShape);
  // 
  aLocalShape = F1.EmptyCopied();
  TopoDS_Face F = TopoDS::Face(aLocalShape);
  //
  BRep_Builder BB; 
  TopoDS_Wire W; 
  BB.MakeWire(W);

  Standard_Real tolE, f,l,tolpc, tol;
  Standard_Boolean haspc ;

  for  (; Bit.More(); Bit.Next()) {
    const TopoDS_Edge& E = 
      TopoDS::Edge(((BOP_BlockBuilder*)myBlockBuilder)->Element(Bit));
    
    tolE = BRep_Tool::Tolerance(E);
    
    haspc = BOPTools_Tools2D::HasCurveOnSurface (E, F);
    if (!haspc) {
      Handle(Geom2d_Curve) C2D;
      BOPTools_Tools2D::CurveOnSurface (E, F, C2D, f, l, tolpc, Standard_False);

      if (!C2D.IsNull()) {
	tol = Max(tolpc,tolE);
	BB.UpdateEdge(E,C2D,F,tol);
      }
    }
    BB.Add(W,E);
  }
  BB.Add(F, W);

  myShape = F;
  return myShape;
}

//=======================================================================
//function : CompareShapes
//purpose  : 
//=======================================================================
  TopAbs_State  BOP_WireEdgeClassifier::CompareShapes (const TopoDS_Shape& B1, 
						       const TopoDS_Shape& B2)
{
  Standard_Boolean bBothWires;
  TopAbs_State aSt;
  TopExp_Explorer anExp1;
  
  aSt = TopAbs_UNKNOWN;
  bBothWires= (B2.ShapeType()==TopAbs_WIRE) && (B1.ShapeType()==TopAbs_WIRE);

  anExp1.Init (B1, TopAbs_EDGE);
  if ( !anExp1.More() ) {
    return aSt;
  }
  //
  for (; anExp1.More(); anExp1.Next() ) {
    const TopoDS_Edge& anE1 = TopoDS::Edge(anExp1.Current());
    ResetShape(anE1);
    //
    TopExp_Explorer anExp2(B2, TopAbs_EDGE);
    for(; anExp2.More(); anExp2.Next()) {
      const TopoDS_Edge& anE2 = TopoDS::Edge(anExp2.Current());
      //
      if(anE2.IsSame(anE1)) {
	//const TopoDS_Face& aF=myBCEdge.Face();
	if (bBothWires) {
	  aSt = TopAbs_OUT;
	  return aSt;
	}
      }
    } 
  }
  //
  TopoDS_Face aFN1, aFN2;
  gp_Pnt2d aP2DNear;
  gp_Pnt   aPNear;
  
  const TopoDS_Face& theFace =  myBCEdge.Face();
  const TopoDS_Wire& aW1=TopoDS::Wire(B1);
  const TopoDS_Wire& aW2=TopoDS::Wire(B2);
 
  IntTools_Tools::MakeFaceFromWireAndFace (aW1, theFace, aFN1);  
  IntTools_Tools::MakeFaceFromWireAndFace (aW2, theFace, aFN2);  
  
  anExp1.Init (aFN1, TopAbs_EDGE);
  for (; anExp1.More(); anExp1.Next()){
    const TopoDS_Edge& aE1=TopoDS::Edge(anExp1.Current());
    //
    // The block that choises step in 2D taking into account
    // tolerances of the Face and Edge on the Face in order to
    // obtain 2D point exactly INside of the Face
    //                             pkv207/A5
    //                             PKV Thu Jun 13 09:09:53 2002
    {
      Standard_Real aT, aT1, aT2, dt2D;

      BRep_Tool::Range(aE1, aT1, aT2);
      aT=BOPTools_Tools2D::IntermediatePoint(aT1, aT2);
      //
      TopoDS_Face aFF=aFN1;
      TopoDS_Edge aERight;
      
      aFF.Orientation(TopAbs_FORWARD);
      BOPTools_Tools3D::OrientEdgeOnFace (aE1, aFF, aERight);
      
      dt2D=MinStepIn2d(aERight, aFF);
      BOPTools_Tools3D::PointNearEdge (aERight, aFF, aT, dt2D, aP2DNear, aPNear);
    }
    
    //
//  Modified by skv - Thu Jul  6 12:01:17 2006 OCC12627 Begin
//     aSt=IntTools_Tools::ClassifyPointByFace (aFN2, aP2DNear);
    Standard_Real     aTol = Precision::PConfusion();
    IntTools_FClass2d aClass2d(aFN2, aTol);

    aSt = aClass2d.Perform(aP2DNear);
//  Modified by skv - Thu Jul  6 12:01:19 2006 OCC12627 End
    
    return aSt;
  }
  return aSt;
}


//=======================================================================
//function : CompareElementToShape
//purpose  : 
//=======================================================================
  TopAbs_State  BOP_WireEdgeClassifier::CompareElementToShape (const TopoDS_Shape& EE,
							       const TopoDS_Shape& B)
{
  // isEdge : edge E inits myPoint2d 
  ResetElement(EE);
  TopExp_Explorer Ex;
  Ex.Init(B,TopAbs_EDGE);
  for(; Ex.More(); Ex.Next()) {
    const TopoDS_Shape& E = Ex.Current();
    CompareElement(E);
  }
  TopAbs_State state = State();
  return state;
}

//=======================================================================
//function : ResetShape
//purpose  : 
//=======================================================================
  void  BOP_WireEdgeClassifier::ResetShape(const TopoDS_Shape& B)
{
  if (B.ShapeType() == TopAbs_EDGE) {
    ResetElement(B);
  }

  else {
    TopExp_Explorer ex(B,TopAbs_EDGE);
    if (ex.More())  {
      const TopoDS_Shape& E = ex.Current();
      ResetElement(E);
    }
  }
}
//=======================================================================
//function : ResetElement
//purpose  : 
//=======================================================================
  void  BOP_WireEdgeClassifier::ResetElement(const TopoDS_Shape& EE)
{
  const TopoDS_Edge& E = TopoDS::Edge(EE);
  const TopoDS_Face& F = myBCEdge.Face();

  Standard_Boolean haspc;
  Standard_Real f2, l2, tolpc, tolE, tol, par ;
  Handle(Geom2d_Curve) C2D;
  BRep_Builder BB; 

  haspc = BOPTools_Tools2D::HasCurveOnSurface(E, F); 
  if (!haspc) { 
    BOPTools_Tools2D::CurveOnSurface(E, F, C2D, f2, l2, tolpc, Standard_True);
    tolE = BRep_Tool::Tolerance(E); 
    tol = Max(tolE,tolpc); 
    BB.UpdateEdge(E,C2D,F,tol); 
  } 
  
  
  BOPTools_Tools2D::CurveOnSurface(E, F, C2D, f2, l2, tolpc, Standard_False);
  if (C2D.IsNull()) {
    Standard_ProgramError::Raise("WEC : ResetElement");
  }

  par = BOPTools_Tools2D::IntermediatePoint (f2, l2);
  myPoint2d = C2D->Value(par);
  
  myFirstCompare = Standard_True;
}


//=======================================================================
//function : CompareElement
//purpose  : 
//=======================================================================
  void BOP_WireEdgeClassifier::CompareElement(const TopoDS_Shape& EE)
{
  const TopoDS_Edge& E = TopoDS::Edge(EE);
  const TopoDS_Face& F = myBCEdge.Face();
  
  Standard_Real f2, l2, tolpc, tolE, tol, par, dist, tol2d;
  Standard_Boolean haspc;

  if (myFirstCompare) {
    
    Handle(Geom2d_Curve) C2D;
    BRep_Builder BB; 

    haspc = BOPTools_Tools2D::HasCurveOnSurface(E, F); 
    if (!haspc) { 
      BOPTools_Tools2D::CurveOnSurface(E, F, C2D, f2, l2, tolpc, Standard_True); 
      tolE = BRep_Tool::Tolerance(E); 
      tol = Max(tolE, tolpc); 
      BB.UpdateEdge(E,C2D,F,tol); 
    } 

    BOPTools_Tools2D::CurveOnSurface(E, F, C2D, f2, l2, tolpc, Standard_False);
    
    par = BOPTools_Tools2D::IntermediatePoint (f2, l2);
 
    gp_Pnt2d p2d = C2D->Value(par);
    
    gp_Vec2d v2d(myPoint2d,p2d);
    gp_Lin2d l2d(myPoint2d,v2d);
    dist  = myPoint2d.Distance(p2d);
    tol2d = Precision::PConfusion(); 

    myFPC.Reset(l2d,dist,tol2d);
    myFirstCompare = Standard_False;
    
  } // end of if (myFirstCompare)
  
  myBCEdge.Edge() = E;
  TopAbs_Orientation Eori = E.Orientation();
  myFPC.Compare(myBCEdge,Eori);
  //ZZ TopAbs_State state = myFPC.State();
}


//=======================================================================
//function : State
//purpose  : 
//=======================================================================
  TopAbs_State BOP_WireEdgeClassifier::State()
{
  TopAbs_State state = myFPC.State();
  return state;
}

//=======================================================================
//function : MinStepIn2d
//purpose  : 
//=======================================================================
Standard_Real MinStepIn2d(const TopoDS_Edge& aE,
			  const TopoDS_Face& aF)
{
  Standard_Real aDt2dMin, aDt2D, aTolE, aURes, aVRes;
  //
  aDt2dMin=1.e-5;
  //
  aTolE=BRep_Tool::Tolerance(aE);
  //
  Handle(Geom_Surface) aS=BRep_Tool::Surface(aF);
  GeomAdaptor_Surface aGAS(aS);
  if (aGAS.GetType()==GeomAbs_Plane) {
    aURes=aGAS.UResolution(aTolE);
    aVRes=aGAS.VResolution(aTolE);
    aDt2D=(aURes>aVRes) ? aURes : aVRes;
    //
    if (aDt2D > aDt2dMin) {
      aDt2D=aDt2dMin;
    }
  }
  else {
    aDt2D=aDt2dMin;
  }
  return aDt2D;
}
