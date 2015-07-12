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


#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_Presentation.hxx>

//=======================================================================
//function : Draw
//purpose  : 
//=======================================================================
void Prs3d_Arrow::Draw(const Handle(Prs3d_Presentation)& aPresentation,
                       const gp_Pnt& aLocation,
                       const gp_Dir& aDirection,
                       const Quantity_PlaneAngle anAngle,
                       const Quantity_Length aLength)
{
  Quantity_Length dx,dy,dz;  aDirection.Coord(dx,dy,dz);
//
// Point of the arrow:
  Quantity_Length xo,yo,zo;  aLocation.Coord(xo,yo,zo);

// Center of the base circle of the arrow:
  Quantity_Length xc = xo - dx * aLength;
  Quantity_Length yc = yo - dy * aLength;
  Quantity_Length zc = zo - dz * aLength;

// Construction of i,j mark for the circle:
  Quantity_Length xn=0., yn=0., zn=0.;

  if ( Abs(dx) <= Abs(dy) && Abs(dx) <= Abs(dz)) xn=1.;
  else if ( Abs(dy) <= Abs(dz) && Abs(dy) <= Abs(dx)) yn=1.;
  else zn=1.;
  Quantity_Length xi = dy * zn - dz * yn;
  Quantity_Length yi = dz * xn - dx * zn;
  Quantity_Length zi = dx * yn - dy * xn;

  Quantity_Length Norme = sqrt ( xi*xi + yi*yi + zi*zi );
  xi = xi / Norme; yi = yi / Norme; zi = zi/Norme;

  const Quantity_Length  xj = dy * zi - dz * yi;
  const Quantity_Length  yj = dz * xi - dx * zi;
  const Quantity_Length  zj = dx * yi - dy * xi;

  const Standard_Integer NbPoints = 15;

  Handle(Graphic3d_ArrayOfSegments) aPrims1 = new Graphic3d_ArrayOfSegments(2*NbPoints);
  Handle(Graphic3d_ArrayOfPolylines) aPrims2 = new Graphic3d_ArrayOfPolylines(NbPoints+1);

  gp_Pnt p1;
  const Standard_Real Tg=tan(anAngle);

  for (Standard_Integer i = 1; i <= NbPoints ; i++)
  {
    const Standard_Real cosinus = cos ( 2 * M_PI / NbPoints * (i-1) );   
    const Standard_Real sinus   = sin ( 2 * M_PI / NbPoints * (i-1) );

    const gp_Pnt pp(xc + (cosinus * xi + sinus * xj) * aLength * Tg,
                    yc + (cosinus * yi + sinus * yj) * aLength * Tg,
                    zc + (cosinus * zi + sinus * zj) * aLength * Tg);

    aPrims1->AddVertex(aLocation);
    aPrims1->AddVertex(pp);
    if(i==1) p1 = pp;
    aPrims2->AddVertex(pp);
  }
  aPrims2->AddVertex(p1);

  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims1);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims2);
}

//=======================================================================
//function : Fill
//purpose  : 
//=======================================================================

void Prs3d_Arrow::Fill(const Handle(Prs3d_Presentation)& /*aPresentation*/,
                       const gp_Pnt& /*aLocation*/,
                       const gp_Dir& /*aDirection*/,
                       const Quantity_PlaneAngle /*anAngle*/,
                       const Quantity_Length /*aLength*/)
{
}
