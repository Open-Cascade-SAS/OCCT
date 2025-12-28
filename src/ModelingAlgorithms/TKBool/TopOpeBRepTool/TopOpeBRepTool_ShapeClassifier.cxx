// Created on: 1994-02-01
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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

#include <BRepAdaptor_Curve2d.hxx>
#include <BRepClass3d_SolidExplorer.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepClass_FacePassiveClassifier.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopOpeBRepTool_CurveTool.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_State.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_List.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopOpeBRepTool_ShapeClassifier.hxx>
#include <TopOpeBRepTool_SolidClassifier.hxx>

//=================================================================================================

TopOpeBRepTool_ShapeClassifier::TopOpeBRepTool_ShapeClassifier()
    : myP3Ddef(false),
      myP2Ddef(false)
{
}

//=================================================================================================

TopOpeBRepTool_ShapeClassifier::TopOpeBRepTool_ShapeClassifier(const TopoDS_Shape& SRef)
    : myP3Ddef(false),
      myP2Ddef(false)
{
  myRef = SRef;
}

//=================================================================================================

void TopOpeBRepTool_ShapeClassifier::ClearAll()
{
  ClearCurrent();
  mySolidClassifier.Clear();
}

//=================================================================================================

void TopOpeBRepTool_ShapeClassifier::ClearCurrent()
{
  mySameDomain = -1;
  myS.Nullify();
  myRef.Nullify();
  myAvS.Nullify();
  myMapAvS.Clear();
  mymre.Clear();
  mymren    = 0;
  mymredone = false;
  myState   = TopAbs_UNKNOWN;
  myEdge.Nullify();
  myFace.Nullify();
  myP3Ddef = myP2Ddef = false;
}

//=================================================================================================

int TopOpeBRepTool_ShapeClassifier::SameDomain() const
{
  return mySameDomain;
}

//=================================================================================================

void TopOpeBRepTool_ShapeClassifier::SameDomain(const int sam)
{
  mySameDomain = sam;
}

//=================================================================================================

void TopOpeBRepTool_ShapeClassifier::SetReference(const TopoDS_Shape& SRef)
{
  myRef = SRef;
  MapRef();
}

//=================================================================================================

void TopOpeBRepTool_ShapeClassifier::MapRef()
{
  mymre.Clear();
  mymren = 0;
  if (myRef.ShapeType() == TopAbs_FACE && mySameDomain == 1)
  {
    TopExp::MapShapes(myRef, TopAbs_EDGE, mymre);
    mymren = mymre.Extent();
    if (mymren == 1)
    {
      TopExp_Explorer    x(myRef, TopAbs_EDGE);
      const TopoDS_Edge& E = TopoDS::Edge(x.Current());
      TopoDS_Vertex      v1, v2;
      TopExp::Vertices(E, v1, v2);
      if (v1.IsSame(v2))
        mymren = 0;
    }
  }
  mymredone = true;
}

//=================================================================================================

TopAbs_State TopOpeBRepTool_ShapeClassifier::StateShapeShape(const TopoDS_Shape& S,
                                                             const TopoDS_Shape& SRef,
                                                             const int           samedomain)
{
  ClearCurrent();
  mySameDomain = samedomain;
  myS          = S;
  myAvS.Nullify();
  myPAvLS = NULL;
  myRef   = SRef;
  Perform();
  return myState;
}

//=================================================================================================

TopAbs_State TopOpeBRepTool_ShapeClassifier::StateShapeShape(const TopoDS_Shape& S,
                                                             const TopoDS_Shape& AvS,
                                                             const TopoDS_Shape& SRef)
{
  ClearCurrent();
  myS     = S;
  myAvS   = AvS;
  myPAvLS = NULL;
  myRef   = SRef;
  Perform();
  return myState;
}

//=================================================================================================

TopAbs_State TopOpeBRepTool_ShapeClassifier::StateShapeShape(
  const TopoDS_Shape&                   S,
  const NCollection_List<TopoDS_Shape>& AvLS,
  const TopoDS_Shape&                   SRef)
{
  ClearCurrent();
  myS = S;
  myAvS.Nullify();
  myPAvLS = (NCollection_List<TopoDS_Shape>*)&AvLS;
  myRef   = SRef;
  Perform();
  return myState;
}

