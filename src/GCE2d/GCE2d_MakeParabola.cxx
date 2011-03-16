// File:	GCE2d_MakeParabola.cxx
// Created:	Fri Oct  2 16:36:22 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GCE2d_MakeParabola.ixx>
#include <gce_MakeParab2d.hxx>
#include <StdFail_NotDone.hxx>

GCE2d_MakeParabola::GCE2d_MakeParabola(const gp_Parab2d& Prb)
{
  TheError = gce_Done;
  TheParabola = new Geom2d_Parabola(Prb);
}

GCE2d_MakeParabola::GCE2d_MakeParabola(const gp_Ax2d&         MirrorAxis,
				       const Standard_Real    Focal     ,
				       const Standard_Boolean Sense     )
{
  if (Focal <0.0) { TheError = gce_NullFocusLength; }
  else {
    TheError = gce_Done;
    TheParabola = new Geom2d_Parabola(MirrorAxis,Focal,Sense);
  }
}

GCE2d_MakeParabola::GCE2d_MakeParabola(const gp_Ax22d&     Axis ,
				       const Standard_Real Focal)
{
  if (Focal <0.0) { TheError = gce_NullFocusLength; }
  else {
    TheError = gce_Done;
    TheParabola = new Geom2d_Parabola(Axis,Focal);
  }
}

GCE2d_MakeParabola::GCE2d_MakeParabola(const gp_Ax22d&     Axis ,
				       const gp_Pnt2d&     F    )
{
  TheError = gce_Done;
  gp_Parab2d para(Axis,F);
  TheParabola = new Geom2d_Parabola(para);
}

GCE2d_MakeParabola::GCE2d_MakeParabola(const gp_Ax2d&         D     ,
				       const gp_Pnt2d&        F     ,
				       const Standard_Boolean Sense )
{
  TheError = gce_Done;
  gp_Parab2d para(D,F,Sense);
  TheParabola = new Geom2d_Parabola(para);
}


GCE2d_MakeParabola::GCE2d_MakeParabola(const gp_Pnt2d& S1 ,
				       const gp_Pnt2d& O  ) {
  gce_MakeParab2d P = gce_MakeParab2d(S1,O);
  TheError = P.Status();
  if (TheError == gce_Done) {
    TheParabola = new Geom2d_Parabola(P.Value());
  }
}

const Handle(Geom2d_Parabola)& GCE2d_MakeParabola::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheParabola;
}

const Handle(Geom2d_Parabola)& GCE2d_MakeParabola::Operator() const 
{
  return Value();
}

GCE2d_MakeParabola::operator Handle(Geom2d_Parabola) () const
{
  return Value();
}

