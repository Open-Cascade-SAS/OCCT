// Created on: 1992-09-02
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


#include <gce_MakeDir.ixx>
#include <StdFail_NotDone.hxx>
#include <gp.hxx>

//=========================================================================
//   Creation d une direction 3d (Dir) de gp a partir de 2 Pnt de gp.     +
//=========================================================================

gce_MakeDir::gce_MakeDir(const gp_Pnt& P1,
			 const gp_Pnt& P2)
{
  if (P1.Distance(P2) <= gp::Resolution()) { TheError = gce_ConfusedPoints; }
  else {
    TheDir = gp_Dir(P2.XYZ()-P1.XYZ());
    TheError = gce_Done;
  }
}

gce_MakeDir::gce_MakeDir(const gp_XYZ& Coord)
{
  if (Coord.Modulus() <= gp::Resolution()) { TheError = gce_NullVector; }
  else {
    TheDir = gp_Dir(Coord);
    TheError = gce_Done;
  }
}

gce_MakeDir::gce_MakeDir(const gp_Vec& V)
{
  if (V.Magnitude() <= gp::Resolution()) { TheError = gce_NullVector; }
  else {
    TheDir = gp_Dir(V);
    TheError = gce_Done;
  }
}

gce_MakeDir::gce_MakeDir(const Standard_Real Xv,
			 const Standard_Real Yv,
			 const Standard_Real Zv)
{
  if (Xv*Xv+Yv*Yv+Zv*Zv <= gp::Resolution()) { TheError = gce_NullVector; }
  else {
    TheDir = gp_Dir(Xv,Yv,Zv);
    TheError = gce_Done;
  }
}

const gp_Dir& gce_MakeDir::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheDir;
}

const gp_Dir& gce_MakeDir::Operator() const 
{
  return Value();
}

gce_MakeDir::operator gp_Dir() const
{
  return Value();
}

