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

//=================================================================================================

void Prs3d_Arrow::Draw(const occ::handle<Graphic3d_Group>& theGroup,
                       const gp_Pnt&                       theLocation,
                       const gp_Dir&                       theDirection,
                       const double                        theAngle,
                       const double                        theLength)
{
  occ::handle<Graphic3d_ArrayOfSegments> aPrimitives =
    Prs3d_Arrow::DrawSegments(theLocation, theDirection, theAngle, theLength, 15);
  theGroup->AddPrimitiveArray(aPrimitives);
}

//=================================================================================================

occ::handle<Graphic3d_ArrayOfSegments> Prs3d_Arrow::DrawSegments(const gp_Pnt& theLocation,
                                                                 const gp_Dir& theDir,
                                                                 const double  theAngle,
                                                                 const double  theLength,
                                                                 const int     theNbSegments)
{
  occ::handle<Graphic3d_ArrayOfSegments> aSegments =
    new Graphic3d_ArrayOfSegments(theNbSegments + 1, 2 * (2 * theNbSegments));

  // center of the base circle of the arrow
  const gp_XYZ aC = theLocation.XYZ() + theDir.XYZ() * (-theLength);

  // construction of i,j mark for the circle
  gp_Dir aN;
  if (std::abs(theDir.X()) <= std::abs(theDir.Y()) && std::abs(theDir.X()) <= std::abs(theDir.Z()))
  {
    aN = gp::DX();
  }
  else if (std::abs(theDir.Y()) <= std::abs(theDir.Z())
           && std::abs(theDir.Y()) <= std::abs(theDir.X()))
  {
    aN = gp::DY();
  }
  else
  {
    aN = gp::DZ();
  }

  const gp_Dir anXYZi = theDir.Crossed(aN.XYZ());
  const gp_XYZ anXYZj = theDir.XYZ().Crossed(anXYZi.XYZ());
  aSegments->AddVertex(theLocation);

  const double Tg = std::tan(theAngle);
  for (int aVertIter = 1; aVertIter <= theNbSegments; ++aVertIter)
  {
    const double aCos = std::cos(2.0 * M_PI / theNbSegments * (aVertIter - 1));
    const double aSin = std::sin(2.0 * M_PI / theNbSegments * (aVertIter - 1));

    const gp_Pnt pp(aC.X() + (aCos * anXYZi.X() + aSin * anXYZj.X()) * theLength * Tg,
                    aC.Y() + (aCos * anXYZi.Y() + aSin * anXYZj.Y()) * theLength * Tg,
                    aC.Z() + (aCos * anXYZi.Z() + aSin * anXYZj.Z()) * theLength * Tg);

    aSegments->AddVertex(pp);
  }

  int aNbVertices         = theNbSegments + 1;
  int aFirstContourVertex = 2;
  for (int aVertIter = aFirstContourVertex; aVertIter <= aNbVertices; ++aVertIter)
  {
    aSegments->AddEdge(1);
    aSegments->AddEdge(aVertIter);
  }
  aSegments->AddEdge(aNbVertices);
  aSegments->AddEdge(aFirstContourVertex);

  for (int aVertIter = aFirstContourVertex; aVertIter <= aNbVertices - 1; ++aVertIter)
  {
    aSegments->AddEdge(aVertIter);
    aSegments->AddEdge(aVertIter + 1);
  }
  return aSegments;
}

//=================================================================================================

occ::handle<Graphic3d_ArrayOfTriangles> Prs3d_Arrow::DrawShaded(const gp_Ax1& theAxis,
                                                                const double  theTubeRadius,
                                                                const double  theAxisLength,
                                                                const double  theConeRadius,
                                                                const double  theConeLength,
                                                                const int     theNbFacettes)
{
  const double aTubeLength = std::max(0.0, theAxisLength - theConeLength);
  const int    aNbTrisTube = (theTubeRadius > 0.0 && aTubeLength > 0.0)
                               ? Prs3d_ToolCylinder::TrianglesNb(theNbFacettes, 1)
                               : 0;
  const int    aNbTrisCone = (theConeRadius > 0.0 && theConeLength > 0.0)
                               ? (Prs3d_ToolDisk ::TrianglesNb(theNbFacettes, 1)
                               + Prs3d_ToolCylinder::TrianglesNb(theNbFacettes, 1))
                               : 0;

  const int aNbTris = aNbTrisTube + aNbTrisCone;
  if (aNbTris == 0)
  {
    return occ::handle<Graphic3d_ArrayOfTriangles>();
  }

  int aMaxVertexs = 0;
  if (aNbTrisTube > 0)
  {
    aMaxVertexs += Prs3d_ToolCylinder::VerticesNb(theNbFacettes, 1);
  }
  if (aNbTrisCone > 0)
  {
    // longer syntax to workaround msvc10 32-bit optimizer bug (#0031876)
    aMaxVertexs += Prs3d_ToolDisk::VerticesNb(theNbFacettes, 1);
    aMaxVertexs += Prs3d_ToolCylinder::VerticesNb(theNbFacettes, 1);
  }

  occ::handle<Graphic3d_ArrayOfTriangles> anArray =
    new Graphic3d_ArrayOfTriangles(aMaxVertexs, aNbTris * 3, Graphic3d_ArrayFlags_VertexNormal);
  if (aNbTrisTube != 0)
  {
    gp_Ax3  aSystem(theAxis.Location(), theAxis.Direction());
    gp_Trsf aTrsf;
    aTrsf.SetTransformation(aSystem, gp_Ax3());

    Prs3d_ToolCylinder aTool(theTubeRadius, theTubeRadius, aTubeLength, theNbFacettes, 1);
    aTool.FillArray(anArray, aTrsf);
  }

  if (aNbTrisCone != 0)
  {
    gp_Pnt aConeOrigin =
      theAxis.Location().Translated(gp_Vec(theAxis.Direction().X() * aTubeLength,
                                           theAxis.Direction().Y() * aTubeLength,
                                           theAxis.Direction().Z() * aTubeLength));
    gp_Ax3  aSystem(aConeOrigin, theAxis.Direction());
    gp_Trsf aTrsf;
    aTrsf.SetTransformation(aSystem, gp_Ax3());
    {
      Prs3d_ToolDisk aTool(0.0, theConeRadius, theNbFacettes, 1);
      aTool.FillArray(anArray, aTrsf);
    }
    {
      Prs3d_ToolCylinder aTool(theConeRadius, 0.0, theConeLength, theNbFacettes, 1);
      aTool.FillArray(anArray, aTrsf);
    }
  }

  return anArray;
}
