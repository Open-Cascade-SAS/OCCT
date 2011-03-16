// File:	GC_MakeEllipse.cxx
// Created:	Fri Oct  2 16:36:08 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GC_MakeEllipse.ixx>
#include <gce_MakeElips.hxx>
#include <StdFail_NotDone.hxx>

GC_MakeEllipse::GC_MakeEllipse(const gp_Elips& E)
{
  TheError = gce_Done;
  TheEllipse = new Geom_Ellipse(E);
}

GC_MakeEllipse::GC_MakeEllipse(const gp_Ax2&       A2         ,
				 const Standard_Real MajorRadius,
				 const Standard_Real MinorRadius)
{
  if ( MinorRadius < 0.0) { TheError = gce_NegativeRadius; }
  else if ( MajorRadius < MinorRadius) { TheError = gce_InvertAxis; }
  else {
    TheError = gce_Done;
    TheEllipse = new Geom_Ellipse(gp_Elips(A2,MajorRadius,MinorRadius));
  }
}

GC_MakeEllipse::GC_MakeEllipse(const gp_Pnt& S1     ,
				 const gp_Pnt& S2     ,
				 const gp_Pnt& Center ) {
  gce_MakeElips E = gce_MakeElips(S1,S2,Center);
  TheError = E.Status();
  if (TheError == gce_Done) {
    TheEllipse = new Geom_Ellipse(E.Value());
  }
}

const Handle(Geom_Ellipse)& GC_MakeEllipse::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheEllipse;
}

const Handle(Geom_Ellipse)& GC_MakeEllipse::Operator() const 
{
  return Value();
}

GC_MakeEllipse::operator Handle(Geom_Ellipse) () const
{
  return Value();
}

