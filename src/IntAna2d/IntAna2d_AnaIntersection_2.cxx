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


#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Vec2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_Conic.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

void IntAna2d_AnaIntersection::Perform (const gp_Circ2d& C1,
					const gp_Circ2d& C2) {

  done=Standard_False;
  Standard_Real d=C1.Location().Distance(C2.Location());
  Standard_Real R1=C1.Radius();
  Standard_Real R2=C2.Radius();
  Standard_Real sum=R1+R2;
  Standard_Real dif=Abs(R1-R2);
 

  if (d<=RealEpsilon()) {                 // Cercle concentriques
    para=Standard_True;
    nbp=0;
    if (dif<=RealEpsilon()) {             // Cercles confondus
      empt=Standard_False;
      iden=Standard_True;
    }
    else {                               // Cercles paralleles
      empt=Standard_True;
      iden=Standard_False;
    }
  }
  else if ((d-sum)>Epsilon(sum)) {        // Cercles exterieurs l un a l autre
                                         // et No solution
    empt=Standard_True;
    para=Standard_False;
    iden=Standard_False;
    nbp=0;
  }
  else if (Abs(d-sum)<=Epsilon(sum)) {    // Cercles exterieurs et tangents
    empt=Standard_False;
    para=Standard_False;
    iden=Standard_False;
    nbp=1;
    gp_Vec2d ax(C1.Location(),C2.Location());
    gp_Vec2d Ox1(C1.XAxis().Direction());
    gp_Vec2d Ox2(C2.XAxis().Direction());

    Standard_Real XS = ( C1.Location().X()*R2 + C2.Location().X()*R1 ) / sum;
    Standard_Real YS = ( C1.Location().Y()*R2 + C2.Location().Y()*R1 ) / sum;
    Standard_Real ang1=Ox1.Angle(ax);                     // Resultat entre -PI et +PI
    Standard_Real ang2=Ox2.Angle(ax) + M_PI;
    if (ang1<0) {ang1=2*M_PI+ang1;}                // On revient entre 0 et 2PI
    lpnt[0].SetValue(XS,YS,ang1,ang2);
  }
  else if (((sum-d)>Epsilon(sum)) && ((d-dif)>Epsilon(d+dif))) {
    empt=Standard_False;
    para=Standard_False;
    iden=Standard_False;
    nbp=2;
    gp_Vec2d ax(C1.Location(),C2.Location());
    gp_Vec2d Ox1(C1.XAxis().Direction());
    gp_Vec2d Ox2(C2.XAxis().Direction());
    Standard_Real ref1=Ox1.Angle(ax);                       // Resultat entre -PI et +PI
    Standard_Real ref2=Ox2.Angle(ax);                       // Resultat entre -PI et +PI

    Standard_Real l1=(d*d + R1*R1 -R2*R2)/(2.0*d);
    Standard_Real aDet = R1*R1-l1*l1;
    if(aDet < 0.) {
      aDet = 0.;
      l1 = (l1 > 0 ? R1 : - R1);
    }
    Standard_Real h= Sqrt(R1*R1-l1*l1);

    Standard_Real XS1= C1.Location().X() + l1*ax.X()/d - h*ax.Y()/d;
    Standard_Real YS1= C1.Location().Y() + l1*ax.Y()/d + h*ax.X()/d;

    Standard_Real XS2= C1.Location().X() + l1*ax.X()/d + h*ax.Y()/d;
    Standard_Real YS2= C1.Location().Y() + l1*ax.Y()/d - h*ax.X()/d;

    Standard_Real sint1=h /R1;
    Standard_Real cost1=l1/R1;

    Standard_Real sint2=h     /R2;
    Standard_Real cost2=(l1-d)/R2;

    Standard_Real ang1,ang2;

    // ang1 et ang2 correspondent aux solutions avec sinus positif
    // si l'axe de reference est l'axe des centres C1C2
    // On prend l'arccos entre pi/2 et 3pi/2, l'arcsin sinon.

    if (Abs(cost1)<=0.707) {
      ang1=ACos(cost1); 
    }
    else {
      ang1=ASin(sint1);
      if (cost1<0.0) {ang1=M_PI-ang1;}
    }
    if (Abs(cost2)<=0.707) {
      ang2=ACos(cost2);
    }
    else {
      ang2=ASin(sint2);
      if (cost2<0.0) {ang2=M_PI-ang2;}
    }
    Standard_Real ang11=ref1+ang1;
    Standard_Real ang21=ref2+ang2;
    Standard_Real ang12=ref1-ang1;
    Standard_Real ang22=ref2-ang2;
    if (ang11<0.) {
      ang11=2*M_PI+ang11;
    }
    else if (ang11>=2*M_PI) {
      ang11=ang11-2*M_PI;
    }
    if (ang21<0.) {
      ang21=2*M_PI+ang21;
    }
    else if (ang21>=2*M_PI) {
      ang21=ang21-2*M_PI;
    }
    if (ang12<0.) {
      ang12=2*M_PI+ang12;
    }
    else if (ang12>=2*M_PI) {
      ang12=ang12-2*M_PI;
    }
    if (ang22<0.) {
      ang22=2*M_PI+ang22;
    }
    else if (ang22>=2*M_PI) {
      ang22=ang22-2*M_PI;
    }
    lpnt[0].SetValue(XS1,YS1,ang11,ang21);
    lpnt[1].SetValue(XS2,YS2,ang12,ang22);
  }
  else if (Abs(d-dif)<=Epsilon(sum)) {    // Cercles tangents interieurs
    empt=Standard_False;
    para=Standard_False;
    iden=Standard_False;
    nbp=1;
    gp_Vec2d ax(C1.Location(),C2.Location());
    if(C1.Radius() < C2.Radius())
      ax.Reverse();

    gp_Vec2d Ox1(C1.XAxis().Direction());
    gp_Vec2d Ox2(C2.XAxis().Direction());
    Standard_Real ang1=Ox1.Angle(ax);                       // Resultat entre -PI et +PI
    Standard_Real ang2=Ox2.Angle(ax);
    if (ang1<0) {ang1=2*M_PI+ang1;}                  // On revient entre 0 et 2PI
    if (ang2<0) {ang2=2*M_PI+ang2;}                  // On revient entre 0 et 2PI
    Standard_Real XS = ( C1.Location().X()*R2 - C2.Location().X()*R1 ) / (R2 - R1);
    Standard_Real YS = ( C1.Location().Y()*R2 - C2.Location().Y()*R1 ) / (R2 - R1);
    lpnt[0].SetValue(XS,YS,ang1,ang2);
  }
  else {                           // On doit avoir d<dif-Resol et d<>0 donc
                                   // 1 cercle dans l autre et no solution
    empt=Standard_True;
    para=Standard_False;
    iden=Standard_False;
    nbp=0;
  }
  done=Standard_True;
}


