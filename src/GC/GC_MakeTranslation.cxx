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


#include <GC_MakeTranslation.ixx>
#include <GC_MakeTranslation.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Creation of a 3D Geom translation of tanslation vector Vec.  +
//=========================================================================

GC_MakeTranslation::GC_MakeTranslation(const gp_Vec&  Vec ) {
  TheTranslation = new Geom_Transformation();
  TheTranslation->SetTranslation(Vec);
}
     
//=========================================================================
//    Creation of a 3D Geom translation of translation vector connecting 
//    Point1 and Point2.                                     +
//=========================================================================

GC_MakeTranslation::GC_MakeTranslation(const gp_Pnt&  Point1 ,
					 const gp_Pnt&  Point2 ) {
  TheTranslation = new Geom_Transformation();
  TheTranslation->SetTranslation(Point1,Point2);
}

const Handle(Geom_Transformation)& GC_MakeTranslation::Value() const
{ 
  return TheTranslation;
}

const Handle(Geom_Transformation)& GC_MakeTranslation::Operator() const 
{
  return TheTranslation;
}

GC_MakeTranslation::operator Handle(Geom_Transformation) () const
{
  return TheTranslation;
}

