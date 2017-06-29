// Created on: 1993-06-17
// Created by: Jean Yves LEBEY
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


#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_BlockBuilder.hxx>
#include <TopOpeBRepBuild_Loop.hxx>
#include <TopOpeBRepBuild_WireEdgeClassifier.hxx>

#ifdef DRAW
#include <DBRep.hxx>
#endif

#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <Geom2d_Curve.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <Standard_ProgramError.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec2d.hxx>
#include <BRep_Builder.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopLoc_Location.hxx>
#include <TopExp.hxx>

#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_SC.hxx>
#include <TopOpeBRepTool_TOOL.hxx>
#include <TopOpeBRepTool_CurveTool.hxx>

#include <TopOpeBRepBuild_Builder.hxx>
#include <TopOpeBRepBuild_define.hxx>

#ifdef OCCT_DEBUG
static TCollection_AsciiString PRODINS("dins ");
#endif

#define MYBB ((TopOpeBRepBuild_BlockBuilder*)myBlockBuilder)


#define SAME     (-1)
#define DIFF     (-2)
#define UNKNOWN  ( 0)
#define oneINtwo ( 1)
#define twoINone ( 2)

//=======================================================================
//function : TopOpeBRepBuild_WireEdgeClassifier
//purpose  : 
//=======================================================================

TopOpeBRepBuild_WireEdgeClassifier::TopOpeBRepBuild_WireEdgeClassifier
(const TopoDS_Shape& F,
 const TopOpeBRepBuild_BlockBuilder& BB) :
TopOpeBRepBuild_CompositeClassifier(BB)
{
  myBCEdge.Face() = TopoDS::Face(F);
}

//=======================================================================
//function : Compare
//purpose  : 
//=======================================================================

TopAbs_State TopOpeBRepBuild_WireEdgeClassifier::Compare
(const Handle(TopOpeBRepBuild_Loop)& L1,
 const Handle(TopOpeBRepBuild_Loop)& L2)
{ 
  TopAbs_State state = TopAbs_UNKNOWN;

  Standard_Boolean isshape1 = L1->IsShape();
  Standard_Boolean isshape2 = L2->IsShape();

  if      ( isshape2 && isshape1 )  { // L1 is Shape , L2 is Shape
    const TopoDS_Shape& s1 = L1->Shape();
    const TopoDS_Shape& s2 = L2->Shape();
    state = CompareShapes(s1,s2);
  }
  else if ( isshape2 && !isshape1 ) { // L1 is Block , L2 is Shape
    TopOpeBRepBuild_BlockIterator Bit1 = L1->BlockIterator();
    Bit1.Initialize();
    Standard_Boolean yena1 = Bit1.More();
    while (yena1) {
      const TopoDS_Shape& s1 = MYBB->Element(Bit1);
      const TopoDS_Shape& s2 = L2->Shape();
      state = CompareElementToShape(s1,s2);
      yena1 = Standard_False;
      if (state == TopAbs_UNKNOWN) { 
	if (Bit1.More()) Bit1.Next();
	yena1 = Bit1.More();
      }
    }
  }
  else if ( !isshape2 && isshape1 ) { // L1 is Shape , L2 is Block
    const TopoDS_Shape& s1 = L1->Shape();
    ResetShape(s1);
    TopOpeBRepBuild_BlockIterator Bit2 = L2->BlockIterator();
    for (Bit2.Initialize(); Bit2.More(); Bit2.Next()) {
      const TopoDS_Shape& s2 = MYBB->Element(Bit2);
      CompareElement(s2);
    }
    state = State();
  }
  else if ( !isshape2 && !isshape1 ) { // L1 is Block , L2 is Block

    if (state == TopAbs_UNKNOWN) {      
      TopOpeBRepBuild_BlockIterator Bit1 = L1->BlockIterator();
      Bit1.Initialize();
      Standard_Boolean yena1 = Bit1.More();
      while (yena1) {
	const TopoDS_Shape& s1 = MYBB->Element(Bit1);
	ResetElement(s1);
	TopOpeBRepBuild_BlockIterator Bit2 = L2->BlockIterator();
	for (Bit2.Initialize(); Bit2.More(); Bit2.Next()) {
	  const TopoDS_Shape& s2 = MYBB->Element(Bit2);
	  CompareElement(s2);
	}
	state = State();
	yena1 = Standard_False;
	if (state == TopAbs_UNKNOWN) { 
	  if (Bit1.More()) Bit1.Next();
	  yena1 = Bit1.More();
	}
      }
    } //UNKNOWN

    if (state == TopAbs_UNKNOWN) {
      TopoDS_Shape s1 = LoopToShape(L1); if (s1.IsNull()) return state;
      TopoDS_Shape s2 = LoopToShape(L2); if (s2.IsNull()) return state;
      TopOpeBRepTool_ShapeClassifier& SC = FSC_GetPSC();
      Standard_Integer samedomain = SC.SameDomain(); SC.SameDomain(1); 
      SC.SetReference(s2);
      const TopoDS_Shape& AvS = s2;
      state = SC.StateShapeReference(s1,AvS);
      SC.SameDomain(samedomain);
    } // UNKNOWN

  }
  return state;
}

