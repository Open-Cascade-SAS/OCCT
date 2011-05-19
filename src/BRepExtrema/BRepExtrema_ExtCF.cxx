// File:	BRepExtrema_ExtCF.cxx
// Created:	Wed Dec 15 16:48:53 1993
// Author:	Christophe MARION

#include <BRepExtrema_ExtCF.hxx>

#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Geom_Curve.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <gp_Pnt2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_HCurve.hxx>

//=======================================================================
//function : BRepExtrema_ExtCF
//purpose  : 
//=======================================================================

BRepExtrema_ExtCF::BRepExtrema_ExtCF(const TopoDS_Edge& E, const TopoDS_Face& F)
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
  const Standard_Real Tol = BRep_Tool::Tolerance(F2);
  Standard_Real U1, U2, V1, V2;
  BRepTools::UVBounds(F2, U1, U2, V1, V2);
  myExtCS.Initialize(myHS->Surface(), U1, U2, V1, V2, Tol, Tol);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepExtrema_ExtCF::Perform(const TopoDS_Edge& E, const TopoDS_Face& F2)
{
  mySqDist.Clear();
  myPointsOnS.Clear();
  myPointsOnC.Clear();

  Standard_Real U1, U2;
  BRep_Tool::Range(E, U1, U2);

  BRepAdaptor_Curve Curv(E);
  Handle(BRepAdaptor_HCurve) HC = new BRepAdaptor_HCurve(Curv);
  myExtCS.Perform(HC->Curve(), U1, U2);

  if(!myExtCS.IsDone())
    return;

  if (myExtCS.IsParallel())
    mySqDist.Append(myExtCS.SquareDistance(1));
  else
  {
    // Exploration of points and classification
    BRepClass_FaceClassifier classifier;
    const Standard_Real Tol = BRep_Tool::Tolerance(F2);
    Extrema_POnCurv P1;
    Extrema_POnSurf P2;

    for (Standard_Integer i = 1; i <= myExtCS.NbExt(); i++)
    {
      myExtCS.Points(i, P1, P2);
      P2.Parameter(U1, U2);
      const gp_Pnt2d Puv(U1, U2);
      classifier.Perform(F2, Puv, Tol);
      const TopAbs_State state = classifier.State();
      if (state == TopAbs_ON || state == TopAbs_IN)
      {
        mySqDist.Append(myExtCS.SquareDistance(i));
        myPointsOnC.Append(P1);
        myPointsOnS.Append(P2);
      }
    }
  }
}
