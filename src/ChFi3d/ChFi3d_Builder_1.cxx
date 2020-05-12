// Created on: 1993-12-15
// Created by: Isabelle GRIGNON
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


#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <AppBlend_Approx.hxx>
#include <Blend_CurvPointFuncInv.hxx>
#include <Blend_FuncInv.hxx>
#include <Blend_Function.hxx>
#include <Blend_RstRstFunction.hxx>
#include <Blend_SurfCurvFuncInv.hxx>
#include <Blend_SurfPointFuncInv.hxx>
#include <Blend_SurfRstFunction.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBlend_Line.hxx>
#include <BRepLProp_SLProps.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <ChFi3d.hxx>
#include <ChFi3d_Builder.hxx>
#include <ChFi3d_Builder_0.hxx>
#include <ChFiDS_CommonPoint.hxx>
#include <ChFiDS_ErrorStatus.hxx>
#include <ChFiDS_FilSpine.hxx>
#include <ChFiDS_HData.hxx>
#include <ChFiDS_HElSpine.hxx>
#include <ChFiDS_ListIteratorOfListOfStripe.hxx>
#include <ChFiDS_ListIteratorOfRegularities.hxx>
#include <ChFiDS_Regul.hxx>
#include <ChFiDS_Spine.hxx>
#include <ChFiDS_State.hxx>
#include <ChFiDS_Stripe.hxx>
#include <ChFiDS_SurfData.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <GeomInt_IntSS.hxx>
#include <Extrema_ExtPC.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <LocalAnalysis_SurfaceContinuity.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <TopAbs.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Surface.hxx>
#include <TopOpeBRepTool_TOOL.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <BRepLib_MakeEdge.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean ChFi3d_GetcontextFORCEBLEND(); 
#endif

static void ReorderFaces(TopoDS_Face&         theF1,
                         TopoDS_Face&         theF2,
                         const TopoDS_Face&   theFirstFace,
                         const TopoDS_Edge&   thePrevEdge,
                         const TopoDS_Vertex& theCommonVertex,
                         const ChFiDS_Map&    theEFmap)
{
  if (theF1.IsSame(theFirstFace))
    return;
  else if (theF2.IsSame(theFirstFace))
  {
    TopoDS_Face TmpFace = theF1; theF1 = theF2; theF2 = TmpFace;
    return;
  }

  //Loop until find <theF1> or <theF2>
  Standard_Boolean ToExchange = Standard_False;
  TopoDS_Edge PrevEdge = thePrevEdge, CurEdge;
  TopoDS_Face PrevFace = theFirstFace, CurFace;
  for (;;)
  {
    TopTools_IndexedDataMapOfShapeListOfShape VEmap;
    TopExp::MapShapesAndAncestors(PrevFace, TopAbs_VERTEX, TopAbs_EDGE, VEmap);
    const TopTools_ListOfShape& Elist = VEmap.FindFromKey(theCommonVertex);
    if (PrevEdge.IsSame(Elist.First()))
      CurEdge = TopoDS::Edge(Elist.Last());
    else
      CurEdge = TopoDS::Edge(Elist.First());

    const TopTools_ListOfShape& Flist = theEFmap.FindFromKey(CurEdge);
    if (PrevFace.IsSame(Flist.First()))
      CurFace = TopoDS::Face(Flist.Last());
    else
      CurFace = TopoDS::Face(Flist.First());

    if (CurFace.IsSame(theF1))
      break;
    else if (CurFace.IsSame(theF2))
    {
      ToExchange = Standard_True;
      break;
    }

    PrevEdge = CurEdge;
    PrevFace = CurFace;
  }

  if (ToExchange)
  {
    TopoDS_Face TmpFace = theF1; theF1 = theF2; theF2 = TmpFace;
  }
}

static void ConcatCurves(TColGeom_SequenceOfCurve& theCurves,
                         TColGeom_SequenceOfCurve& theNewCurves)
{
  while (!theCurves.IsEmpty())
  {
    GeomConvert_CompCurveToBSplineCurve Concat;
    Standard_Boolean Success = Standard_False;
    for (Standard_Integer i = 1; i <= theCurves.Length(); i++)
    {
      const Handle(Geom_Curve)& aCurve = theCurves(i);
      Handle(Geom_BoundedCurve) aBoundedCurve = Handle(Geom_BoundedCurve)::DownCast(aCurve);
      Success = Concat.Add(aBoundedCurve, 1.e-5, Standard_True);
      if (!Success)
        Success = Concat.Add(aBoundedCurve, 1.e-5, Standard_False);
      if (Success)
      {
        theCurves.Remove(i);
        i--;
      }
    }
    Handle(Geom_Curve) aNewCurve = Concat.BSplineCurve();
    theNewCurves.Append(aNewCurve);
  }
}

