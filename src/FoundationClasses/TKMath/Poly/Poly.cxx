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

#include <Poly.hxx>

#include <gp_Ax1.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <Poly_Triangle.hxx>
#include <NCollection_Array1.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Precision.hxx>
#include <Standard_Macro.hxx>
#include <iostream>
#include <iomanip>
#include <fstream>

//=======================================================================
// function : Catenate
// purpose  : Join several triangulations to one new triangulation object
//=======================================================================
occ::handle<Poly_Triangulation> Poly::Catenate(
  const NCollection_List<occ::handle<Poly_Triangulation>>& lstTri)
{
  int nNodes(0);
  int nTrian(0);

  // Sum up the total number of nodes.
  NCollection_List<occ::handle<Poly_Triangulation>>::Iterator anIter(lstTri);
  for (; anIter.More(); anIter.Next())
  {
    const occ::handle<Poly_Triangulation>& aTri = anIter.Value();
    if (!aTri.IsNull())
    {
      nNodes += aTri->NbNodes();
      nTrian += aTri->NbTriangles();
    }
  }

  if (nNodes == 0)
  {
    return occ::handle<Poly_Triangulation>();
  }

  occ::handle<Poly_Triangulation> aResult  = new Poly_Triangulation(nNodes, nTrian, false);
  int                             iNode[3] = {};
  nNodes                                   = 0;
  nTrian                                   = 0;
  for (anIter.Init(lstTri); anIter.More(); anIter.Next())
  {
    const occ::handle<Poly_Triangulation>& aTri = anIter.Value();
    if (aTri.IsNull())
    {
      continue;
    }

    const int nbNodes = aTri->NbNodes();
    const int nbTrian = aTri->NbTriangles();
    for (int i = 1; i <= nbNodes; i++)
    {
      aResult->SetNode(i + nNodes, aTri->Node(i));
    }
    for (int i = 1; i <= nbTrian; i++)
    {
      aTri->Triangle(i).Get(iNode[0], iNode[1], iNode[2]);
      aResult->SetTriangle(i + nTrian,
                           Poly_Triangle(iNode[0] + nNodes, iNode[1] + nNodes, iNode[2] + nNodes));
    }
    nNodes += nbNodes;
    nTrian += nbTrian;
  }
  return aResult;
}

//=================================================================================================

void Poly::Write(const occ::handle<Poly_Triangulation>& T, Standard_OStream& OS, const bool Compact)
{
  OS << "Poly_Triangulation\n";
  if (Compact)
  {
    OS << T->NbNodes() << " " << T->NbTriangles() << " ";
    OS << ((T->HasUVNodes()) ? "1" : "0") << "\n";
  }
  else
  {
    OS << std::setw(8) << T->NbNodes() << " Nodes\n";
    OS << std::setw(8) << T->NbTriangles() << " Triangles\n";
    OS << ((T->HasUVNodes()) ? "with" : "without") << " UV nodes\n";
  }

  // write the deflection

  if (!Compact)
    OS << "Deflection : ";
  OS << T->Deflection() << "\n";

  // write the 3d nodes

  if (!Compact)
    OS << "\n3D Nodes :\n";

  int i, nbNodes = T->NbNodes();
  for (i = 1; i <= nbNodes; i++)
  {
    const gp_Pnt aNode = T->Node(i);
    if (!Compact)
      OS << std::setw(10) << i << " : ";
    if (!Compact)
      OS << std::setw(17);
    OS << aNode.X() << " ";
    if (!Compact)
      OS << std::setw(17);
    OS << aNode.Y() << " ";
    if (!Compact)
      OS << std::setw(17);
    OS << aNode.Z() << "\n";
  }

  if (T->HasUVNodes())
  {
    if (!Compact)
      OS << "\nUV Nodes :\n";
    for (i = 1; i <= nbNodes; i++)
    {
      const gp_Pnt2d aNode2d = T->UVNode(i);
      if (!Compact)
        OS << std::setw(10) << i << " : ";
      if (!Compact)
        OS << std::setw(17);
      OS << aNode2d.X() << " ";
      if (!Compact)
        OS << std::setw(17);
      OS << aNode2d.Y() << "\n";
    }
  }

  if (!Compact)
    OS << "\nTriangles :\n";
  int nbTriangles = T->NbTriangles();
  int n1, n2, n3;
  for (i = 1; i <= nbTriangles; i++)
  {
    if (!Compact)
      OS << std::setw(10) << i << " : ";
    T->Triangle(i).Get(n1, n2, n3);
    if (!Compact)
      OS << std::setw(10);
    OS << n1 << " ";
    if (!Compact)
      OS << std::setw(10);
    OS << n2 << " ";
    if (!Compact)
      OS << std::setw(10);
    OS << n3 << "\n";
  }
}

