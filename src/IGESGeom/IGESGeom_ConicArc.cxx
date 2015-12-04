// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//#59 rln 29.12.98 PRO17015

#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <IGESGeom_ConicArc.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGeom_ConicArc,IGESData_IGESEntity)

IGESGeom_ConicArc::IGESGeom_ConicArc ()    {  }


    void  IGESGeom_ConicArc::Init
  (const Standard_Real A, const Standard_Real B,
   const Standard_Real C, const Standard_Real D,  const Standard_Real E,
   const Standard_Real F, const Standard_Real ZT, const gp_XY& aStart,
   const gp_XY& anEnd)
{
  theA = A;
  theB = B;
  theC = C;
  theD = D;
  theE = E;
  theF = F;
  theZT = ZT;
  theStart = aStart;
  theEnd   = anEnd;

  Standard_Integer fn = FormNumber();
  if (fn == 0) fn = ComputedFormNumber();
  InitTypeAndForm(104,fn);
}

    Standard_Boolean  IGESGeom_ConicArc::OwnCorrect ()
{
  Standard_Integer cfn = ComputedFormNumber();
  if (FormNumber() == cfn) return Standard_False;
  InitTypeAndForm(104,cfn);
  return Standard_True;
}

    void  IGESGeom_ConicArc::Equation
  (Standard_Real& A, Standard_Real& B, Standard_Real& C, Standard_Real& D,
   Standard_Real& E, Standard_Real& F) const
{
  A = theA;
  B = theB;
  C = theC;
  D = theD;
  E = theE;
  F = theF;
}

    Standard_Real  IGESGeom_ConicArc::ZPlane () const
{
  return theZT;
}

    gp_Pnt2d  IGESGeom_ConicArc::StartPoint () const
{
  gp_Pnt2d start(theStart.X(), theStart.Y());
  return start;
}

    gp_Pnt  IGESGeom_ConicArc::TransformedStartPoint () const
{
  gp_XYZ start(theStart.X(), theStart.Y(), theZT);
  if (HasTransf()) Location().Transforms(start);
  gp_Pnt transStart(start);
  return transStart;
}

    gp_Pnt2d  IGESGeom_ConicArc::EndPoint () const
{
  gp_Pnt2d end(theEnd.X(), theEnd.Y());
  return end;
}

    gp_Pnt  IGESGeom_ConicArc::TransformedEndPoint () const
{
  gp_XYZ end(theEnd.X(), theEnd.Y(), theZT);
  if (HasTransf()) Location().Transforms(end);
  gp_Pnt transEnd(end);
  return transEnd;
}

    Standard_Integer  IGESGeom_ConicArc::ComputedFormNumber () const
{
  Standard_Real eps,eps2,eps4;
  eps = 1.E-08;  eps2 = eps*eps; eps4 = eps2*eps2;//#59 rln
  Standard_Real Q1 = theA   * (theC*theF   - theE*theE/4.)
    + theB/2. * (theE*theD/4. - theB*theF/2.)
      + theD/2. * (theB*theE/4. - theC*theD/2.);
  Standard_Real Q2 = theA*theC - theB*theB/4;
  Standard_Real Q3 = theA + theC;

//  Resultats
  //#59 rln 29.12.98 PRO17015 face#67, ellipse
  //each Qi has its own dimension:
  //[Q1] = L^-4, [Q2]=L^-4, [Q3]=L^-2
  if (Q2       >  eps4 && Q1*Q3    < 0   ) return 1;    // Ellipse
  if (Q2       < -eps4 && Abs (Q1) > eps4) return 2;    // Hyperbola
  if (Abs (Q2) <= eps4 && Abs (Q1) > eps4) return 3;    // Parabola
  return 0;
}

    Standard_Boolean  IGESGeom_ConicArc::IsFromParabola () const
{
  Standard_Integer fn = FormNumber();
  if (fn == 0)     fn = ComputedFormNumber();
  return (fn == 3);
}

    Standard_Boolean  IGESGeom_ConicArc::IsFromEllipse () const
{
  Standard_Integer fn = FormNumber();
  if (fn == 0)     fn = ComputedFormNumber();
  return (fn == 1);
}

    Standard_Boolean  IGESGeom_ConicArc::IsFromHyperbola () const
{
  Standard_Integer fn = FormNumber();
  if (fn == 0)     fn = ComputedFormNumber();
  return (fn == 2);
}

    Standard_Boolean  IGESGeom_ConicArc::IsClosed () const
{
  return ((theStart.X() == theEnd.X()) && (theStart.Y() == theEnd.Y()));
}

    gp_Dir IGESGeom_ConicArc::Axis () const
{
  gp_Dir axis(0.0 , 0.0, 1.0);
  return axis;
}

