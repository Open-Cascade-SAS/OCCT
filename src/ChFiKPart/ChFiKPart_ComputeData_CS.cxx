// Created on: 1994-04-13
// Created by: Isabelle GRIGNON
// Copyright (c) 1994-1999 Matra Datavision
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



#include <ChFiKPart_ComputeData_CS.hxx>
#include <gp_Ax3.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <ElCLib.hxx>


void ChFiKPart_CornerSpine(const Handle(Adaptor3d_HSurface)& S1, 
			   const Handle(Adaptor3d_HSurface)& S2,
			   const gp_Pnt2d& P1S1,
			   const gp_Pnt2d& P2S1,
			   const gp_Pnt2d& P1S2,
			   const gp_Pnt2d& P2S2,
			   const Standard_Real R,
			   gp_Cylinder& cyl,
			   gp_Circ& circ,
			   Standard_Real& First,
			   Standard_Real& Last)
     
{
  gp_Ax3 ax = S1->Plane().Position();
  gp_Vec V1(ax.XDirection());
  gp_Vec V2(ax.YDirection());
  gp_Pnt P;
  gp_Vec du,dv;
  S2->D1(P1S2.X(),P1S2.Y(),P,du,dv);
  gp_Vec V(P,S1->Value(P1S1.X(),P1S1.Y()));
  V = V.Dot(V1)*V1+V.Dot(V2)*V2;
  V.Normalize();
  gp_Pnt P2 = S2->Value(P2S2.X(),P2S2.Y());
  gp_Vec Vorien(P,P2);
  gp_Pnt cent;
  gp_Dir dx(V);
  if(V.Dot(Vorien) >= 0.){
    cent.SetCoord(P.X()+R*V.X(),P.Y()+R*V.Y(),P.Z()+R*V.Z());
    dx.Reverse();
  }
  else {
    cent.SetCoord(P.X()-R*V.X(),P.Y()-R*V.Y(),P.Z()-R*V.Z());
  }
  gp_Dir dy(gp_Vec(cent,P2));
  dy = (dx^dy)^dx;
  gp_Ax2 circax2(cent,dx^dy,dx);
  gp_Ax3 cylax3(circax2);
  if((du^dv).Dot(dx) < 0.) cylax3.ZReverse();
  First = 0.;
  Last = ElCLib::CircleParameter(circax2,P2);
  circ.SetPosition(circax2);
  circ.SetRadius(R);
  cyl.SetPosition(cylax3);
  cyl.SetRadius(R);
}
