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


#include <gce_MakeMirror2d.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>

//=========================================================================
//   Creation d une symetrie 2d de gp par rapport a un point.             +
//=========================================================================
gce_MakeMirror2d::gce_MakeMirror2d(const gp_Pnt2d&  Point ) 
{
  TheMirror2d.SetMirror(Point); 
}

//=========================================================================
//   Creation d une symetrie 2d de gp par rapport a une droite.           +
//=========================================================================

gce_MakeMirror2d::gce_MakeMirror2d(const gp_Ax2d& Axis ) 
{
  TheMirror2d.SetMirror(Axis); 
}

//=========================================================================
//   Creation d une symetrie 2d de gp par rapport a une droite.           +
//=========================================================================

gce_MakeMirror2d::gce_MakeMirror2d(const gp_Lin2d&  Line ) 
{
  TheMirror2d.SetMirror(gp_Ax2d(Line.Location(),Line.Direction()));
}

//=========================================================================
//   Creation d une symetrie 2d de gp par rapport a une droite definie    +
//   par un point et une direction.                                       +
//=========================================================================

gce_MakeMirror2d::gce_MakeMirror2d(const gp_Pnt2d&  Point ,
				   const gp_Dir2d&  Direc ) 
{
  TheMirror2d.SetMirror(gp_Ax2d(Point,Direc));
}

const gp_Trsf2d& gce_MakeMirror2d::Value() const 
{ 
  return TheMirror2d; 
}

const gp_Trsf2d& gce_MakeMirror2d::Operator() const 
{
  return TheMirror2d;
}

gce_MakeMirror2d::operator gp_Trsf2d() const
{
  return TheMirror2d;
}
