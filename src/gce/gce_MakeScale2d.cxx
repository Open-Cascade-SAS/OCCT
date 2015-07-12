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


#include <gce_MakeScale2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>

//=========================================================================
//   Creation d un homothetie de gp de centre Point et de rapport Scale.  +
//=========================================================================
gce_MakeScale2d::
  gce_MakeScale2d(const gp_Pnt2d&     Point ,
		  const Standard_Real Scale ) {
  TheScale2d.SetScale(Point,Scale);
}

const gp_Trsf2d& gce_MakeScale2d::Value() const
{ 
  return TheScale2d; 
}

const gp_Trsf2d& gce_MakeScale2d::Operator() const 
{
  return TheScale2d;
}

gce_MakeScale2d::operator gp_Trsf2d() const
{
  return TheScale2d;
}