//=======================================================================
//function : LoopToShape
//purpose  : 
//=======================================================================

TopoDS_Shape TopOpeBRepBuild_WireEdgeClassifier::LoopToShape(const Handle(TopOpeBRepBuild_Loop)& L)
{
  myShape.Nullify();
  TopOpeBRepBuild_BlockIterator Bit = L->BlockIterator();
  Bit.Initialize();
  if ( !Bit.More() ) return myShape;

  TopoDS_Shape aLocalShape = myBCEdge.Face();
  const TopoDS_Face& F1 = TopoDS::Face(aLocalShape);
//  const TopoDS_Face& F1 = TopoDS::Face(myBCEdge.Face());
  aLocalShape = F1.EmptyCopied();
  TopoDS_Face F = TopoDS::Face(aLocalShape);
//  TopoDS_Face F = TopoDS::Face(F1.EmptyCopied());
  BRep_Builder BB; TopoDS_Wire W; BB.MakeWire(W);
  for  (; Bit.More(); Bit.Next()) {
    const TopoDS_Edge& E = TopoDS::Edge(MYBB->Element(Bit));
    Standard_Real tolE; tolE = BRep_Tool::Tolerance(E);
    Standard_Boolean haspc = FC2D_HasCurveOnSurface(E,F);
    if (!haspc) {
      Standard_Real f,l,tolpc;Handle(Geom2d_Curve) C2D;
      C2D = FC2D_CurveOnSurface(E,F,f,l,tolpc);
      if (!C2D.IsNull()) {
	Standard_Real tol = Max(tolpc,tolE);
	BB.UpdateEdge(E,C2D,F,tol);
      }
    }
    BB.Add(W,E);
  }
  BB.Add(F,W);

  myShape = F;
  return myShape;
}

static gp_Vec FUN_tgINE(const TopoDS_Vertex& v, const TopoDS_Vertex& vl, const TopoDS_Edge& e)
// tg oriented INSIDE 1d(e)
// vl : last vertex of e
{
  Standard_Real par = BRep_Tool::Parameter(v,e);  
  gp_Vec tg; Standard_Boolean ok = TopOpeBRepTool_TOOL::TggeomE(par,e,tg);
  if (!ok) return gp_Vec(0.,0.,0.); //NYIRAISE
  if (v.IsSame(vl)) tg.Reverse();
  return tg;
}

//=======================================================================
//function : CompareShapes
//purpose  : 
//=======================================================================