//=================================================================================================

void Poly::Write(const occ::handle<Poly_Polygon3D>& P, Standard_OStream& OS, const bool Compact)
{
  OS << "Poly_Polygon3D\n";
  if (Compact)
  {
    OS << P->NbNodes() << " ";
    OS << ((P->HasParameters()) ? "1" : "0") << "\n";
  }
  else
  {
    OS << std::setw(8) << P->NbNodes() << " Nodes\n";
    OS << ((P->HasParameters()) ? "with" : "without") << " parameters\n";
  }

  // write the deflection

  if (!Compact)
    OS << "Deflection : ";
  OS << P->Deflection() << "\n";

  // write the nodes

  if (!Compact)
    OS << "\nNodes :\n";

  int                               i, nbNodes = P->NbNodes();
  const NCollection_Array1<gp_Pnt>& Nodes = P->Nodes();
  for (i = 1; i <= nbNodes; i++)
  {
    if (!Compact)
      OS << std::setw(10) << i << " : ";
    if (!Compact)
      OS << std::setw(17);
    OS << Nodes(i).X() << " ";
    if (!Compact)
      OS << std::setw(17);
    OS << Nodes(i).Y() << " ";
    if (!Compact)
      OS << std::setw(17);
    OS << Nodes(i).Z() << "\n";
  }

  if (P->HasParameters())
  {
    if (!Compact)
      OS << "\nParameters :\n";
    const NCollection_Array1<double>& Param = P->Parameters();
    for (i = 1; i <= nbNodes; i++)
    {
      OS << Param(i) << " ";
    }
    OS << "\n";
  }
}

//=================================================================================================

void Poly::Write(const occ::handle<Poly_Polygon2D>& P, Standard_OStream& OS, const bool Compact)
{
  OS << "Poly_Polygon2D\n";
  if (Compact)
  {
    OS << P->NbNodes() << " ";
  }
  else
  {
    OS << std::setw(8) << P->NbNodes() << " Nodes\n";
  }

  // write the deflection

  if (!Compact)
    OS << "Deflection : ";
  OS << P->Deflection() << "\n";

  // write the nodes

  if (!Compact)
    OS << "\nNodes :\n";

  int                                 i, nbNodes = P->NbNodes();
  const NCollection_Array1<gp_Pnt2d>& Nodes = P->Nodes();
  for (i = 1; i <= nbNodes; i++)
  {
    if (!Compact)
      OS << std::setw(10) << i << " : ";
    if (!Compact)
      OS << std::setw(17);
    OS << Nodes(i).X() << " ";
    if (!Compact)
      OS << std::setw(17);
    OS << Nodes(i).Y() << "\n";
  }
}

//=================================================================================================

void Poly::Dump(const occ::handle<Poly_Triangulation>& T, Standard_OStream& OS)
{
  Poly::Write(T, OS, false);
}

//=================================================================================================

void Poly::Dump(const occ::handle<Poly_Polygon3D>& P, Standard_OStream& OS)
{
  Poly::Write(P, OS, false);
}

//=================================================================================================

