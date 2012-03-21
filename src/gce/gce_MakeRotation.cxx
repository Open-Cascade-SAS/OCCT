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


#include <gce_MakeRotation.ixx>

//=========================================================================
//   Creation d une rotation 3d de gp d angle Angle par rapport a une     +
//   droite Line.                                                         +
//=========================================================================

gce_MakeRotation::
  gce_MakeRotation(const gp_Lin&  Line  ,
		   const Standard_Real     Angle ) {
   TheRotation.SetRotation(gp_Ax1(Line.Position()),Angle);
 }

//=========================================================================
//   Creation d une rotation 3d de gp d angle Angle par rapport a un      +
//   axe Axis.                                                            +
//=========================================================================

gce_MakeRotation::
 gce_MakeRotation(const gp_Ax1&  Axis  ,
		  const Standard_Real     Angle ) {
   TheRotation.SetRotation(Axis,Angle);
 }

//=========================================================================
//   Creation d une rotation 3d de gp d angle Angle par rapport a une     +
//   droite issue du point Point et de direction Direc.                   +
//=========================================================================

gce_MakeRotation::
 gce_MakeRotation(const gp_Pnt&  Point ,
		  const gp_Dir&  Direc ,
		  const Standard_Real     Angle ) {
   TheRotation.SetRotation(gp_Ax1(Point,Direc),Angle);
 }

const gp_Trsf& gce_MakeRotation::Value() const
{ 
  return TheRotation; 
}

const gp_Trsf& gce_MakeRotation::Operator() const 
{
  return TheRotation;
}

gce_MakeRotation::operator gp_Trsf() const
{
  return TheRotation;
}
