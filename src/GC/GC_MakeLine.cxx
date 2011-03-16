// File:	GC_MakeLine.cxx
// Created:	Fri Oct  2 16:36:34 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GC_MakeLine.ixx>
#include <gce_MakeLin.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Constructions of 3d geometrical elements from Geom.
//=========================================================================

GC_MakeLine::GC_MakeLine(const gp_Pnt& P ,
			   const gp_Dir& V )
{
  TheError = gce_Done;
  TheLine = new Geom_Line(P,V);
}

GC_MakeLine::GC_MakeLine(const gp_Ax1& A1 )
{
  TheError = gce_Done;
  TheLine = new Geom_Line(A1);
}

GC_MakeLine::GC_MakeLine(const gp_Lin& L )
{
  TheError = gce_Done;
  TheLine = new Geom_Line(L);
}

GC_MakeLine::GC_MakeLine(const gp_Pnt& P1 ,
			   const gp_Pnt& P2 ) 
{
  gce_MakeLin L(P1,P2);
  TheError = L.Status();
  if (TheError == gce_Done) {
    TheLine = new Geom_Line(L.Value());
  }
}

GC_MakeLine::GC_MakeLine(const gp_Lin& Lin   ,
			   const gp_Pnt& Point ) 
{
  gce_MakeLin L(Lin,Point);
  TheError = L.Status();
  if (TheError == gce_Done) {
    TheLine = new Geom_Line(L.Value());
  }
}

const Handle(Geom_Line)& GC_MakeLine::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheLine;
}

const Handle(Geom_Line)& GC_MakeLine::Operator() const 
{
  return Value();
}

GC_MakeLine::operator Handle(Geom_Line) () const
{
  return Value();
}

