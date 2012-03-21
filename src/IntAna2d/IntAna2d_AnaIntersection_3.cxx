// Copyright (c) 1995-1999 Matra Datavision
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

#include <IntAna2d_AnaIntersection.jxx>
#include <ElCLib.hxx>

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void IntAna2d_AnaIntersection::Perform(const gp_Lin2d& L,
				       const gp_Circ2d& C) 
{
 
  done=Standard_False;

  iden=Standard_False; 
  para=Standard_False;
  //
  Standard_Real A,B,C0, d;
  gp_Pnt2d aP2D, aP2D1, aP2D2;
  //
  L.Coefficients(A,B,C0);
  d=A*C.Location().X() + B*C.Location().Y() + C0;
  
  if (Abs(d)-C.Radius()>Epsilon(C.Radius())) {
    empt=Standard_True;
    nbp=0;
  }
  else {                                       // Au moins 1 solution
    empt=Standard_False;
    //
    //modified by NIZNHY-PKV Fri Jun 15 09:55:00 2007f
    //Standard_Real ang;
    //ang = C.XAxis().Direction().Angle(L.Direction());
    //ang = ang + M_PI / 2.0;
    //modified by NIZNHY-PKV Fri Jun 15 09:55:29 2007t
    if (Abs(Abs(d)-C.Radius())<=Epsilon(C.Radius())) {    // Cas de tangence
      
      Standard_Real u, XS, YS, ang;
      //
      nbp=1;
      XS=C.Location().X() - d*A;
      YS=C.Location().Y() - d*B;
      //
      //modified by NIZNHY-PKV Fri Jun 15 09:55:35 2007f
      aP2D.SetCoord(XS, YS);
      u=ElCLib::Parameter(L, aP2D);
      ang=ElCLib::Parameter(C, aP2D);
      /*
      u=B*(L.Location().X()-C.Location().X()) -
	A*(L.Location().Y()-C.Location().Y());
      if (d<0.0) {ang=ang+M_PI;}
      if (ang>=2.0*M_PI) {
	ang=ang-2.0*M_PI;
      }
      else if (ang<0.0) {
	ang=ang+2.0*M_PI;
      }
      */
      //modified by NIZNHY-PKV Fri Jun 15 09:55:41 2007t
      lpnt[0].SetValue(XS,YS,u,ang);
    }
    else { // 2 points d intersection
      Standard_Real h, XS1,YS1, XS2,YS2, ang1,ang2, u1,u2;//,cost,sint angt;                                        
      nbp=2;
      h=Sqrt(C.Radius()*C.Radius()-d*d);
      //modified by NIZNHY-PKV Fri Jun 15 09:55:47 2007f
      //cost=d/C.Radius();
      //sint=h/C.Radius();
      //modified by NIZNHY-PKV Fri Jun 15 09:55:52 2007t
      XS1=C.Location().X() - d*A - h*B;
      YS1=C.Location().Y() - d*B + h*A;
      XS2=C.Location().X() - d*A + h*B;
      YS2=C.Location().Y() - d*B - h*A;
      //
      //modified by NIZNHY-PKV Fri Jun 15 09:55:57 2007f
      aP2D1.SetCoord(XS1, YS1);
      aP2D2.SetCoord(XS2, YS2);
      u1=ElCLib::Parameter(L, aP2D1);
      u2=ElCLib::Parameter(L, aP2D2);
      ang1=ElCLib::Parameter(C, aP2D1);
      ang2=ElCLib::Parameter(C, aP2D2);
      //
      /*
      if (Abs(cost)<=0.707) {
	angt=ACos(cost);
      }
      else {
	angt=ASin(sint);
	if (cost<0) {angt=M_PI-angt;}
      }
      
      ang1=ang-angt;
      ang2=ang+angt;
      if (ang1<0.0) {
	ang1=ang1+2.0*M_PI;
      }
      else if (ang1>=2.0*M_PI) {
	ang1=ang1-2.0*M_PI;
      }
      if (ang2<0.0) {
	ang2=ang2+2.0*M_PI;
      }
      else if (ang2>=2.0*M_PI) {
	ang2=ang2-2.0*M_PI;
      }

      u1=B*(L.Location().X()-C.Location().X()) -
	      A*(L.Location().Y()-C.Location().Y()) +h;
      u2=u1-2.0*h;
      */
      //modified by NIZNHY-PKV Fri Jun 15 09:56:19 2007t
      lpnt[0].SetValue(XS1,YS1,u1,ang1);
      lpnt[1].SetValue(XS2,YS2,u2,ang2);
    }
  }
  done=Standard_True;
}

