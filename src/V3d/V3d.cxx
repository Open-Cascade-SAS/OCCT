// Created by: GG
// Copyright (c) 1991-1999 Matra Datavision
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

#include <V3d.hxx>

#include <Aspect_Grid.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_Group.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

namespace
{
  static Standard_CString V3d_Table_PrintTypeOfOrientation[26] =
  {
    "XPOS", "YPOS", "ZPOS", "XNEG", "YNEG", "ZNEG", "XPOSYPOS", "XPOSZPOS", "XPOSZPOS", "XNEGYNEG",
    "XNEGYPOS", "XNEGZNEG", "XNEGZPOS", "YNEGZNEG", "YNEGZPOS", "XPOSYNEG", "XPOSZNEG", "YPOSZNEG",
    "XPOSYPOSZPOS", "XPOSYNEGZPOS", "XPOSYPOSZNEG", "XNEGYPOSZPOS", "XPOSYNEGZNEG", "XNEGYPOSZNEG",
    "XNEGYNEGZPOS", "XNEGYNEGZNEG"
  };
}

void V3d::ArrowOfRadius(const Handle(Graphic3d_Group)& garrow,const Standard_Real X0,const Standard_Real Y0,const Standard_Real Z0,const Standard_Real Dx,const Standard_Real Dy,const Standard_Real Dz,const Standard_Real Alpha,const Standard_Real Lng)
{
  Standard_Real Xc, Yc, Zc, Xi, Yi, Zi, Xj, Yj, Zj;
  Standard_Real Xn, Yn, Zn, X, Y, Z, X1 = 0., Y1 = 0., Z1 = 0., Norme;
  const Standard_Integer NbPoints = 10;

//      Centre du cercle base de la fleche :
  Xc = X0 - Dx * Lng;
  Yc = Y0 - Dy * Lng;
  Zc = Z0 - Dz * Lng;

//      Construction d'un repere i,j pour le cercle:
  Xn=0., Yn=0., Zn=0.;

  if ( Abs(Dx) <= Abs(Dy) && Abs(Dx) <= Abs(Dz)) Xn=1.;
  else if ( Abs(Dy) <= Abs(Dz) && Abs(Dy) <= Abs(Dx)) Yn=1.;
  else Zn=1.;
  Xi = Dy * Zn - Dz * Yn;
  Yi = Dz * Xn - Dx * Zn;
  Zi = Dx * Yn - Dy * Xn;

  Norme = Sqrt ( Xi*Xi + Yi*Yi + Zi*Zi );
  Xi= Xi / Norme; Yi = Yi / Norme; Zi = Zi/Norme;

  Xj = Dy * Zi - Dz * Yi;
  Yj = Dz * Xi - Dx * Zi;
  Zj = Dx * Yi - Dy * Xi;

  Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(3*NbPoints,NbPoints);

  Standard_Integer i;
  const Standard_Real Tg = Tan(Alpha);
  for (i = 1; i <= NbPoints; i++)
  {
    const Standard_Real cosinus = Cos ( 2. * M_PI / NbPoints * (i-1) );
    const Standard_Real sinus = Sin ( 2. * M_PI / NbPoints * (i-1) );

    X = Xc + (cosinus * Xi + sinus * Xj) * Lng * Tg;
    Y = Yc + (cosinus * Yi + sinus * Yj) * Lng * Tg;
    Z = Zc + (cosinus * Zi + sinus * Zj) * Lng * Tg;

    if(i==1) { X1=X, Y1=Y, Z1=Z; }
    else aPrims->AddVertex(X,Y,Z);
    aPrims->AddBound(3);
    aPrims->AddVertex(X0,Y0,Z0);
    aPrims->AddVertex(X,Y,Z);
  }
  aPrims->AddVertex(X1,Y1,Z1);

  garrow->AddPrimitiveArray(aPrims);
}


void V3d::CircleInPlane(const Handle(Graphic3d_Group)& gcircle,const Standard_Real X0,const Standard_Real Y0,const Standard_Real Z0,const Standard_Real DX,const Standard_Real DY,const Standard_Real DZ,const Standard_Real Rayon)
{
  Standard_Real Norme = Sqrt ( DX*DX + DY*DY + DZ*DZ );
  if ( Norme >= 0.0001 )
  {
    Standard_Real VX,VY,VZ,X,Y,Z,Xn,Yn,Zn,Xi,Yi,Zi,Xj,Yj,Zj;

    VX= DX/Norme; VY = DY/Norme; VZ = DZ/Norme;

//Construction of marker i,j for the circle:
    Xn=0., Yn=0., Zn=0.;   
    if ( Abs(VX) <= Abs(VY) && Abs(VX) <= Abs(VZ)) Xn=1.;
    else if ( Abs(VY) <= Abs(VZ) && Abs(VY) <= Abs(VX)) Yn=1.;
    else Zn=1.;
    Xi = VY * Zn - VZ * Yn;
    Yi = VZ * Xn - VX * Zn;
    Zi = VX * Yn - VY * Xn;

    Norme = Sqrt ( Xi*Xi + Yi*Yi + Zi*Zi );
    Xi= Xi / Norme; Yi = Yi / Norme; Zi = Zi/Norme;

    Xj = VY * Zi - VZ * Yi;
    Yj = VZ * Xi - VX * Zi;
    Zj = VX * Yi - VY * Xi;      

    const Standard_Integer NFACES = 30;
    Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(NFACES+1);

    Standard_Integer i = 0;
    Standard_Real Alpha = 0.;
    const Standard_Real Dalpha = 2. * M_PI / NFACES;
    for (; i <= NFACES; i++, Alpha += Dalpha)
    {
      const Standard_Real cosinus = Cos(Alpha);
      const Standard_Real sinus = Sin(Alpha);

      X = X0 + (cosinus * Xi + sinus * Xj) * Rayon;
      Y = Y0 + (cosinus * Yi + sinus * Yj) * Rayon;
      Z = Z0 + (cosinus * Zi + sinus * Zj) * Rayon;

      aPrims->AddVertex(X,Y,Z);
    }
    gcircle->AddPrimitiveArray(aPrims);
  }
}


void V3d::SwitchViewsinWindow(const Handle(V3d_View)& aPreviousView,
                              const Handle(V3d_View)& aNextView) {
  aPreviousView->Viewer()->SetViewOff(aPreviousView);
  if(!aNextView->IfWindow())
    aNextView->SetWindow(aPreviousView->Window());
  aNextView->Viewer()->SetViewOn(aNextView);
    
}

//=======================================================================
//function : TypeOfOrientationToString
//purpose  :
//=======================================================================
Standard_CString V3d::TypeOfOrientationToString (V3d_TypeOfOrientation theType)
{
  return V3d_Table_PrintTypeOfOrientation[theType];
}

//=======================================================================
//function : TypeOfOrientationFromString
//purpose  :
//=======================================================================
Standard_Boolean V3d::TypeOfOrientationFromString (Standard_CString theTypeString,
                                                   V3d_TypeOfOrientation& theType)
{
  TCollection_AsciiString aName (theTypeString);
  aName.UpperCase();
  for (Standard_Integer aTypeIter = 0; aTypeIter <= V3d_XnegYnegZneg; ++aTypeIter)
  {
    Standard_CString aTypeName = V3d_Table_PrintTypeOfOrientation[aTypeIter];
    if (aName == aTypeName)
    {
      theType = V3d_TypeOfOrientation (aTypeIter);
      return Standard_True;
    }
  }
  return Standard_False;
}