void Poly::Dump(const occ::handle<Poly_Polygon2D>& P, Standard_OStream& OS)
{
  Poly::Write(P, OS, false);
}

//=================================================================================================

occ::handle<Poly_Triangulation> Poly::ReadTriangulation(Standard_IStream& IS)
{
  // Read a triangulation

  char line[100];
  IS >> line;
  if (strcmp(line, "Poly_Triangulation"))
  {
#ifdef OCCT_DEBUG
    std::cout << "Not a Triangulation in the file" << std::endl;
#endif
    return occ::handle<Poly_Triangulation>();
  }

  int  nbNodes, nbTriangles;
  bool hasUV;
  IS >> nbNodes >> nbTriangles >> hasUV;

  double d;
  IS >> d;

  // read the 3d nodes
  double                       x, y, z;
  int                          i;
  NCollection_Array1<gp_Pnt>   Nodes(1, nbNodes);
  NCollection_Array1<gp_Pnt2d> UVNodes(1, nbNodes);

  for (i = 1; i <= nbNodes; i++)
  {
    IS >> x >> y >> z;
    Nodes(i).SetCoord(x, y, z);
  }

  // read the UV points if necessary

  if (hasUV)
  {
    for (i = 1; i <= nbNodes; i++)
    {
      IS >> x >> y;
      UVNodes(i).SetCoord(x, y);
    }
  }

  // read the triangles
  int                               n1, n2, n3;
  NCollection_Array1<Poly_Triangle> Triangles(1, nbTriangles);
  for (i = 1; i <= nbTriangles; i++)
  {
    IS >> n1 >> n2 >> n3;
    Triangles(i).Set(n1, n2, n3);
  }

  occ::handle<Poly_Triangulation> T;

  if (hasUV)
    T = new Poly_Triangulation(Nodes, UVNodes, Triangles);
  else
    T = new Poly_Triangulation(Nodes, Triangles);

  T->Deflection(d);

  return T;
}

//=================================================================================================

occ::handle<Poly_Polygon3D> Poly::ReadPolygon3D(Standard_IStream& IS)
{
  // Read a 3d polygon

  char line[100];
  IS >> line;
  if (strcmp(line, "Poly_Polygon3D"))
  {
#ifdef OCCT_DEBUG
    std::cout << "Not a Polygon3D in the file" << std::endl;
#endif
    return occ::handle<Poly_Polygon3D>();
  }

  int nbNodes;
  IS >> nbNodes;

  bool hasparameters;
  IS >> hasparameters;

  double d;
  IS >> d;

  // read the nodes
  double                     x, y, z;
  int                        i;
  NCollection_Array1<gp_Pnt> Nodes(1, nbNodes);

  for (i = 1; i <= nbNodes; i++)
  {
    IS >> x >> y >> z;
    Nodes(i).SetCoord(x, y, z);
  }

  NCollection_Array1<double> Param(1, nbNodes);
  if (hasparameters)
  {
    for (i = 1; i <= nbNodes; i++)
    {
      IS >> Param(i);
    }
  }

  occ::handle<Poly_Polygon3D> P;
  if (!hasparameters)
    P = new Poly_Polygon3D(Nodes);
  else
    P = new Poly_Polygon3D(Nodes, Param);

  P->Deflection(d);

  return P;
}

//=================================================================================================

occ::handle<Poly_Polygon2D> Poly::ReadPolygon2D(Standard_IStream& IS)
{
  // Read a 2d polygon

  char line[100];
  IS >> line;
  if (strcmp(line, "Poly_Polygon2D"))
  {
#ifdef OCCT_DEBUG
    std::cout << "Not a Polygon2D in the file" << std::endl;
#endif
    return occ::handle<Poly_Polygon2D>();
  }

  int nbNodes;
  IS >> nbNodes;

  double d;
  IS >> d;

  // read the nodes
  double                       x, y;
  int                          i;
  NCollection_Array1<gp_Pnt2d> Nodes(1, nbNodes);

  for (i = 1; i <= nbNodes; i++)
  {
    IS >> x >> y;
    Nodes(i).SetCoord(x, y);
  }

  occ::handle<Poly_Polygon2D> P = new Poly_Polygon2D(Nodes);

  P->Deflection(d);

  return P;
}

