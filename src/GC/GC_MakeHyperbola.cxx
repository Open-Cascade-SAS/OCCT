// File:	GC_MakeHyperbola.cxx
// Created:	Fri Oct  2 16:36:22 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GC_MakeHyperbola.ixx>
#include <gce_MakeHypr.hxx>
#include <StdFail_NotDone.hxx>

GC_MakeHyperbola::GC_MakeHyperbola(const gp_Hypr& H)
{
  TheError = gce_Done;
  TheHyperbola = new Geom_Hyperbola(H);
}

GC_MakeHyperbola::GC_MakeHyperbola(const gp_Ax2&       A2         ,
				     const Standard_Real MajorRadius,
				     const Standard_Real MinorRadius)
{
  if (MajorRadius < 0. || MinorRadius < 0.0) { TheError = gce_NegativeRadius; }
  else {
    TheError = gce_Done;
    TheHyperbola = new Geom_Hyperbola(gp_Hypr(A2,MajorRadius,MinorRadius));
  }
}

GC_MakeHyperbola::GC_MakeHyperbola(const gp_Pnt& S1     ,
				     const gp_Pnt& S2     ,
				     const gp_Pnt& Center ) {
  gce_MakeHypr H = gce_MakeHypr(S1,S2,Center);
  TheError = H.Status();
  if (TheError == gce_Done) {
    TheHyperbola = new Geom_Hyperbola(H.Value());
  }
}

const Handle(Geom_Hyperbola)& GC_MakeHyperbola::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheHyperbola;
}

const Handle(Geom_Hyperbola)& GC_MakeHyperbola::Operator() const 
{
  return Value();
}

GC_MakeHyperbola::operator Handle(Geom_Hyperbola) () const
{
  return Value();
}

