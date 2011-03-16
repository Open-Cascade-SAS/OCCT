// File:	GC_MakeCircle.cxx
// Created:	Fri Oct  2 16:32:04 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GC_MakeCircle.ixx>
#include <gce_MakeCirc.hxx>
#include <StdFail_NotDone.hxx>

GC_MakeCircle::GC_MakeCircle(const gp_Circ& C)
{
  TheError = gce_Done;
  TheCircle = new Geom_Circle(C);
}

GC_MakeCircle::GC_MakeCircle(const gp_Ax2&       A2    ,
			       const Standard_Real Radius)
{
  if (Radius < 0.) { TheError = gce_NegativeRadius; }
  else {
    TheError = gce_Done;
    TheCircle = new Geom_Circle(gp_Circ(A2,Radius));
  }
}

GC_MakeCircle::GC_MakeCircle(const gp_Circ& Circ  ,
			       const gp_Pnt&  Point ) 
{
  gp_Circ C = gce_MakeCirc(Circ,Point);
  TheCircle = new Geom_Circle(C);
  TheError = gce_Done;
}

GC_MakeCircle::GC_MakeCircle(const gp_Circ& Circ ,
			       const Standard_Real     Dist ) 
{
  gce_MakeCirc C = gce_MakeCirc(Circ,Dist);
  TheError = C.Status();
  if (TheError == gce_Done) {
    TheCircle = new Geom_Circle(C.Value());
  }
}

GC_MakeCircle::GC_MakeCircle(const gp_Pnt& P1 ,
			       const gp_Pnt& P2 ,
			       const gp_Pnt& P3 ) 
{
  gce_MakeCirc C = gce_MakeCirc(P1,P2,P3);
  TheError = C.Status();
  if (TheError == gce_Done) {
    TheCircle = new Geom_Circle(C.Value());
  }
}

GC_MakeCircle::GC_MakeCircle(const gp_Pnt& Point  ,
			       const gp_Dir& Norm   ,
			       const Standard_Real    Radius ) 
{
  gce_MakeCirc C = gce_MakeCirc(Point,Norm,Radius);
  TheError = C.Status();
  if (TheError == gce_Done) {
    TheCircle = new Geom_Circle(C.Value());
  }
}

GC_MakeCircle::GC_MakeCircle(const gp_Pnt&        Point  ,
			       const gp_Pnt&        PtAxis ,
			       const Standard_Real  Radius ) 
{
  gce_MakeCirc C = gce_MakeCirc(Point,PtAxis,Radius);
  TheError = C.Status();
  if (TheError == gce_Done) {
    TheCircle = new Geom_Circle(C.Value());
  }
}

GC_MakeCircle::GC_MakeCircle(const gp_Ax1& Axis   ,
			       const Standard_Real    Radius ) 
{
  gce_MakeCirc C = gce_MakeCirc(Axis,Radius);
  TheError = C.Status();
  if (TheError == gce_Done) {
    TheCircle = new Geom_Circle(C.Value());
  }
}

const Handle(Geom_Circle)& GC_MakeCircle::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheCircle;
}

const Handle(Geom_Circle)& GC_MakeCircle::Operator() const 
{
  return Value();
}

GC_MakeCircle::operator Handle(Geom_Circle) () const
{
  return Value();
}

