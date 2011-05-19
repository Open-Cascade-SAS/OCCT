// File:	BRepExtrema_ExtCC.cxx
// Created:	Wed Dec 15 16:48:53 1993
// Author:	Christophe MARION

#include <BRepExtrema_ExtCC.hxx>

#include <BRep_Tool.hxx>
#include <Extrema_POnCurv.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_HCurve.hxx>


//=======================================================================
//function : BRepExtrema_ExtCC
//purpose  : 
//=======================================================================

BRepExtrema_ExtCC::BRepExtrema_ExtCC(const TopoDS_Edge& E1, const TopoDS_Edge& E2)
{
  Initialize(E2);
  Perform(E1);
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void BRepExtrema_ExtCC::Initialize(const TopoDS_Edge& E2)
{
  Standard_Real V1,V2;
  BRepAdaptor_Curve Curv(E2);
  myHC = new BRepAdaptor_HCurve(Curv);
  BRep_Tool::Range(E2,V1,V2);
  myExtCC.SetCurve(2,myHC->Curve(),V1,V2);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepExtrema_ExtCC::Perform(const TopoDS_Edge& E1)
{
  Standard_Real U1, U2;
  BRepAdaptor_Curve Curv(E1);
  Handle(BRepAdaptor_HCurve) HC = new BRepAdaptor_HCurve(Curv);
  BRep_Tool::Range(E1,U1,U2);
  myExtCC.SetCurve (1, HC->Curve(), U1, U2);
  myExtCC.Perform();
}

//=======================================================================
//function : ParameterOnE1
//purpose  : 
//=======================================================================

Standard_Real BRepExtrema_ExtCC::ParameterOnE1(const Standard_Integer N) const
{
  Extrema_POnCurv POnE1, POnE2;
  myExtCC.Points(N, POnE1, POnE2);
  return POnE1.Parameter();
}

//=======================================================================
//function : PointOnE1
//purpose  : 
//=======================================================================

gp_Pnt BRepExtrema_ExtCC::PointOnE1(const Standard_Integer N) const
{
  Extrema_POnCurv POnE1, POnE2;
  myExtCC.Points(N, POnE1, POnE2);
  return POnE1.Value();
}

//=======================================================================
//function : ParameterOnE2
//purpose  : 
//=======================================================================

Standard_Real BRepExtrema_ExtCC::ParameterOnE2(const Standard_Integer N) const
{
  Extrema_POnCurv POnE1, POnE2;
  myExtCC.Points(N, POnE1, POnE2);
  return POnE2.Parameter();
}

//=======================================================================
//function : PointOnE2
//purpose  : 
//=======================================================================

gp_Pnt BRepExtrema_ExtCC::PointOnE2(const Standard_Integer N) const
{
  Extrema_POnCurv POnE1, POnE2;
  myExtCC.Points(N, POnE1, POnE2);
  return POnE2.Value();
}


//=======================================================================
//function : TrimmedSquareDistances
//purpose  : 
//=======================================================================

void BRepExtrema_ExtCC::TrimmedSquareDistances
  (Standard_Real& dist11,
   Standard_Real& dist12,
   Standard_Real& dist21,
   Standard_Real& dist22,
   gp_Pnt& pnt11,
   gp_Pnt& pnt12,
   gp_Pnt& pnt21,
   gp_Pnt& pnt22) const
{
  myExtCC.TrimmedSquareDistances(dist11,dist12,dist21,dist22,
                                 pnt11,pnt12,pnt21,pnt22);
}
