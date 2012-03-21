// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <GProp_PGProps.ixx>
#include <Standard_DomainError.hxx>
#include <Standard_DimensionError.hxx>

#include <gp.hxx>
#include <gp_XYZ.hxx>
//#include <gp.hxx>

typedef gp_Pnt Pnt;
typedef gp_Mat Mat;
typedef gp_XYZ XYZ;
typedef TColgp_Array1OfPnt          Array1OfPnt;
typedef TColgp_Array2OfPnt          Array2OfPnt;
typedef TColStd_Array1OfReal Array1OfReal;
typedef TColStd_Array2OfReal Array2OfReal;



GProp_PGProps::GProp_PGProps ()
{
  g = gp::Origin();
  loc = gp::Origin();
  dim = 0.0;
}

void GProp_PGProps::AddPoint (const Pnt& P)
{
  Standard_Real Xp, Yp, Zp;
  P.Coord (Xp, Yp, Zp);
  Standard_Real Ixy = - Xp * Yp;
  Standard_Real Ixz = - Xp * Zp;
  Standard_Real Iyz = - Yp * Zp;

  Standard_Real Ixx = Yp * Yp + Zp * Zp;
  Standard_Real Iyy = Xp * Xp + Zp * Zp;
  Standard_Real Izz = Xp * Xp + Yp * Yp;
  Mat Mp (XYZ (Ixx, Ixy, Ixz), XYZ (Ixy, Iyy, Iyz), XYZ (Ixz, Iyz, Izz));
  if (dim == 0) {
    dim = 1;
    g = P;
    inertia = Mp;
  }
  else {
    Standard_Real X, Y, Z;
    g.Coord (X, Y, Z);
    X = X * dim + Xp;
    Y = Y * dim + Yp;
    Z = Z * dim + Zp;
    dim = dim + 1;
    X /= dim;
    Y /= dim;
    Z /= dim;
    g.SetCoord (X, Y, Z);
    inertia = inertia + Mp;
  }      
}

void GProp_PGProps::AddPoint (const gp_Pnt& P, const Standard_Real Density)
{
  if (Density <= gp::Resolution())  Standard_DomainError::Raise();
  Standard_Real Xp, Yp, Zp;
  P.Coord (Xp, Yp, Zp);
  Standard_Real Ixy = - Xp * Yp;
  Standard_Real Ixz = - Xp * Zp;
  Standard_Real Iyz = - Yp * Zp;
  Standard_Real Ixx = Yp * Yp + Zp * Zp;
  Standard_Real Iyy = Xp * Xp + Zp * Zp;
  Standard_Real Izz = Xp * Xp + Yp * Yp;
  Mat Mp (XYZ (Ixx, Ixy, Ixz), XYZ (Ixy, Iyy, Iyz), XYZ (Ixz, Iyz, Izz));
  if (dim == 0) {
    dim = Density;
    g.SetXYZ (P.XYZ().Multiplied (Density));
    inertia = Mp * Density;
  }
  else {
    Standard_Real X, Y, Z;
    g.Coord (X, Y, Z);
    X = X * dim + Xp * Density;
    Y = Y * dim + Yp * Density;
    Z = Z * dim + Zp * Density;
    dim = dim + Density;
    X /= dim;
    Y /= dim;
    Z /= dim;
    g.SetCoord (X, Y, Z);
    inertia = inertia + Mp * Density;
  }      
}

GProp_PGProps::GProp_PGProps (const Array1OfPnt& Pnts)
{
  for (Standard_Integer i = Pnts.Lower(); i <= Pnts.Upper(); i++) AddPoint(Pnts(i));
}

GProp_PGProps::GProp_PGProps (const Array2OfPnt& Pnts)
{
  for (Standard_Integer j = Pnts.LowerCol(); j <= Pnts.UpperCol(); j++)
    {
      for (Standard_Integer i = Pnts.LowerRow(); i <= Pnts.UpperRow(); i++) AddPoint(Pnts (i, j));
    }      
}

