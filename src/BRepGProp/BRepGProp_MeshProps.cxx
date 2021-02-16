// Copyright (c) 2018 OPEN CASCADE SAS
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

#include <BRepGProp_MeshProps.hxx>

#include <BRepGProp.hxx>
#include <ElSLib.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <GProp.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Triangle.hxx>
#include <TopLoc_Location.hxx>

//=======================================================================
//function : CalculateElSProps
//purpose  : Calculate one Gauss point for surface properties 
//           of triangle p1, p2, p3
//           relatively point Apex 
//=======================================================================
static void CalculateElSProps(const Standard_Real x, 
  const Standard_Real y,
  const Standard_Real z, const Standard_Real ds, Standard_Real* GProps)
{
  //GProps[0] = Volume
  // Static moments of inertia.
  //GProps[1] = Ix, GProps[2] = Iy, GProps[3] = Iz
  //Matrix of moments of inertia.
  //GProps[4] = Ixx, GProps[5] = Iyy, GProps[6] = Izz,
  //GProps[7] = Ixy, aGProps[8] = Ixz, GProps[9] = Iyz,
  //
  Standard_Real x2, y2, z2;
  x2 = x * x;
  y2 = y * y;
  z2 = z * z;

  GProps[0] += ds;       //Area       
  GProps[1] += x * ds;  //Ix
  GProps[2] += y * ds;  //Iy
  GProps[3] += z * ds;  //Iz
  //
  GProps[7] += x * y * ds; //Ixy
  GProps[8] += x * z * ds; //Ixz
  GProps[9] += y * z * ds; //Iyz
  GProps[4] += (y2 + z2) * ds; //Ixx
  GProps[5] += (x2 + z2) * ds; //Iyy
  GProps[6] += (x2 + y2) * ds; //Izz
}
//=======================================================================
//function : CalculateElVProps
//purpose  : Calculate one Gauss point for volume properties of pyramid, 
//           based on triangle p1, p2, p3 with apex Apex 
//=======================================================================
static void CalculateElVProps(const Standard_Real x, 
  const Standard_Real y,
  const Standard_Real z, const Standard_Real dv, Standard_Real* GProps)
{
  Standard_Real x2, y2, z2;
  x2 = x * x;
  y2 = y * y;
  z2 = z * z;
  GProps[0] += dv / 3.0;       //Volume       
  GProps[1] += 0.25 * x * dv;  //Ix
  GProps[2] += 0.25 * y * dv;  //Iy
  GProps[3] += 0.25 * z * dv;  //Iz
  Standard_Real dv1 = 0.2 * dv;
  GProps[7] += x * y * dv1; //Ixy
  GProps[8] += x * z * dv1; //Ixz
  GProps[9] += y * z * dv1; //Iyz
  GProps[4] += (y2 + z2) * dv1; //Ixx
  GProps[5] += (x2 + z2) * dv1; //Iyy
  GProps[6] += (x2 + y2) * dv1; //Izz
}
//=======================================================================
//function : CalculateProps
//purpose  : Calculate global surface properties of triangle 
//           or volume properties of pyramid, based on triangle
//           p1, p2, p3 with apex Apex by Gauss integration over triangle area.
//=======================================================================
 void BRepGProp_MeshProps::CalculateProps(const gp_Pnt& p1, const gp_Pnt& p2, 
                                          const gp_Pnt& p3,
                                          const gp_Pnt& Apex,
                                          const Standard_Boolean isVolume,
                                          Standard_Real GProps[10],
                                          const Standard_Integer NbGaussPoints,
                                          const Standard_Real* GaussPnts)
{
  //GProps[0] = Volume
  // Static moments of inertia.
  //GProps[1] = Ix, GProps[2] = Iy, GProps[3] = Iz
  //Matrix of moments of inertia.
  //GProps[4] = Ixx, GProps[5] = Iyy, GProps[6] = Izz,
  //GProps[7] = Ixy, aGProps[8] = Ixz, GProps[9] = Iyz,
  //

  //Define plane and coordinates of triangle nodes on plane 
  gp_Vec aV12(p2, p1);
  gp_Vec aV23(p3, p2);
  gp_Vec aNorm = aV12 ^ aV23;
  Standard_Real aDet = aNorm.Magnitude();
  if (aDet <= gp::Resolution())
  {
    return;
  }
  gp_XYZ aCenter = (p1.XYZ() + p2.XYZ() + p3.XYZ()) / 3.;
  gp_Pnt aPC(aCenter);
  gp_Dir aDN(aNorm);
  gp_Ax3 aPosPln(aPC, aDN);
  //Coordinates of nodes on plane
  Standard_Real x1, y1, x2, y2, x3, y3;
  ElSLib::PlaneParameters(aPosPln, p1, x1, y1);
  ElSLib::PlaneParameters(aPosPln, p2, x2, y2);
  ElSLib::PlaneParameters(aPosPln, p3, x3, y3);
  //
  Standard_Real l1, l2; //barycentriche coordinates
  Standard_Real x, y, z;
  Standard_Real w; //weight
  Standard_Integer i;
  for (i = 0; i < NbGaussPoints; ++i)
  {
    Standard_Integer ind = 3 * i;
    l1 = GaussPnts[ind];
    l2 = GaussPnts[ind + 1];
    w = GaussPnts[ind + 2];
    w *= aDet;
    x = l1*(x1 - x3) + l2*(x2 - x3) + x3;
    y = l1*(y1 - y3) + l2*(y2 - y3) + y3;
    gp_Pnt aP = ElSLib::PlaneValue(x, y, aPosPln);
    x = aP.X() - Apex.X();
    y = aP.Y() - Apex.Y();
    z = aP.Z() - Apex.Z();
    //
    if (isVolume)
    {
      Standard_Real xn = aDN.X() * w;
      Standard_Real yn = aDN.Y() * w;
      Standard_Real zn = aDN.Z() * w;
      Standard_Real dv = x * xn + y * yn + z * zn;
      CalculateElVProps(x, y, z, dv, GProps);
    }
    else
    {
      Standard_Real ds = w;
      CalculateElSProps(x, y, z, ds, GProps);
    }

  }
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BRepGProp_MeshProps::Perform(const Handle(Poly_Triangulation)& theMesh,
                                  const TopLoc_Location& theLoc,
                                  const TopAbs_Orientation theOri)
{
  if (theMesh.IsNull() || theMesh->NbNodes() == 0 || theMesh->NbTriangles() == 0)
  {
    return;
  }
  if (theLoc.IsIdentity())
  {
    Perform (theMesh, theOri);
  }
  else
  {
    const gp_Trsf& aTr = theLoc.Transformation();
    //
    Standard_Boolean isToCopy = aTr.ScaleFactor()*aTr.HVectorialPart().Determinant() < 0. ||
                                Abs(Abs(aTr.ScaleFactor()) - 1.) > gp::Resolution();
    if (isToCopy)
    {
      Handle(Poly_Triangulation) aCopy = new Poly_Triangulation (theMesh->NbNodes(), theMesh->NbTriangles(), false);
      TColgp_Array1OfPnt aNodes (1, theMesh->NbNodes());
      for (Standard_Integer i = 1; i <= theMesh->NbNodes(); ++i)
      {
        aCopy->SetNode (i, theMesh->Node (i).Transformed (aTr));
      }
      for (Standard_Integer i = 1; i <= theMesh->NbTriangles(); ++i)
      {
        aCopy->SetTriangle (i, theMesh->Triangle (i));
      }
      Perform (aCopy, theOri);
      return;
    }
    //
    gp_Trsf aTrInv = aTr.Inverted();
    gp_Pnt loc_save = loc;
    loc.Transform(aTrInv);
    Perform (theMesh, theOri);
    //Computes the inertia tensor at mesh gravity center
    gp_Mat HMat, inertia0;
    gp_Pnt g0 = g;
    g.SetXYZ(g.XYZ() + loc.XYZ());
    if (g0.XYZ().Modulus() > gp::Resolution())
    {
      GProp::HOperator(g, loc, dim, HMat);
      inertia0 = inertia - HMat;
    }
    else
    {
      inertia0 = inertia;
    }
    //Transform inertia tensor for rotation
    gp_Mat HVec = aTrInv.HVectorialPart();
    gp_Mat HVecT = HVec.Transposed();
    HVecT.Multiply(inertia0);
    inertia0 = HVecT.Multiplied(HVec);
    //Put gravity center in true position of mesh  
    g.Transform(aTr);
    g0 = g;
    g.SetXYZ(g.XYZ() - loc_save.XYZ());
    loc = loc_save;
    //
    //Computes the inertia tensor for loc
    GProp::HOperator(g0, loc, dim, HMat);
    inertia = inertia0 + HMat;
  }
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BRepGProp_MeshProps::Perform (const Handle(Poly_Triangulation)& theMesh,
                                   const TopAbs_Orientation theOri)
{
  if (theMesh.IsNull()
   || theMesh->NbNodes() == 0
   || theMesh->NbTriangles() == 0)
  {
    return;
  }
  //
  // Gauss points for barycentriche coordinates
  static const Standard_Real GPtsWg[] =
  {  1. / 6., 1. / 6., 1. / 6., /*3 points*/
     2. / 3., 1. / 6., 1. / 6.,
     1. / 6., 2. / 3., 1. / 6. };
  //
  Standard_Integer aNbGaussPoints = 3;
  
  // Array to store global properties
  Standard_Real aGProps[10] = { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0 };
  //aGProps[0] = Volume
  // Static moments of inertia.
  //aGProps[1] = Ix, aGProps[2] = Iy, aGProps[3] = Iz
  //Matrix of moments of inertia.
  //aGProps[4] = Ixx, aGProps[5] = Iyy, aGProps[6] = Izz,
  //aGProps[7] = Ixy, aGProps[8] = Ixz, aGProps[9] = Iyz,

  Standard_Boolean isVolume = myType == Vinert;
  Standard_Integer n1, n2, n3; //node indices
  for (Standard_Integer i = 1; i <= theMesh->NbTriangles(); ++i)
  {
    const Poly_Triangle aTri = theMesh->Triangle (i);
    aTri.Get (n1, n2, n3);
    if (theOri == TopAbs_REVERSED)
    {
      std::swap (n2, n3);
    }
    // Calculate properties of a pyramid built on face and apex
    const gp_Pnt p1 = theMesh->Node (n1);
    const gp_Pnt p2 = theMesh->Node (n2);
    const gp_Pnt p3 = theMesh->Node (n3);
    CalculateProps (p1, p2, p3, loc, isVolume, aGProps, aNbGaussPoints, GPtsWg);
  }

  dim = aGProps[0];
  if (Abs(dim) >= 1.e-20) //To be consistent with GProp_GProps
  {
    g.SetX(aGProps[1] / dim);
    g.SetY(aGProps[2] / dim);
    g.SetZ(aGProps[3] / dim);
  }
  else
  {
    g.SetX(aGProps[1]);
    g.SetY(aGProps[2]);
    g.SetZ(aGProps[3]);
  }
  inertia(1, 1) = aGProps[4];
  inertia(1, 2) = -aGProps[7];
  inertia(1, 3) = -aGProps[8];
  inertia(2, 1) = -aGProps[7];
  inertia(2, 2) = aGProps[5];
  inertia(2, 3) = -aGProps[9];
  inertia(3, 1) = -aGProps[8];
  inertia(3, 2) = -aGProps[9];
  inertia(3, 3) = aGProps[6];
}
