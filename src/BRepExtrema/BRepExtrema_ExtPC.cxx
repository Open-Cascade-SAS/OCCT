// File:	BRepExtrema_ExtPC.cxx
// Created:	Wed Dec 15 16:48:53 1993
// Author:	Christophe MARION
//		<cma@sdsun1>

#include <BRepExtrema_ExtPC.ixx>
#include <BRep_Tool.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_Failure.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_HCurve.hxx>


//=======================================================================
//function : BRepExtrema_ExtPC
//purpose  : 
//=======================================================================

BRepExtrema_ExtPC::BRepExtrema_ExtPC()
{
}

//=======================================================================
//function : BRepExtrema_ExtPC
//purpose  : 
//=======================================================================

BRepExtrema_ExtPC::BRepExtrema_ExtPC
  (const TopoDS_Vertex& V,
   const TopoDS_Edge& E)
{
  Initialize(E);
  Perform(V);
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void BRepExtrema_ExtPC::Initialize(const TopoDS_Edge& E)
{
  Standard_Real U1,U2;
  BRepAdaptor_Curve Curv(E);
  myHC = new BRepAdaptor_HCurve(Curv);
  BRep_Tool::Range(E,U1,U2);
  myExtrem.Initialize(myHC->Curve(),U1,U2);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepExtrema_ExtPC::Perform(const TopoDS_Vertex& V)
{
  gp_Pnt P = BRep_Tool::Pnt(V);
  myExtrem.Perform(P);
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepExtrema_ExtPC::IsDone()const
{
  return myExtrem.IsDone();
}

//=======================================================================
//function : NbExt
//purpose  : 
//=======================================================================

Standard_Integer BRepExtrema_ExtPC::NbExt() const
{
  if(!myExtrem.IsDone()) StdFail_NotDone::Raise();
  return myExtrem.NbExt();
}

//=======================================================================
//function : IsMin
//purpose  : 
//=======================================================================

Standard_Boolean BRepExtrema_ExtPC::IsMin
  (const Standard_Integer N) const
{
  if(!myExtrem.IsDone()) StdFail_NotDone::Raise();
  if ((N < 1) || (N > myExtrem.NbExt())) Standard_OutOfRange::Raise();
  return myExtrem.IsMin(N);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Real BRepExtrema_ExtPC::SquareDistance
  (const Standard_Integer N) const
{
  if(!myExtrem.IsDone()) StdFail_NotDone::Raise();
  if ((N < 1) || (N > myExtrem.NbExt())) Standard_OutOfRange::Raise();
  return myExtrem.SquareDistance(N);
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Standard_Real BRepExtrema_ExtPC::Parameter
  (const Standard_Integer N) const
{
  if(!myExtrem.IsDone()) StdFail_NotDone::Raise();
  if ((N < 1) || (N > myExtrem.NbExt())) Standard_OutOfRange::Raise();
  return myExtrem.Point(N).Parameter();
}

//=======================================================================
//function : Point
//purpose  : 
//=======================================================================

gp_Pnt BRepExtrema_ExtPC::Point
  (const Standard_Integer N) const
{
  if(!myExtrem.IsDone()) StdFail_NotDone::Raise();
  if ((N < 1) || (N > myExtrem.NbExt())) Standard_OutOfRange::Raise();
  return (myExtrem.Point(N).Value()); 
}

//=======================================================================
//function : TrimmedDistances
//purpose  : 
//=======================================================================

void BRepExtrema_ExtPC::TrimmedSquareDistances
  (Standard_Real& dist1,
   Standard_Real& dist2,
   gp_Pnt& pnt1,
   gp_Pnt& pnt2) const
{
  myExtrem.TrimmedSquareDistances(dist1,dist2,pnt1,pnt2);
}

