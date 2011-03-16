// File:	GCE2d_MakeHyperbola.cxx
// Created:	Fri Oct  2 16:36:22 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GCE2d_MakeHyperbola.ixx>
#include <gce_MakeHypr2d.hxx>
#include <StdFail_NotDone.hxx>

GCE2d_MakeHyperbola::GCE2d_MakeHyperbola(const gp_Hypr2d& H)
{
  TheError = gce_Done;
  TheHyperbola = new Geom2d_Hyperbola(H);
}

GCE2d_MakeHyperbola::GCE2d_MakeHyperbola(const gp_Ax2d&      MajorAxis  ,
					 const Standard_Real MajorRadius,
					 const Standard_Real MinorRadius,
					 const Standard_Boolean Sense   )
{
  gce_MakeHypr2d H = gce_MakeHypr2d(MajorAxis,MajorRadius,MinorRadius,Sense);
  TheError = H.Status();
  if (TheError == gce_Done) {
    TheHyperbola = new Geom2d_Hyperbola(H.Value());
  }
}

GCE2d_MakeHyperbola::GCE2d_MakeHyperbola(const gp_Ax22d&     Axis       ,
					 const Standard_Real MajorRadius,
					 const Standard_Real MinorRadius)
{
  gce_MakeHypr2d H = gce_MakeHypr2d(Axis,MajorRadius,MinorRadius);
  TheError = H.Status();
  if (TheError == gce_Done) {
    TheHyperbola = new Geom2d_Hyperbola(H.Value());
  }
}

GCE2d_MakeHyperbola::GCE2d_MakeHyperbola(const gp_Pnt2d& S1     ,
					 const gp_Pnt2d& S2     ,
					 const gp_Pnt2d& Center ) {
  gce_MakeHypr2d H = gce_MakeHypr2d(S1,S2,Center);
  TheError = H.Status();
  if (TheError == gce_Done) {
    TheHyperbola = new Geom2d_Hyperbola(H.Value());
  }
}

const Handle(Geom2d_Hyperbola)& GCE2d_MakeHyperbola::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheHyperbola;
}

const Handle(Geom2d_Hyperbola)& GCE2d_MakeHyperbola::Operator() const 
{
  return Value();
}

GCE2d_MakeHyperbola::operator Handle(Geom2d_Hyperbola) () const
{
  return Value();
}

