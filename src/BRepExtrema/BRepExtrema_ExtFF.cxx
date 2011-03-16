// File:	BRepExtrema_ExtFF.cxx
// Created:	Wed Dec 15 16:48:53 1993
// Author:	Christophe MARION
//		<cma@sdsun1>
// modified by mps (juillet 96 ): on utilise BRepAdaptor a la place de 
// GeomAdaptor dans Initialize et Perform.
#include <BRepExtrema_ExtFF.ixx>
#include <BRepExtrema_ExtCF.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Geom_Curve.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_Failure.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <TopoDS.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <Geom_Surface.hxx>
#include <Extrema_POnSurf.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <BRepAdaptor_HSurface.hxx>

//=======================================================================
//function : BRepExtrema_ExtFF
//purpose  : 
//=======================================================================

BRepExtrema_ExtFF::BRepExtrema_ExtFF()
{
}

//=======================================================================
//function : BRepExtrema_ExtFF
//purpose  : 
//=======================================================================

BRepExtrema_ExtFF::BRepExtrema_ExtFF
  (const TopoDS_Face& F1,
   const TopoDS_Face& F2)
{
 Initialize(F2);
 Perform(F1,F2);
}
//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void BRepExtrema_ExtFF::Initialize(const TopoDS_Face& F2)
{
  BRepAdaptor_Surface Surf(F2);
  myHS = new BRepAdaptor_HSurface(Surf);
  Standard_Real Tol = BRep_Tool::Tolerance(F2);
  Standard_Real U1, U2, V1, V2;
  BRepTools::UVBounds(F2, U1, U2, V1, V2);
  myExtrem.Initialize(myHS->Surface(), U1, U2, V1, V2, Tol);
 
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepExtrema_ExtFF::Perform(const TopoDS_Face& F1,
				const TopoDS_Face& F2)
{ 
  Standard_Real U1, U2, V1, V2;
  Standard_Integer i;   
  mySqDist.Clear();
  myPointsOnS1.Clear();
  myPointsOnS2.Clear();
  BRepAdaptor_Surface Surf1(F1);
  Handle(BRepAdaptor_HSurface) HS1 = new BRepAdaptor_HSurface(Surf1);
  Standard_Real Tol1 = BRep_Tool::Tolerance(F1);
  BRepTools::UVBounds(F1, U1, U2, V1, V2);
  myExtrem.Perform(HS1->Surface(), U1, U2, V1, V2, Tol1);
 

  // exploration des points et classification:
  BRepClass_FaceClassifier classifier;
  gp_Pnt2d Puv;
  TopAbs_State state1, state2;
  Standard_Real Tol2 = BRep_Tool::Tolerance(F2);
  Extrema_POnSurf P1, P2;
  mynbext = 0;

  if (myExtrem.IsParallel()) {
    mySqDist.Append(myExtrem.SquareDistance(1));
    mynbext = 1;
  }
  else {
    for (i = 1; i <= myExtrem.NbExt(); i++) {
      myExtrem.Points(i, P1, P2);
      P1.Parameter(U1, U2);
      Puv.SetCoord(U1, U2);
      classifier.Perform(F1, Puv, Tol1);
      state1 = classifier.State();
      P2.Parameter(U1, U2);
      Puv.SetCoord(U1, U2);
      classifier.Perform(F2, Puv, Tol2);
      state2 = classifier.State();
      if((state1 == TopAbs_ON || state1 == TopAbs_IN) &&
	 (state2 == TopAbs_ON || state2 == TopAbs_IN)) {
	mynbext++;
	mySqDist.Append(myExtrem.SquareDistance(i));
	myPointsOnS1.Append(P1);
	myPointsOnS2.Append(P2);
      }
    }
  }
}
  
//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepExtrema_ExtFF::IsDone()const
{
  return myExtrem.IsDone();
}

//=======================================================================
//function : IsParallel
//purpose  : 
//=======================================================================

Standard_Boolean BRepExtrema_ExtFF::IsParallel()const
{
  return myExtrem.IsParallel();
}


//=======================================================================
//function : NbExt
//purpose  : 
//=======================================================================

Standard_Integer BRepExtrema_ExtFF::NbExt() const
{
  if(!myExtrem.IsDone()) StdFail_NotDone::Raise();
  return mynbext;
}


//=======================================================================
//function : SquareDistance
//purpose  : 
//=======================================================================

Standard_Real BRepExtrema_ExtFF::SquareDistance
  (const Standard_Integer N) const
{
  if(!myExtrem.IsDone()) StdFail_NotDone::Raise();
  if ((N < 1) || (N > mynbext)) Standard_OutOfRange::Raise();
  return mySqDist.Value(N);
}

//=======================================================================
//function : ParameterOnFace1
//purpose  : 
//=======================================================================

void BRepExtrema_ExtFF::ParameterOnFace1(const Standard_Integer N,
					  Standard_Real& U,
					  Standard_Real& V) const
{
  if(!myExtrem.IsDone()) StdFail_NotDone::Raise();
  if ((N < 1) || (N > mynbext)) Standard_OutOfRange::Raise();
  myPointsOnS1.Value(N).Parameter(U, V);
}

//=======================================================================
//function : PointOnFace1
//purpose  : 
//=======================================================================

gp_Pnt BRepExtrema_ExtFF::PointOnFace1
  (const Standard_Integer N) const
{
  if(!myExtrem.IsDone()) StdFail_NotDone::Raise();
  if ((N < 1) || (N > mynbext)) Standard_OutOfRange::Raise();
  gp_Pnt P = myPointsOnS1.Value(N).Value();
  return P; 
}


//=======================================================================
//function : ParameterOnFace2
//purpose  : 
//=======================================================================

void BRepExtrema_ExtFF::ParameterOnFace2(const Standard_Integer N,
					  Standard_Real& U,
					  Standard_Real& V) const
{
  if(!myExtrem.IsDone()) StdFail_NotDone::Raise();
  if ((N < 1) || (N > mynbext)) Standard_OutOfRange::Raise();
  myPointsOnS2.Value(N).Parameter(U, V);
}

//=======================================================================
//function : PointOnFace1
//purpose  : 
//=======================================================================

gp_Pnt BRepExtrema_ExtFF::PointOnFace2
  (const Standard_Integer N) const
{
  if(!myExtrem.IsDone()) StdFail_NotDone::Raise();
  if ((N < 1) || (N > mynbext)) Standard_OutOfRange::Raise();
  gp_Pnt P = myPointsOnS2.Value(N).Value();
  return P; 
}


