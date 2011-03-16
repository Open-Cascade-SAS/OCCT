// File:	BRepExtrema_ExtCF.cxx
// Created:	Wed Dec 15 16:48:53 1993
// Author:	Christophe MARION
//		<cma@sdsun1>

#include <BRepExtrema_ExtCF.ixx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Geom_Curve.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_Failure.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Extrema_POnCurv.hxx>
#include <Extrema_POnSurf.hxx>
#include <gp_Pnt2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_HCurve.hxx>

//=======================================================================
//function : BRepExtrema_ExtCF
//purpose  : 
//=======================================================================

BRepExtrema_ExtCF::BRepExtrema_ExtCF()
{
}

//=======================================================================
//function : BRepExtrema_ExtCF
//purpose  : 
//=======================================================================

BRepExtrema_ExtCF::BRepExtrema_ExtCF
  (const TopoDS_Edge& E,
   const TopoDS_Face& F)
{
  Initialize(F);
  Perform(E, F);
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void BRepExtrema_ExtCF::Initialize(const TopoDS_Face& F2)
{
  BRepAdaptor_Surface Surf(F2);
  myHS = new BRepAdaptor_HSurface(Surf);
  Standard_Real Tol = BRep_Tool::Tolerance(F2);
  Standard_Real U1, U2, V1, V2;
  BRepTools::UVBounds(F2, U1, U2, V1, V2);
  myExtrem.Initialize(myHS->Surface(), U1, U2, V1, V2, Tol, Tol);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepExtrema_ExtCF::Perform(const TopoDS_Edge& E,
				const TopoDS_Face& F2)
{
  mySqDist.Clear();
  myPointsOnS.Clear();
  myPointsOnC.Clear();
  Standard_Real U1, U2;
  BRep_Tool::Range(E, U1, U2);

  BRepAdaptor_Curve Curv(E);
  Handle(BRepAdaptor_HCurve) HC = new BRepAdaptor_HCurve(Curv);
  myExtrem.Perform(HC->Curve(), U1, U2);

  // exploration des points et classification:
  BRepClass_FaceClassifier classifier;
  gp_Pnt2d Puv;
  TopAbs_State state;
  Standard_Real Tol = BRep_Tool::Tolerance(F2);
  Extrema_POnCurv P1;
  Extrema_POnSurf P2;
  mynbext = 0;

  if(!myExtrem.IsDone())
    return;

  if (myExtrem.IsParallel()) {
    mySqDist.Append(myExtrem.SquareDistance(1));
    mynbext = 1;
  }
  else {
    for (Standard_Integer i = 1; i <= myExtrem.NbExt(); i++) {
      myExtrem.Points(i, P1, P2);
      P2.Parameter(U1, U2);
      Puv.SetCoord(U1, U2);
      classifier.Perform(F2, Puv, Tol);
      state = classifier.State();
      if(state == TopAbs_ON || state == TopAbs_IN) {
	mynbext++;
	mySqDist.Append(myExtrem.SquareDistance(i));
	myPointsOnC.Append(P1);
	myPointsOnS.Append(P2);
      }
    }
  }
}
//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepExtrema_ExtCF::IsDone()const
{
  return myExtrem.IsDone();
}


//=======================================================================
//function : IsParallel
//purpose  : 
//=======================================================================

Standard_Boolean BRepExtrema_ExtCF::IsParallel()const
{
  return myExtrem.IsParallel();
}

//=======================================================================
//function : NbExt
//purpose  : 
//=======================================================================

Standard_Integer BRepExtrema_ExtCF::NbExt() const
{
  if(!myExtrem.IsDone()) StdFail_NotDone::Raise();
  return mynbext;
}


//=======================================================================
//function : SquareDistance
//purpose  : 
//=======================================================================

Standard_Real BRepExtrema_ExtCF::SquareDistance
  (const Standard_Integer N) const
{
  if(!myExtrem.IsDone()) StdFail_NotDone::Raise();
  if ((N < 1) || (N > mynbext)) Standard_OutOfRange::Raise();
  return mySqDist.Value(N);
}

//=======================================================================
//function : ParameterOnFace
//purpose  : 
//=======================================================================

void BRepExtrema_ExtCF::ParameterOnFace(const Standard_Integer N,
					 Standard_Real& U,
					 Standard_Real& V) const
{
  if(!myExtrem.IsDone()) StdFail_NotDone::Raise();
  if ((N < 1) || (N > mynbext)) Standard_OutOfRange::Raise();
  myPointsOnS.Value(N).Parameter(U, V);
}

//=======================================================================
//function : ParameterOnEdge
//purpose  : 
//=======================================================================

Standard_Real BRepExtrema_ExtCF::ParameterOnEdge
  (const Standard_Integer N) const
{
  if(!myExtrem.IsDone()) StdFail_NotDone::Raise();
  if ((N < 1) || (N > mynbext)) Standard_OutOfRange::Raise();
  return myPointsOnC.Value(N).Parameter();
}

//=======================================================================
//function : PointOnEdge
//purpose  : 
//=======================================================================

gp_Pnt BRepExtrema_ExtCF::PointOnEdge
  (const Standard_Integer N) const
{
  if(!myExtrem.IsDone()) StdFail_NotDone::Raise();
  if ((N < 1) || (N > mynbext)) Standard_OutOfRange::Raise();
  gp_Pnt P = myPointsOnC.Value(N).Value();
  return P; 
}

//=======================================================================
//function : PointOnFace
//purpose  : 
//=======================================================================

gp_Pnt BRepExtrema_ExtCF::PointOnFace
  (const Standard_Integer N) const
{
  if(!myExtrem.IsDone()) StdFail_NotDone::Raise();
  if ((N < 1) || (N > mynbext)) Standard_OutOfRange::Raise();
  gp_Pnt P = myPointsOnS.Value(N).Value();
  return P; 
}

