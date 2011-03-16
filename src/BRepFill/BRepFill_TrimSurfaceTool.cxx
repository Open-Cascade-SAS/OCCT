// File:	BRepFill_TrimSurfaceTool.cxx
// Created:	Fri Oct 21 14:56:32 1994
// Author:	Bruno DUMORTIER
//		<dub@fuegox>


#include <stdio.h>

#include <BRepFill_TrimSurfaceTool.ixx>

#include <Precision.hxx>

#include <Adaptor3d_SurfaceOfRevolution.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <BRepFill_ComputeCLine.hxx>
#include <BRepFill_MultiLine.hxx>
#include <BRepIntCurveSurface_Inter.hxx>
#include <BRepFill_ApproxSeewing.hxx>
#include <BRep_Tool.hxx>
#include <ElCLib.hxx>
#include <BSplCLib.hxx>
#include <PLib.hxx>
#include <gp.hxx>
#include <gp_Sphere.hxx>
#include <gp_Cone.hxx>
#include <gp_Torus.hxx>
#include <gp_Cylinder.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomProjLib.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <Precision.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_NotImplemented.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TopAbs_State.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>

#ifdef DRAW
#include <DrawTrSurf.hxx>
#include <DBRep.hxx>
#endif
#ifdef DEB
static Standard_Boolean Affich       = Standard_False;
static Standard_Integer NBCALL  = 1;
#endif

//=======================================================================
//function : BRepFill_TrimSurfaceTool
//purpose  : Initialisation  avec les deux face voisines
//          Edge1 et Edge2 sont les edges paralleles correspondant
//          a une iso minimum sur F1 et F2 respectivement.
//          ie Edge1 est Umin ou VMin sur F1.
//          Inv1 et Inv2 indique si Edge1 et Edge2 sont des
//          parallleles retournees.   
//=======================================================================

BRepFill_TrimSurfaceTool::BRepFill_TrimSurfaceTool
(const Handle(Geom2d_Curve)& Bis, 
 const TopoDS_Face&          Face1, 
 const TopoDS_Face&          Face2,
 const TopoDS_Edge&          Edge1,
 const TopoDS_Edge&          Edge2,
 const Standard_Boolean      Inv1,
 const Standard_Boolean      Inv2 ) :
myFace1(Face1),
myFace2(Face2),
myEdge1(Edge1),
myEdge2(Edge2),
myInv1(Inv1),
myInv2(Inv2),
myBis  (Bis)
{
#ifdef DEB
  if ( Affich) {
    cout << " ---------->TrimSurfaceTool : NBCALL = " << NBCALL << endl;
    
#ifdef DRAW
    char name[256];
    
    sprintf(name,"FACE1_%d",NBCALL);
    DBRep::Set(name,myFace1);
    
    sprintf(name,"FACE2_%d",NBCALL);
    DBRep::Set(name,myFace2);
    
    sprintf(name,"EDGE1_%d",NBCALL);
    DBRep::Set(name,myEdge1);
    
    sprintf(name,"EDGE2_%d",NBCALL);
    DBRep::Set(name,myEdge2);
    
    sprintf(name,"BISSEC_%d",NBCALL);
    DrawTrSurf::Set(name,myBis);
#endif    
    NBCALL++;
  }
#endif
}


//=======================================================================
//function : Bubble
//purpose  : Ordonne la sequence de point en x croissant. 
//=======================================================================

static void Bubble(TColgp_SequenceOfPnt& Seq) 
{
  Standard_Boolean Invert = Standard_True;
  Standard_Integer NbPoints = Seq.Length();
  while (Invert) {
    Invert = Standard_False;
    for ( Standard_Integer i = 1; i < NbPoints; i++) {
      gp_Pnt P1 = Seq.Value(i);
      gp_Pnt P2 = Seq.Value(i+1);
      if (P2.X()<P1.X())  {
	Seq.Exchange(i,i+1);
	Invert = Standard_True;
      }
    }
  }
}


//=======================================================================
//function : EvalPhase
//purpose  : 
//=======================================================================

static Standard_Real EvalPhase(const TopoDS_Edge& Edge,
			       const TopoDS_Face& Face,
			       const GeomAdaptor_Surface& GAS,
			       const gp_Ax3& Axis)
{
  gp_Pnt2d PE1,PE2,PF1,PF2;
  Standard_Real VDeg;
#ifndef DEB
  Standard_Real V = 0.;
#else
  Standard_Real V;
#endif
  BRep_Tool::UVPoints(Edge,Face,PE1,PE2);
  VDeg = PE1.Y();
  TopExp_Explorer Exp(Face,TopAbs_EDGE);
  for (; Exp.More(); Exp.Next()) {
    if ( !TopoDS::Edge(Exp.Current()).IsSame(Edge)) {
      BRep_Tool::UVPoints(TopoDS::Edge(Exp.Current()),Face,PF1,PF2);
      V = ( Abs(PF1.Y() - VDeg) > Abs(PF2.Y() - VDeg)) ? PF1.Y() : PF2.Y();
      break;
    }
  }
  gp_Pnt P = GAS.Value(0., V);
  
  if ( gp_Vec(Axis.Location(), P).Dot(Axis.XDirection()) < 0.) 
    return PI;
  else
    return 0.;
}