TopAbs_State  TopOpeBRepBuild_WireEdgeClassifier::CompareShapes
(const TopoDS_Shape& B1, const TopoDS_Shape& B2)
{
  // evolution xpu101198 : - cto009K4, regularisation de f24, 
  // on classifie 1 wire / 1 des wires connexes.
  // (face rectangulaire - face circulaire tangente aux bords) - 
  // WEC completement aleatoire : depend du choix de e1 dans le cas
  // ou e1 est tangent a E. - e1 droite touche l'autre wire -
  // on fait l'hypothese que les shapes B1 et B2 proviennent du meme shape,
  // et que si ils ne se touchent pas, on ne passe pas par le WEC.
  // INCOMPLET!!!

#ifdef OCCT_DEBUG
//  TopAbs_ShapeEnum t1 = B1.ShapeType();
//  TopAbs_ShapeEnum t2 = B2.ShapeType();
#endif
  TopAbs_State state = TopAbs_UNKNOWN;
  TopExp_Explorer ex1(B1,TopAbs_EDGE);
  if ( !ex1.More() ) return state;
  for ( ; ex1.More(); ex1.Next() ) {
    const TopoDS_Edge& e1 = TopoDS::Edge(ex1.Current());
    TopoDS_Vertex vf1,vl1; TopExp::Vertices(e1,vf1,vl1);//xpu101198
    Standard_Boolean e1clo = vf1.IsSame(vl1);
    TopTools_IndexedMapOfShape mapv1; mapv1.Add(vf1); mapv1.Add(vl1);
    
    ResetShape(e1);
    Standard_Integer iE = 0; Standard_Boolean indy = Standard_False;
    TopExp_Explorer Ex;
    for(Ex.Init(B2,TopAbs_EDGE); Ex.More(); Ex.Next()) {
//    for(TopExp_Explorer Ex(B2,TopAbs_EDGE); Ex.More(); Ex.Next()) {
      const TopoDS_Edge& E = TopoDS::Edge(Ex.Current());
      if (E.IsSame(e1)) { state = TopAbs_UNKNOWN; break; } // eap occ416
      TopoDS_Vertex vf,vl; TopExp::Vertices(E,vf,vl);//xpu101198
      Standard_Boolean Eclo = vf.IsSame(vl);//xpu101198
      Standard_Boolean hasf = mapv1.Contains(vf);//xpu101198
      Standard_Boolean hasl = mapv1.Contains(vl);//xpu101198     
      Standard_Boolean filter = (hasf || hasl) && (!e1clo) && (!Eclo);// nyi : Eclo || e1clo
      if (filter) {//xpu101198
	TopoDS_Vertex vshared; if (hasf) vshared=vf; if (hasl) vshared=vl;
	gp_Vec tg1 = FUN_tgINE(vshared,vl1,e1);
	gp_Vec tg = FUN_tgINE(vshared,vl,E);
	Standard_Real dot = tg1.Dot(tg);
	Standard_Real tol = Precision::Angular()*1.e4; // nyixpu
	Standard_Boolean undecided = (Abs(1+dot) < tol);
	if (undecided) {indy = Standard_True;}
      }//xpu101198
      if (indy) {state = TopAbs_UNKNOWN; break;}
      CompareElement(E);
      state = State();
      iE++;
    } // ex(B2,EDGE)    
    if (state != TopAbs_UNKNOWN) {   
      break;
    }
  }// ex1
  
  Standard_Boolean resta = (state == TopAbs_UNKNOWN);
  resta = resta && (B2.ShapeType()==TopAbs_WIRE) && (B1.ShapeType()==TopAbs_WIRE);
  if (resta) {
    TopTools_IndexedMapOfShape mape1; TopExp::MapShapes(B1,TopAbs_EDGE,mape1);
    // recall : avoid auto-intersection wires (ie B1 and B2 are disjoint)
    TopExp_Explorer ex2(B2,TopAbs_EDGE);
    for (; ex2.More(); ex2.Next()){
      const TopoDS_Edge& E2 = TopoDS::Edge(ex2.Current());
      if (mape1.Contains(E2)) continue;
      
      const TopoDS_Face& theFace =  myBCEdge.Face();
      BRep_Builder BB; 

      // p2d on E2 of B2, E2 not shared by B1
      TopoDS_Shape aLocalShape = theFace.Oriented(TopAbs_FORWARD);
      TopoDS_Face ftmp = TopoDS::Face(aLocalShape);
//      TopoDS_Face ftmp = TopoDS::Face(theFace.Oriented(TopAbs_FORWARD));
      aLocalShape = ftmp.EmptyCopied();
      TopoDS_Face F2 = TopoDS::Face(aLocalShape);
//      TopoDS_Face F2 = TopoDS::Face(ftmp.EmptyCopied());
      BB.Add(F2,TopoDS::Wire(B2));

      BRepAdaptor_Curve2d BC2d(E2,F2); 
      Standard_Real f,l; FUN_tool_bounds(E2,f,l); Standard_Real x = 0.45678; Standard_Real p2 = (1-x)*l + x*f;
      gp_Pnt2d p2d = BC2d.Value(p2);
      
      aLocalShape = ftmp.EmptyCopied();
      TopoDS_Face F1 = TopoDS::Face(aLocalShape);
//      TopoDS_Face F1 = TopoDS::Face(ftmp.EmptyCopied());
      BB.Add(F1,TopoDS::Wire(B1));

      Standard_Real tolF1 = BRep_Tool::Tolerance(F1);
      BRepClass_FaceClassifier Fclass(F1, p2d, tolF1);
      state = Fclass.State();
      return state;
    } // ex2	
  }

  /*if (state == TopAbs_UNKNOWN) {
    const TopoDS_Face& F = TopoDS::Face(myBCEdge.Face());
    Bnd_Array1OfBox2d bnd(1,2);
    FUN_tool_mkBnd2d(B1,F,bnd(1)); FUN_tool_mkBnd2d(B2,F,bnd(2));
    TopTools_Array1OfShape B(1,2);
    B(1)=B1; B(2)=B2;
//    Standard_Boolean chklarge=Standard_True; Standard_Integer isma = 0; TopAbs_State osta = FUN_tool_classiBnd2d(bnd,isma,chklarge);
//    if (osta == TopAbs_OUT) return TopAbs_OUT;
//    if (osta == TopAbs_IN) {
//      if (isma == 2) return TopAbs_IN;  //B2 is IN B1 
//      else           return TopAbs_OUT; //B2 is OUT B1 (contains B1)  
//    }     
    Standard_Boolean chklarge=Standard_True; Standard_Integer sta = FUN_tool_classiBnd2d(bnd,chklarge);
    if ((sta == SAME)||(sta == UNKNOWN)) sta = FUN_tool_classiwithp2d(B);
    if ((sta == SAME)||(sta == UNKNOWN)) return TopAbs_UNKNOWN;  
    if (sta == DIFF) return TopAbs_OUT; // B1 OUT B2     
    Standard_Integer isma = (sta == oneINtwo) ? 1 : 2; 
    if (isma == 2)   return TopAbs_IN;  //B2 is IN B1 
    else             return TopAbs_OUT; //B2 is OUT B1 (contains B1) 
  }*/
  return state;
}