//=================================================================================================

TopAbs_State TopOpeBRepTool_ShapeClassifier::StateShapeReference(const TopoDS_Shape& S,
                                                                 const TopoDS_Shape& AvS)
{
  myS     = S;
  myAvS   = AvS;
  myPAvLS = NULL;
  Perform();
  return myState;
}

//=================================================================================================

TopAbs_State TopOpeBRepTool_ShapeClassifier::StateShapeReference(
  const TopoDS_Shape&                   S,
  const NCollection_List<TopoDS_Shape>& AvLS)
{
  myS = S;
  myAvS.Nullify();
  myPAvLS = (NCollection_List<TopoDS_Shape>*)&AvLS;
  Perform();
  return myState;
}

//=================================================================================================

TopOpeBRepTool_SolidClassifier& TopOpeBRepTool_ShapeClassifier::ChangeSolidClassifier()
{
  return mySolidClassifier;
}

//=================================================================================================

void TopOpeBRepTool_ShapeClassifier::FindEdge()
{
  myEdge.Nullify();
  myFace.Nullify();

  TopAbs_ShapeEnum t = myS.ShapeType();
  if (t < TopAbs_FACE)
  { // compsolid .. shell
    FindFace(myS);
    FindEdge(myFace);
  }
  else
  {
    FindEdge(myS);
  }
}

//=================================================================================================

void TopOpeBRepTool_ShapeClassifier::FindEdge(const TopoDS_Shape& S)
{
  myEdge.Nullify();
  bool isavls = HasAvLS();
  bool isavs  = (!myAvS.IsNull());
  if (S.IsNull())
    return;

  TopAbs_ShapeEnum tS = S.ShapeType();
  TopExp_Explorer  eex;
  if (!myFace.IsNull())
    eex.Init(myFace, TopAbs_EDGE);
  else
    eex.Init(S, TopAbs_EDGE);

  for (; eex.More(); eex.Next())
  {
    const TopoDS_Edge& E       = TopoDS::Edge(eex.Current());
    bool               toavoid = false;
    if (isavls || isavs)
    {
      toavoid = toavoid || myMapAvS.Contains(E);
      if (!myAvS.IsNull())
        toavoid = toavoid || E.IsSame(myAvS);
    }
    else if (BRep_Tool::Degenerated(E))
      toavoid = (tS != TopAbs_EDGE);
    if (toavoid)
      continue;
    myEdge = E;
    break;
  }
}

//=================================================================================================

void TopOpeBRepTool_ShapeClassifier::FindFace(const TopoDS_Shape& S)
{
  myFace.Nullify();
  bool            isavls = HasAvLS();
  bool            isavs  = (!myAvS.IsNull());
  TopExp_Explorer fex(S, TopAbs_FACE);
  for (; fex.More(); fex.Next())
  {
    const TopoDS_Face& F       = TopoDS::Face(fex.Current());
    bool               toavoid = false;
    if (isavls || isavs)
    {
      toavoid = toavoid || myMapAvS.Contains(F);
      if (!myAvS.IsNull())
        toavoid = toavoid || F.IsSame(myAvS);
    }
    if (toavoid)
      continue;
    myFace = F;
    break;
  }
}

//=================================================================================================

