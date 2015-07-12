// Copyright (c) 1995-1999 Matra Datavision
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


#include <ElCLib.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_Conic.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

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

