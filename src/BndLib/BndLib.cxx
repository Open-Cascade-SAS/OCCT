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


#include <Bnd_Box.hxx>
#include <Bnd_Box2d.hxx>
#include <BndLib.hxx>
#include <ElCLib.hxx>
#include <gp_Circ.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Elips.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>

static 
  Standard_Integer ComputeBox(const gp_Hypr& aHypr, 
                              const Standard_Real aT1, 
                              const Standard_Real aT2, 
                              Bnd_Box& aBox);


namespace
{
  //! Compute method
  template<class PointType, class BndBoxType>
  void Compute (const Standard_Real theP1, const Standard_Real theP2,
                const Standard_Real theRa ,const Standard_Real theRb,
                const PointType& theXd, const PointType& theYd, const PointType& theO,
                BndBoxType& theB)
  {
    Standard_Real aTeta1;
    Standard_Real aTeta2;
    if(theP2 < theP1)
    { 
      aTeta1 = theP2;
      aTeta2 = theP1;
    }
    else
    {
      aTeta1 = theP1;
      aTeta2 = theP2;
    }

    Standard_Real aDelta = Abs(aTeta2-aTeta1); 
    if(aDelta > 2. * M_PI)
    {
      aTeta1 = 0.;
      aTeta2 = 2. * M_PI;
    }
    else
    {
      if(aTeta1 < 0.)
      {
        do{ aTeta1 += 2.*M_PI; } while (aTeta1 < 0.);
      }
      else if (aTeta1 > 2.*M_PI)
      {
        do { aTeta1 -= 2.*M_PI; } while (aTeta1 > 2.*M_PI);
      }
      aTeta2 = aTeta1 + aDelta;
    }

    // One places already both ends
    Standard_Real aCn1, aSn1 ,aCn2, aSn2;
    aCn1 = Cos(aTeta1); aSn1 = Sin(aTeta1);
    aCn2 = Cos(aTeta2); aSn2 = Sin(aTeta2);
    theB.Add(PointType( theO.Coord() +theRa*aCn1*theXd.Coord() +theRb*aSn1*theYd.Coord()));
    theB.Add(PointType(theO.Coord() +theRa*aCn2*theXd.Coord() +theRb*aSn2*theYd.Coord()));
    
    Standard_Real aRam, aRbm;
    if (aDelta > M_PI/8.)
    {
      // Main radiuses to take into account only 8 points (/cos(Pi/8.))
      aRam = theRa/0.92387953251128674;
      aRbm = theRb/0.92387953251128674;
    }
    else
    {
      // Main radiuses to take into account the arrow
      Standard_Real aTc = cos(aDelta/2);
      aRam = theRa/aTc;
      aRbm = theRb/aTc;
    }
    theB.Add(PointType(theO.Coord() + aRam*aCn1*theXd.Coord() + aRbm*aSn1*theYd.Coord()));
    theB.Add(PointType(theO.Coord() + aRam*aCn2*theXd.Coord() + aRbm*aSn2*theYd.Coord()));

// cos or sin M_PI/4.
#define PI4 0.70710678118654746

// 8 points of the polygon
#define addPoint0 theB.Add(PointType(theO.Coord() +aRam*theXd.Coord()))
#define addPoint1 theB.Add(PointType(theO.Coord() +aRam*PI4*theXd.Coord() +aRbm*PI4*theYd.Coord()))
#define addPoint2 theB.Add(PointType(theO.Coord() +aRbm*theYd.Coord()))
#define addPoint3 theB.Add(PointType(theO.Coord() -aRam*PI4*theXd.Coord() +aRbm*PI4*theYd.Coord()))
#define addPoint4 theB.Add(PointType(theO.Coord() -aRam*theXd.Coord() ))
#define addPoint5 theB.Add(PointType(theO.Coord() -aRam*PI4*theXd.Coord() -aRbm*PI4*theYd.Coord()))
#define addPoint6 theB.Add(PointType(theO.Coord() -aRbm*theYd.Coord()))
#define addPoint7 theB.Add(PointType(theO.Coord() +aRam*PI4*theXd.Coord() -aRbm*PI4*theYd.Coord()))

    Standard_Integer aDeb = (Standard_Integer )( aTeta1/(M_PI/4.));
    Standard_Integer aFin = (Standard_Integer )( aTeta2/(M_PI/4.));
    aDeb++;

    if (aDeb > aFin) return;

    switch (aDeb)
    {
    case 1:
      {
        addPoint1;
        if (aFin <= 1) break;
      }
    case 2:
      {
        addPoint2;
        if (aFin <= 2) break;
      }
    case 3:
      {
        addPoint3;
        if (aFin <= 3) break;
      }
    case 4:
      {
        addPoint4;
        if (aFin <= 4) break;
      }
    case 5:
      {
        addPoint5;
        if (aFin <= 5) break;
      }
    case 6:
      {
        addPoint6;
        if (aFin <= 6) break;
      }
    case 7:
      {
        addPoint7;
        if (aFin <= 7) break;
      }
    case 8:
      {
        addPoint0;
        if (aFin <= 8) break;
      }
    case 9:
      {
        addPoint1;
        if (aFin <= 9) break;
      }  
    case 10:
      {
        addPoint2;
        if (aFin <= 10) break;
      }
    case 11:
      {
        addPoint3;
        if (aFin <= 11) break;
      }  
    case 12:
      {
        addPoint4;
        if (aFin <= 12) break;
      }  
    case 13:
      {
        addPoint5;
        if (aFin <= 13) break;
      }
    case 14:
      {
        addPoint6;
        if (aFin <= 14) break;
      }
    case 15:
      {
        addPoint7;
        if (aFin <= 15) break;
      }
    }
  }
} // end namespace