void TopOpeBRepTool_ShapeClassifier::Perform()
{
  myState = TopAbs_UNKNOWN;
  if (myS.IsNull())
    return;
  if (myRef.IsNull())
    return;

  if (!mymredone)
  {
    MapRef();
  }

  if (!myAvS.IsNull())
  {
    // tAvS = FACE,EDGE --> map(AvS,EDGE)
    // rejet des aretes de myAvS comme arete de classification
    // (le rejet simple de myAvS est insuffisant (connexite))
    myMapAvS.Clear();
    TopAbs_ShapeEnum tAvS = myAvS.ShapeType();
    if (tAvS == TopAbs_FACE)
    {
      myMapAvS.Add(myAvS);
      TopExp::MapShapes(myAvS, TopAbs_EDGE, myMapAvS);
    }
    else if (tAvS == TopAbs_EDGE)
    {
      TopExp::MapShapes(myAvS, TopAbs_EDGE, myMapAvS);
    }
  }
  else if (HasAvLS())
  {
    // tAvS = FACE,EDGE --> map(AvS,EDGE)
    // rejet des aretes de myPAvLS comme arete de classification
    // (le rejet simple de myPAvLS est insuffisant (connexite))
    myMapAvS.Clear();
    TopAbs_ShapeEnum tAvS = myPAvLS->First().ShapeType();
    if (tAvS == TopAbs_FACE)
    {
      NCollection_List<TopoDS_Shape>::Iterator it((*myPAvLS));
      for (; it.More(); it.Next())
      {
        const TopoDS_Shape& S = it.Value();
        myMapAvS.Add(S);
        TopExp::MapShapes(S, TopAbs_EDGE, myMapAvS);
      }
    }
    else if (tAvS == TopAbs_EDGE)
    {
      NCollection_List<TopoDS_Shape>::Iterator it((*myPAvLS));
      for (; it.More(); it.Next())
      {
        const TopoDS_Shape& S = it.Value();
        TopExp::MapShapes(S, TopAbs_EDGE, myMapAvS);
      }
    }
  }
  else
  {
    if (myS.ShapeType() == TopAbs_FACE)
    {
      myP3Ddef = BRepClass3d_SolidExplorer::FindAPointInTheFace(TopoDS::Face(myS), myP3D);
    }
  }

  TopAbs_ShapeEnum tS = myS.ShapeType();
  TopAbs_ShapeEnum tR = myRef.ShapeType();

  if (tS == TopAbs_VERTEX)
  {
    if (tR <= TopAbs_SOLID)
    {
      gp_Pnt P3D = BRep_Tool::Pnt(TopoDS::Vertex(myS));
      StateP3DReference(P3D);
    }
  }
  else if (tS == TopAbs_EDGE)
  {
    if (tR == TopAbs_FACE || tR <= TopAbs_SOLID)
    {
      FindEdge();
      StateEdgeReference();
    }
  }
  else if (tS == TopAbs_WIRE)
  {
    if (tR == TopAbs_FACE || tR <= TopAbs_SOLID)
    {
      FindEdge();
      StateEdgeReference();
    }
  }
  else if (tS == TopAbs_FACE)
  {
    if (tR == TopAbs_FACE)
    {
      FindEdge();
      if (mySameDomain == 1)
      {
        StateEdgeReference();
      }
      else
      {
        if (!myP3Ddef)
        {
          myP3Ddef = BRepClass3d_SolidExplorer::FindAPointInTheFace(TopoDS::Face(myS), myP3D);
        }
        if (myP3Ddef)
        {
          StateP3DReference(myP3D);
        }
        else
        {
          myState = TopAbs_UNKNOWN;
          throw Standard_ProgramError("TopOpeBRepTool_ShapeClassifier !P3Ddef");
        }
      }
    }
    else if (tR <= TopAbs_SOLID)
    {
      FindEdge();
      if (myP3Ddef)
      {
        StateP3DReference(myP3D);
      }
      else
      {
        StateEdgeReference();
      }
    }
  }
  else if (tS == TopAbs_SHELL)
  {
    if (tR <= TopAbs_SOLID)
    {
      FindEdge();
      StateEdgeReference();
    }
  }
  else if (tS == TopAbs_SOLID)
  {
    if (tR <= TopAbs_SOLID)
    {
      FindEdge();
      StateEdgeReference();
    }
  }
  else
  {
    throw Standard_ProgramError("StateShapeShape : bad operands");
  }

  // take orientation of reference shape in account
  TopAbs_Orientation oriRef = myRef.Orientation();
  if (oriRef == TopAbs_EXTERNAL || oriRef == TopAbs_INTERNAL)
  {
    if (myState == TopAbs_IN)
      myState = TopAbs_OUT;
  }
}

