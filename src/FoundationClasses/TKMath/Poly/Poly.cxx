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
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_ListOfTriangulation.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <Standard_Stream.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>

//=======================================================================
// function : Catenate
// purpose  : Join several triangulations to one new triangulation object
//=======================================================================
Handle(Poly_Triangulation) Poly::Catenate(const Poly_ListOfTriangulation& lstTri)
{
  Standard_Integer nNodes(0);
  Standard_Integer nTrian(0);

  // Sum up the total number of nodes.
  Poly_ListOfTriangulation::Iterator anIter(lstTri);
  for (; anIter.More(); anIter.Next())
  {
    const Handle(Poly_Triangulation)& aTri = anIter.Value();
    if (!aTri.IsNull())
    {
      nNodes += aTri->NbNodes();
      nTrian += aTri->NbTriangles();
    }
  }

  if (nNodes == 0)
  {
    return Handle(Poly_Triangulation)();
  }

  Handle(Poly_Triangulation) aResult  = new Poly_Triangulation(nNodes, nTrian, Standard_False);
  Standard_Integer           iNode[3] = {};
  nNodes                              = 0;
  nTrian                              = 0;
  for (anIter.Init(lstTri); anIter.More(); anIter.Next())
  {
    const Handle(Poly_Triangulation)& aTri = anIter.Value();
    if (aTri.IsNull())
    {
      continue;
    }

    const Standard_Integer nbNodes = aTri->NbNodes();
    const Standard_Integer nbTrian = aTri->NbTriangles();
    for (Standard_Integer i = 1; i <= nbNodes; i++)
    {
      aResult->SetNode(i + nNodes, aTri->Node(i));
    }
    for (Standard_Integer i = 1; i <= nbTrian; i++)
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

void Poly::Write(const Handle(Poly_Triangulation)& T,
                 Standard_OStream&                 OS,
                 const Standard_Boolean            Compact)
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

  Standard_Integer i, nbNodes = T->NbNodes();
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
  Standard_Integer nbTriangles = T->NbTriangles();
  Standard_Integer n1, n2, n3;
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

void Poly::Write(const Handle(Poly_Polygon3D)& P,
                 Standard_OStream&             OS,
                 const Standard_Boolean        Compact)
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

  Standard_Integer          i, nbNodes = P->NbNodes();
  const TColgp_Array1OfPnt& Nodes = P->Nodes();
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
    const TColStd_Array1OfReal& Param = P->Parameters();
    for (i = 1; i <= nbNodes; i++)
    {
      OS << Param(i) << " ";
    }
    OS << "\n";
  }
}

//=================================================================================================

void Poly::Write(const Handle(Poly_Polygon2D)& P,
                 Standard_OStream&             OS,
                 const Standard_Boolean        Compact)
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

  Standard_Integer            i, nbNodes = P->NbNodes();
  const TColgp_Array1OfPnt2d& Nodes = P->Nodes();
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

void Poly::Dump(const Handle(Poly_Triangulation)& T, Standard_OStream& OS)
{
  Poly::Write(T, OS, Standard_False);
}

//=================================================================================================

void Poly::Dump(const Handle(Poly_Polygon3D)& P, Standard_OStream& OS)
{
  Poly::Write(P, OS, Standard_False);
}

//=================================================================================================

void Poly::Dump(const Handle(Poly_Polygon2D)& P, Standard_OStream& OS)
{
  Poly::Write(P, OS, Standard_False);
}

//=================================================================================================

Handle(Poly_Triangulation) Poly::ReadTriangulation(Standard_IStream& IS)
{
  // Read a triangulation

  char line[100];
  IS >> line;
  if (strcmp(line, "Poly_Triangulation"))
  {
#ifdef OCCT_DEBUG
    std::cout << "Not a Triangulation in the file" << std::endl;
#endif
    return Handle(Poly_Triangulation)();
  }

  Standard_Integer nbNodes, nbTriangles;
  Standard_Boolean hasUV;
  IS >> nbNodes >> nbTriangles >> hasUV;

  Standard_Real d;
  IS >> d;

  // read the 3d nodes
  Standard_Real        x, y, z;
  Standard_Integer     i;
  TColgp_Array1OfPnt   Nodes(1, nbNodes);
  TColgp_Array1OfPnt2d UVNodes(1, nbNodes);

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
  Standard_Integer      n1, n2, n3;
  Poly_Array1OfTriangle Triangles(1, nbTriangles);
  for (i = 1; i <= nbTriangles; i++)
  {
    IS >> n1 >> n2 >> n3;
    Triangles(i).Set(n1, n2, n3);
  }

  Handle(Poly_Triangulation) T;

  if (hasUV)
    T = new Poly_Triangulation(Nodes, UVNodes, Triangles);
  else
    T = new Poly_Triangulation(Nodes, Triangles);

  T->Deflection(d);

  return T;
}

//=================================================================================================

