// Created on: 1992-10-02
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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

