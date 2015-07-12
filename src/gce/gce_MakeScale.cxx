// Created on: 1992-09-04
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


#include <gce_MakeScale.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>

//=========================================================================
//   Creation d un homothetie de gp de centre Point et de rapport Scale.  +
//=========================================================================
gce_MakeScale::
  gce_MakeScale(const gp_Pnt&       Point ,
		const Standard_Real Scale ) {
   TheScale.SetScale(Point,Scale);
 }

const gp_Trsf& gce_MakeScale::Value() const
{ 
  return TheScale; 
}

const gp_Trsf& gce_MakeScale::Operator() const 
{
  return TheScale;
}

gce_MakeScale::operator gp_Trsf() const
{
  return TheScale;
}