//=======================================================================
//function : EvalParameters
//purpose  : 
//=======================================================================

static void EvalParameters(const TopoDS_Edge&          Edge,
			   const TopoDS_Face&          Face,
			   const Handle(Geom2d_Curve)& Bis ,
			         TColgp_SequenceOfPnt& Seq  ) 
{
  Standard_Boolean Degener = BRep_Tool::Degenerated(Edge);
  // recuperer les courbes 3d associees aux edges.
  TopLoc_Location L;
  Standard_Real   f,l;

  Handle(Geom_TrimmedCurve) CT;
  Handle(Geom_Plane) Plane = new Geom_Plane(0,0,1,0);

  Geom2dInt_GInter Intersector;

  Standard_Integer NbPoints, NbSegments;
  Standard_Real U1, U2;
  gp_Pnt  P;//,PSeq;

//  Standard_Real Tol = Precision::Intersection();
  //  modified by NIZHNY-EAP Wed Dec 22 15:00:51 1999 ___BEGIN___
  Standard_Real Tol = 1.e-6;  // BRepFill_Precision();
  Standard_Real TolC = 0.;
  
  if ( !Degener) {
    Handle(Geom_Curve) C = BRep_Tool::Curve(Edge,L,f,l);
    CT = new Geom_TrimmedCurve(C,f,l);
    CT->Transform(L.Transformation());
    // projection de ces courbes 3d dans le plan xOy
    Handle(Geom2d_Curve) C2d = GeomProjLib::Curve2d(CT,Plane);

    Geom2dAdaptor_Curve AC(C2d);
    Geom2dAdaptor_Curve ABis(Bis);

    Intersector = Geom2dInt_GInter(ABis, AC, TolC, Tol);
  
    if ( !Intersector.IsDone()) {
      StdFail_NotDone::Raise("BRepFill_TrimSurfaceTool::IntersectWith");
    }
    
    NbPoints = Intersector.NbPoints();

    if (NbPoints < 1) {
      // try to elongate curves and enlarge tolerance
      
      // don't do it rightaway from the beginning in order not to get
      // extra solutions those would cause *Exception*: incoherent intersection
      
      GeomAbs_CurveType CType = AC.GetType(), BisType = ABis.GetType();
      Standard_Boolean	canElongateC = !(CType == GeomAbs_BezierCurve ||
					 CType == GeomAbs_BSplineCurve ||
					 CType == GeomAbs_OtherCurve);
      Standard_Boolean canElongateBis = !(BisType == GeomAbs_BezierCurve ||
					  BisType == GeomAbs_BSplineCurve ||
					  BisType == GeomAbs_OtherCurve);
      
      Handle(Geom2d_TrimmedCurve) TBis = Handle(Geom2d_TrimmedCurve)::DownCast(Bis);
      Handle(Geom2d_TrimmedCurve) TC2d = Handle(Geom2d_TrimmedCurve)::DownCast(C2d);
      
      if (canElongateC) {
	TC2d->SetTrim(TC2d->FirstParameter() - Tol, TC2d->LastParameter() + Tol);
	AC.Load(TC2d);
      }
      if (canElongateBis) {
	TBis->SetTrim(TBis->FirstParameter() - Tol, TBis->LastParameter() + Tol);
	ABis.Load(TBis);
      }
      Intersector = Geom2dInt_GInter(ABis, AC, TolC, Tol*10);
      
      if ( !Intersector.IsDone()) {
	StdFail_NotDone::Raise("BRepFill_TrimSurfaceTool::IntersectWith");
	}
      
      NbPoints = Intersector.NbPoints();
    }
    //  modified by NIZHNY-EAP Wed Dec 22 15:00:56 1999 ___END___
    if (NbPoints > 0) {
    
      for ( Standard_Integer i = 1; i <= NbPoints; i++) {
	U1 = Intersector.Point(i).ParamOnFirst();
	U2 = Intersector.Point(i).ParamOnSecond();
	P = gp_Pnt(U1,U2,0.);
	Seq.Append(P);
      }
    }

    NbSegments = Intersector.NbSegments();
    
    if (NbSegments > 0) {
#ifdef DEB
      cout << " IntersectWith : " << NbSegments  
	   << " Segments d`intersection" << endl;
#endif
      IntRes2d_IntersectionSegment Seg;
      for ( Standard_Integer i = 1; i <= NbSegments; i++) {
	Seg = Intersector.Segment(i);
	U1  = Seg.FirstPoint().ParamOnFirst();
	U1 += Seg.LastPoint().ParamOnFirst();
	U1 /= 2.;
	U2  = Seg.FirstPoint().ParamOnSecond();
	U2 += Seg.LastPoint().ParamOnSecond();
	U2 /= 2.;
	P = gp_Pnt(U1,U2,0.);
	Seq.Append(P);
      }
    }
    // Ordonne la sequence en param croissant sur la bissectrice.
    Bubble( Seq);
    
//  modified by NIZHNY-EAP Fri Dec 24 18:47:24 1999 ___BEGIN___
    // Remove double points
    gp_Pnt P1, P2;
    for ( Standard_Integer i = 1; i < NbPoints; i++) {
      P1 = Seq.Value(i);
      P2 = Seq.Value(i+1);
      if ( P2.X()-P1.X() < Tol ) {
//	cout<<"REMOVE "<<P1.X()<<endl;
	Seq.Remove(i--);
	NbPoints--;
      }
    }
//  modified by NIZHNY-EAP Fri Dec 24 18:47:28 1999 ___END___
  }
  else {
    // l`edge est degenere : on recupere le point et on cherche s`il est sur
    // la bissectrice.

    gp_Pnt P3d = BRep_Tool::Pnt( TopExp::FirstVertex(Edge));
    gp_Pnt2d P2d( P3d.X(), P3d.Y());

    Standard_Real UBis = Bis->FirstParameter();
    gp_Pnt2d PBis = Bis->Value( UBis);
    
//  modified by NIZHNY-EAP Wed Jan 12 11:41:30 2000 ___BEGIN___
    // inside gp_Pnt2d::Distance
    // Infinite * Infinite => Exception: DefaultNumericError
    // Case encounered: UBis < Precision::Infinite()
    // but PBis.X() > Precision::Infinite()
    if (Precision::IsPositiveInfinite(Abs(PBis.X())) ||
	Precision::IsPositiveInfinite(Abs(PBis.Y())) ||
	PBis.Distance(P2d) > Tol) {
//  modified by NIZHNY-EAP Wed Jan 12 11:41:40 2000 ___END___
      UBis = Bis->LastParameter();
      if (UBis >= Precision::Infinite()) return;
      PBis = Bis->Value( UBis);
      if ( PBis.Distance(P2d) > Tol) return;
    }

    // eval parametre intersection.
    Handle(Geom_Surface) GS = BRep_Tool::Surface(Face);
    GeomAdaptor_Surface GAS(GS);

    gp_Ax3 Axis;
    Standard_Real Phase = 0.;

    switch ( GAS.GetType()) {
      
    case GeomAbs_Sphere: 
      Axis = GAS.Sphere().Position(); break;
    case GeomAbs_Cone: {
      //----------------------------------------------------------
      // si myFace1 n est pas du meme cote de l apex que le point
      // de parametre 0 0 sur le cone => phase = PI.
      //----------------------------------------------------------
      Axis = GAS.Cone().Position(); 
      Phase = EvalPhase(Edge,Face,GAS,Axis);
      break;
    }
    case GeomAbs_Torus:
      Axis = GAS.Torus().Position(); break;
    case GeomAbs_Cylinder:
      Axis = GAS.Cylinder().Position(); break;
    case GeomAbs_SurfaceOfRevolution: {      
      //----------------------------------------------------------
      // si myFace1 n est pas du meme cote de l apex que le point
      // de parametre 0 0 sur le cone => phase = PI.
      //----------------------------------------------------------
      Handle(Geom_SurfaceOfRevolution) GSRev = 
	Handle(Geom_SurfaceOfRevolution)::DownCast(GS);
      Handle(GeomAdaptor_HCurve) HC = 
	new GeomAdaptor_HCurve(GSRev->BasisCurve());
      Adaptor3d_SurfaceOfRevolution ASRev(HC,GAS.AxeOfRevolution());
      Axis  = ASRev.Axis();       
      Phase = EvalPhase(Edge,Face,GAS,Axis);
      break;
    }
    default:
      Standard_NotImplemented::Raise(" BRepFill_TrimSurfaceTool");
    }

    gp_Vec2d D12d = Bis->DN(UBis,1);
    gp_Vec   D1( D12d.X(), D12d.Y(), 0.);
    
    Standard_Real U = Axis.XDirection().
      AngleWithRef(D1,Axis.XDirection()^Axis.YDirection());
    U += Phase;
    if ( U < 0.) U += 2*PI;

    P = gp_Pnt(Bis->FirstParameter(), U, 0.);
    Seq.Append(P);
  }
}


