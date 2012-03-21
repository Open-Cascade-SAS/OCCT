// Created on: 1992-09-03
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


#include <gce_MakeTranslation2d.ixx>

//=========================================================================
//   Creation d une translation 2d de gp de vecteur de tanslation Vec.    +
//=========================================================================

gce_MakeTranslation2d::
  gce_MakeTranslation2d(const gp_Vec2d&  Vec ) {
   TheTranslation2d.SetTranslation(Vec);
 }

//=========================================================================
//   Creation d une translation 2d de gp de vecteur de tanslation le      +
//   vecteur reliant Point1 a Point2.                                     +
//=========================================================================

gce_MakeTranslation2d::
  gce_MakeTranslation2d(const gp_Pnt2d&  Point1 ,
			const gp_Pnt2d&  Point2 ) {
   TheTranslation2d.SetTranslation(gp_Vec2d(Point1,Point2));
 }

const gp_Trsf2d& gce_MakeTranslation2d::Value() const
{ 
  return TheTranslation2d; 
}

const gp_Trsf2d& gce_MakeTranslation2d::Operator() const 
{
  return TheTranslation2d;
}

gce_MakeTranslation2d::operator gp_Trsf2d() const
{
  return TheTranslation2d;
}