//=======================================================================
//function : CompareElementToShape
//purpose  : 
//=======================================================================

TopAbs_State  TopOpeBRepBuild_WireEdgeClassifier::CompareElementToShape
(const TopoDS_Shape& EE,const TopoDS_Shape& B)
{
  // isEdge : edge E inits myPoint2d 
  ResetElement(EE);
  TopExp_Explorer Ex;
  for(Ex.Init(B,TopAbs_EDGE); Ex.More(); Ex.Next()) {
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

void  TopOpeBRepBuild_WireEdgeClassifier::ResetShape(const TopoDS_Shape& B)
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

void  TopOpeBRepBuild_WireEdgeClassifier::ResetElement(const TopoDS_Shape& EE)
{
  const TopoDS_Edge& E = TopoDS::Edge(EE);
  const TopoDS_Face& F = myBCEdge.Face();
  Standard_Real f2,l2,tolpc;Handle(Geom2d_Curve) C2D; //jyl980406+
  Standard_Boolean haspc = FC2D_HasCurveOnSurface(E,F); //jyl980406+
  if (!haspc) { //jyl980406+
    Standard_Boolean trim3d = Standard_True; C2D = FC2D_CurveOnSurface(E,F,f2,l2,tolpc,trim3d); //jyl980406+
    Standard_Real tolE = BRep_Tool::Tolerance(E); //jyl980406+  
    Standard_Real tol = Max(tolE,tolpc); //jyl980406+
    BRep_Builder BB; BB.UpdateEdge(E,C2D,F,tol); //jyl980406+
  } //jyl980406+
  
  C2D = FC2D_CurveOnSurface(E,F,f2,l2,tolpc);
  if (C2D.IsNull()) throw Standard_ProgramError("WEC : ResetElement");

  Standard_Real t = 0.397891143689; Standard_Real par = ((1-t)*f2 + t*l2);
  myPoint2d = C2D->Value(par);
  
#ifdef OCCT_DEBUG
  Standard_Real f3,l3; Handle(Geom_Curve) C3D = BRep_Tool::Curve(E,f3,l3);
  gp_Pnt P3D; if (!C3D.IsNull()) P3D = C3D->Value(par);
#endif
  
  myFirstCompare = Standard_True;
}


//=======================================================================
//function : CompareElement
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_WireEdgeClassifier::CompareElement(const TopoDS_Shape& EE)
{
  Standard_Boolean bRet = Standard_True;
  const TopoDS_Edge& E = TopoDS::Edge(EE);
  const TopoDS_Face& F = myBCEdge.Face();

  Standard_Real f2,l2,tolpc;Handle(Geom2d_Curve) C2D; //jyl980402+
  Standard_Boolean haspc = FC2D_HasCurveOnSurface(E,F); //jyl980402+
  if (!haspc) { //jyl980402+
    Standard_Boolean trim3d = Standard_True; C2D = FC2D_CurveOnSurface(E,F,f2,l2,tolpc,trim3d); //jyl980406+
    // C2D = FC2D_CurveOnSurface(E,F,f2,l2,tolpc,trim3d); //jyl980406-
    Standard_Real tolE = BRep_Tool::Tolerance(E); //jyl980402+  
    Standard_Real tol = Max(tolE,tolpc); //jyl980402+
    BRep_Builder BB; BB.UpdateEdge(E,C2D,F,tol); //jyl980402+
  } //jyl980402+

  if (myFirstCompare) {
    C2D = FC2D_CurveOnSurface(E,F,f2,l2,tolpc);
    Standard_Real t = 0.33334567; Standard_Real par = ((1-t)*f2 + t*l2);
    gp_Pnt2d p2d = C2D->Value(par);
    
#ifdef OCCT_DEBUG
    Standard_Real f3,l3; Handle(Geom_Curve) C3D = BRep_Tool::Curve(E,f3,l3);
    gp_Pnt P3D; if (!C3D.IsNull()) P3D = C3D->Value(par);
#endif
    
    // NYI : p2d peut etre un point ou la courbe n'est pas C1.
    // NYI : voir TopOpeBRepTool_ShapeClassifier_FindAPointInTheFace
    gp_Vec2d v2d(myPoint2d,p2d);
    gp_Lin2d l2d(myPoint2d,v2d);
    Standard_Real dist = myPoint2d.Distance(p2d);
    Standard_Real tol2d = Precision::PConfusion(); // NYI : a voir
    myFPC.Reset(l2d,dist,tol2d);
    myFirstCompare = Standard_False;
  }
  
  myBCEdge.Edge() = E;
  TopAbs_Orientation Eori = E.Orientation();
  myFPC.Compare(myBCEdge,Eori);
#ifdef OCCT_DEBUG
//  TopAbs_State state = myFPC.State();
#endif
  return bRet;
}


//=======================================================================
//function : State
//purpose  : 
//=======================================================================

TopAbs_State TopOpeBRepBuild_WireEdgeClassifier::State()
{
  TopAbs_State state = myFPC.State();
  return state;
}
