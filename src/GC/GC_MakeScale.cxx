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


#include <GC_MakeScale.hxx>
#include <Geom_Transformation.hxx>
#include <gp_Pnt.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Creation d un homothetie de gp de centre Point et de rapport Scale.  +
//=========================================================================
GC_MakeScale::GC_MakeScale(const gp_Pnt&  Point ,
			     const Standard_Real     Scale ) {
  TheScale = new Geom_Transformation();
  TheScale->SetScale(Point,Scale);
}

const Handle(Geom_Transformation)& GC_MakeScale::Value() const
{ 
  return TheScale;
}