//=================================================================================================

void Poly::ComputeNormals(const occ::handle<Poly_Triangulation>& theTri)
{
  theTri->ComputeNormals();
}

//=================================================================================================

double Poly::PointOnTriangle(const gp_XY& theP1,
                             const gp_XY& theP2,
                             const gp_XY& theP3,
                             const gp_XY& theP,
                             gp_XY&       theUV)
{
  gp_XY  aDP  = theP - theP1;
  gp_XY  aDU  = theP2 - theP1;
  gp_XY  aDV  = theP3 - theP1;
  double aDet = aDU ^ aDV;

  // case of non-degenerated triangle
  if (std::abs(aDet) > gp::Resolution())
  {
    double aU = (aDP ^ aDV) / aDet;
    double aV = -(aDP ^ aDU) / aDet;

    // if point is inside triangle, just return parameters
    if (aU > -gp::Resolution() && aV > -gp::Resolution() && 1. - aU - aV > -gp::Resolution())
    {
      theUV.SetCoord(aU, aV);
      return 0.;
    }

    // else find closest point on triangle sides; note that in general case
    // triangle can be very distorted and it is necessary to check
    // projection on all sides regardless of values of computed parameters

    // project on side U=0
    aU        = 0.;
    aV        = std::min(1., std::max(0., (aDP * aDV) / aDV.SquareModulus()));
    double aD = (aV * aDV - aDP).SquareModulus();

    // project on side V=0
    double u = std::min(1., std::max(0., (aDP * aDU) / aDU.SquareModulus()));
    double d = (u * aDU - aDP).SquareModulus();
    if (d < aD)
    {
      aU = u;
      aV = 0.;
      aD = d;
    }

    // project on side U+V=1
    gp_XY  aDUV = aDV - aDU;
    double v    = std::min(1., std::max(0., ((aDP - aDU) * aDUV) / aDUV.SquareModulus()));
    d           = (theP2 + v * aDUV - theP).SquareModulus();
    if (d < aD)
    {
      aU = 1. - v;
      aV = v;
      aD = d;
    }

    theUV.SetCoord(aU, aV);
    return aD;
  }

  // degenerated triangle
  double aL2U = aDU.SquareModulus();
  double aL2V = aDV.SquareModulus();
  if (aL2U < gp::Resolution()) // side 1-2 is degenerated
  {
    if (aL2V < gp::Resolution()) // whole triangle is degenerated to point
    {
      theUV.SetCoord(0., 0.);
      return (theP - theP1).SquareModulus();
    }
    else
    {
      theUV.SetCoord(0., (aDP * aDV) / aL2V);
      return (theP - (theP1 + theUV.Y() * aDV)).SquareModulus();
    }
  }
  else if (aL2V < gp::Resolution()) // side 1-3 is degenerated
  {
    theUV.SetCoord((aDP * aDU) / aL2U, 0.);
    return (theP - (theP1 + theUV.X() * aDU)).SquareModulus();
  }
  else // sides 1-2 and 1-3 are collinear
  {
    // select parameter on one of sides so as to have points closer to picked
    double aU  = std::min(1., std::max(0., (aDP * aDU) / aL2U));
    double aV  = std::min(1., std::max(0., (aDP * aDV) / aL2V));
    double aD1 = (aDP - aU * aDU).SquareModulus();
    double aD2 = (aDP - aV * aDV).SquareModulus();
    if (aD1 < aD2)
    {
      theUV.SetCoord((aDP * aDU) / aL2U, 0.);
      return aD1;
    }
    else
    {
      theUV.SetCoord(0., (aDP * aDV) / aL2V);
      return aD2;
    }
  }
}

