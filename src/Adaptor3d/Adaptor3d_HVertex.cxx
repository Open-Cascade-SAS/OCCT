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

#include <Adaptor3d_HVertex.ixx>

#include <Precision.hxx>
#include <ElCLib.hxx>
#include <Adaptor2d_HCurve2d.hxx>


Adaptor3d_HVertex::Adaptor3d_HVertex ()
{}


Adaptor3d_HVertex::Adaptor3d_HVertex (const gp_Pnt2d& P,
				  const TopAbs_Orientation Or,
				  const Standard_Real Resolution):
       myPnt(P),myTol(Resolution),myOri(Or)
{}


gp_Pnt2d Adaptor3d_HVertex::Value ()
{
  return myPnt;
}

Standard_Real Adaptor3d_HVertex::Parameter (const Handle(Adaptor2d_HCurve2d)& C)
{
  return ElCLib::Parameter(C->Line(),myPnt);
}

Standard_Real Adaptor3d_HVertex::Resolution (const Handle(Adaptor2d_HCurve2d)&)
{
  return myTol;
}

TopAbs_Orientation Adaptor3d_HVertex::Orientation ()
{
  return myOri;
}


Standard_Boolean Adaptor3d_HVertex::IsSame(const Handle(Adaptor3d_HVertex)& Other)
{
  return (myPnt.Distance(Other->Value())<= Precision::Confusion());
}

