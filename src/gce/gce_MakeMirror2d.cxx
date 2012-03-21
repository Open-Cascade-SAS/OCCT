// Created on: 1992-09-04
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


#include <gce_MakeMirror2d.ixx>

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