//=================================================================================================

void TopOpeBRepTool_ShapeClassifier::StateEdgeReference()
{
  myState = TopAbs_UNKNOWN;

  if (myEdge.IsNull())
    return;
  if (myRef.IsNull())
    return;

  occ::handle<Geom_Curve> C3D;
  gp_Pnt                  P3D;
  double                  f3d, l3d;

  occ::handle<Geom2d_Curve> C2D;
  gp_Pnt2d                  P2D;
  double                    f2d, l2d, tol2d;

  TopAbs_ShapeEnum tR = myRef.ShapeType();
  // myEdge est une arete de myS, pas de myRef
  if (tR == TopAbs_FACE)
  {
    const TopoDS_Face& F = TopoDS::Face(myRef);
    if (mySameDomain)
    {
      bool trimCurve = true;
      C2D            = FC2D_CurveOnSurface(myEdge, F, f2d, l2d, tol2d, trimCurve);

      if (C2D.IsNull())
        throw Standard_ProgramError("StateShapeShape : no 2d curve");

      double t = 0.127956477;
      double p = (1 - t) * f2d + t * l2d;
      P2D      = C2D->Value(p);

#ifdef OCCT_DEBUG
      C3D = BRep_Tool::Curve(myEdge, f3d, l3d);
      if (!C3D.IsNull())
        P3D = C3D->Value(p);
#endif
      StateP2DReference(P2D);
      return;
    }
    else
    { // myEdge/myRef=face en 3d
      C3D = BRep_Tool::Curve(myEdge, f3d, l3d);

      if (C3D.IsNull())
        throw Standard_ProgramError("StateShapeShape : no 3d curve");

      double t = 0.127956477;
      double p = (1 - t) * f3d + t * l3d;
      P3D      = C3D->Value(p);
      StateP3DReference(P3D);
      return;
    }
  }
  else if (tR <= TopAbs_SOLID)
  {
    bool degen = BRep_Tool::Degenerated(myEdge);
    if (degen)
    {
      const TopoDS_Vertex& v = TopExp::FirstVertex(myEdge);
      P3D                    = BRep_Tool::Pnt(v);
      StateP3DReference(P3D);
      return;
    }
    else
    {
      C3D = BRep_Tool::Curve(myEdge, f3d, l3d);

      if (C3D.IsNull())
        throw Standard_ProgramError("StateShapeShape : no 3d curve");

      double t = 0.127956477;
      double p = (1 - t) * f3d + t * l3d;
      P3D      = C3D->Value(p);
      StateP3DReference(P3D);
      return;
    }
  }
  else
    throw Standard_ProgramError("StateShapeShape : bad operands");
}

//=================================================================================================

void TopOpeBRepTool_ShapeClassifier::StateP2DReference(const gp_Pnt2d& P2D)
{
  myState = TopAbs_UNKNOWN;
  if (myRef.IsNull())
    return;
  TopAbs_ShapeEnum tR = myRef.ShapeType();

  if (tR == TopAbs_FACE)
  {
    if (mymren == 1)
    {
      TopExp_Explorer x;
      for (x.Init(myRef, TopAbs_EDGE); x.More(); x.Next())
      {
        //      for(TopExp_Explorer x(myRef,TopAbs_EDGE);x.More();x.Next()) {
        TopAbs_Orientation o = x.Current().Orientation();
        //	if      (o == TopAbs_EXTERNAL) myState == TopAbs_OUT;
        if (o == TopAbs_EXTERNAL)
          myState = TopAbs_OUT;
        //	else if (o == TopAbs_INTERNAL) myState == TopAbs_IN;
        else if (o == TopAbs_INTERNAL)
          myState = TopAbs_IN;
        else
        {
#ifdef OCCT_DEBUG
          std::cout << "StateP2DReference o<>E,I" << std::endl;
#endif
          break;
        }
      }
    }
    else
    {
      myP2D         = P2D;
      myP2Ddef      = true;
      TopoDS_Face F = TopoDS::Face(myRef);
      F.Orientation(TopAbs_FORWARD);
      double                  TolClass = 1e-8;
      BRepTopAdaptor_FClass2d FClass2d(F, TolClass);
      myState = FClass2d.Perform(P2D);
    }
  }
  else
  {
    throw Standard_ProgramError("StateShapeShape : bad operands");
  }
}