static void OpenMin(const gp_Dir& V,Bnd_Box& B) {
  gp_Dir OX(1.,0.,0.);
  gp_Dir OY(0.,1.,0.);
  gp_Dir OZ(0.,0.,1.);
  if (V.IsParallel(OX,Precision::Angular())) 
    B.OpenXmin();
  else if (V.IsParallel(OY,Precision::Angular())) 
    B.OpenYmin();
  else if (V.IsParallel(OZ,Precision::Angular())) 
    B.OpenZmin();
  else {
    B.OpenXmin();B.OpenYmin();B.OpenZmin();
  }
}

static void OpenMax(const gp_Dir& V,Bnd_Box& B) {
  gp_Dir OX(1.,0.,0.);
  gp_Dir OY(0.,1.,0.);
  gp_Dir OZ(0.,0.,1.);
  if (V.IsParallel(OX,Precision::Angular())) 
    B.OpenXmax();
  else if (V.IsParallel(OY,Precision::Angular())) 
    B.OpenYmax();
  else if (V.IsParallel(OZ,Precision::Angular())) 
    B.OpenZmax();
  else {
    B.OpenXmax();B.OpenYmax();B.OpenZmax();
  }
}

static void OpenMinMax(const gp_Dir& V,Bnd_Box& B) {
  gp_Dir OX(1.,0.,0.);
  gp_Dir OY(0.,1.,0.);
  gp_Dir OZ(0.,0.,1.);
  if (V.IsParallel(OX,Precision::Angular())) {
    B.OpenXmax();B.OpenXmin();
  }
  else if (V.IsParallel(OY,Precision::Angular())) {
    B.OpenYmax();B.OpenYmin();
  }
  else if (V.IsParallel(OZ,Precision::Angular())) {
    B.OpenZmax();B.OpenZmin();
  }
  else {
    B.OpenXmin();B.OpenYmin();B.OpenZmin();
    B.OpenXmax();B.OpenYmax();B.OpenZmax();
  }
}

static void OpenMin(const gp_Dir2d& V,Bnd_Box2d& B) {
  gp_Dir2d OX(1.,0.);
  gp_Dir2d OY(0.,1.);
  if (V.IsParallel(OX,Precision::Angular())) 
    B.OpenXmin();
  else if (V.IsParallel(OY,Precision::Angular())) 
    B.OpenYmin();
  else {
    B.OpenXmin();B.OpenYmin();
  }
}

static void OpenMax(const gp_Dir2d& V,Bnd_Box2d& B) {
  gp_Dir2d OX(1.,0.);
  gp_Dir2d OY(0.,1.);
  if (V.IsParallel(OX,Precision::Angular())) 
    B.OpenXmax();
  else if (V.IsParallel(OY,Precision::Angular())) 
    B.OpenYmax();
  else {
    B.OpenXmax();B.OpenYmax();
  }
}

static void OpenMinMax(const gp_Dir2d& V,Bnd_Box2d& B) {
  gp_Dir2d OX(1.,0.);
  gp_Dir2d OY(0.,1.);
  if (V.IsParallel(OX,Precision::Angular())) {
    B.OpenXmax();B.OpenXmin();
  }
  else if (V.IsParallel(OY,Precision::Angular())) {
    B.OpenYmax();B.OpenYmin();
  }
  else {
    B.OpenXmin();B.OpenYmin();
    B.OpenXmax();B.OpenYmax();
  }
}