static TopoDS_Edge MakeOffsetEdge(const TopoDS_Edge&         theEdge,
                                  const Standard_Real        Distance,
                                  const BRepAdaptor_Surface& S1,
                                  const BRepAdaptor_Surface& S2)
{
  TopoDS_Edge OffsetEdge;
  
  TopoDS_Face F1 = S1.Face();
  TopoDS_Face F2 = S2.Face();
  Handle(Geom_Surface) GS1 = BRep_Tool::Surface(F1);
  Handle(Geom_Surface) TrGS1 =
    new Geom_RectangularTrimmedSurface(GS1,
                                       S1.FirstUParameter(), S1.LastUParameter(),
                                       S1.FirstVParameter(), S1.LastVParameter());
  Standard_Real Offset = -Distance;
  if (F1.Orientation() == TopAbs_REVERSED)
    Offset = Distance;
  Handle(Geom_OffsetSurface) MakeOffsetSurf = new Geom_OffsetSurface(TrGS1, Offset);
  Handle(Geom_Surface) OffsetTrGS1 = MakeOffsetSurf->Surface();
  if (OffsetTrGS1.IsNull())
    OffsetTrGS1 = MakeOffsetSurf;
  Handle(Geom_Surface) GS2 = BRep_Tool::Surface(F2);
  Handle(Geom_Surface) TrGS2 =
    new Geom_RectangularTrimmedSurface(GS2,
                                       S2.FirstUParameter(), S2.LastUParameter(),
                                       S2.FirstVParameter(), S2.LastVParameter());
  GeomInt_IntSS Intersector(OffsetTrGS1, TrGS2, Precision::Confusion());
  if (!Intersector.IsDone() || Intersector.NbLines() == 0)
  {
    return OffsetEdge;
  }

  Handle(Geom_Curve) IntCurve = Intersector.Line(1);
  gp_Pnt Ends [2];
  BRepAdaptor_Curve aBAcurve(theEdge);
  Ends[0] = aBAcurve.Value(aBAcurve.FirstParameter());
  Ends[1] = aBAcurve.Value(aBAcurve.LastParameter());

  if (Intersector.NbLines() > 1)
  {
    TColGeom_SequenceOfCurve Curves, NewCurves;
    for (Standard_Integer i = 1; i <= Intersector.NbLines(); i++)
      Curves.Append(Intersector.Line(i));

    ConcatCurves(Curves, NewCurves);

    Standard_Real MinDist = RealLast();
    Standard_Integer imin = 1;
    for (Standard_Integer i = 1; i <= NewCurves.Length(); i++)
    {
      GeomAdaptor_Curve GAcurve(NewCurves(i));
      Extrema_ExtPC Projector(Ends[0], GAcurve);
      if (!Projector.IsDone() || Projector.NbExt() == 0)
        continue;
      for (Standard_Integer iext = 1; iext <= Projector.NbExt(); iext++)
      {
        Standard_Real aDist = Projector.SquareDistance(iext);
        if (aDist < MinDist)
        {
          MinDist = aDist;
          imin = i;
        }
      }
    }
    IntCurve = NewCurves(imin);
  }
  if (IntCurve.IsNull())
  {
    return OffsetEdge;
  }
  //Projection of extremities onto <IntCurve>
  GeomAdaptor_Curve GAcurve(IntCurve);
  Standard_Real Params [2];
  for (Standard_Integer ind_end = 0; ind_end < 2; ind_end++)
  {
    if (ind_end == 1 && aBAcurve.IsClosed()/*HGuide->IsPeriodic()*//*HGuide->IsClosed()*/)
      break;
    Extrema_ExtPC Projector(Ends[ind_end], GAcurve);
    Standard_Real param[4], dist[4];
    gp_Pnt Pnt[4];
    param[1] = GAcurve.FirstParameter();
    param[2] = GAcurve.LastParameter();
    Projector.TrimmedSquareDistances(dist[1], dist[2], Pnt[1], Pnt[2]);
    dist[3] = RealLast();
    if (Projector.IsDone() && Projector.NbExt() > 0)
    {
      Standard_Integer imin = 1;
      for (Standard_Integer i = 2; i <= Projector.NbExt(); i++)
        if (Projector.SquareDistance(i) < Projector.SquareDistance(imin))
          imin = i;
      param[3] = Projector.Point(imin).Parameter();
      dist[3]  = Projector.SquareDistance(imin);
      Pnt[3]   = Projector.Point(imin).Value();
    }

    Standard_Integer imin = 1;
    for (Standard_Integer i = 2; i <= 3; i++)
      if (dist[i] < dist[imin])
        imin = i;
    
    Params[ind_end] = param[imin]; //Projector.Point(imin).Parameter();
  }
  if (aBAcurve.IsClosed()/*HGuide->IsPeriodic()*//*HGuide->IsClosed()*/)
    Params[1] = GAcurve.LastParameter(); //temporary
  if (Params[0] > Params[1])
  {
    Standard_Boolean IsClosed = Standard_False;
    gp_Pnt fpnt = IntCurve->Value(IntCurve->FirstParameter());
    gp_Pnt lpnt = IntCurve->Value(IntCurve->LastParameter());
    if (fpnt.SquareDistance(lpnt) <= Precision::SquareConfusion())
      IsClosed = Standard_True;
    if (IsClosed)
      Params[1] = IntCurve->LastParameter();
    else
    {
      Standard_Real NewFirstPar = IntCurve->ReversedParameter(Params[0]);
      Standard_Real NewLastPar  = IntCurve->ReversedParameter(Params[1]);
      IntCurve->Reverse();
      Params[0] = NewFirstPar;
      Params[1] = NewLastPar;
    }
  }
  if (aBAcurve.IsClosed()/*HGuide->IsPeriodic()*//*HGuide->IsClosed()*/) //check the direction of closed curve
  {
    gp_Pnt aPnt, anOffsetPnt;
    gp_Vec Tangent, OffsetTangent;
    aBAcurve.D1(aBAcurve.FirstParameter(), aPnt, Tangent);
    IntCurve->D1(Params[0], anOffsetPnt, OffsetTangent);
    if (Tangent*OffsetTangent < 0)
      IntCurve->Reverse();
  }

  /*
  Standard_Real ParTol = 1.e-5;
  Standard_Real FirstDiff = aBAcurve.FirstParameter() - Params[0];
  Standard_Real LastDiff  = aBAcurve.LastParameter()  - Params[1];
  if (Abs(FirstDiff) > ParTol ||
      Abs(LastDiff)  > ParTol)
  {
    Handle(Geom_BSplineCurve) BsplCurve = Handle(Geom_BSplineCurve)::DownCast(IntCurve);
    TColStd_Array1OfReal aKnots(1, BsplCurve->NbKnots());
    BsplCurve->Knots(aKnots);
    BSplCLib::Reparametrize(aBAcurve.FirstParameter(), aBAcurve.LastParameter(), aKnots);
    BsplCurve->SetKnots(aKnots);
    if (aBAcurve.IsPeriodic() && !BsplCurve->IsPeriodic())
      BsplCurve->SetPeriodic();
    IntCurve = BsplCurve;
  }
  */
  
  OffsetEdge = BRepLib_MakeEdge(IntCurve, Params[0], Params[1]);
  return OffsetEdge;
}

