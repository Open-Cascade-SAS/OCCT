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

#include <Prs3d_Arrow.hxx>

#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_ToolCylinder.hxx>
#include <Prs3d_ToolDisk.hxx>
#include <Prs3d_ToolSphere.hxx>

//=======================================================================
//function : Draw
//purpose  : 
//=======================================================================
void Prs3d_Arrow::Draw(const Handle(Graphic3d_Group)& theGroup,
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

  theGroup->AddPrimitiveArray (aPrims1);
  theGroup->AddPrimitiveArray (aPrims2);
}

// ============================================================================
// function : DrawShaded
// purpose  :
// ============================================================================
Handle(Graphic3d_ArrayOfTriangles) Prs3d_Arrow::DrawShaded (const gp_Ax1&          theAxis,
                                                            const Standard_Real    theTubeRadius,
                                                            const Standard_Real    theAxisLength,
                                                            const Standard_Real    theConeRadius,
                                                            const Standard_Real    theConeLength,
                                                            const Standard_Integer theNbFacettes)
{
  const Standard_Real aTubeLength = Max (0.0, theAxisLength - theConeLength);
  const Standard_Integer aNbTrisTube = (theTubeRadius > 0.0 && aTubeLength > 0.0)
                                     ? Prs3d_ToolCylinder::TrianglesNb (theNbFacettes, 1)
                                     : 0;
  const Standard_Integer aNbTrisCone = (theConeRadius > 0.0 && theConeLength > 0.0)
                                     ? (Prs3d_ToolDisk    ::TrianglesNb (theNbFacettes, 1)
                                      + Prs3d_ToolCylinder::TrianglesNb (theNbFacettes, 1))
                                     : 0;

  const Standard_Integer aNbTris = aNbTrisTube + aNbTrisCone;
  if (aNbTris == 0)
  {
    return Handle(Graphic3d_ArrayOfTriangles)();
  }

  Handle(Graphic3d_ArrayOfTriangles) anArray = new Graphic3d_ArrayOfTriangles (aNbTris * 3, 0, Standard_True);
  if (aNbTrisTube != 0)
  {
    gp_Ax3  aSystem (theAxis.Location(), theAxis.Direction());
    gp_Trsf aTrsf;
    aTrsf.SetTransformation (aSystem, gp_Ax3());

    Prs3d_ToolCylinder aTool (theTubeRadius, theTubeRadius, aTubeLength, theNbFacettes, 1);
    aTool.FillArray (anArray, aTrsf);
  }

  if (aNbTrisCone != 0)
  {
    gp_Pnt aConeOrigin = theAxis.Location().Translated (gp_Vec (theAxis.Direction().X() * aTubeLength,
                                                                theAxis.Direction().Y() * aTubeLength,
                                                                theAxis.Direction().Z() * aTubeLength));
    gp_Ax3  aSystem (aConeOrigin, theAxis.Direction());
    gp_Trsf aTrsf;
    aTrsf.SetTransformation (aSystem, gp_Ax3());
    {
      Prs3d_ToolDisk aTool (0.0, theConeRadius, theNbFacettes, 1);
      aTool.FillArray (anArray, aTrsf);
    }
    {
      Prs3d_ToolCylinder aTool (theConeRadius, 0.0, theConeLength, theNbFacettes, 1);
      aTool.FillArray (anArray, aTrsf);
    }
  }

  return anArray;
}
