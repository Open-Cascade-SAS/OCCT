// Created on: 1992-09-03
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <gce_MakeTranslation.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

//=========================================================================
//   Creation d une translation 3d de gp de vecteur de tanslation Vec.    +
//=========================================================================
gce_MakeTranslation::
  gce_MakeTranslation(const gp_Vec&  Vec ) {
   TheTranslation.SetTranslation(Vec);
 }

//=========================================================================
//   Creation d une translation 3d de gp de vecteur de tanslation le      +
//   vecteur reliant Point1 a Point2.                                     +
//=========================================================================

gce_MakeTranslation::
  gce_MakeTranslation(const gp_Pnt&  Point1 ,
		      const gp_Pnt&  Point2 ) {
   TheTranslation.SetTranslation(gp_Vec(Point1,Point2));
 }

const gp_Trsf& gce_MakeTranslation::Value() const
{ 
  return TheTranslation; 
}

const gp_Trsf& gce_MakeTranslation::Operator() const 
{
  return TheTranslation;
}

gce_MakeTranslation::operator gp_Trsf() const
{
  return TheTranslation;
}
