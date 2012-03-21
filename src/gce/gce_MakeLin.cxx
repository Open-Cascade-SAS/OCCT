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


#include <gce_MakeLin.ixx>
#include <StdFail_NotDone.hxx>
#include <gp.hxx>

//=========================================================================
//   Creation d une ligne 3d de gp a partir d un Ax1 de gp.               +
//=========================================================================

gce_MakeLin::gce_MakeLin(const gp_Ax1& A1)
{
  TheLin = gp_Lin(A1);
  TheError = gce_Done;
}

//=========================================================================
//   Creation d une ligne 3d de gp a partir de son origine P (Pnt de gp)  +
//   et d une direction V (Dir de gp).                                    +
//=========================================================================

gce_MakeLin::gce_MakeLin(const gp_Pnt& P,
			 const gp_Dir& V)
{
  TheLin = gp_Lin(P,V);
  TheError = gce_Done;
}

//=========================================================================
//   Creation d une ligne 3d de gp passant par les deux points <P1> et    +
//   <P2>.                                                                +
//=========================================================================

gce_MakeLin::gce_MakeLin(const gp_Pnt& P1 ,
			 const gp_Pnt& P2 ) 
{
  if (P1.Distance(P2) >= gp::Resolution()) {
    TheLin = gp_Lin(P1,gp_Dir(P2.XYZ()-P1.XYZ()));
    TheError = gce_Done;
  }
  else { TheError = gce_ConfusedPoints; }
}

//=========================================================================
//   Creation d une ligne 3d de gp parallele a une autre <Lin> et passant +
//   par le point <P>.                                                    +
//=========================================================================

gce_MakeLin::gce_MakeLin(const gp_Lin& Lin ,
			 const gp_Pnt& P   )
{
  TheLin = gp_Lin(P,Lin.Direction());
  TheError = gce_Done;
}

const gp_Lin& gce_MakeLin::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheLin;
}

const gp_Lin& gce_MakeLin::Operator() const 
{
  return Value();
}

gce_MakeLin::operator gp_Lin() const
{
  return Value();
}