Handle(Poly_Polygon3D) Poly::ReadPolygon3D(Standard_IStream& IS)
{
  // Read a 3d polygon

  char line[100];
  IS >> line;
  if (strcmp(line, "Poly_Polygon3D"))
  {
#ifdef OCCT_DEBUG
    std::cout << "Not a Polygon3D in the file" << std::endl;
#endif
    return Handle(Poly_Polygon3D)();
  }

  Standard_Integer nbNodes;
  IS >> nbNodes;

  Standard_Boolean hasparameters;
  IS >> hasparameters;

  Standard_Real d;
  IS >> d;

  // read the nodes
  Standard_Real      x, y, z;
  Standard_Integer   i;
  TColgp_Array1OfPnt Nodes(1, nbNodes);

  for (i = 1; i <= nbNodes; i++)
  {
    IS >> x >> y >> z;
    Nodes(i).SetCoord(x, y, z);
  }

  TColStd_Array1OfReal Param(1, nbNodes);
  if (hasparameters)
  {
    for (i = 1; i <= nbNodes; i++)
    {
      IS >> Param(i);
    }
  }

  Handle(Poly_Polygon3D) P;
  if (!hasparameters)
    P = new Poly_Polygon3D(Nodes);
  else
    P = new Poly_Polygon3D(Nodes, Param);

  P->Deflection(d);

  return P;
}

//=================================================================================================

Handle(Poly_Polygon2D) Poly::ReadPolygon2D(Standard_IStream& IS)
{
  // Read a 2d polygon

  char line[100];
  IS >> line;
  if (strcmp(line, "Poly_Polygon2D"))
  {
#ifdef OCCT_DEBUG
    std::cout << "Not a Polygon2D in the file" << std::endl;
#endif
    return Handle(Poly_Polygon2D)();
  }

  Standard_Integer nbNodes;
  IS >> nbNodes;

  Standard_Real d;
  IS >> d;

  // read the nodes
  Standard_Real        x, y;
  Standard_Integer     i;
  TColgp_Array1OfPnt2d Nodes(1, nbNodes);

  for (i = 1; i <= nbNodes; i++)
  {
    IS >> x >> y;
    Nodes(i).SetCoord(x, y);
  }

  Handle(Poly_Polygon2D) P = new Poly_Polygon2D(Nodes);

  P->Deflection(d);

  return P;
}

//=================================================================================================

void Poly::ComputeNormals(const Handle(Poly_Triangulation)& theTri)
{
  theTri->ComputeNormals();
}

//=================================================================================================

Standard_Real Poly::PointOnTriangle(const gp_XY& theP1,
                                    const gp_XY& theP2,
                                    const gp_XY& theP3,
                                    const gp_XY& theP,
                                    gp_XY&       theUV)
{
  gp_XY         aDP  = theP - theP1;
  gp_XY         aDU  = theP2 - theP1;
  gp_XY         aDV  = theP3 - theP1;
  Standard_Real aDet = aDU ^ aDV;

  // case of non-degenerated triangle
  if (Abs(aDet) > gp::Resolution())
  {
    Standard_Real aU = (aDP ^ aDV) / aDet;
    Standard_Real aV = -(aDP ^ aDU) / aDet;

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
    aU               = 0.;
    aV               = Min(1., Max(0., (aDP * aDV) / aDV.SquareModulus()));
    Standard_Real aD = (aV * aDV - aDP).SquareModulus();

    // project on side V=0
    Standard_Real u = Min(1., Max(0., (aDP * aDU) / aDU.SquareModulus()));
    Standard_Real d = (u * aDU - aDP).SquareModulus();
    if (d < aD)
    {
      aU = u;
      aV = 0.;
      aD = d;
    }

    // project on side U+V=1
    gp_XY         aDUV = aDV - aDU;
    Standard_Real v    = Min(1., Max(0., ((aDP - aDU) * aDUV) / aDUV.SquareModulus()));
    d                  = (theP2 + v * aDUV - theP).SquareModulus();
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
  Standard_Real aL2U = aDU.SquareModulus();
  Standard_Real aL2V = aDV.SquareModulus();
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
    Standard_Real aU  = Min(1., Max(0., (aDP * aDU) / aL2U));
    Standard_Real aV  = Min(1., Max(0., (aDP * aDV) / aL2V));
    Standard_Real aD1 = (aDP - aU * aDU).SquareModulus();
    Standard_Real aD2 = (aDP - aV * aDV).SquareModulus();
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

Standard_Boolean Poly::Intersect(const Handle(Poly_Triangulation)& theTri,
                                 const gp_Ax1&                     theAxis,
                                 const Standard_Boolean            theIsClosest,
                                 Poly_Triangle&                    theTriangle,
                                 Standard_Real&                    theDistance)
{
  const Standard_Real aConf = 1E-15;
  const gp_XYZ&       aLoc  = theAxis.Location().XYZ();
  const gp_Dir&       aDir  = theAxis.Direction();

  Standard_Real    aResult      = theIsClosest ? RealLast() : 0.0;
  Standard_Real    aParam       = 0.0;
  Standard_Integer aTriNodes[3] = {};
  for (Standard_Integer aTriIter = 1; aTriIter <= theTri->NbTriangles(); ++aTriIter)
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
    return Standard_True;
  }
  return Standard_False;
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
Standard_Integer Poly::IntersectTriLine(const gp_XYZ&  theStart,
                                        const gp_Dir&  theDir,
                                        const gp_XYZ&  theV0,
                                        const gp_XYZ&  theV1,
                                        const gp_XYZ&  theV2,
                                        Standard_Real& theParam)
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