//=================================================================================================

void TopOpeBRepTool_ShapeClassifier::StateP3DReference(const gp_Pnt& P3D)
{
  myState = TopAbs_UNKNOWN;
  if (myRef.IsNull())
    return;
  TopAbs_ShapeEnum tR = myRef.ShapeType();

  if (tR == TopAbs_SOLID)
  {
    myP3D                     = P3D;
    myP3Ddef                  = true;
    const TopoDS_Solid& SO    = TopoDS::Solid(myRef);
    double              tol3d = Precision::Confusion();
    mySolidClassifier.Classify(SO, P3D, tol3d);
    myState = mySolidClassifier.State();
  }
  else if (tR < TopAbs_SOLID)
  {
    myP3D    = P3D;
    myP3Ddef = true;
    TopExp_Explorer ex;
    for (ex.Init(myRef, TopAbs_SOLID); ex.More(); ex.Next())
    {
      //    for (TopExp_Explorer ex(myRef,TopAbs_SOLID);ex.More();ex.Next()) {
      const TopoDS_Solid& SO    = TopoDS::Solid(ex.Current());
      double              tol3d = Precision::Confusion();
      mySolidClassifier.Classify(SO, P3D, tol3d);
      myState = mySolidClassifier.State();
      if (myState == TopAbs_IN || myState == TopAbs_ON)
      {
        break;
      }
    }
  }
  else
  {
    throw Standard_ProgramError("StateShapeShape : bad operands");
  }
}

//=================================================================================================

TopAbs_State TopOpeBRepTool_ShapeClassifier::State() const
{
  return myState;
}

//=================================================================================================

const gp_Pnt& TopOpeBRepTool_ShapeClassifier::P3D() const
{
  if (myP3Ddef)
  {
    return myP3D;
  }
  throw Standard_ProgramError("ShapeClassifier::P3D undefined");
}

//=================================================================================================

const gp_Pnt2d& TopOpeBRepTool_ShapeClassifier::P2D() const
{
  if (myP2Ddef)
  {
    return myP2D;
  }
  throw Standard_ProgramError("ShapeClassifier::P2D undefined");
}

//=================================================================================================

bool TopOpeBRepTool_ShapeClassifier::HasAvLS() const
{
  bool hasavls = (myPAvLS) ? (!myPAvLS->IsEmpty()) : false;
  return hasavls;
}

#if 0
//=================================================================================================

void TopOpeBRepTool_ShapeClassifier::FindEdge(const TopoDS_Shape& S)
{
  myEdge.Nullify();
  bool isavs = (! myAvS.IsNull());
  bool isavls = HasAvLS();
  bool isav = (isavs || isavls);

  if (S.IsNull()) return;
  TopAbs_ShapeEnum tS = S.ShapeType();

  TopExp_Explorer eex;
  if ( ! myFace.IsNull() ) eex.Init(myFace,TopAbs_EDGE);
  else eex.Init(S,TopAbs_EDGE);

  for(; eex.More(); eex.Next()) {
    const TopoDS_Edge& E = TopoDS::Edge(eex.Current());
    if ( isav ) {
      bool toavoid = false;
      if ( isavls ) toavoid = myMapAvS.Contains(E);
      else if ( isavs )	toavoid = E.IsSame(myAvS);      
      if ( toavoid ) continue;
    }
    else if ( BRep_Tool::Degenerated(E) ) {
      if ( tS != TopAbs_EDGE ) continue;
    }
    myEdge = E;
    break;
  }
}

