// File:	GCE2d_MakeLine.cxx
// Created:	Fri Oct  2 16:36:34 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GCE2d_MakeLine.ixx>
#include <gce_MakeLin2d.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Constructions of 2d geometrical elements from Geom2d.
//=========================================================================

GCE2d_MakeLine::GCE2d_MakeLine(const gp_Ax2d& A)
{
  TheError = gce_Done;
  TheLine = new Geom2d_Line(A);
}

GCE2d_MakeLine::GCE2d_MakeLine(const gp_Lin2d& L)
{
  TheError = gce_Done;
  TheLine = new Geom2d_Line(L);
}

GCE2d_MakeLine::GCE2d_MakeLine(const gp_Pnt2d& P,
			       const gp_Dir2d& V)
{
  TheError = gce_Done;
  TheLine = new Geom2d_Line(P,V);
}

GCE2d_MakeLine::GCE2d_MakeLine(const gp_Pnt2d& P1 ,
			       const gp_Pnt2d& P2 ) 
{
  gce_MakeLin2d L(P1,P2);
  TheError = L.Status();
  if (TheError == gce_Done) {
    TheLine = new Geom2d_Line(L.Value());
  }
}

GCE2d_MakeLine::GCE2d_MakeLine(const gp_Lin2d& Lin   ,
			       const gp_Pnt2d& Point ) 
{
  gce_MakeLin2d L(Lin,Point);
  TheError = L.Status();
  if (TheError == gce_Done) {
    TheLine = new Geom2d_Line(L.Value());
  }
}

GCE2d_MakeLine::GCE2d_MakeLine(const gp_Lin2d&     Lin  ,
			       const Standard_Real Dist ) 
{
  gce_MakeLin2d L(Lin,Dist);
  TheError = L.Status();
  if (TheError == gce_Done) {
    TheLine = new Geom2d_Line(L.Value());
  }
}

const Handle(Geom2d_Line)& GCE2d_MakeLine::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheLine;
}

const Handle(Geom2d_Line) & GCE2d_MakeLine::Operator() const 
{
  return Value();
}

GCE2d_MakeLine::operator Handle(Geom2d_Line) () const
{
  return Value();
}

