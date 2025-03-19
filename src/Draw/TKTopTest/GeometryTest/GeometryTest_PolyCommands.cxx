// Created on: 1995-03-06
// Created by: Laurent PAINNOT
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

#include <Draw_Appli.hxx>
#include <DrawTrSurf.hxx>
#include <DrawTrSurf_Triangulation.hxx>
#include <GeometryTest.hxx>
#include <Poly.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif
//=================================================================================================

static Standard_Integer polytr(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 4)
    return 1;

  Standard_Integer nbNodes = Draw::Atoi(a[2]);
  Standard_Integer nbTri   = Draw::Atoi(a[3]);

  // read the nodes
  Standard_Integer   i, j = 4;
  TColgp_Array1OfPnt Nodes(1, nbNodes);

  for (i = 1; i <= nbNodes; i++)
  {
    if (j + 2 >= n)
    {
      di << "Not enough nodes";
      return 1;
    }
    Nodes(i).SetCoord(Draw::Atof(a[j]), Draw::Atof(a[j + 1]), Draw::Atof(a[j + 2]));
    j += 3;
  }

  // read the triangles

  Poly_Array1OfTriangle Triangles(1, nbTri);
  for (i = 1; i <= nbTri; i++)
  {
    if (j + 2 >= n)
    {
      di << "Not enough triangles";
      return 1;
    }
    Triangles(i).Set(Draw::Atoi(a[j]), Draw::Atoi(a[j + 1]), Draw::Atoi(a[j + 2]));
    j += 3;
  }

  Handle(Poly_Triangulation) T = new Poly_Triangulation(Nodes, Triangles);

  DrawTrSurf::Set(a[1], T);

  return 0; // wnt
}

//=================================================================================================

static Standard_Integer polygon3d(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 4)
    return 1;

  Standard_Integer nbNodes = Draw::Atoi(a[2]);

  // read the nodes
  Standard_Integer   i, j = 3;
  TColgp_Array1OfPnt Nodes(1, nbNodes);

  for (i = 1; i <= nbNodes; i++)
  {
    if (j + 2 >= n)
    {
      di << "Not enough nodes";
      return 1;
    }
    Nodes(i).SetCoord(Draw::Atof(a[j]), Draw::Atof(a[j + 1]), Draw::Atof(a[j + 2]));
    j += 3;
  }

  Handle(Poly_Polygon3D) P3d = new Poly_Polygon3D(Nodes);

  DrawTrSurf::Set(a[1], P3d);

  return 0;
}

//=================================================================================================

static Standard_Integer polygon2d(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 4)
    return 1;

  Standard_Integer nbNodes = Draw::Atoi(a[2]);

  // read the nodes
  Standard_Integer     i, j = 3;
  TColgp_Array1OfPnt2d Nodes(1, nbNodes);

  for (i = 1; i <= nbNodes; i++)
  {
    if (j + 1 >= n)
    {
      di << "Not enough nodes";
      return 1;
    }
    Nodes(i).SetCoord(Draw::Atof(a[j]), Draw::Atof(a[j + 1]));
    j += 2;
  }

  Handle(Poly_Polygon2D) P2d = new Poly_Polygon2D(Nodes);

  DrawTrSurf::Set(a[1], P2d);

  return 0;
}

//=================================================================================================

static Standard_Integer shnodes(Draw_Interpretor&, Standard_Integer n, const char** a)
{
  if (n != 2)
    return 1;
  Handle(DrawTrSurf_Triangulation) T = Handle(DrawTrSurf_Triangulation)::DownCast(Draw::Get(a[1]));

  if (!T.IsNull())
  {
    Standard_Boolean SHOWNODES = T->ShowNodes();
    T->ShowNodes(!SHOWNODES);
  }

  dout.RepaintAll();

  return 0; // wnt
}

//=================================================================================================

static Standard_Integer shtriangles(Draw_Interpretor&, Standard_Integer n, const char** a)
{
  if (n != 2)
    return 1;

  Handle(DrawTrSurf_Triangulation) T = Handle(DrawTrSurf_Triangulation)::DownCast(Draw::Get(a[1]));
  Standard_Boolean                 SHOWTRIANGLES = T->ShowTriangles();
  T->ShowTriangles(!SHOWTRIANGLES);
  dout.RepaintAll();
  return 0; // wnt
}

//=================================================================================================