GProp_PGProps::GProp_PGProps (const Array1OfPnt& Pnts,const Array1OfReal& Density)
{
  if (Pnts.Length() != Density.Length())  Standard_DomainError::Raise();
  Standard_Integer ip = Pnts.Lower();
  Standard_Integer id = Density.Lower();
  while (id <= Pnts.Upper()) {
    Standard_Real D = Density (id);
    if (D <= gp::Resolution()) Standard_DomainError::Raise();
    AddPoint(Pnts (ip),D); 
    ip++;  id++;
  }      
}

GProp_PGProps::GProp_PGProps (const Array2OfPnt& Pnts,const Array2OfReal& Density)
{
  if (Pnts.ColLength() != Density.ColLength() || Pnts.RowLength() != Density.RowLength()) Standard_DomainError::Raise();
  Standard_Integer ip = Pnts.LowerRow();
  Standard_Integer id = Density.LowerRow();
  Standard_Integer jp = Pnts.LowerCol();
  Standard_Integer jd = Density.LowerCol();
  while (jp <= Pnts.UpperCol()) {
    while (ip <= Pnts.UpperRow()) {
      Standard_Real D = Density (id, jd);
      if (D <= gp::Resolution())  Standard_DomainError::Raise();
      AddPoint(Pnts (ip, jp),D); 
      ip++; id++;
    }      
    jp++; jd++;
  }
}



void GProp_PGProps::Barycentre(const Array1OfPnt&  Pnts,
			       const Array1OfReal& Density,
			       Standard_Real&      Mass, 
			       Pnt&                G)
{
  if (Pnts.Length() != Density.Length())  Standard_DimensionError::Raise();
  Standard_Integer ip = Pnts.Lower();
  Standard_Integer id = Density.Lower();
  Mass = Density (id);
  XYZ Gxyz = Pnts (ip).XYZ();
  Gxyz.Multiply (Mass);
  while (ip <= Pnts.Upper()) {
    Mass = Mass + Density (id);
    Gxyz.Add ( (Pnts(ip).XYZ()).Multiplied (Density (id)) );
    ip++;
    id++;
  }
  Gxyz.Divide (Mass);
  G.SetXYZ (Gxyz);
}




void GProp_PGProps::Barycentre(const Array2OfPnt&  Pnts, 
			       const Array2OfReal& Density,
			       Standard_Real&      Mass,
			       Pnt&                G)
{
  if (Pnts.RowLength() != Density.RowLength() || Pnts.ColLength() != Density.ColLength()) Standard_DimensionError::Raise();
  Standard_Integer ip = Pnts.LowerRow();
  Standard_Integer id = Density.LowerRow();
  Standard_Integer jp = Pnts.LowerCol();
  Standard_Integer jd = Density.LowerCol();
  Mass = 0.0;
  XYZ Gxyz (0.0, 0.0, 0.0);
  while (jp <= Pnts.UpperCol()) {
    while (ip <= Pnts.UpperRow()) {
      Mass = Mass + Density (id, jd);
      Gxyz.Add ((Pnts(ip, jp).XYZ()).Multiplied (Density (id, jd)));
      ip++;   id++;
    }
    jp++;   jd++;
  }
  Gxyz.Divide (Mass);
  G.SetXYZ (Gxyz);
}




Pnt GProp_PGProps::Barycentre (const Array1OfPnt& Pnts) {

  XYZ Gxyz = Pnts (Pnts.Lower()).XYZ();
  for (Standard_Integer i = Pnts.Lower() + 1; i <= Pnts.Upper(); i++) {
    Gxyz.Add (Pnts(i).XYZ());
  }
  Gxyz.Divide (Pnts.Length());
  return Pnt (Gxyz);
}




Pnt GProp_PGProps::Barycentre (const Array2OfPnt& Pnts) {

  XYZ Gxyz (0.0, 0.0, 0.0);
  for (Standard_Integer j = Pnts.LowerCol(); j <= Pnts.UpperCol(); j++) {
    for (Standard_Integer i = Pnts.LowerRow(); i <= Pnts.UpperRow(); i++) {
      Gxyz.Add (Pnts(i, j).XYZ());
    }
  }
  Gxyz.Divide (Pnts.RowLength() * Pnts.ColLength());
  return Pnt (Gxyz);
}