//=======================================================================
//function : IntersectWith
//purpose  : 
//=======================================================================

void BRepFill_TrimSurfaceTool::IntersectWith
(const TopoDS_Edge&          EdgeOnF1,
 const TopoDS_Edge&          EdgeOnF2,
       TColgp_SequenceOfPnt& Points   ) 
const 
{
  Points.Clear();
  TColgp_SequenceOfPnt Points2;

  EvalParameters(EdgeOnF1, myFace1, myBis, Points);
  EvalParameters(EdgeOnF2, myFace2, myBis, Points2);

  StdFail_NotDone_Raise_if
    ( Points.Length() != Points2.Length(),
     "BRepFill_TrimSurfaceTool::IntersectWith: incoherent intersection");

  gp_Pnt PSeq;
  Standard_Integer NbPoints = Points.Length();
  for ( Standard_Integer i = 1; i <= NbPoints; i++) {
    PSeq = Points(i);
    PSeq.SetZ((Points2.Value(i)).Y());
    Points.SetValue(i,PSeq);
//    cout<<"BisPar "<<PSeq.X()<<endl;
  }
}


//=======================================================================
//function : IsOnFace
//purpose  : 
//=======================================================================

Standard_Boolean BRepFill_TrimSurfaceTool::IsOnFace
(const gp_Pnt2d& Point) const 
{
  gp_Pnt P( Point.X(), Point.Y(), 0.);
  gp_Lin Line( P, gp::DZ());

  BRepIntCurveSurface_Inter Inter;

  // eval if is on face 1
//  modified by NIZHNY-EAP Fri Jan 21 09:49:09 2000 ___BEGIN___
  Inter.Init(myFace1, Line, 1e-6);//Precision::PConfusion());
  if (Inter.More()) return Standard_True;
  
  // eval if is on face 2
  Inter.Init(myFace2, Line, 1e-6);//Precision::PConfusion());
  return Inter.More();
//  modified by NIZHNY-EAP Fri Jan 21 09:49:14 2000 ___END___
}