//=================================================================================================

bool Poly::Intersect(const occ::handle<Poly_Triangulation>& theTri,
                     const gp_Ax1&                          theAxis,
                     const bool                             theIsClosest,
                     Poly_Triangle&                         theTriangle,
                     double&                                theDistance)
{
  const double  aConf = 1E-15;
  const gp_XYZ& aLoc  = theAxis.Location().XYZ();
  const gp_Dir& aDir  = theAxis.Direction();

  double aResult      = theIsClosest ? RealLast() : 0.0;
  double aParam       = 0.0;
  int    aTriNodes[3] = {};
  for (int aTriIter = 1; aTriIter <= theTri->NbTriangles(); ++aTriIter)
  {
    const Poly_Triangle& aTri = theTri->Triangle(aTriIter);
    aTri.Get(aTriNodes[0], aTriNodes[1], aTriNodes[2]);
    if (IntersectTriLine(aLoc,
                         aDir,
                         theTri->Node(aTriNodes[0]).XYZ(),
                         theTri->Node(aTriNodes[1]).XYZ(),
                         theTri->Node(aTriNodes[2]).XYZ(),
                         aParam))
    {
      if (aParam > aConf)
      {
        if (theIsClosest)
        {
          if (aParam < aResult)
          {
            aResult     = aParam;
            theTriangle = aTri;
          }
        }
        else if (aParam > aResult)
        {
          aResult     = aParam;
          theTriangle = aTri;
        }
      }
    }
  }

  if (aConf < aResult && aResult < RealLast())
  {
    theDistance = aResult;
    return true;
  }
  return false;
}

//! Calculate the minor of the given matrix, defined by the columns specified by values c1, c2, c3.
static double Determinant(const double a[3][4], const int c1, const int c2, const int c3)
{
  return a[0][c1] * a[1][c2] * a[2][c3] + a[0][c2] * a[1][c3] * a[2][c1]
         + a[0][c3] * a[1][c1] * a[2][c2] - a[0][c3] * a[1][c2] * a[2][c1]
         - a[0][c2] * a[1][c1] * a[2][c3] - a[0][c1] * a[1][c3] * a[2][c2];
}

//=======================================================================
// function : IntersectTriLine
// purpose  : Intersect a triangle with a line
//=======================================================================
int Poly::IntersectTriLine(const gp_XYZ& theStart,
                           const gp_Dir& theDir,
                           const gp_XYZ& theV0,
                           const gp_XYZ& theV1,
                           const gp_XYZ& theV2,
                           double&       theParam)
{
  int          aRes  = 0;
  const double aConf = 1E-15;

  const double aMat34[3][4] = {
    {-theDir.X(), theV1.X() - theV0.X(), theV2.X() - theV0.X(), theStart.X() - theV0.X()},
    {-theDir.Y(), theV1.Y() - theV0.Y(), theV2.Y() - theV0.Y(), theStart.Y() - theV0.Y()},
    {-theDir.Z(), theV1.Z() - theV0.Z(), theV2.Z() - theV0.Z(), theStart.Z() - theV0.Z()}};

  const double aD  = Determinant(aMat34, 0, 1, 2);
  const double aDt = Determinant(aMat34, 3, 1, 2);
  if (aD > aConf)
  {
    const double aDa = Determinant(aMat34, 0, 3, 2);
    if (aDa > -aConf)
    {
      const double aDb = Determinant(aMat34, 0, 1, 3);
      aRes             = ((aDb > -aConf) && (aDa + aDb <= aD + aConf));
    }
  }
  else if (aD < -aConf)
  {
    const double aDa = Determinant(aMat34, 0, 3, 2);
    if (aDa < aConf)
    {
      const double aDb = Determinant(aMat34, 0, 1, 3);
      aRes             = ((aDb < aConf) && (aDa + aDb >= aD - aConf));
    }
  }
  if (aRes != 0)
  {
    theParam = aDt / aD;
  }

  return aRes;
}