static bool FindAPointInTheFace
(const TopoDS_Face& _face,gp_Pnt& APoint,double& u,double& v) 
{ 
  TopoDS_Face face=_face;
  face.Orientation(TopAbs_FORWARD);

  TopExp_Explorer     faceexplorer;
  BRepAdaptor_Curve2d c;
  gp_Vec2d T;
  gp_Pnt2d P;
  bool Ok = false;
  int nbiter=0;
  double myParamOnEdge = 0.5;
  do { 
    nbiter++;
    if(myParamOnEdge==0.5)  myParamOnEdge = 0.4;
    else if(myParamOnEdge==0.4)  myParamOnEdge = 0.6; 
    else if(myParamOnEdge==0.6)  myParamOnEdge = 0.3; 
    else if(myParamOnEdge==0.3)  myParamOnEdge = 0.7; 
    else if(myParamOnEdge==0.7)  myParamOnEdge = 0.2; 
    else if(myParamOnEdge==0.2)  myParamOnEdge = 0.8; 
    else if(myParamOnEdge==0.8)  myParamOnEdge = 0.1; 
    else if(myParamOnEdge==0.1)  myParamOnEdge = 0.9;
    else { myParamOnEdge*=0.5; } 
    
    for (faceexplorer.Init(face,TopAbs_EDGE); 
	 faceexplorer.More(); 
	 faceexplorer.Next()) {
      TopoDS_Edge Edge = TopoDS::Edge(faceexplorer.Current());
      c.Initialize(Edge,face);
      int nbinterval = c.NbIntervals(GeomAbs_C1); 
      c.D1((c.LastParameter() - c.FirstParameter()) * myParamOnEdge + c.FirstParameter(),P,T);
      
      double x=T.X();
      double y=T.Y();
      //-- std::cout<<"Param:"<<(c.IntervalFirst() + c.IntervalLast()) * param<<" U:"<<P.X()<<" V:"<<P.Y();
      //-- std::cout<<" tguv x:"<<x<<" , y:"<<y<<std::endl;
      
      
      if(Edge.Orientation() == TopAbs_FORWARD) { 
	T.SetCoord(-y,x);
      }
      else { 
	T.SetCoord(y,-x);
      }
      
      double ParamInit = RealLast();
      double TolInit   = 0.00001;
      bool APointExist = false;
      
      BRepClass_FacePassiveClassifier FClassifier;
      
      T.Normalize();
      P.SetCoord(P.X()+TolInit*T.X(),P.Y()+TolInit*T.Y());
// clang-format off
      FClassifier.Reset(gp_Lin2d(P,T),ParamInit,RealEpsilon());   //-- Longueur et Tolerance #######
// clang-format on
      
      TopExp_Explorer otherfaceexplorer;
      for (otherfaceexplorer.Init(face,TopAbs_EDGE); 
	   otherfaceexplorer.More(); 
	   otherfaceexplorer.Next()) {
	TopoDS_Edge OtherEdge = TopoDS::Edge(otherfaceexplorer.Current());
	if((OtherEdge.Orientation() == TopAbs_EXTERNAL)) { 
	}
	else { 
	  BRepClass_Edge AEdge(OtherEdge,face);
	  FClassifier.Compare(AEdge,OtherEdge.Orientation());
	  if(FClassifier.ClosestIntersection()) { 
	    //-- std::cout<<" ---> Edge : "<<FClassifier.Parameter()<<std::endl;
	    if(ParamInit > FClassifier.Parameter()) { 
	      ParamInit = FClassifier.Parameter();
	      APointExist = true;
	    }
	  }
	}
      }
      if(APointExist) { 
	ParamInit*=0.5;
	u = P.X() + ParamInit* T.X();
	v = P.Y() + ParamInit* T.Y();
	BRepAdaptor_Surface  s;
	bool computerestriction = false;
	s.Initialize(face,computerestriction);
	s.D0(u,v,APoint);
	//-- std::cout<<" u="<<u<<" v="<<v<<"  -> ("<<APoint.X()<<","<<APoint.Y()<<","<<APoint.Z()<<std::endl;
	return(true);
      }
    }
  }
  while(nbiter<100);
  return(false);
}

#endif
