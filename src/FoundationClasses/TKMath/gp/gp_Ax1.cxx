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

// JCV 1/10/90 Changement de nom du package vgeom -> gp
// JCV 12/12/90 modif introduction des classes XYZ et Mat dans le package
// LPA, JCV  07/92 passage sur C1.
// JCV 07/92 Introduction de la method Dump

#define No_Standard_OutOfRange

#include <gp_Ax1.hxx>

#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_Dump.hxx>

Standard_Boolean gp_Ax1::IsCoaxial(const gp_Ax1&       Other,
                                   const Standard_Real AngularTolerance,
                                   const Standard_Real LinearTolerance) const
{
  gp_XYZ XYZ1 = loc.XYZ();
  XYZ1.Subtract(Other.loc.XYZ());
  XYZ1.Cross(Other.vdir.XYZ());
  Standard_Real D1   = XYZ1.Modulus();
  gp_XYZ        XYZ2 = Other.loc.XYZ();
  XYZ2.Subtract(loc.XYZ());
  XYZ2.Cross(vdir.XYZ());
  Standard_Real D2 = XYZ2.Modulus();
  return (vdir.IsEqual(Other.vdir, AngularTolerance) && D1 <= LinearTolerance
          && D2 <= LinearTolerance);
}

void gp_Ax1::Mirror(const gp_Pnt& P)
{
  loc.Mirror(P);
  vdir.Reverse();
}

gp_Ax1 gp_Ax1::Mirrored(const gp_Pnt& P) const
{
  gp_Ax1 A1 = *this;
  A1.Mirror(P);
  return A1;
}

void gp_Ax1::Mirror(const gp_Ax1& A1)
{
  loc.Mirror(A1);
  vdir.Mirror(A1.vdir);
}

gp_Ax1 gp_Ax1::Mirrored(const gp_Ax1& A1) const
{
  gp_Ax1 A = *this;
  A.Mirror(A1);
  return A;
}

void gp_Ax1::Mirror(const gp_Ax2& A2)
{
  loc.Mirror(A2);
  vdir.Mirror(A2);
}

gp_Ax1 gp_Ax1::Mirrored(const gp_Ax2& A2) const
{
  gp_Ax1 A1 = *this;
  A1.Mirror(A2);
  return A1;
}

void gp_Ax1::DumpJson(Standard_OStream& theOStream, Standard_Integer) const {
  OCCT_DUMP_VECTOR_CLASS(theOStream, "Location", 3, loc.X(), loc.Y(), loc.Z())
    OCCT_DUMP_VECTOR_CLASS(theOStream, "Direction", 3, vdir.X(), vdir.Y(), vdir.Z())}

Standard_Boolean gp_Ax1::InitFromJson(const Standard_SStream& theSStream,
                                      Standard_Integer&       theStreamPos)
{
  Standard_Integer        aPos       = theStreamPos;
  TCollection_AsciiString aStreamStr = Standard_Dump::Text(theSStream);

  gp_XYZ& anXYZLoc = loc.ChangeCoord();
  OCCT_INIT_VECTOR_CLASS(aStreamStr,
                         "Location",
                         aPos,
                         3,
                         &anXYZLoc.ChangeCoord(1),
                         &anXYZLoc.ChangeCoord(2),
                         &anXYZLoc.ChangeCoord(3))
  gp_XYZ aDir;
  OCCT_INIT_VECTOR_CLASS(aStreamStr,
                         "Direction",
                         aPos,
                         3,
                         &aDir.ChangeCoord(1),
                         &aDir.ChangeCoord(2),
                         &aDir.ChangeCoord(3))
  SetDirection(aDir);

  theStreamPos = aPos;
  return Standard_True;
}
