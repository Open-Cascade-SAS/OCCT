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


#include <GCE2d_MakeRotation.ixx>

//=========================================================================
//   Creation d une rotation 3d de gp d angle Angle par rapport a une     +
//   droite Line.                                                         +
//=========================================================================

GCE2d_MakeRotation::GCE2d_MakeRotation(const gp_Pnt2d&     Point  ,
				       const Standard_Real Angle ) {
  TheRotation = new Geom2d_Transformation();
  TheRotation->SetRotation(Point,Angle);
}

const Handle(Geom2d_Transformation)& GCE2d_MakeRotation::Value() const
{ 
  return TheRotation;
}

const Handle(Geom2d_Transformation)& GCE2d_MakeRotation::Operator() const 
{
  return TheRotation;
}

GCE2d_MakeRotation::operator Handle(Geom2d_Transformation) () const
{
  return TheRotation;
}