template <typename Poly, typename Point, typename PointArr>
static inline void AddNode(const Handle(Poly)& thePolygon, const Point& thePnt, PointArr& theNodes)
{
  for (Standard_Integer i = thePolygon->Nodes().Lower(); i <= thePolygon->Nodes().Upper(); i++)
  {
    theNodes[i] = thePolygon->Nodes()[i];
  }

  theNodes.ChangeLast() = thePnt;
}

//=================================================================================================

static Standard_Integer AddNode(Draw_Interpretor& theDI,
                                Standard_Integer  theNArg,
                                const char**      theArgVal)
{
  if (theNArg < 4)
  {
    theDI << "Not enough arguments\n";
    return 1;
  }

  if (theNArg == 4)
  {
    Handle(Poly_Polygon2D) aPoly2d = DrawTrSurf::GetPolygon2D(theArgVal[1]);
    TColgp_Array1OfPnt2d   aNodes(aPoly2d->Nodes().Lower(), aPoly2d->Nodes().Upper() + 1);
    AddNode(aPoly2d, gp_Pnt2d(Draw::Atof(theArgVal[2]), Draw::Atof(theArgVal[3])), aNodes);
    aPoly2d.Nullify();
    aPoly2d = new Poly_Polygon2D(aNodes);
    DrawTrSurf::Set(theArgVal[1], aPoly2d);
  }
  else
  {
    Handle(Poly_Polygon3D) aPoly3d = DrawTrSurf::GetPolygon3D(theArgVal[1]);
    TColgp_Array1OfPnt     aNodes(aPoly3d->Nodes().Lower(), aPoly3d->Nodes().Upper() + 1);
    AddNode(aPoly3d,
            gp_Pnt(Draw::Atof(theArgVal[2]), Draw::Atof(theArgVal[3]), Draw::Atof(theArgVal[4])),
            aNodes);
    aPoly3d.Nullify();
    aPoly3d = new Poly_Polygon3D(aNodes);
    DrawTrSurf::Set(theArgVal[1], aPoly3d);
  }

  return 0;
}

//=================================================================================================

static Standard_Integer PolygonProps(Draw_Interpretor& theDI,
                                     Standard_Integer  theNArg,
                                     const char**      theArgVal)
{
  if (theNArg < 2)
  {
    theDI << "Use: polygonprops polygon2d [-area val] [-perimeter val]\n";
    return 1;
  }

  Handle(Poly_Polygon2D) aPoly2d = DrawTrSurf::GetPolygon2D(theArgVal[1]);

  Standard_Real anArea = 0.0, aPerimeter = 0.0;
  Poly::PolygonProperties(aPoly2d->Nodes(), anArea, aPerimeter);

  theDI << "Area      = " << anArea << "\n";
  theDI << "Perimeter = " << aPerimeter << "\n";

  for (Standard_Integer i = 2; i < theNArg; i++)
  {
    if (!strcmp(theArgVal[i], "-area"))
    {
      Draw::Set(theArgVal[++i], anArea);
      continue;
    }

    if (!strcmp(theArgVal[i], "-perimeter"))
    {
      Draw::Set(theArgVal[++i], aPerimeter);
      continue;
    }

    theDI << "Error: Wrong option: \"" << theArgVal[i] << "\"\n";
    break;
  }

  return 0;
}

//=================================================================================================

void GeometryTest::PolyCommands(Draw_Interpretor& theCommands)
{

  const char* g = "Poly Commands";

  theCommands.Add("polytr",
                  "polytr name nbnodes nbtri x1 y1 z1 ... n1 n2 n3 ...",
                  __FILE__,
                  polytr,
                  g);
  theCommands.Add("polygon3d", "polygon3d name nbnodes x1 y1 z1  ...", __FILE__, polygon3d, g);
  theCommands.Add("polygon2d", "polygon2d name nbnodes x1 y1  ...", __FILE__, polygon2d, g);
  theCommands.Add("addpolygonnode", "addpolygonnode polygon3d(2d) x y [z]", __FILE__, AddNode, g);
  theCommands.Add("polygonprops",
                  "Computes area and perimeter of 2D-polygon. "
                  "Run \"polygonprops\" w/o any arguments to read help.\n",
                  __FILE__,
                  PolygonProps,
                  g);
  theCommands.Add("shnodes", "shnodes name", __FILE__, shnodes, g);
  theCommands.Add("shtriangles", "shtriangles name", __FILE__, shtriangles, g);
}
