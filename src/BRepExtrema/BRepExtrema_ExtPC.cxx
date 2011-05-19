// File:	BRepExtrema_ExtPC.cxx
// Created:	Wed Dec 15 16:48:53 1993
// Author:	Christophe MARION

#include <BRepExtrema_ExtPC.hxx>

#include <BRep_Tool.hxx>
//#include <StdFail_NotDone.hxx>
//#include <Standard_Failure.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_HCurve.hxx>


//=======================================================================
//function : BRepExtrema_ExtPC
//purpose  : 
//=======================================================================

BRepExtrema_ExtPC::BRepExtrema_ExtPC(const TopoDS_Vertex& V, const TopoDS_Edge& E)
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
  myExtPC.Initialize(myHC->Curve(),U1,U2);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepExtrema_ExtPC::Perform(const TopoDS_Vertex& V)
{
  gp_Pnt P = BRep_Tool::Pnt(V);
  myExtPC.Perform(P);
}
