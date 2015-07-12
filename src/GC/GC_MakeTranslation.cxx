// Created on: 1992-10-02
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


#include <GC_MakeTranslation.hxx>
#include <Geom_Transformation.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
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