//=======================================================================
//function : ProjOn
//purpose  : 
//=======================================================================

Standard_Real BRepFill_TrimSurfaceTool::ProjOn(const gp_Pnt2d& Point,
					       const TopoDS_Edge& Edge) const 
{
  TopLoc_Location L;
  Standard_Real   f,l;

  

  Handle(Geom_Curve) C1 = BRep_Tool::Curve(Edge,L,f,l);
  Handle(Geom_TrimmedCurve) CT = new Geom_TrimmedCurve(C1,f,l);
  CT->Transform(L.Transformation());
  
  // projection de ces courbes 3d dans le plan xOy
  Handle(Geom_Plane) Plane = new Geom_Plane(0,0,1,0);
  Handle(Geom2d_Curve) C2d = GeomProjLib::Curve2d(CT,Plane);

  // eval the projection of the point on the curve.
  Geom2dAPI_ProjectPointOnCurve Projector(Point, C2d);
#ifdef DEB  
  Standard_Real Dist = 
#endif
    Projector.LowerDistance();
#ifdef DEB
  if ( Dist > Precision::Confusion() ) {
    cout << " *** WARNING  TrimSurfaceTool:  *** " << endl;
    cout << "      --> le point n'est pas sur l'edge" <<endl;
    cout << "          distance  = " << Dist << endl;
  }
#endif

  Standard_Real U = Projector.LowerDistanceParameter();
  return U;
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

void BRepFill_TrimSurfaceTool::Project
(const Standard_Real U1,
 const Standard_Real U2,
       Handle(Geom_Curve)&   Curve,
       Handle(Geom2d_Curve)& PCurve1,
       Handle(Geom2d_Curve)& PCurve2,
       GeomAbs_Shape&        Cont) const 
{
  Standard_Integer Deg1, Deg2;
  Deg1 = Deg2 = 8;

  Handle(Geom2d_TrimmedCurve) CT = 
    new Geom2d_TrimmedCurve(myBis,U1,U2);
  BRepFill_MultiLine ML(myFace1,myFace2,
			myEdge1,myEdge2,myInv1,myInv2,CT);
  
  Cont = ML.Continuity();

  if ( ML.IsParticularCase()) {
    ML.Curves(Curve,PCurve1,PCurve2);
  }
  else {
    BRepFill_ApproxSeewing AppSeew(ML);

    Curve   = AppSeew.Curve();
    PCurve1 = AppSeew.CurveOnF1();
    PCurve2 = AppSeew.CurveOnF2();
  }
}
