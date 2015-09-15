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
#include <DrawTrSurf_Polygon2D.hxx>
#include <DrawTrSurf_Polygon3D.hxx>
#include <DrawTrSurf_Triangulation.hxx>
#include <GeometryTest.hxx>
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
//=======================================================================
//function : polytr
//purpose  : 
//=======================================================================

static Standard_Integer polytr(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 4)
    return 1;

  Standard_Integer nbNodes = Draw::Atoi(a[2]);
  Standard_Integer nbTri   = Draw::Atoi(a[3]);

  // read the nodes
  Standard_Integer i, j = 4;
  TColgp_Array1OfPnt Nodes(1, nbNodes);

  for (i = 1; i <= nbNodes; i++) {
    if (j + 2 >= n) {
      di << "Not enough nodes";
      return 1;
    }
    Nodes(i).SetCoord(Draw::Atof(a[j]),Draw::Atof(a[j+1]),Draw::Atof(a[j+2]));
    j += 3;
  }

  // read the triangles

  Poly_Array1OfTriangle Triangles(1, nbTri);
  for (i = 1; i <= nbTri; i++) {
    if (j + 2 >= n) {
      di << "Not enough triangles";
      return 1;
    }
    Triangles(i).Set(Draw::Atoi(a[j]),Draw::Atoi(a[j+1]),Draw::Atoi(a[j+2]));
    j += 3;
  }

  Handle(Poly_Triangulation) T = new Poly_Triangulation(Nodes,Triangles);

  DrawTrSurf::Set(a[1],T);

  return 0;//wnt
}


//=======================================================================
//function : polygon3d
//purpose  : 
//=======================================================================

static Standard_Integer polygon3d(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 4)
    return 1;

  Standard_Integer nbNodes = Draw::Atoi(a[2]);

  // read the nodes
  Standard_Integer i, j = 3;
  TColgp_Array1OfPnt Nodes(1, nbNodes);

  for (i = 1; i <= nbNodes; i++) {
    if (j + 2 >= n) {
      di << "Not enough nodes";
      return 1;
    }
    Nodes(i).SetCoord(Draw::Atof(a[j]),Draw::Atof(a[j+1]),Draw::Atof(a[j+2]));
    j += 3;
  }

  Handle(Poly_Polygon3D) P3d = new Poly_Polygon3D(Nodes);

  DrawTrSurf::Set(a[1], P3d);

  return 0;
}

//=======================================================================
//function : polygon2d
//purpose  : 
//=======================================================================

static Standard_Integer polygon2d(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 4)
    return 1;

  Standard_Integer nbNodes = Draw::Atoi(a[2]);

  // read the nodes
  Standard_Integer i, j = 3;
  TColgp_Array1OfPnt2d Nodes(1, nbNodes);

  for (i = 1; i <= nbNodes; i++) {
    if (j + 1 >= n) {
      di << "Not enough nodes";
      return 1;
    }
    Nodes(i).SetCoord(Draw::Atof(a[j]),Draw::Atof(a[j+1]));
    j += 2;
  }

  Handle(Poly_Polygon2D) P2d = new Poly_Polygon2D(Nodes);

  DrawTrSurf::Set(a[1], P2d);

  return 0;
}


//=======================================================================
//function : shnodes
//purpose  : 
//=======================================================================

static Standard_Integer shnodes(Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n != 2) return 1;
  Handle(DrawTrSurf_Triangulation) T 
    = Handle(DrawTrSurf_Triangulation)::DownCast(Draw::Get(a[1]));

  if (!T.IsNull()) {
    Standard_Boolean SHOWNODES = T->ShowNodes();
    T->ShowNodes(!SHOWNODES);
  }

  

  dout.RepaintAll();

  return 0;//wnt
}

//=======================================================================
//function : shtriangles
//purpose  : 
//=======================================================================

static Standard_Integer shtriangles(Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n != 2) return 1;
  
  Handle(DrawTrSurf_Triangulation) T 
    = Handle(DrawTrSurf_Triangulation)::DownCast(Draw::Get(a[1]));
  Standard_Boolean SHOWTRIANGLES = T->ShowTriangles();
  T->ShowTriangles(!SHOWTRIANGLES);
  dout.RepaintAll();
  return 0;//wnt
}

//=======================================================================
//function : PolyCommands
//purpose  : 
//=======================================================================

void GeometryTest::PolyCommands(Draw_Interpretor& theCommands)
{

  const char* g = "Poly Commands";

  theCommands.Add("polytr","polytr name nbnodes nbtri x1 y1 z1 ... n1 n2 n3 ...",__FILE__,polytr,g);
  theCommands.Add("polygon3d","polygon3d name nbnodes x1 y1 z1  ...",__FILE__,polygon3d,g);
  theCommands.Add("polygon2d","polygon2d name nbnodes x1 y1  ...",__FILE__,polygon2d,g);
  theCommands.Add("shnodes","shnodes name", __FILE__,shnodes, g);
  theCommands.Add("shtriangles","shtriangles name", __FILE__,shtriangles, g);
}
