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


#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Torus.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DimensionError.hxx>

void gp_Torus::Coefficients (TColStd_Array1OfReal& theCoef) const
{
  //  R = majorRadius;
  //  r = minorRadius.

  //  X = (R + r*cos(V))*cos(U)
  //  Y = (R + r*cos(V))*sin(U)
  //  Z = r*sin(V)

  //Therefore,
  //  4*R*R*(r*r - Z*Z) = (X*X + Y*Y + Z*Z - R*R - r*r)^2
  //Or
  //  X^4+Y^4+Z^4+
  //  2*((X*Y)^2+(X*Z)^2+(Y*Z)^2)-
  //  2*(R^2+r^2)*(X^2+Y^2)+
  //  2*(R^2-r^2)*Z^2+(R^2-r^2)^2 = 0.0

  const Standard_Integer aLowIndex = theCoef.Lower();
  Standard_DimensionError_Raise_if (theCoef.Length() < 35,
              "gp_Torus::theCoefficients(): Dimension mismatch");
  
  gp_Trsf aTr;
  aTr.SetTransformation (pos);
  const Standard_Real aT11 = aTr.Value (1, 1);
  const Standard_Real aT12 = aTr.Value (1, 2);
  const Standard_Real aT13 = aTr.Value (1, 3);
  const Standard_Real aT14 = aTr.Value (1, 4);
  const Standard_Real aT21 = aTr.Value (2, 1);
  const Standard_Real aT22 = aTr.Value (2, 2);
  const Standard_Real aT23 = aTr.Value (2, 3);
  const Standard_Real aT24 = aTr.Value (2, 4);
  const Standard_Real aT31 = aTr.Value (3, 1);
  const Standard_Real aT32 = aTr.Value (3, 2);
  const Standard_Real aT33 = aTr.Value (3, 3);
  const Standard_Real aT34 = aTr.Value (3, 4);

  const Standard_Real aTcol1sq = aT11*aT11 + aT21*aT21 + aT31*aT31;
  const Standard_Real aTcol2sq = aT12*aT12 + aT22*aT22 + aT32*aT32;
  const Standard_Real aTcol3sq = aT13*aT13 + aT23*aT23 + aT33*aT33;
  const Standard_Real aTcol4sq = aT14*aT14 + aT24*aT24 + aT34*aT34;
  const Standard_Real aTcol1Tcol2 = aT11*aT12 + aT21*aT22 + aT31*aT32;
  const Standard_Real aTcol1Tcol3 = aT11*aT13 + aT21*aT23 + aT31*aT33;
  const Standard_Real aTcol2Tcol3 = aT12*aT13 + aT22*aT23 + aT32*aT33;
  const Standard_Real aTcol1Tcol4 = aT11*aT14 + aT21*aT24 + aT31*aT34;
  const Standard_Real aTcol2Tcol4 = aT12*aT14 + aT22*aT24 + aT32*aT34;
  const Standard_Real aTcol3Tcol4 = aT13*aT14 + aT23*aT24 + aT33*aT34;
  
  const Standard_Real aSumRadius = (majorRadius*majorRadius +
                                    minorRadius*minorRadius);
  const Standard_Real aSubRadius = (majorRadius*majorRadius -
                                    minorRadius*minorRadius);

  /*
  After substitution
  Transpose([X Y Z 1]) = aTr*Transpose([X Y Z 1])
  we will obtain:
  */

  theCoef(aLowIndex)     = aTcol1sq*aTcol1sq;  //X^4
  theCoef(aLowIndex+1)   = aTcol2sq*aTcol2sq;  //Y^4
  theCoef(aLowIndex+2)   = aTcol3sq*aTcol3sq;  //Z^4
  theCoef(aLowIndex+3)   = 4.0*aTcol1sq*aTcol1Tcol2; //X^3*Y
  theCoef(aLowIndex+4)   = 4.0*aTcol1sq*aTcol1Tcol3; //X^3*Z
  theCoef(aLowIndex+5)   = 4.0*aTcol2sq*aTcol1Tcol2; //X*Y^3
  theCoef(aLowIndex+6)   = 4.0*aTcol2sq*aTcol2Tcol3; //Y^3*Z
  theCoef(aLowIndex+7)   = 4.0*aTcol3sq*aTcol1Tcol3; //X*Z^3
  theCoef(aLowIndex+8)   = 4.0*aTcol3sq*aTcol2Tcol3; //Y*Z^3
  theCoef(aLowIndex+9)   = 2.0*(aTcol1sq*aTcol2sq + 
                                2.0*aTcol1Tcol2*aTcol1Tcol2);  //X^2*Y^2
  theCoef(aLowIndex+10)  = 2.0*(aTcol1sq*aTcol3sq +
                                2.0*aTcol1Tcol3*aTcol1Tcol3);  //X^2*Z^2
  theCoef(aLowIndex+11)  = 2.0*(aTcol2sq*aTcol3sq +
                                2.0*aTcol2Tcol3*aTcol2Tcol3);  //Y^2*Z^2
  theCoef(aLowIndex+12)  = 4.0*(aTcol1sq*aTcol2Tcol3 +
                                2.0*aTcol1Tcol2*aTcol1Tcol3); //X^2*Y*Z
  theCoef(aLowIndex+13)  = 4.0*(aTcol2sq*aTcol1Tcol3 +
                                2.0*aTcol1Tcol2*aTcol2Tcol3); //X*Y^2*Z
  theCoef(aLowIndex+14)  = 4.0*(aTcol3sq*aTcol1Tcol2 +
                                2.0*aTcol1Tcol3*aTcol2Tcol3); //X*Y*Z^2

  theCoef(aLowIndex+15)  = 4.0*aTcol1sq*aTcol1Tcol4; //X^3
  theCoef(aLowIndex+16)  = 4.0*aTcol2sq*aTcol2Tcol4; //Y^3
  theCoef(aLowIndex+17)  = 4.0*aTcol3sq*aTcol3Tcol4; //Z^3
  theCoef(aLowIndex+18)  = 4.0*(aTcol1sq*aTcol2Tcol4 +
                                2.0*aTcol1Tcol4*aTcol1Tcol2); //X^2*Y
  theCoef(aLowIndex+19)  = 4.0*(aTcol1sq*aTcol3Tcol4 +
                                2.0*aTcol1Tcol4*aTcol1Tcol3); //X^2*Z
  theCoef(aLowIndex+20)  = 4.0*(aTcol2sq*aTcol1Tcol4 +
                                2.0*aTcol2Tcol4*aTcol1Tcol2); //X*Y^2
  theCoef(aLowIndex+21)  = 4.0*(aTcol2sq*aTcol3Tcol4 +
                                2.0*aTcol2Tcol4*aTcol2Tcol3); //Y^2*Z
  theCoef(aLowIndex+22)  = 4.0*(aTcol3sq*aTcol1Tcol4 +
                                2.0*aTcol3Tcol4*aTcol1Tcol3); //X*Z^2
  theCoef(aLowIndex+23)  = 4.0*(aTcol3sq*aTcol2Tcol4 +
                                2.0*aTcol3Tcol4*aTcol2Tcol3); //Y*Z^2
  theCoef(aLowIndex+24)  = 8.0*(aTcol1Tcol2*aTcol3Tcol4 +
                                aTcol2Tcol3*aTcol1Tcol4 +
                                aTcol2Tcol4*aTcol1Tcol3); //X*Y*Z

  theCoef(aLowIndex+25)  = 2.0*(aSubRadius*aT31*aT31 -
                                aSumRadius*(aT11*aT11 + aT21*aT21) +
                                aTcol4sq*aTcol1sq +
                                2.0*aTcol1Tcol4*aTcol1Tcol4); //X^2
  theCoef(aLowIndex+26)  = 2.0*(aSubRadius*aT32*aT32 -
                                aSumRadius*(aT12*aT12 + aT22*aT22) +
                                aTcol4sq*aTcol2sq +
                                2.0*aTcol2Tcol4*aTcol2Tcol4); //Y^2
  theCoef(aLowIndex+27)  = 2.0*(aSubRadius*aT33*aT33 -
                                aSumRadius*(aT13*aT13 + aT23*aT23) +
                                aTcol4sq*aTcol3sq +
                                2.0*aTcol3Tcol4*aTcol3Tcol4); //Z^2
  theCoef(aLowIndex+28)  = 4.0*(aSubRadius*aT31*aT32 -
                                aSumRadius*(aT11*aT12 + aT21*aT22) +
                                aTcol4sq*aTcol1Tcol2 +
                                2.0*aTcol1Tcol4*aTcol2Tcol4); //X*Y
  theCoef(aLowIndex+29)  = 4.0*(aSubRadius*aT31*aT33 - 
                                aSumRadius*(aT11*aT13 + aT21*aT23) +
                                aTcol4sq*aTcol1Tcol3 +
                                2.0*aTcol1Tcol4*aTcol3Tcol4); //X*Z
  theCoef(aLowIndex+30)  = 4.0*(aSubRadius*aT32*aT33 -
                                aSumRadius*(aT12*aT13 + aT22*aT23) +
                                aTcol4sq*aTcol2Tcol3 +
                                2.0*aTcol2Tcol4*aTcol3Tcol4); //Y*Z

  theCoef(aLowIndex+31)  = 4.0*(aTcol4sq*aTcol1Tcol4 +
                                aSubRadius*aT31*aT34 -
                                aSumRadius*(aT11*aT14 + aT21*aT24)); //X
  theCoef(aLowIndex+32)  = 4.0*(aTcol4sq*aTcol2Tcol4 +
                                aSubRadius*aT32*aT34 -
                                aSumRadius*(aT12*aT14 + aT22*aT24)); //Y
  theCoef(aLowIndex+33)  = 4.0*(aTcol4sq*aTcol3Tcol4 +
                                aSubRadius*aT33*aT34 -
                                aSumRadius*(aT13*aT14 + aT23*aT24)); //Z;

  theCoef(aLowIndex+34)  = 2.0*aSubRadius*aT34*aT34 - 
                           2.0*aSumRadius*(aT14*aT14 + aT24*aT24) +
                           aTcol4sq*aTcol4sq + aSubRadius*aSubRadius;
}

void gp_Torus::Mirror (const gp_Pnt& P)
{ pos.Mirror (P); }

gp_Torus gp_Torus::Mirrored (const gp_Pnt& P) const
{
  gp_Torus C = *this;
  C.pos.Mirror (P);
  return C;
}

void gp_Torus::Mirror (const gp_Ax1& A1)
{ pos.Mirror (A1); }

gp_Torus gp_Torus::Mirrored (const gp_Ax1& A1) const
{
  gp_Torus C = *this;
  C.pos.Mirror (A1);
  return C;
}

void gp_Torus::Mirror (const gp_Ax2& A2)
{ pos.Mirror (A2); }

gp_Torus gp_Torus::Mirrored (const gp_Ax2& A2) const
{
  gp_Torus C = *this;
  C.pos.Mirror (A2);
  return C;
}

