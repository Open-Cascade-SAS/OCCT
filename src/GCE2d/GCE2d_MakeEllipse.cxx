// File:	GCE2d_MakeEllipse.cxx
// Created:	Fri Oct  2 16:36:08 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GCE2d_MakeEllipse.ixx>
#include <gce_MakeElips2d.hxx>
#include <StdFail_NotDone.hxx>

GCE2d_MakeEllipse::GCE2d_MakeEllipse(const gp_Elips2d& E)
{
  TheError = gce_Done;
  TheEllipse = new Geom2d_Ellipse(E);
}

GCE2d_MakeEllipse::GCE2d_MakeEllipse(const gp_Ax22d&     Axis        ,
				     const Standard_Real MajorRadius ,
				     const Standard_Real MinorRadius )
{
  gce_MakeElips2d E = gce_MakeElips2d(Axis,MajorRadius,MinorRadius);
  TheError = E.Status();
  if (TheError == gce_Done) {
    TheEllipse = new Geom2d_Ellipse(E.Value());
  }
}

GCE2d_MakeEllipse::GCE2d_MakeEllipse(const gp_Ax2d&         MajorAxis   ,
				     const Standard_Real    MajorRadius ,
				     const Standard_Real    MinorRadius ,
				     const Standard_Boolean Sense       )
{
  gce_MakeElips2d E = gce_MakeElips2d(MajorAxis,MajorRadius,MinorRadius,Sense);
  TheError = E.Status();
  if (TheError == gce_Done) {
    TheEllipse = new Geom2d_Ellipse(E.Value());
  }
}

GCE2d_MakeEllipse::GCE2d_MakeEllipse(const gp_Pnt2d& S1     ,
				     const gp_Pnt2d& S2     ,
				     const gp_Pnt2d& Center ) {
  gce_MakeElips2d E = gce_MakeElips2d(S1,S2,Center);
  TheError = E.Status();
  if (TheError == gce_Done) {
    TheEllipse = new Geom2d_Ellipse(E.Value());
  }
}

const Handle(Geom2d_Ellipse)& GCE2d_MakeEllipse::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheEllipse;
}

const Handle(Geom2d_Ellipse)& GCE2d_MakeEllipse::Operator() const 
{
  return Value();
}

GCE2d_MakeEllipse::operator Handle(Geom2d_Ellipse) () const
{
  return Value();
}

