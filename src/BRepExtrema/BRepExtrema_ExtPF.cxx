// File:	BRepExtrema_ExtPF.cxx
// Created:	Wed Dec 15 16:48:53 1993
// Author:	Christophe MARION
//		<cma@sdsun1>
// modified by MPS (june 96) : on utilise BRepClass_FaceClassifier seulement 
//  si IsDone de Extrema est vrai  
#include <BRepExtrema_ExtPF.ixx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_Failure.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <gp_Pnt2d.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_HCurve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_HSurface.hxx>

//=======================================================================
//function : BRepExtrema_ExtPF
//purpose  : 
//=======================================================================

BRepExtrema_ExtPF::BRepExtrema_ExtPF()
{
}

//=======================================================================
//function : BRepExtrema_ExtPF
//purpose  : 
//=======================================================================

BRepExtrema_ExtPF::BRepExtrema_ExtPF
  (const TopoDS_Vertex& V,
   const TopoDS_Face&   E)
{
  Initialize(E);
  Perform(V, E);
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void BRepExtrema_ExtPF::Initialize(const TopoDS_Face& F)
{
  // cette surface doit etre en champ. Extrema ne fait
  // pas de copie et prend seulement un pointeur dessus.
  mySurf.Initialize(F, Standard_False); 
  Standard_Real Tol = BRep_Tool::Tolerance(F);
  Standard_Real U1, U2, V1, V2;
  BRepTools::UVBounds(F, U1, U2, V1, V2);
  myExtrem.Initialize(mySurf, U1, U2, V1, V2, Tol, Tol);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepExtrema_ExtPF::Perform(const TopoDS_Vertex& V,
				const TopoDS_Face&   E)
{
  mySqDist.Clear();
  myPoints.Clear();
  gp_Pnt P = BRep_Tool::Pnt(V);
  myExtrem.Perform(P);

  // exploration des points et classification:
  if (myExtrem.IsDone()) {
  BRepClass_FaceClassifier classifier;
  gp_Pnt2d Puv;
  Standard_Real U1, U2;
  TopAbs_State state;
  Standard_Real Tol = BRep_Tool::Tolerance(E);
  mynbext = 0;
  for (Standard_Integer i = 1; i <= myExtrem.NbExt(); i++) {
    myExtrem.Point(i).Parameter(U1, U2);
    Puv.SetCoord(U1, U2);
    classifier.Perform(E, Puv, Tol);
    state = classifier.State();
    if(state == TopAbs_ON || state == TopAbs_IN) {
      mynbext++;
      mySqDist.Append(myExtrem.SquareDistance(i));
      myPoints.Append(myExtrem.Point(i));
    }
  }
 }
}
//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepExtrema_ExtPF::IsDone()const
{
  return myExtrem.IsDone();
}

//=======================================================================
//function : NbExt
//purpose  : 
//=======================================================================

Standard_Integer BRepExtrema_ExtPF::NbExt() const
{
  if(!myExtrem.IsDone()) StdFail_NotDone::Raise();
  return mynbext;
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Real BRepExtrema_ExtPF::SquareDistance
  (const Standard_Integer N) const
{
  if(!myExtrem.IsDone()) StdFail_NotDone::Raise();
  if ((N < 1) || (N > mynbext)) Standard_OutOfRange::Raise();
  return mySqDist.Value(N);
}

//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================


void BRepExtrema_ExtPF::Parameter(const Standard_Integer N,
				  Standard_Real& U,
				  Standard_Real& V) const
{
  if(!myExtrem.IsDone()) StdFail_NotDone::Raise();
  if ((N < 1) || (N > mynbext)) Standard_OutOfRange::Raise();
  myPoints.Value(N).Parameter(U, V);
}

//=======================================================================
//function : Point
//purpose  : 
//=======================================================================

gp_Pnt BRepExtrema_ExtPF::Point
  (const Standard_Integer N) const
{
  if(!myExtrem.IsDone()) StdFail_NotDone::Raise();
  if ((N < 1) || (N > mynbext)) Standard_OutOfRange::Raise();
  gp_Pnt P = myPoints.Value(N).Value();
  return P; 
}