//    Valeurs calculees

    gp_Dir IGESGeom_ConicArc::TransformedAxis () const
{
  gp_XYZ axis(0.0 , 0.0, 1.0);
  if (!HasTransf()) return gp_Dir(axis);
  gp_GTrsf loc = Location();
  loc.SetTranslationPart (gp_XYZ(0.,0.,0.));
  loc.Transforms(axis);
  return gp_Dir(axis);
}


    void  IGESGeom_ConicArc::Definition
  (gp_Pnt& Center, gp_Dir& MainAxis,
   Standard_Real& Rmin, Standard_Real& Rmax) const
{
  Standard_Real Xcen,Ycen, Xax,Yax;
  ComputedDefinition (Xcen,Ycen, Xax,Yax, Rmin,Rmax);
  Center.SetCoord (Xcen,Ycen,theZT);
  MainAxis.SetCoord (Xax,Yax,0.);
}

    void  IGESGeom_ConicArc::TransformedDefinition
  (gp_Pnt& Center, gp_Dir& MainAxis,
   Standard_Real& Rmin, Standard_Real& Rmax) const
{
  if (!HasTransf()) {
    Definition (Center,MainAxis,Rmin,Rmax);
    return;
  }
  Standard_Real Xcen,Ycen, Xax,Yax;
  ComputedDefinition (Xcen,Ycen, Xax,Yax, Rmin,Rmax);
  gp_GTrsf loc = Location();
  gp_XYZ cen  (Xcen,Ycen,theZT);
  gp_XYZ axis (Xax, Yax, 0.);
  loc.Transforms (cen);
  loc.SetTranslationPart (gp_XYZ(0.,0.,0.));
  loc.Transforms (axis);
  Center.SetCoord   (cen.X(), cen.Y(), cen.Z() );
  MainAxis.SetCoord (axis.X(),axis.Y(),axis.Z());
}


    void  IGESGeom_ConicArc::ComputedDefinition
  (Standard_Real& Xcen, Standard_Real& Ycen,
   Standard_Real& Xax,  Standard_Real& Yax,
   Standard_Real& Rmin, Standard_Real& Rmax) const
{
  Standard_Real a,b,c,d,e,f;
  //  conic : a*x2 + 2*b*x*y + c*y2 + 2*d*x + 2*e*y + f = 0.
  Equation (a,b,c,d,e,f);
  b = b/2.;  d = d/2.;  e = e/2.;    // chgt de variable

  Standard_Real eps = 1.E-08;  // ?? comme ComputedForm

  if (IsFromParabola()) {
    Rmin = Rmax = -1.;  // rayons : yena pas
    if ( (Abs(a) <= eps) && (Abs(b) <= eps)) {
      Xcen = (f*c - e*e) /c /d /2.;
      Ycen = e/c;
      Standard_Real focal = -d/c;
      Xax  = (focal >= 0 ? 1. : -1.);
      Yax  = 0.;
      Rmin = Rmax = Abs(focal);
    } 
    else {
      Standard_Real ss = a+c;
      Standard_Real cc =   - (a*d+b*e) / ss;
      Standard_Real dd = d + (c*d - b*e) / ss;
      Standard_Real fc =     (a*e - b*d) / ss;
      Standard_Real ee = e + fc;

      Standard_Real dn = a*ee - dd*b;
      Xcen = ( cc*ee + f*b) / dn;
      Ycen = (-cc*dd - f*a) / dn;

      Standard_Real teta = M_PI/2.;
      if (Abs(b) > eps) teta = ATan (-a/b);
      if (fc < 0) teta += M_PI;
      Xax  = Cos(teta);
      Yax  = Sin(teta);

      Rmin = Rmax = Abs(fc)/sqrt(a*a+b*b)/2.;
    }
  } 

  else {
    //   -> Conique a centre, cas general
    //  On utilise les Determinants des matrices :
    //               | a b d |
    //  gdet (3x3) = | b c e |  et pdet (2X2) = | a b |
    //               | d e f |                  | b c |
    
    Standard_Real gdet = a*c*f + 2*b*d*e - c*d*d - a*e*e - b*b*f;
    Standard_Real pdet = a*c - b*b;
    
    Xcen = (b*e - c*d) / pdet;
    Ycen = (b*d - a*e) / pdet;

    Standard_Real term1 = a-c;
    Standard_Real term2 = 2*b;
    Standard_Real cos2t;
    Standard_Real auxil;

    if (Abs(term1)< gp::Resolution()) {
      cos2t = 1.;
      auxil = term2;
    }
    else {
      Standard_Real t2d = term2/term1; //skl 28.12.2001
      cos2t = 1./sqrt(1+t2d*t2d);
      auxil = sqrt (term1*term1 + term2*term2);
    }
    
    Standard_Real cost = sqrt ( (1+cos2t)/2. );
    Standard_Real sint = sqrt ( (1-cos2t)/2. );

    Standard_Real aprim = (a+c+auxil)/2.;
    Standard_Real cprim = (a+c-auxil)/2.;
    
    term1 = -gdet/(aprim*pdet);
    term2 = -gdet/(cprim*pdet);
      
    if (IsFromEllipse()) {
      Xax = cost;
      Yax = sint;
      Rmin = sqrt ( term1);
      Rmax = sqrt ( term2);
      if(Rmax<Rmin){    //skl 28.12.2001
        Rmax = sqrt ( term1);
        Rmin = sqrt ( term2);
      }
    }
    else if (term1 <= eps){
      Xax  = -sint;
      Yax  =  cost;
      Rmin = sqrt (-term1);
      Rmax = sqrt (term2);
    } 
    else {
      Xax  =  cost;
      Yax  =  sint;
      Rmin = sqrt (-term2);
      Rmax = sqrt (term1);
    }
  }
}