static TopOpeBRepDS_BuildTool mkbuildtool()
{
  TopOpeBRepTool_GeomTool GT2(TopOpeBRepTool_BSPLINE1,
			      Standard_True,
			      Standard_False,
			      Standard_False);
  TopOpeBRepDS_BuildTool BT(GT2);
  BT.OverWrite(Standard_False);
  BT.Translate(Standard_False);
  return BT;
}

//=======================================================================
//function : ChFi3d_Builder
//purpose  : 
//=======================================================================
ChFi3d_Builder::ChFi3d_Builder(const TopoDS_Shape& S,
			       const Standard_Real Ta) :  
   done(Standard_False), myShape(S)
{
  myDS = new TopOpeBRepDS_HDataStructure();
  myCoup = new TopOpeBRepBuild_HBuilder(mkbuildtool());
  myEFMap.Fill(S,TopAbs_EDGE,TopAbs_FACE);
  myESoMap.Fill(S,TopAbs_EDGE,TopAbs_SOLID);
  myEShMap.Fill(S,TopAbs_EDGE,TopAbs_SHELL);
  myVFMap.Fill(S,TopAbs_VERTEX,TopAbs_FACE);
  myVEMap.Fill(S,TopAbs_VERTEX,TopAbs_EDGE);
  SetParams(Ta,1.e-4,1.e-5,1.e-4,1.e-5,1.e-3);
  SetContinuity(GeomAbs_C1, Ta);
}

//=======================================================================
//function : SetParams
//purpose  : 
//=======================================================================

void ChFi3d_Builder::SetParams(const Standard_Real Tang, 
			       const Standard_Real Tesp, 
			       const Standard_Real T2d, 
			       const Standard_Real TApp3d, 
			       const Standard_Real TolApp2d, 
			       const Standard_Real Fleche)
{
  angular = Tang;
  tolesp = Tesp;
  tol2d =  T2d;
  tolapp3d = TApp3d;
  tolapp2d = TolApp2d;
  fleche = Fleche;
}

//=======================================================================
//function : SetContinuity
//purpose  : 
//=======================================================================

