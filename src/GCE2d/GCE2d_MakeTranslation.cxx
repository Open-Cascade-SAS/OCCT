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


#include <GCE2d_MakeTranslation.ixx>

//=========================================================================
//   Creation d une translation 3d de Geom2d de vecteur de tanslation Vec.  +
//=========================================================================

GCE2d_MakeTranslation::GCE2d_MakeTranslation(const gp_Vec2d&  Vec ) {
  TheTranslation = new Geom2d_Transformation();
  TheTranslation->SetTranslation(Vec);
}
     
//=========================================================================
//   Creation d une translation 3d de Geom2d de vecteur de tanslation le    +
//   vecteur reliant Point1 a Point2.                                     +
//=========================================================================

GCE2d_MakeTranslation::GCE2d_MakeTranslation(const gp_Pnt2d&  Point1 ,
					     const gp_Pnt2d&  Point2 ) {
  TheTranslation = new Geom2d_Transformation();
  TheTranslation->SetTranslation(Point1,Point2);
}

const Handle(Geom2d_Transformation)& GCE2d_MakeTranslation::Value() const
{ 
  return TheTranslation;
}

const Handle(Geom2d_Transformation)& GCE2d_MakeTranslation::Operator() const 
{
  return TheTranslation;
}

GCE2d_MakeTranslation::operator Handle(Geom2d_Transformation) () const
{
  return TheTranslation;
}