void BndLib::Add( const gp_Lin& L,const Standard_Real P1,
                 const Standard_Real P2,
                 const Standard_Real Tol, Bnd_Box& B) {

  if (Precision::IsNegativeInfinite(P1)) {
    if (Precision::IsNegativeInfinite(P2)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else if (Precision::IsPositiveInfinite(P2)) {
      OpenMinMax(L.Direction(),B);
      B.Add(ElCLib::Value(0.,L));
    }
    else {
      OpenMin(L.Direction(),B);
      B.Add(ElCLib::Value(P2,L));
    }
  }
  else if (Precision::IsPositiveInfinite(P1)) {
    if (Precision::IsNegativeInfinite(P2)) {
      OpenMinMax(L.Direction(),B);
      B.Add(ElCLib::Value(0.,L));
    }
    else if (Precision::IsPositiveInfinite(P2)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else {
      OpenMax(L.Direction(),B);
      B.Add(ElCLib::Value(P2,L));
    }
  }
  else  {
    B.Add(ElCLib::Value(P1,L));
    if (Precision::IsNegativeInfinite(P2)) {
      OpenMin(L.Direction(),B);
    }
    else if (Precision::IsPositiveInfinite(P2)){
      OpenMax(L.Direction(),B);
    }
    else {
      B.Add(ElCLib::Value(P2,L));
    }
  }
  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Lin2d& L,const Standard_Real P1,
                 const Standard_Real P2,
                 const Standard_Real Tol, Bnd_Box2d& B) {

  if (Precision::IsNegativeInfinite(P1)) {
    if (Precision::IsNegativeInfinite(P2)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else if (Precision::IsPositiveInfinite(P2)) {
      OpenMinMax(L.Direction(),B);
      B.Add(ElCLib::Value(0.,L));
    }
    else {
      OpenMin(L.Direction(),B);
      B.Add(ElCLib::Value(P2,L));
    }
  }
  else if (Precision::IsPositiveInfinite(P1)) {
    if (Precision::IsNegativeInfinite(P2)) {
      OpenMinMax(L.Direction(),B);
      B.Add(ElCLib::Value(0.,L));
    }
    else if (Precision::IsPositiveInfinite(P2)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else {
      OpenMax(L.Direction(),B);
      B.Add(ElCLib::Value(P2,L));
    }
  }
  else  {
    B.Add(ElCLib::Value(P1,L));
    if (Precision::IsNegativeInfinite(P2)) {
      OpenMin(L.Direction(),B);
    }
    else if (Precision::IsPositiveInfinite(P2)){
      OpenMax(L.Direction(),B);
    }
    else {
      B.Add(ElCLib::Value(P2,L));
    }
  }
  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Circ& C,const Standard_Real Tol, Bnd_Box& B) {

  Standard_Real R = C.Radius();
  gp_XYZ O  = C.Location().XYZ();
  gp_XYZ Xd = C.XAxis().Direction().XYZ();
  gp_XYZ Yd = C.YAxis().Direction().XYZ();
  B.Add(gp_Pnt(O -R*Xd -R*Yd));
  B.Add(gp_Pnt(O -R*Xd +R*Yd));
  B.Add(gp_Pnt(O +R*Xd -R*Yd));
  B.Add(gp_Pnt(O +R*Xd +R*Yd));
  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Circ& C,const Standard_Real P1,
  const Standard_Real P2,
  const Standard_Real Tol, Bnd_Box& B) {

  Compute(P1,P2,C.Radius(),C.Radius(),gp_Pnt(C.XAxis().Direction().XYZ()), 
          gp_Pnt(C.YAxis().Direction().XYZ()),C.Location(),B);
  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Circ2d& C,const Standard_Real Tol, Bnd_Box2d& B) {

  Standard_Real R = C.Radius();
  gp_XY O  = C.Location().XY();
  gp_XY Xd = C.XAxis().Direction().XY();
  gp_XY Yd = C.YAxis().Direction().XY();
  B.Add(gp_Pnt2d(O -R*Xd -R*Yd));
  B.Add(gp_Pnt2d(O -R*Xd +R*Yd));
  B.Add(gp_Pnt2d(O +R*Xd -R*Yd));
  B.Add(gp_Pnt2d(O +R*Xd +R*Yd));
  B.Enlarge(Tol);
}

void BndLib::Add(const gp_Circ2d& C,const Standard_Real P1,
   const Standard_Real P2,
   const Standard_Real Tol, Bnd_Box2d& B) {

       Compute(P1,P2,C.Radius(),C.Radius(),gp_Pnt2d(C.XAxis().Direction().XY()),
         gp_Pnt2d(C.YAxis().Direction().XY()),C.Location(),B);
  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Elips& C,const Standard_Real Tol, Bnd_Box& B) {

  Standard_Real Ra = C.MajorRadius();
  Standard_Real Rb = C.MinorRadius();
  gp_XYZ Xd = C.XAxis().Direction().XYZ();
  gp_XYZ Yd = C.YAxis().Direction().XYZ();
  gp_XYZ O  = C.Location().XYZ();
  B.Add(gp_Pnt(O +Ra*Xd +Rb*Yd));
  B.Add(gp_Pnt(O -Ra*Xd +Rb*Yd));
  B.Add(gp_Pnt(O -Ra*Xd -Rb*Yd));
  B.Add(gp_Pnt(O +Ra*Xd -Rb*Yd));
  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Elips& C,const Standard_Real P1,
                 const Standard_Real P2,
                 const Standard_Real Tol, Bnd_Box& B) {

  Compute(P1,P2,C.MajorRadius(),C.MinorRadius(),gp_Pnt(C.XAxis().Direction().XYZ()), 
          gp_Pnt(C.YAxis().Direction().XYZ()),C.Location(),B);
  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Elips2d& C,const Standard_Real Tol, Bnd_Box2d& B) {

  Standard_Real Ra= C.MajorRadius();
  Standard_Real Rb= C.MinorRadius();
  gp_XY Xd = C.XAxis().Direction().XY();
  gp_XY Yd = C.YAxis().Direction().XY();
  gp_XY O  = C.Location().XY();
  B.Add(gp_Pnt2d(O +Ra*Xd +Rb*Yd));
  B.Add(gp_Pnt2d(O -Ra*Xd +Rb*Yd));
  B.Add(gp_Pnt2d(O -Ra*Xd -Rb*Yd));
  B.Add(gp_Pnt2d(O +Ra*Xd -Rb*Yd));
  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Elips2d& C,const Standard_Real P1,
                 const Standard_Real P2,
                 const Standard_Real Tol, Bnd_Box2d& B) {

  Compute(P1,P2,C.MajorRadius(),C.MinorRadius(),
   gp_Pnt2d(C.XAxis().Direction().XY()),
   gp_Pnt2d(C.YAxis().Direction().XY()),C.Location(),B);
  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Parab& P,const Standard_Real P1,
                 const Standard_Real P2,
                 const Standard_Real Tol, Bnd_Box& B) {

  if (Precision::IsNegativeInfinite(P1)) {
    if (Precision::IsNegativeInfinite(P2)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else if (Precision::IsPositiveInfinite(P2)) {
      B.OpenXmax();B.OpenYmax();B.OpenZmax();
    }
    else {
      B.Add(ElCLib::Value(P2,P));
    }
    B.OpenXmin();B.OpenYmin();B.OpenZmin();
  }
  else if (Precision::IsPositiveInfinite(P1)) {
    if (Precision::IsNegativeInfinite(P2)) {
      B.OpenXmin();B.OpenYmin();B.OpenZmin();
    }
    else if (Precision::IsPositiveInfinite(P2)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else {
      B.Add(ElCLib::Value(P2,P));
    }
    B.OpenXmax();B.OpenYmax();B.OpenZmax();
  }
  else  {
    B.Add(ElCLib::Value(P1,P));
    if (Precision::IsNegativeInfinite(P2)) {
      B.OpenXmin();B.OpenYmin();B.OpenZmin();
    }
    else if (Precision::IsPositiveInfinite(P2)){
      B.OpenXmax();B.OpenYmax();B.OpenZmax();
    }
    else {
      B.Add(ElCLib::Value(P2,P));
      if (P1*P2<0) B.Add(ElCLib::Value(0.,P));
    }
  }
  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Parab2d& P,const Standard_Real P1,
                 const Standard_Real P2,
                 const Standard_Real Tol, Bnd_Box2d& B) {

  if (Precision::IsNegativeInfinite(P1)) {
    if (Precision::IsNegativeInfinite(P2)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else if (Precision::IsPositiveInfinite(P2)) {
      B.OpenXmax();B.OpenYmax();
    }
    else {
      B.Add(ElCLib::Value(P2,P));
    }
    B.OpenXmin();B.OpenYmin();
  }
  else if (Precision::IsPositiveInfinite(P1)) {
    if (Precision::IsNegativeInfinite(P2)) {
      B.OpenXmin();B.OpenYmin();
    }
    else if (Precision::IsPositiveInfinite(P2)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else {
      B.Add(ElCLib::Value(P2,P));
    }
    B.OpenXmax();B.OpenYmax();
  }
  else  {
    B.Add(ElCLib::Value(P1,P));
    if (Precision::IsNegativeInfinite(P2)) {
      B.OpenXmin();B.OpenYmin();
    }
    else if (Precision::IsPositiveInfinite(P2)){
      B.OpenXmax();B.OpenYmax();
    }
    else {
      B.Add(ElCLib::Value(P2,P));
      if (P1*P2<0) B.Add(ElCLib::Value(0.,P));
    }
  }
  B.Enlarge(Tol);
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void BndLib::Add(const gp_Hypr& H,
                 const Standard_Real P1,
                 const Standard_Real P2,
                 const Standard_Real Tol, 
                 Bnd_Box& B) 
{
  if (Precision::IsNegativeInfinite(P1)) {
    if (Precision::IsNegativeInfinite(P2)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else if (Precision::IsPositiveInfinite(P2)) {
      B.OpenXmax();B.OpenYmax();B.OpenZmax();
    }
    else {
      B.Add(ElCLib::Value(P2,H));
    }
    B.OpenXmin();B.OpenYmin();B.OpenZmin();
  }
  else if (Precision::IsPositiveInfinite(P1)) {
    if (Precision::IsNegativeInfinite(P2)) {
      B.OpenXmin();B.OpenYmin();B.OpenZmin();
    }
    else if (Precision::IsPositiveInfinite(P2)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else {
      B.Add(ElCLib::Value(P2,H));
    }
    B.OpenXmax();B.OpenYmax();B.OpenZmax();
  }
  else  {
    B.Add(ElCLib::Value(P1,H));
    if (Precision::IsNegativeInfinite(P2)) {
      B.OpenXmin();B.OpenYmin();B.OpenZmin();
    }
    else if (Precision::IsPositiveInfinite(P2)){
      B.OpenXmax();B.OpenYmax();B.OpenZmax();
    }
    else {
      ComputeBox(H, P1, P2, B);
    }
  }
  B.Enlarge(Tol);
}

void BndLib::Add(const gp_Hypr2d& H,const Standard_Real P1,
                 const Standard_Real P2,
                 const Standard_Real Tol, Bnd_Box2d& B) {
  
  if (Precision::IsNegativeInfinite(P1)) {
    if (Precision::IsNegativeInfinite(P2)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else if (Precision::IsPositiveInfinite(P2)) {
      B.OpenXmax();B.OpenYmax();
    }
    else {
      B.Add(ElCLib::Value(P2,H));
    }
    B.OpenXmin();B.OpenYmin();
  }
  else if (Precision::IsPositiveInfinite(P1)) {
    if (Precision::IsNegativeInfinite(P2)) {
      B.OpenXmin();B.OpenYmin();
    }
    else if (Precision::IsPositiveInfinite(P2)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else {
      B.Add(ElCLib::Value(P2,H));
    }
    B.OpenXmax();B.OpenYmax();
  }
  else  {
    B.Add(ElCLib::Value(P1,H));
    if (Precision::IsNegativeInfinite(P2)) {
      B.OpenXmin();B.OpenYmin();
    }
    else if (Precision::IsPositiveInfinite(P2)){
      B.OpenXmax();B.OpenYmax();
    }
    else {
      B.Add(ElCLib::Value(P2,H));
      if (P1*P2<0) B.Add(ElCLib::Value(0.,H));
    }
  }
  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Cylinder& S,const Standard_Real UMin,
                 const Standard_Real UMax,const Standard_Real VMin,
                 const Standard_Real VMax,const Standard_Real Tol, Bnd_Box& B) {

  if (Precision::IsNegativeInfinite(VMin)) {
    if (Precision::IsNegativeInfinite(VMax)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else if (Precision::IsPositiveInfinite(VMax)) {
       OpenMinMax(S.Axis().Direction(),B);
    }
    else {
      Compute(UMin,UMax,S.Radius(),S.Radius(),
       gp_Pnt(S.XAxis().Direction().XYZ()),
       gp_Pnt(S.YAxis().Direction().XYZ()),
       gp_Pnt(S.Location().XYZ() + VMax*S.Axis().Direction().XYZ()),B);
      OpenMin(S.Axis().Direction(),B);
    }
  }
  else if (Precision::IsPositiveInfinite(VMin)) {
    if (Precision::IsNegativeInfinite(VMax)) {
      OpenMinMax(S.Axis().Direction(),B);
    }
    else if (Precision::IsPositiveInfinite(VMax)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else {
      Compute(UMin,UMax,S.Radius(),S.Radius(),
       gp_Pnt(S.XAxis().Direction().XYZ()),
       gp_Pnt(S.YAxis().Direction().XYZ()),
       gp_Pnt(S.Location().XYZ() + VMax*S.Axis().Direction().XYZ()),B);
      OpenMax(S.Axis().Direction(),B);
    }

  }
  else {
    Compute(UMin,UMax,S.Radius(),S.Radius(),
     gp_Pnt(S.XAxis().Direction().XYZ()),
     gp_Pnt(S.YAxis().Direction().XYZ()),
     gp_Pnt(S.Location().XYZ() + VMin*S.Axis().Direction().XYZ()),B);
    if (Precision::IsNegativeInfinite(VMax)) {
      OpenMin(S.Axis().Direction(),B);
    }
    else if (Precision::IsPositiveInfinite(VMax)) {
      OpenMax(S.Axis().Direction(),B); 
    }
    else {
      Compute(UMin,UMax,S.Radius(),S.Radius(),
       gp_Pnt(S.XAxis().Direction().XYZ()),
       gp_Pnt(S.YAxis().Direction().XYZ()),
       gp_Pnt(S.Location().XYZ() + VMax*S.Axis().Direction().XYZ()),B);
    }
  }

  B.Enlarge(Tol);

}

void BndLib::Add( const gp_Cylinder& S,const Standard_Real VMin,
                 const Standard_Real VMax,const Standard_Real Tol, Bnd_Box& B) {

  BndLib::Add(S,0.,2.*M_PI,VMin,VMax,Tol,B);
}

void BndLib::Add(const gp_Cone& S,const Standard_Real UMin,
                 const Standard_Real UMax,const Standard_Real VMin,
                 const Standard_Real VMax,const Standard_Real Tol, Bnd_Box& B) {

  Standard_Real R = S.RefRadius();
  Standard_Real A = S.SemiAngle();
  if (Precision::IsNegativeInfinite(VMin)) {
    if (Precision::IsNegativeInfinite(VMax)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else if (Precision::IsPositiveInfinite(VMax)) {
      gp_Dir D(Cos(A)*S.Axis().Direction());
      OpenMinMax(D,B); 
    }
    else {
      Compute(UMin,UMax,R+VMax*Sin(A),R+VMax*Sin(A),
       gp_Pnt(S.XAxis().Direction().XYZ()),
       gp_Pnt(S.YAxis().Direction().XYZ()),
       gp_Pnt(S.Location().XYZ() + 
       VMax*Cos(A)*S.Axis().Direction().XYZ()),B);
      gp_Dir D(Cos(A)*S.Axis().Direction());
      OpenMin(D,B);     
    }

  }
  else if (Precision::IsPositiveInfinite(VMin)) {
    if (Precision::IsNegativeInfinite(VMax)) {
      gp_Dir D(Cos(A)*S.Axis().Direction());
      OpenMinMax(D,B);
    }
    else if (Precision::IsPositiveInfinite(VMax)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else {
      Compute(UMin,UMax,R+VMax*Sin(A),R+VMax*Sin(A),
       gp_Pnt(S.XAxis().Direction().XYZ()),
       gp_Pnt(S.YAxis().Direction().XYZ()),
       gp_Pnt(S.Location().XYZ() + 
       VMax*Cos(A)*S.Axis().Direction().XYZ()),B);
      gp_Dir D(Cos(A)*S.Axis().Direction());
      OpenMax(D,B);
    }

  }
  else {
    Compute(UMin,UMax,R+VMin*Sin(A),R+VMin*Sin(A),
     gp_Pnt(S.XAxis().Direction().XYZ()),
     gp_Pnt(S.YAxis().Direction().XYZ()),
     gp_Pnt(S.Location().XYZ() + 
     VMin*Cos(A)*S.Axis().Direction().XYZ()),B);
    if (Precision::IsNegativeInfinite(VMax)) {
      gp_Dir D(Cos(A)*S.Axis().Direction());
      OpenMin(D,B);
    }
    else if (Precision::IsPositiveInfinite(VMax)) {
      gp_Dir D(Cos(A)*S.Axis().Direction());
      OpenMax(D,B);
    }
    else {
      Compute(UMin,UMax,R+VMax*Sin(A),R+VMax*Sin(A),
       gp_Pnt(S.XAxis().Direction().XYZ()),
       gp_Pnt(S.YAxis().Direction().XYZ()),
       gp_Pnt(S.Location().XYZ() + 
       VMax*Cos(A)*S.Axis().Direction().XYZ()),B);
    }
  }


  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Cone& S,const Standard_Real VMin,
                 const Standard_Real VMax,const Standard_Real Tol, Bnd_Box& B) {

  BndLib::Add(S,0.,2.*M_PI,VMin,VMax,Tol,B);
}

void BndLib::Add(const gp_Sphere& S,const Standard_Real UMin,
                 const Standard_Real UMax,const Standard_Real VMin,
                 const Standard_Real VMax,const Standard_Real Tol, Bnd_Box& B) {

#if 0
 Standard_Real Fi1;
 Standard_Real Fi2;
 if (VMax<VMin) {
   Fi1 = VMax;
   Fi2 = VMin;
 }
 else {
   Fi1 = VMin;
   Fi2 = VMax;
 }
 
 if (-Fi1>Precision::Angular()) {
   if (-Fi2>Precision::Angular()) {
     Compute(UMin,UMax,S.Radius(),S.Radius(),
        gp_Pnt(S.XAxis().Direction().XYZ()),gp_Pnt(S.YAxis().Direction().XYZ()),
        S.Location(),B);
     Compute(UMin,UMax,S.Radius(),S.Radius(),
        gp_Pnt(S.XAxis().Direction().XYZ()),gp_Pnt(S.YAxis().Direction().XYZ()),
     gp_Pnt(S.Location().XYZ()- S.Radius()*S.Position().Axis().Direction().XYZ()),B);
   }
   else {
     Compute(UMin,UMax,S.Radius(),S.Radius(),
     gp_Pnt(S.XAxis().Direction().XYZ()),gp_Pnt(S.YAxis().Direction().XYZ()),
     gp_Pnt(S.Location().XYZ()+ S.Radius()*S.Position().Axis().Direction().XYZ()),B);
     Compute(UMin,UMax,S.Radius(),S.Radius(),
     gp_Pnt(S.XAxis().Direction().XYZ()),gp_Pnt(S.YAxis().Direction().XYZ()),
     gp_Pnt(S.Location().XYZ()- S.Radius()*S.Position().Axis().Direction().XYZ()),B);
   }
 }
 else {
   Compute(UMin,UMax,S.Radius(),S.Radius(),
      gp_Pnt(S.XAxis().Direction().XYZ()),gp_Pnt(S.YAxis().Direction().XYZ()),
      S.Location(),B);
   Compute(UMin,UMax,S.Radius(),S.Radius(),
   gp_Pnt(S.XAxis().Direction().XYZ()),gp_Pnt(S.YAxis().Direction().XYZ()),
   gp_Pnt(S.Location().XYZ() +S.Radius()*S.Position().Axis().Direction().XYZ()),B);
 } 
 B.Enlarge(Tol);
#else
 Standard_Real u,v,du,dv;
 Standard_Integer iu,iv;
 du = (UMax-UMin)/10;
 dv = (VMax-VMin)/10;
 Standard_Real COSV[11];
 Standard_Real SINV[11];
 for(iv=0,v=VMin;iv<=10;iv++) { 
   COSV[iv]=cos(v);
   SINV[iv]=sin(v);   
   v+=dv;
 }
 for(u=UMin,iu=0; iu<=10 ; iu++) { 
   Standard_Real Radiuscosu=S.Radius()*cos(u);
   Standard_Real Radiussinu=S.Radius()*sin(u);
   for(v=VMin,iv=0; iv<=10 ; iv++) { 
     Standard_Real sinv=SINV[iv];
     Standard_Real cosv=COSV[iv];
     gp_XYZ M;
     M.SetLinearForm (cosv*Radiuscosu, S.Position().XDirection().XYZ(),
        cosv*Radiussinu, S.Position().YDirection().XYZ(),
        S.Radius()*sinv,     S.Position().Direction().XYZ() ,
        S.Position().Location().XYZ()  );
     //-- static int t=0;
     //-- cout<<"point p"<<++t<<" "<<M.X()<<" "<<M.Y()<<" "<<M.Z()<<endl;
     B.Add(gp_Pnt(M));
     v+=dv;
   }
   u+=du;
 }
 
 Standard_Real Maxduv = Max(du,dv)*0.5;
 Standard_Real Fleche = S.Radius() * (1 - cos(Maxduv));
 B.Enlarge(Fleche);
 B.Enlarge(10*Tol);
#endif
}

void BndLib::Add( const gp_Sphere& S,const Standard_Real Tol, Bnd_Box& B) {

  Standard_Real R = S.Radius();
  gp_XYZ O = S.Location().XYZ();
  gp_XYZ Xd = S.XAxis().Direction().XYZ();
  gp_XYZ Yd = S.YAxis().Direction().XYZ();
  gp_XYZ Zd = S.Position().Axis().Direction().XYZ();
  B.Add(gp_Pnt(O -R*Xd -R*Yd+ R*Zd)); 
  B.Add(gp_Pnt(O -R*Xd +R*Yd+ R*Zd)); 
  B.Add(gp_Pnt(O +R*Xd -R*Yd+ R*Zd)); 
  B.Add(gp_Pnt(O +R*Xd +R*Yd+ R*Zd)); 
  B.Add(gp_Pnt(O +R*Xd -R*Yd- R*Zd)); 
  B.Add(gp_Pnt(O -R*Xd -R*Yd- R*Zd)); 
  B.Add(gp_Pnt(O +R*Xd +R*Yd- R*Zd)); 
  B.Add(gp_Pnt(O -R*Xd +R*Yd- R*Zd)); 
  B.Enlarge(Tol);
}

void BndLib::Add(const gp_Torus& S,const Standard_Real UMin,
   const Standard_Real UMax,const Standard_Real VMin,
   const Standard_Real VMax,const Standard_Real Tol, Bnd_Box& B) {

  Standard_Integer Fi1;
  Standard_Integer Fi2;
  if (VMax<VMin) {
    Fi1 = (Standard_Integer )( VMax/(M_PI/4.));
    Fi2 = (Standard_Integer )( VMin/(M_PI/4.));
  }
  else {
    Fi1 = (Standard_Integer )( VMin/(M_PI/4.));
    Fi2 = (Standard_Integer )( VMax/(M_PI/4.));
  }
  Fi2++;
  
  Standard_Real Ra = S.MajorRadius();
  Standard_Real Ri = S.MinorRadius();

  if (Fi2<Fi1) return;

#define SC 0.71
#define addP0    (Compute(UMin,UMax,Ra+Ri,Ra+Ri,gp_Pnt(S.XAxis().Direction().XYZ()),gp_Pnt(S.YAxis().Direction().XYZ()),S.Location(),B))
#define addP1    (Compute(UMin,UMax,Ra+Ri*SC,Ra+Ri*SC,gp_Pnt(S.XAxis().Direction().XYZ()),gp_Pnt(S.YAxis().Direction().XYZ()),gp_Pnt(S.Location().XYZ()+(Ri*SC)*S.Axis().Direction().XYZ()),B))
#define addP2    (Compute(UMin,UMax,Ra,Ra,gp_Pnt(S.XAxis().Direction().XYZ()),gp_Pnt(S.YAxis().Direction().XYZ()),gp_Pnt(S.Location().XYZ()+Ri*S.Axis().Direction().XYZ()),B))
#define addP3    (Compute(UMin,UMax,Ra-Ri*SC,Ra-Ri*SC,gp_Pnt(S.XAxis().Direction().XYZ()),gp_Pnt(S.YAxis().Direction().XYZ()),gp_Pnt(S.Location().XYZ()+(Ri*SC)*S.Axis().Direction().XYZ()),B))
#define addP4    (Compute(UMin,UMax,Ra-Ri,Ra-Ri,gp_Pnt(S.XAxis().Direction().XYZ()),gp_Pnt(S.YAxis().Direction().XYZ()),S.Location(),B))
#define addP5    (Compute(UMin,UMax,Ra-Ri*SC,Ra-Ri*SC,gp_Pnt(S.XAxis().Direction().XYZ()),gp_Pnt(S.YAxis().Direction().XYZ()),gp_Pnt(S.Location().XYZ()-(Ri*SC)*S.Axis().Direction().XYZ()),B))
#define addP6    (Compute(UMin,UMax,Ra,Ra,gp_Pnt(S.XAxis().Direction().XYZ()),gp_Pnt(S.YAxis().Direction().XYZ()),gp_Pnt(S.Location().XYZ()-Ri*S.Axis().Direction().XYZ()),B))
#define addP7    (Compute(UMin,UMax,Ra+Ri*SC,Ra+Ri*SC,gp_Pnt(S.XAxis().Direction().XYZ()),gp_Pnt(S.YAxis().Direction().XYZ()),gp_Pnt(S.Location().XYZ()-(Ri*SC)*S.Axis().Direction().XYZ()),B))
  
  switch (Fi1) {
  case 0 : 
    {
      addP0;
      if (Fi2 <= 0) break;
    }
  case 1 : 
    {
      addP1;
      if (Fi2 <= 1) break;
    }
  case 2 :  
    {
      addP2;
      if (Fi2 <= 2) break;
    }
  case 3 :  
    {
      addP3;
      if (Fi2 <= 3) break;
    }
  case 4 :  
    {
      addP4;
      if (Fi2 <= 4) break;
    }
  case 5 :  
    {
      addP5;
      if (Fi2 <= 5) break;
    }
  case 6 :  
    {
      addP6;
      if (Fi2 <= 6) break;
    }
  case 7 :  
    {
      addP7;
      if (Fi2 <= 7) break;
    }
  case 8 :  
  default :
    {
      addP0;
      switch (Fi2) {
      case 15 :  
        addP7;
      case 14 :  
        addP6;
      case 13 :  
        addP5;
      case 12 :  
        addP4;
      case 11 :  
        addP3;
      case 10 :  
        addP2;
      case 9 : 
        addP1;
      case 8 : 
        break;
      }    
    }
  }    
  B.Enlarge(Tol);
}


void BndLib::Add( const gp_Torus& S,const Standard_Real Tol, Bnd_Box& B) {

  Standard_Real RMa = S.MajorRadius();
  Standard_Real Rmi = S.MinorRadius(); 
  gp_XYZ O = S.Location().XYZ();
  gp_XYZ Xd = S.XAxis().Direction().XYZ();
  gp_XYZ Yd = S.YAxis().Direction().XYZ();
  gp_XYZ Zd = S.Axis().Direction().XYZ();
  B.Add(gp_Pnt(O -(RMa+Rmi)*Xd -(RMa+Rmi)*Yd+ Rmi*Zd)); 
  B.Add(gp_Pnt(O -(RMa+Rmi)*Xd -(RMa+Rmi)*Yd- Rmi*Zd)); 
  B.Add(gp_Pnt(O +(RMa+Rmi)*Xd -(RMa+Rmi)*Yd+ Rmi*Zd)); 
  B.Add(gp_Pnt(O +(RMa+Rmi)*Xd -(RMa+Rmi)*Yd- Rmi*Zd)); 
  B.Add(gp_Pnt(O -(RMa+Rmi)*Xd +(RMa+Rmi)*Yd+ Rmi*Zd)); 
  B.Add(gp_Pnt(O -(RMa+Rmi)*Xd +(RMa+Rmi)*Yd- Rmi*Zd)); 
  B.Add(gp_Pnt(O +(RMa+Rmi)*Xd +(RMa+Rmi)*Yd+ Rmi*Zd)); 
  B.Add(gp_Pnt(O +(RMa+Rmi)*Xd +(RMa+Rmi)*Yd- Rmi*Zd)); 
  B.Enlarge(Tol);
}
//=======================================================================
//function : ComputeBox
//purpose  : 
//=======================================================================
Standard_Integer ComputeBox(const gp_Hypr& aHypr, 
                            const Standard_Real aT1, 
                            const Standard_Real aT2, 
                            Bnd_Box& aBox)
{
  Standard_Integer i, iErr;
  Standard_Real aRmaj, aRmin, aA, aB, aABP, aBAM, aT3, aCf, aEps;
  gp_Pnt aP1, aP2, aP3, aP0;
  //
  //
  aP1=ElCLib::Value(aT1, aHypr);
  aP2=ElCLib::Value(aT2, aHypr);
  //
  aBox.Add(aP1);
  aBox.Add(aP2);
  //
  if (aT1*aT2<0.) {
    aP0=ElCLib::Value(0., aHypr);
    aBox.Add(aP0);
  }
  //
  aEps=Epsilon(1.);
  iErr=1;
  //
  const gp_Ax2& aPos=aHypr.Position();
  const gp_XYZ& aXDir = aPos.XDirection().XYZ();
  const gp_XYZ& aYDir = aPos.YDirection().XYZ();
  aRmaj=aHypr.MajorRadius();
  aRmin=aHypr.MinorRadius();
  //
  aT3=0;
  for (i=1; i<=3; ++i) {
    aA=aRmin*aYDir.Coord(i);
    aB=aRmaj*aXDir.Coord(i);
    //
    aABP=aA+aB;
    aBAM=aB-aA;
    //
    aABP=fabs(aABP);
    aBAM=fabs(aBAM);
    //
    if (aABP<aEps || aBAM<aEps) {
      continue;
    }
    //
    aCf=aBAM/aABP;
    aT3=log(sqrt(aCf));
    //
    if (aT3<aT1 || aT3>aT2) {
      continue;
    }
    iErr=0;
    break;
  }
  //
  if (iErr) {
    return iErr;
  }
  //
  aP3=ElCLib::Value(aT3, aHypr);
  aBox.Add(aP3);
  //
  return iErr;
}
