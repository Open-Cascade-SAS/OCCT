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

#include <GProp_PGProps.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_DomainError.hxx>

// #include <gp.hxx>
typedef gp_Pnt               Pnt;
typedef gp_Mat               Mat;
typedef gp_XYZ               XYZ;
typedef NCollection_Array1<gp_Pnt>   Array1OfPnt;
typedef NCollection_Array2<gp_Pnt>   Array2OfPnt;
typedef NCollection_Array1<double> Array1OfReal;
typedef NCollection_Array2<double> Array2OfReal;

GProp_PGProps::GProp_PGProps()
{
  g   = gp::Origin();
  loc = gp::Origin();
  dim = 0.0;
}

void GProp_PGProps::AddPoint(const Pnt& P)
{
  double Xp, Yp, Zp;
  P.Coord(Xp, Yp, Zp);
  double Ixy = -Xp * Yp;
  double Ixz = -Xp * Zp;
  double Iyz = -Yp * Zp;

  double Ixx = Yp * Yp + Zp * Zp;
  double Iyy = Xp * Xp + Zp * Zp;
  double Izz = Xp * Xp + Yp * Yp;
  Mat           Mp(XYZ(Ixx, Ixy, Ixz), XYZ(Ixy, Iyy, Iyz), XYZ(Ixz, Iyz, Izz));
  if (dim == 0)
  {
    dim     = 1;
    g       = P;
    inertia = Mp;
  }
  else
  {
    double X, Y, Z;
    g.Coord(X, Y, Z);
    X   = X * dim + Xp;
    Y   = Y * dim + Yp;
    Z   = Z * dim + Zp;
    dim = dim + 1;
    X /= dim;
    Y /= dim;
    Z /= dim;
    g.SetCoord(X, Y, Z);
    inertia = inertia + Mp;
  }
}

void GProp_PGProps::AddPoint(const gp_Pnt& P, const double Density)
{
  if (Density <= gp::Resolution())
    throw Standard_DomainError();
  double Xp, Yp, Zp;
  P.Coord(Xp, Yp, Zp);
  double Ixy = -Xp * Yp;
  double Ixz = -Xp * Zp;
  double Iyz = -Yp * Zp;
  double Ixx = Yp * Yp + Zp * Zp;
  double Iyy = Xp * Xp + Zp * Zp;
  double Izz = Xp * Xp + Yp * Yp;
  Mat           Mp(XYZ(Ixx, Ixy, Ixz), XYZ(Ixy, Iyy, Iyz), XYZ(Ixz, Iyz, Izz));
  if (dim == 0)
  {
    dim     = Density;
    g       = P;
    inertia = Mp * Density;
  }
  else
  {
    double X, Y, Z;
    g.Coord(X, Y, Z);
    X   = X * dim + Xp * Density;
    Y   = Y * dim + Yp * Density;
    Z   = Z * dim + Zp * Density;
    dim = dim + Density;
    X /= dim;
    Y /= dim;
    Z /= dim;
    g.SetCoord(X, Y, Z);
    inertia = inertia + Mp * Density;
  }
}

GProp_PGProps::GProp_PGProps(const Array1OfPnt& Pnts)
{
  for (int i = Pnts.Lower(); i <= Pnts.Upper(); i++)
    AddPoint(Pnts(i));
}

GProp_PGProps::GProp_PGProps(const Array2OfPnt& Pnts)
{
  for (int j = Pnts.LowerCol(); j <= Pnts.UpperCol(); j++)
  {
    for (int i = Pnts.LowerRow(); i <= Pnts.UpperRow(); i++)
      AddPoint(Pnts(i, j));
  }
}

GProp_PGProps::GProp_PGProps(const Array1OfPnt& Pnts, const Array1OfReal& Density)
{
  if (Pnts.Length() != Density.Length())
    throw Standard_DomainError();
  int ip = Pnts.Lower();
  int id = Density.Lower();
  while (id <= Pnts.Upper())
  {
    double D = Density(id);
    if (D <= gp::Resolution())
      throw Standard_DomainError();
    AddPoint(Pnts(ip), D);
    ip++;
    id++;
  }
}

GProp_PGProps::GProp_PGProps(const Array2OfPnt& Pnts, const Array2OfReal& Density)
{
  if (Pnts.ColLength() != Density.ColLength() || Pnts.RowLength() != Density.RowLength())
    throw Standard_DomainError();
  int ip = Pnts.LowerRow();
  int id = Density.LowerRow();
  int jp = Pnts.LowerCol();
  int jd = Density.LowerCol();
  while (jp <= Pnts.UpperCol())
  {
    while (ip <= Pnts.UpperRow())
    {
      double D = Density(id, jd);
      if (D <= gp::Resolution())
        throw Standard_DomainError();
      AddPoint(Pnts(ip, jp), D);
      ip++;
      id++;
    }
    jp++;
    jd++;
  }
}

void GProp_PGProps::Barycentre(const Array1OfPnt&  Pnts,
                               const Array1OfReal& Density,
                               double&      Mass,
                               Pnt&                G)
{
  if (Pnts.Length() != Density.Length())
    throw Standard_DimensionError();
  int ip = Pnts.Lower();
  int id = Density.Lower();
  Mass                = Density(id);
  XYZ Gxyz            = Pnts(ip).XYZ();
  Gxyz.Multiply(Mass);
  while (ip <= Pnts.Upper())
  {
    Mass = Mass + Density(id);
    Gxyz.Add((Pnts(ip).XYZ()).Multiplied(Density(id)));
    ip++;
    id++;
  }
  Gxyz.Divide(Mass);
  G.SetXYZ(Gxyz);
}

void GProp_PGProps::Barycentre(const Array2OfPnt&  Pnts,
                               const Array2OfReal& Density,
                               double&      Mass,
                               Pnt&                G)
{
  if (Pnts.RowLength() != Density.RowLength() || Pnts.ColLength() != Density.ColLength())
    throw Standard_DimensionError();
  int ip = Pnts.LowerRow();
  int id = Density.LowerRow();
  int jp = Pnts.LowerCol();
  int jd = Density.LowerCol();
  Mass                = 0.0;
  XYZ Gxyz(0.0, 0.0, 0.0);
  while (jp <= Pnts.UpperCol())
  {
    while (ip <= Pnts.UpperRow())
    {
      Mass = Mass + Density(id, jd);
      Gxyz.Add((Pnts(ip, jp).XYZ()).Multiplied(Density(id, jd)));
      ip++;
      id++;
    }
    jp++;
    jd++;
  }
  Gxyz.Divide(Mass);
  G.SetXYZ(Gxyz);
}

Pnt GProp_PGProps::Barycentre(const Array1OfPnt& Pnts)
{

  XYZ Gxyz = Pnts(Pnts.Lower()).XYZ();
  for (int i = Pnts.Lower() + 1; i <= Pnts.Upper(); i++)
  {
    Gxyz.Add(Pnts(i).XYZ());
  }
  Gxyz.Divide(Pnts.Length());
  return Pnt(Gxyz);
}

Pnt GProp_PGProps::Barycentre(const Array2OfPnt& Pnts)
{

  XYZ Gxyz(0.0, 0.0, 0.0);
  for (int j = Pnts.LowerCol(); j <= Pnts.UpperCol(); j++)
  {
    for (int i = Pnts.LowerRow(); i <= Pnts.UpperRow(); i++)
    {
      Gxyz.Add(Pnts(i, j).XYZ());
    }
  }
  Gxyz.Divide(Pnts.RowLength() * Pnts.ColLength());
  return Pnt(Gxyz);
}
