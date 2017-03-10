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
                       const gp_Pnt& theLocation,
                       const gp_Dir& theDirection,
                       const Quantity_PlaneAngle theAngle,
                       const Quantity_Length theLength)
{
  Handle(Graphic3d_ArrayOfSegments) aPrimitives = Prs3d_Arrow::DrawSegments(theLocation,
                                                  theDirection, theAngle, theLength, 15);
  theGroup->AddPrimitiveArray (aPrimitives);
}

//=======================================================================
//function : DrawSegments
//purpose  : 
//=======================================================================
Handle(Graphic3d_ArrayOfSegments) Prs3d_Arrow::DrawSegments (const gp_Pnt& theLocation,
                                                             const gp_Dir& theDirection,
                                                             const Quantity_PlaneAngle theAngle,
                                                             const Quantity_Length theLength,
                                                             const int theNumberOfSegments)
{
  Handle(Graphic3d_ArrayOfSegments) aSegments = new Graphic3d_ArrayOfSegments(theNumberOfSegments+1,
                                                                              2*(2*theNumberOfSegments));

  Quantity_Length dx,dy,dz;  theDirection.Coord(dx,dy,dz);
//
// Point of the arrow:
  Quantity_Length xo,yo,zo;  theLocation.Coord(xo,yo,zo);

// Center of the base circle of the arrow:
  Quantity_Length xc = xo - dx * theLength;
  Quantity_Length yc = yo - dy * theLength;
  Quantity_Length zc = zo - dz * theLength;

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

  const Standard_Integer NbPoints = theNumberOfSegments;

  aSegments->AddVertex(theLocation);

  gp_Pnt p1;
  const Standard_Real Tg=tan(theAngle);

  for (Standard_Integer i = 1; i <= NbPoints ; i++)
  {
    const Standard_Real cosinus = cos ( 2 * M_PI / NbPoints * (i-1) );
    const Standard_Real sinus   = sin ( 2 * M_PI / NbPoints * (i-1) );

    const gp_Pnt pp(xc + (cosinus * xi + sinus * xj) * theLength * Tg,
                    yc + (cosinus * yi + sinus * yj) * theLength * Tg,
                    zc + (cosinus * zi + sinus * zj) * theLength * Tg);

    aSegments->AddVertex(pp);
  }

  int aNbVertices = NbPoints + 1;
  int aFirstContourVertex = 2;
  int anEdgeCount = 0;
  for (Standard_Integer i = aFirstContourVertex; i <= aNbVertices ; i++) {
    aSegments->AddEdge(1); // location vertex
    aSegments->AddEdge(i);
    anEdgeCount++;
  }
  aSegments->AddEdge(aNbVertices);
  aSegments->AddEdge(aFirstContourVertex);
  anEdgeCount++;

  for (Standard_Integer i = aFirstContourVertex; i <= aNbVertices-1 ; i++) {
    aSegments->AddEdge(i);
    aSegments->AddEdge(i+1);
    anEdgeCount++;
  }
  return aSegments;
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