void ChFi3d_Builder::SetContinuity(const GeomAbs_Shape InternalContinuity,
				   const Standard_Real AngularTolerance)
{
  myConti = InternalContinuity;
  tolappangle = AngularTolerance;
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean ChFi3d_Builder::IsDone() const 
{
  return done;
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

TopoDS_Shape ChFi3d_Builder::Shape()const
{
  Standard_NoSuchObject_Raise_if (!done, "ChFi3d_Builder::Shape() - no result");
  return myShapeResult;
}

//=======================================================================
//function : NbFaultyContours
//purpose  : 
//=======================================================================

Standard_Integer ChFi3d_Builder::NbFaultyContours() const
{
  return badstripes.Extent();
}

//=======================================================================
//function : FaultyContour
//purpose  : 
//=======================================================================

Standard_Integer ChFi3d_Builder::FaultyContour(const Standard_Integer I) const
{
  ChFiDS_ListIteratorOfListOfStripe itel;
  Standard_Integer k = 0;
  Handle(ChFiDS_Stripe) st;
  for (itel.Initialize(badstripes);itel.More(); itel.Next()) {
    k += 1;
    if(k == I) {
      st = itel.Value();
      break;
    }
  }
  if(st.IsNull()) return 0;
  k = 0;
  for (itel.Initialize(myListStripe);itel.More(); itel.Next()) {
    k += 1;
    if(st == itel.Value()) return k;
  }
  return 0;
}

//=======================================================================
//function : NbComputedSurfaces
//purpose  : 
//=======================================================================

Standard_Integer ChFi3d_Builder::NbComputedSurfaces(const Standard_Integer IC) const
{
  ChFiDS_ListIteratorOfListOfStripe itel;
  Standard_Integer k = 0;
  Handle(ChFiDS_Stripe) st;
  for (itel.Initialize(myListStripe);itel.More(); itel.Next()) {
    k += 1;
    if(k == IC) {
      st = itel.Value();
      break;
    }
  }
  if(st.IsNull()) return 0;
  if(st->Spine().IsNull()) return 0;
  Handle(ChFiDS_HData) hd = st->SetOfSurfData();
  if(hd.IsNull()) return 0;
  return hd->Length();
}

//=======================================================================
//function : ComputedSurface
//purpose  : 
//=======================================================================

Handle(Geom_Surface) ChFi3d_Builder::ComputedSurface(const Standard_Integer IC,
						     const Standard_Integer IS) const
{
 ChFiDS_ListIteratorOfListOfStripe itel;
  Standard_Integer k = 0;
  Handle(ChFiDS_Stripe) st;
  for (itel.Initialize(myListStripe);itel.More(); itel.Next()) {
    k += 1;
    if(k == IC) {
      st = itel.Value();
      break;
    }
  }
  Handle(ChFiDS_HData) hd = st->SetOfSurfData();
  Standard_Integer isurf=hd->Value(IS)->Surf();
  return  myDS->Surface(isurf).Surface();
}

//=======================================================================
//function : NbFaultyVertices
//purpose  : 
//=======================================================================

Standard_Integer ChFi3d_Builder::NbFaultyVertices() const
{
  return badvertices.Extent();
}

//=======================================================================
//function : FaultyVertex
//purpose  : 
//=======================================================================

TopoDS_Vertex ChFi3d_Builder::FaultyVertex(const Standard_Integer IV) const
{
  TopTools_ListIteratorOfListOfShape it;
  TopoDS_Vertex V;
  Standard_Integer k = 0;
  for(it.Initialize(badvertices);it.More(); it.Next()) {
    k += 1;
    if(k == IV) {
      V = TopoDS::Vertex(it.Value());
      break;
    }
  }
  return V;
}

//=======================================================================
//function : HasResult
//purpose  : 
//=======================================================================

Standard_Boolean ChFi3d_Builder::HasResult() const 
{
  return hasresult;
}

//=======================================================================
//function : BadShape
//purpose  : 
//=======================================================================

TopoDS_Shape ChFi3d_Builder::BadShape()const
{
  Standard_NoSuchObject_Raise_if (!hasresult, "ChFi3d_Builder::BadShape() - no result");
  return badShape;
}

//=======================================================================
//function : StripeStatus
//purpose  : 
//=======================================================================

ChFiDS_ErrorStatus ChFi3d_Builder::StripeStatus(const Standard_Integer IC)const
{  
  ChFiDS_ListIteratorOfListOfStripe itel;
  Standard_Integer k =0;
  Handle(ChFiDS_Stripe) st;
  for (itel.Initialize(myListStripe);itel.More(); itel.Next()) {
    k += 1;
    if(k == IC) {
      st = itel.Value();
      break;
    }
  }
  ChFiDS_ErrorStatus stat=st->Spine()->ErrorStatus();
  return stat;
}

//=======================================================================
//function : Builder
//purpose  : 
//=======================================================================

Handle(TopOpeBRepBuild_HBuilder) ChFi3d_Builder::Builder()const
{
  return myCoup;
}

//=======================================================================
//function : ChFi3d_FaceTangency
//purpose  : determine if the faces opposing to edges are tangent
//           to go from opposing faces on e0 to opposing faces 
//           on e1, consider all faces starting at a common top.
//=======================================================================

Standard_Boolean ChFi3d_Builder::FaceTangency(const TopoDS_Edge& E0,
					      const TopoDS_Edge& E1,
					      const TopoDS_Vertex& V) const
{
  TopTools_ListIteratorOfListOfShape It,Jt;
  TopoDS_Edge Ec;
  Standard_Integer Nbf;
  TopoDS_Face F[2];

  //It is checked if the connection is not on a regular edge.
  for (It.Initialize(myEFMap(E1)), Nbf= 0 ;It.More();It.Next(), Nbf++) {
    if (Nbf>1) 
      throw Standard_ConstructionError("ChFi3d_Builder:only 2 faces");
    F[Nbf] = TopoDS::Face(It.Value());
  }      
  if(Nbf < 2) return Standard_False;
//  Modified by Sergey KHROMOV - Fri Dec 21 17:44:19 2001 Begin
//if (BRep_Tool::Continuity(E1,F[0],F[1]) != GeomAbs_C0) {
  if (ChFi3d::IsTangentFaces(E1,F[0],F[1])) {
//  Modified by Sergey KHROMOV - Fri Dec 21 17:44:21 2001 End
    return Standard_False;
  }

  for (Jt.Initialize(myVEMap(V));Jt.More();Jt.Next()) {
    Ec = TopoDS::Edge(Jt.Value());
    if (!Ec.IsSame(E0) && !Ec.IsSame(E1) && 
	Ec.Orientation() != TopAbs_INTERNAL && 
	Ec.Orientation() != TopAbs_EXTERNAL &&
	!BRep_Tool::Degenerated(Ec)) {
      for (It.Initialize(myEFMap(Ec)), Nbf= 0 ;It.More();It.Next(), Nbf++) {
	if (Nbf>1) 
	  throw Standard_ConstructionError("ChFi3d_Builder:only 2 faces");
	F[Nbf] = TopoDS::Face(It.Value());
      }      
      if(Nbf < 2) return Standard_False;
//  Modified by Sergey KHROMOV - Tue Dec 18 18:10:40 2001 Begin
//    if (BRep_Tool::Continuity(Ec,F[0],F[1]) < GeomAbs_G1) {
      if (!ChFi3d::IsTangentFaces(Ec,F[0],F[1])) {
//  Modified by Sergey KHROMOV - Tue Dec 18 18:10:41 2001 End
	return Standard_False;
      }
    }
  }
  return Standard_True;
  
}


//=======================================================================
//function : TangentExtremity
//purpose  : Test if 2 faces are tangent at the end of an edge
//=======================================================================
static Standard_Boolean TangentExtremity(const TopoDS_Vertex&                V,
					 const TopoDS_Edge&                  E,
					 const Handle(BRepAdaptor_HSurface)& hs1,
					 const Handle(BRepAdaptor_HSurface)& hs2,
//					 const Standard_Real                 t3d,
					 const Standard_Real                 tang)
{
  TopoDS_Face f1 = hs1->ChangeSurface().Face();
  TopAbs_Orientation O1 = f1.Orientation();
  f1.Orientation(TopAbs_FORWARD);
  TopoDS_Face f2 = hs2->ChangeSurface().Face();
  TopAbs_Orientation O2 = f2.Orientation();
  f2.Orientation(TopAbs_FORWARD);
  TopoDS_Edge e1 = E, e2 = E;
  e1.Orientation(TopAbs_FORWARD);
  e2.Orientation(TopAbs_FORWARD);
  if(f1.IsSame(f2) && BRep_Tool::IsClosed(e1,f1))
    e2.Orientation(TopAbs_REVERSED);
  Standard_Real p1 = BRep_Tool::Parameter(V,e1,f1);
  Standard_Real p2 = BRep_Tool::Parameter(V,e2,f2);
  Standard_Real u,v,f,l, Eps = 1.e-9;
  gp_Vec n1, n2;//   gp_Pnt pt1,pt2;
  Handle(Geom2d_Curve) pc1 = BRep_Tool::CurveOnSurface(e1,f1,f,l);
  pc1->Value(p1).Coord(u,v);
  BRepLProp_SLProps theProp1(hs1->ChangeSurface(), u, v, 1, Eps);
  if  (theProp1.IsNormalDefined()) {
    n1.SetXYZ(theProp1.Normal().XYZ());
    if (O1 == TopAbs_REVERSED) n1.Reverse();
  }
  else return Standard_False; // It is not known...

 
  Handle(Geom2d_Curve) pc2 = BRep_Tool::CurveOnSurface(e2,f2,f,l);
  pc2->Value(p2).Coord(u,v);
  BRepLProp_SLProps theProp2(hs2->ChangeSurface(), u, v, 1, Eps);
  if  (theProp2.IsNormalDefined()) {
    n2.SetXYZ(theProp2.Normal().XYZ());
    if(O2 == TopAbs_REVERSED) n2.Reverse();
  }
  else return Standard_False; //  It is not known...

  return (n1.Angle(n2) < tang);
}

//=======================================================================
//function : TangentOnVertex
//purpose  : Test if support faces of an edge are tangent at end.
//=======================================================================
static Standard_Boolean TangentOnVertex(const TopoDS_Vertex&    V,
					const TopoDS_Edge&      E,
					const ChFiDS_Map&       EFMap,
					const Standard_Real     tang)
{
  TopoDS_Face ff1,ff2;
  ChFi3d_conexfaces(E,ff1,ff2,EFMap);
  if(ff1.IsNull() || ff2.IsNull()) return 0;
  Handle(BRepAdaptor_HSurface) S1 = new (BRepAdaptor_HSurface)(ff1);
  Handle(BRepAdaptor_HSurface) S2 = new (BRepAdaptor_HSurface)(ff2);
  return TangentExtremity(V, E, S1, S2, tang);
}

//=======================================================================
//function : PerformExtremity
//purpose  : In case if PerformElement returned BreakPoint at one or  
//           another extremity, it is attempted to refine 
//           depending on concavities between neighbour faces of the top.
//=======================================================================

void ChFi3d_Builder::PerformExtremity (const Handle(ChFiDS_Spine)& Spine) 
{
  Standard_Integer NbG1Connections = 0;
  
  for(Standard_Integer ii = 1; ii <= 2; ii++){
    TopoDS_Edge E[3];
    TopoDS_Vertex V;
    ChFiDS_State sst;
    Standard_Integer iedge;
    Handle(BRepAdaptor_HSurface) hs1,hs2;
    if(ii == 1){
      sst = Spine->FirstStatus();
      iedge = 1;
      V = Spine->FirstVertex();
    }
    else{
      sst = Spine->LastStatus(); 
      iedge = Spine->NbEdges();
      E[0] = Spine->Edges(iedge);
      V = Spine->LastVertex();
    }
    //Before all it is checked if the tangency is not dead.
    E[0] = Spine->Edges(iedge);
    ConexFaces (Spine,iedge,hs1,hs2);
    if(TangentExtremity(V,E[0],hs1,hs2,angular)){
      Spine->SetTangencyExtremity(Standard_True, (ii == 1));
    }

    if(sst == ChFiDS_BreakPoint){
      TopTools_ListIteratorOfListOfShape It;//,Jt;
      Standard_Boolean sommetpourri = Standard_False;
      TopTools_IndexedMapOfOrientedShape EdgesOfV;
      TopTools_MapOfShape Edges;
      Edges.Add(E[0]);
      EdgesOfV.Add(E[0]);
      Standard_Integer IndOfE = 0;
      for (It.Initialize(myVEMap(V)); It.More(); It.Next())
      {
        TopoDS_Edge anEdge = TopoDS::Edge(It.Value());
        if (BRep_Tool::Degenerated(anEdge))
          continue;
        TopoDS_Face F1, F2;
        ChFi3d_conexfaces(anEdge, F1, F2, myEFMap);
        if (!F2.IsNull() && ChFi3d::IsTangentFaces(anEdge, F1, F2, GeomAbs_G2)) //smooth edge
        {
          if (!F1.IsSame(F2))
            NbG1Connections++;
          continue;
        }
        
        if (Edges.Add(anEdge))
        {
          EdgesOfV.Add(anEdge);
          if (IndOfE < 2)
          {
            IndOfE++;
            E[IndOfE] = anEdge;
          }
        }
        else
        {
          TopoDS_Vertex V1, V2;
          TopExp::Vertices(anEdge, V1, V2);
          if (V1.IsSame(V2)) //edge is closed - two ends of the edge in the vertex
          {
            Standard_Integer anInd = EdgesOfV.FindIndex(anEdge);
            if (anInd == 0)
              anInd = EdgesOfV.FindIndex(anEdge.Reversed());
            anEdge = TopoDS::Edge(EdgesOfV(anInd));
            anEdge.Reverse();
            if (EdgesOfV.Add(anEdge))
            {
              if (IndOfE < 2)
              {
                IndOfE++;
                E[IndOfE] = anEdge;
              }
            }
          }
        }
      }
      
      if (EdgesOfV.Extent() != 3)
        sommetpourri = Standard_True;
      
      if(!sommetpourri){
	sst = ChFi3d_EdgeState(E,myEFMap);
      }
      if(ii==1)Spine->SetFirstStatus(sst);
      else Spine->SetLastStatus(sst);
    }
  }
  
  if (!Spine->IsPeriodic()) {
    TopTools_ListIteratorOfListOfShape It,Jt;
    Standard_Integer nbf = 0, jf = 0;
    for (It.Initialize(myVFMap(Spine->FirstVertex())); It.More(); It.Next()){
      jf++;
      Standard_Integer kf = 1;
      const TopoDS_Shape& cur = It.Value();
      for (Jt.Initialize(myVFMap(Spine->FirstVertex())); Jt.More() && (kf < jf); Jt.Next(), kf++){
	if(cur.IsSame(Jt.Value())) break;
      }
      if(kf == jf) nbf++;
    }
    nbf -= NbG1Connections;
    if(nbf>3) {
      Spine->SetFirstStatus(ChFiDS_BreakPoint);
#ifdef OCCT_DEBUG
      std::cout<<"top has : "<<nbf<<" faces."<<std::endl;
#endif
    }
    nbf = 0, jf = 0;
    for (It.Initialize(myVFMap(Spine->LastVertex())); It.More(); It.Next()){
      jf++;
      Standard_Integer kf = 1;
      const TopoDS_Shape& cur = It.Value();
      for (Jt.Initialize(myVFMap(Spine->LastVertex())); Jt.More() && (kf < jf); Jt.Next(), kf++){
	if(cur.IsSame(Jt.Value())) break;
      }
      if(kf == jf) nbf++;
    }
    nbf -= NbG1Connections;
    if(nbf>3) {
      Spine->SetLastStatus(ChFiDS_BreakPoint);
#ifdef OCCT_DEBUG
      std::cout<<"top has : "<<nbf<<" faces."<<std::endl;
#endif
    }
  }
}

//=======================================================================
//function : PerformElement
//purpose  :  find all mutually tangent edges ;
// Each edge has 2 opposing faces. For 2 adjacent tangent edges it is required that 
// the opposing faces were tangent.
//=======================================================================

Standard_Boolean ChFi3d_Builder::PerformElement(const Handle(ChFiDS_Spine)& Spine,
                                                const Standard_Real         Offset,
                                                const TopoDS_Face&          theFirstFace)
{
  Standard_Real ta = angular;
  TopTools_ListIteratorOfListOfShape It;
  Standard_Integer Nbface;
  TopTools_ListIteratorOfListOfShape Jt;
  Standard_Real Wl,Wf;
  Standard_Boolean degeneOnEc;
  gp_Pnt P2;
  gp_Vec V1,V2;
  TopoDS_Vertex Ve1,VStart,FVEc,LVEc,FVEv,LVEv;
  TopoDS_Edge Ev,Ec(Spine->Edges(1));
  if(BRep_Tool::Degenerated(Ec)) return 0;
  //it is checked if the edge is a cut edge
  TopoDS_Face ff1,ff2;
  ChFi3d_conexfaces(Ec,ff1,ff2,myEFMap);
  if(ff1.IsNull() || ff2.IsNull()) return 0;
//  Modified by Sergey KHROMOV - Fri Dec 21 17:46:22 2001 End
//if(BRep_Tool::Continuity(Ec,ff1,ff2) != GeomAbs_C0) return 0;
  if (ChFi3d::IsTangentFaces(Ec,ff1,ff2)) return 0;
//  Modified by Sergey KHROMOV - Fri Dec 21 17:46:24 2001 Begin

  TopoDS_Face FirstFace = ff1;
  if (!theFirstFace.IsNull() && ff2.IsSame(theFirstFace))
  {
    FirstFace = ff2;
    ff2 = ff1; ff1 = FirstFace;
  }
  myEdgeFirstFace.Bind(Ec, FirstFace);

  //Define concavity
  ChFiDS_TypeOfConcavity TypeOfConcavity = ChFi3d::DefineConnectType(Ec, ff1, ff2,
                                                                     1.e-5, Standard_True);
  Spine->SetTypeOfConcavity(TypeOfConcavity);
  
  Standard_Boolean ToRestrict = (Offset > 0)? Standard_True : Standard_False;
  BRepAdaptor_Surface Sb1(ff1, ToRestrict);
  BRepAdaptor_Surface Sb2(ff2, ToRestrict);
  if (Offset > 0)
  {
    TopoDS_Edge OffsetEdge = MakeOffsetEdge(Ec, Offset, Sb1, Sb2);
    OffsetEdge.Orientation(Ec.Orientation());
    Spine->SetOffsetEdges(OffsetEdge);
  }
  
  BRepAdaptor_Curve CEc,CEv;
  TopAbs_Orientation curor = Ec.Orientation();
  TopExp::Vertices(Ec,VStart,LVEc);


  Standard_Boolean Fini = Standard_False;
  Standard_Integer Nb;
  ChFiDS_State CurSt = ChFiDS_Closed;
  if (VStart.IsSame(LVEc)) {//case if only one edge is closed
    CEc.Initialize(Ec);
    Wl = BRep_Tool::Parameter(VStart,Ec);
    CEc.D1(Wl,P2,V1);
    Wl = BRep_Tool::Parameter(LVEc,Ec);
    CEc.D1(Wl,P2,V2);
    Standard_Boolean IsFaceTangency = FaceTangency(Ec,Ec,VStart);
    if (V1.IsParallel(V2,ta) ||
        IsFaceTangency)
    {
      if (IsFaceTangency) {
	CurSt = ChFiDS_Closed; 
      }
      else {
	CurSt = ChFiDS_BreakPoint; 
      }	
    }
    else {
      CurSt = ChFiDS_BreakPoint; 
    }
    Spine->SetLastStatus(CurSt);
    Spine->SetFirstStatus(CurSt);
  }
  else { // Downstream progression
    FVEc = VStart;
    TopAbs_Orientation Or1;
    while (!Fini) {
      CurSt = ChFiDS_FreeBoundary;
      Wl = BRep_Tool::Parameter(LVEc,Ec);
      degeneOnEc = TangentOnVertex(LVEc, Ec, myEFMap, ta);
      CEc.Initialize(Ec);
      CEc.D1(Wl,P2,V1);
      Nb = Spine->NbEdges();

      for (It.Initialize(myVEMap(LVEc));It.More();It.Next()) {
	Ev = TopoDS::Edge(It.Value());
	if (!Ev.IsSame(Ec) && !BRep_Tool::Degenerated(Ev)){
	  TopExp::Vertices(Ev,FVEv,LVEv);
	  if (LVEc.IsSame(LVEv)) {
	    Ve1 = FVEv;
	    FVEv = LVEv;
	    LVEv = Ve1;
	    Or1 = TopAbs_REVERSED;
	  }
	  else Or1 = TopAbs_FORWARD;

	  Wf = BRep_Tool::Parameter(FVEv,Ev);
	  CEv.Initialize(Ev);
	  CEv.D1(Wf,P2,V2);   
	  Standard_Real av1v2 = V1.Angle(V2);
	  Standard_Boolean rev = (Or1 != curor);    
	  Standard_Boolean OnAjoute = Standard_False;
	  if (FaceTangency(Ec,Ev,FVEv)) {
	    // there is no need of tolerance
	    // to make a decision (PRO9486) the regularity is enough.
            // However, the abcense of turn-back is checked (PRO9810)
	    OnAjoute = ((!rev && av1v2 < M_PI/2) 
			||(rev && av1v2 > M_PI/2));
            // mate attention to the single case (cf CTS21610_1)
            if (OnAjoute && (degeneOnEc || 
                TangentOnVertex(LVEc, Ev,myEFMap, ta)) )
	      OnAjoute=((!rev && av1v2 < ta) || (rev && (M_PI - av1v2) < ta));
	  }
	  if (OnAjoute) {
	    Fini = Standard_False; // If this can be useful (Cf PRO14713)
            TopoDS_Vertex CommonVertex;
            TopExp::CommonVertex(Ec, Ev, CommonVertex);
            TopoDS_Edge PrevEdge = Ec;
	    Ec = Ev; 
//	    Ec = TopoDS::Edge(Ev); 
	    Ec.Orientation(Or1);
	    Wl = Wf; LVEc = LVEv; 
	    Spine->SetEdges(Ec);
            TopoDS_Face CurF1, CurF2;
            ChFi3d_conexfaces(Ec,CurF1,CurF2,myEFMap);
            ReorderFaces(CurF1, CurF2, FirstFace, PrevEdge, CommonVertex, myEFMap);
            myEdgeFirstFace.Bind(Ec, CurF1);
            if (Offset > 0)
            {
              BRepAdaptor_Surface CurSb1(CurF1), CurSb2(CurF2);
              TopoDS_Edge anOffsetEdge = MakeOffsetEdge(Ec, Offset, CurSb1, CurSb2);
              anOffsetEdge.Orientation(Or1);
              Spine->SetOffsetEdges(anOffsetEdge);
            }
            FirstFace = CurF1;
	    curor = Or1;
	    if (VStart.IsSame(LVEv)) {
	      if (FaceTangency(Ev,Spine->Edges(1),LVEv)) {
		CurSt = ChFiDS_Closed; Fini = Standard_True;
	      }
	      else {
		CurSt = ChFiDS_BreakPoint;Fini = Standard_True;
	      }
	    }
	    break;
	  }
	  else {
	    for (Jt.Initialize(myEFMap(Ev)), Nbface= 0 ;Jt.More();Jt.Next(), 
		 Nbface++) {}
	    if (Nbface> 1) CurSt = ChFiDS_BreakPoint;
	    Fini = ((!rev && av1v2 < ta) || (rev && (M_PI - av1v2) < ta)); 
	  }
	} 
      } 
      Fini = Fini || (Nb == Spine->NbEdges());
    }
    Spine->SetLastStatus(CurSt);
    if (CurSt == ChFiDS_Closed) {
      Spine->SetFirstStatus(CurSt);
    }
    else {// Upstream progression
      Fini = Standard_False;
      Ec = Spine->Edges(1);
      FirstFace = TopoDS::Face(myEdgeFirstFace(Ec));
      curor = Ec.Orientation();
      FVEc = VStart;
      while (!Fini) {
	CurSt = ChFiDS_FreeBoundary;
	Wl = BRep_Tool::Parameter(FVEc,Ec);
	degeneOnEc = TangentOnVertex(FVEc, Ec, myEFMap, ta);
	CEc.Initialize(Ec);
	CEc.D1(Wl,P2,V1);
	Nb = Spine->NbEdges();

	for (It.Initialize(myVEMap(FVEc));It.More();It.Next()) {
	  Ev = TopoDS::Edge(It.Value());
	  if (!Ev.IsSame(Ec) && !BRep_Tool::Degenerated(Ev)) {
	    TopExp::Vertices(Ev,FVEv,LVEv);
	    if (FVEc.IsSame(FVEv)) {
	      Ve1 = FVEv;
	      FVEv = LVEv;
	      LVEv = Ve1;
	      Or1 = TopAbs_REVERSED;
	    }
	    else {
	      Or1 = TopAbs_FORWARD;
	    }
	    Wf = BRep_Tool::Parameter(LVEv,Ev);
	    CEv.Initialize(Ev);
	    CEv.D1(Wf,P2,V2);
	    Standard_Real av1v2 = V1.Angle(V2);
	    Standard_Boolean rev = (Or1 != curor);
            Standard_Boolean OnAjoute =  Standard_False;
	    if (FaceTangency(Ec,Ev,LVEv)) {
	      OnAjoute = ((!rev && av1v2 < M_PI/2) 
			||(rev && av1v2 > M_PI/2));
            if (OnAjoute && (degeneOnEc || 
                TangentOnVertex(FVEc, Ev,myEFMap, ta)) )
	      OnAjoute=((!rev && av1v2 < ta) || (rev && (M_PI-av1v2) < ta));
	    }
	    if  (OnAjoute) {
              TopoDS_Vertex CommonVertex;
              TopExp::CommonVertex(Ec, Ev, CommonVertex);
              TopoDS_Edge PrevEdge = Ec;
	      Ec = Ev; 
//	      Ec = TopoDS::Edge(Ev); 
	      Ec.Orientation(Or1);
	      Wl = Wf; FVEc = FVEv; 
	      Spine->PutInFirst(Ec);
              TopoDS_Face CurF1, CurF2;
              ChFi3d_conexfaces(Ec,CurF1,CurF2,myEFMap);
              ReorderFaces(CurF1, CurF2, FirstFace, PrevEdge, CommonVertex, myEFMap);
              myEdgeFirstFace.Bind(Ec, CurF1);
              if (Offset > 0)
              {
                BRepAdaptor_Surface CurSb1(CurF1), CurSb2(CurF2);
                TopoDS_Edge anOffsetEdge = MakeOffsetEdge(Ec, Offset, CurSb1, CurSb2);
                anOffsetEdge.Orientation(Or1);
                Spine->PutInFirstOffset(anOffsetEdge);
              }
              FirstFace = CurF1;
	      curor = Or1;
	      break;
	    }
	    else {
	      for(Jt.Initialize(myEFMap(Ev)),Nbface= 0 ;Jt.More();Jt.Next(), 
		  Nbface++) {}
	      if (Nbface> 1) CurSt = ChFiDS_BreakPoint;
	      Fini = ((!rev && av1v2 < ta) || (rev && (M_PI - av1v2) < ta));
	    }
	  } 
	} 
	Fini = Fini || (Nb == Spine->NbEdges());
      }
      Spine->SetFirstStatus(CurSt);
    }
  }
  return 1;
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void  ChFi3d_Builder::Remove(const TopoDS_Edge& E)
{
  ChFiDS_ListIteratorOfListOfStripe itel(myListStripe);

  for ( ; itel.More(); itel.Next()) {
    const Handle(ChFiDS_Spine)& sp = itel.Value()->Spine();
    for (Standard_Integer j = 1; j <= sp->NbEdges(); j++){
      if (E.IsSame(sp->Edges(j))){
	myListStripe.Remove(itel);
	return;
      }
    }
  }
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Handle(ChFiDS_Spine) ChFi3d_Builder::Value
(const Standard_Integer I)const 
{
  ChFiDS_ListIteratorOfListOfStripe itel(myListStripe);
  for (Standard_Integer ic = 1; ic < I; ic++) {itel.Next();}
  return itel.Value()->Spine();
}

//=======================================================================
//function : NbElements
//purpose  : 
//=======================================================================

Standard_Integer ChFi3d_Builder::NbElements()const 
{
  Standard_Integer i = 0;
  ChFiDS_ListIteratorOfListOfStripe itel(myListStripe);
  for ( ;itel.More(); itel.Next()){
    const Handle(ChFiDS_Spine)& sp = itel.Value()->Spine();
    if(sp.IsNull()) break;
    i++;
  }
  return i;
}

//=======================================================================
//function : Contains
//purpose  : 
//=======================================================================

Standard_Integer ChFi3d_Builder::Contains(const TopoDS_Edge& E)const
{
  Standard_Integer i = 1,j;
  ChFiDS_ListIteratorOfListOfStripe itel(myListStripe);
  for ( ;itel.More(); itel.Next(), i++){
    const Handle(ChFiDS_Spine)& sp = itel.Value()->Spine();
    if(sp.IsNull()) break;
    for (j = 1; j <= sp->NbEdges(); j++){
      if(E.IsSame(sp->Edges(j))) return i;
    }
  }
  return 0;
}

//=======================================================================
//function : Contains
//purpose  : 
//=======================================================================

Standard_Integer ChFi3d_Builder::Contains(const TopoDS_Edge& E,
					  Standard_Integer&  IndexInSpine)const
{
  Standard_Integer i = 1,j;
  IndexInSpine = 0;
  ChFiDS_ListIteratorOfListOfStripe itel(myListStripe);
  for ( ;itel.More(); itel.Next(), i++){
    const Handle(ChFiDS_Spine)& sp = itel.Value()->Spine();
    if(sp.IsNull()) break;
    for (j = 1; j <= sp->NbEdges(); j++){
      if(E.IsSame(sp->Edges(j)))
	{
	  IndexInSpine = j;
	  return i;
	}
    }
  }
  return 0;
}

//=======================================================================
//function : Length
//purpose  : 
//=======================================================================

Standard_Real ChFi3d_Builder::Length(const Standard_Integer IC)const
{
  if(IC <= NbElements()){
    const Handle(ChFiDS_Spine)& sp = Value(IC);
    return sp->LastParameter(sp->NbEdges());
  }
  return -1;
}


//=======================================================================
//function : FirstVertex
//purpose  : 
//=======================================================================

TopoDS_Vertex ChFi3d_Builder::FirstVertex(const Standard_Integer IC) const
{
  if(IC <= NbElements()){
    return Value(IC)->FirstVertex();
  }
  return TopoDS_Vertex();
}

//=======================================================================
//function : LastVertex
//purpose  : 
//=======================================================================

TopoDS_Vertex ChFi3d_Builder::LastVertex(const Standard_Integer IC) const
{
  if(IC <= NbElements()){
    return Value(IC)->LastVertex();
  }
  return TopoDS_Vertex();
}

//=======================================================================
//function : Abscissa
//purpose  : 
//=======================================================================

Standard_Real ChFi3d_Builder::Abscissa(const Standard_Integer IC,
				       const TopoDS_Vertex& V) const
{
  if(IC <= NbElements()){
    return Value(IC)->Absc(V);
  }
  return -1;
}

//=======================================================================
//function : RelativeAbscissa
//purpose  : 
//=======================================================================

Standard_Real ChFi3d_Builder::RelativeAbscissa(const Standard_Integer IC,
					       const TopoDS_Vertex& V) const
{
  if(IC <= NbElements()){
    return Abscissa(IC,V)/Length(IC);
  }
  return -1;
}

//=======================================================================
//function : Closed
//purpose  : 
//=======================================================================

Standard_Boolean ChFi3d_Builder::Closed(const Standard_Integer IC)const
{
  if(IC <= NbElements()){
    return Value(IC)->IsClosed();
  }
  return Standard_False;
}

//=======================================================================
//function : ClosedAndTangent
//purpose  : 
//=======================================================================

Standard_Boolean ChFi3d_Builder::ClosedAndTangent
(const Standard_Integer IC)const
{
  if(IC <= NbElements()){
    return Value(IC)->IsPeriodic();
  }
  return Standard_False;
}

