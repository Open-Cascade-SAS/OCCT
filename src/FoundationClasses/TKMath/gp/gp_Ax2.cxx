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
// JCV 12/12/90 Modif mineur suite a la premiere revue de projet
// LPA, JCV  07/92 passage sur C1.
// JCV 07/92 Introduction de la method Dump

#define No_Standard_OutOfRange

#include <gp_Ax2.hxx>

#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Dump.hxx>

gp_Ax2::gp_Ax2(const gp_Pnt& P, const gp_Dir& V)
    : axis(P, V)
{
  Standard_Real A    = V.X();
  Standard_Real B    = V.Y();
  Standard_Real C    = V.Z();
  Standard_Real Aabs = A;
  if (Aabs < 0)
    Aabs = -Aabs;
  Standard_Real Babs = B;
  if (Babs < 0)
    Babs = -Babs;
  Standard_Real Cabs = C;
  if (Cabs < 0)
    Cabs = -Cabs;
  gp_Dir D;

  //  pour determiner l axe X :
  //  on dit que le produit scalaire Vx.V = 0.
  //  et on recherche le max(A,B,C) pour faire la division.
  //  l'une des coordonnees du vecteur est nulle.

  if (Babs <= Aabs && Babs <= Cabs)
  {
    if (Aabs > Cabs)
      D.SetCoord(-C, 0., A);
    else
      D.SetCoord(C, 0., -A);
  }
  else if (Aabs <= Babs && Aabs <= Cabs)
  {
    if (Babs > Cabs)
      D.SetCoord(0., -C, B);
    else
      D.SetCoord(0., C, -B);
  }
  else
  {
    if (Aabs > Babs)
      D.SetCoord(-B, A, 0.);
    else
      D.SetCoord(B, -A, 0.);
  }
  SetXDirection(D);
}

void gp_Ax2::Mirror(const gp_Pnt& P)
{
  gp_Pnt Temp = axis.Location();
  Temp.Mirror(P);
  axis.SetLocation(Temp);
  vxdir.Reverse();
  vydir.Reverse();
}

gp_Ax2 gp_Ax2::Mirrored(const gp_Pnt& P) const
{
  gp_Ax2 Temp = *this;
  Temp.Mirror(P);
  return Temp;
}

void gp_Ax2::Mirror(const gp_Ax1& A1)
{
  vydir.Mirror(A1);
  vxdir.Mirror(A1);
  gp_Pnt Temp = axis.Location();
  Temp.Mirror(A1);
  axis.SetLocation(Temp);
  axis.SetDirection(vxdir.Crossed(vydir));
}

gp_Ax2 gp_Ax2::Mirrored(const gp_Ax1& A1) const
{
  gp_Ax2 Temp = *this;
  Temp.Mirror(A1);
  return Temp;
}

void gp_Ax2::Mirror(const gp_Ax2& A2)
{
  vydir.Mirror(A2);
  vxdir.Mirror(A2);
  gp_Pnt Temp = axis.Location();
  Temp.Mirror(A2);
  axis.SetLocation(Temp);
  axis.SetDirection(vxdir.Crossed(vydir));
}

gp_Ax2 gp_Ax2::Mirrored(const gp_Ax2& A2) const
{
  gp_Ax2 Temp = *this;
  Temp.Mirror(A2);
  return Temp;
}

void gp_Ax2::DumpJson(Standard_OStream& theOStream, Standard_Integer) const {
  OCCT_DUMP_VECTOR_CLASS(theOStream,
                         "Location",
                         3,
                         axis.Location().X(),
                         axis.Location().Y(),
                         axis.Location().Z()) OCCT_DUMP_VECTOR_CLASS(theOStream,
                                                                     "Direction",
                                                                     3,
                                                                     axis.Direction().X(),
                                                                     axis.Direction().Y(),
                                                                     axis.Direction().Z())

    OCCT_DUMP_VECTOR_CLASS(theOStream, "XDirection", 3, vxdir.X(), vxdir.Y(), vxdir.Z())
      OCCT_DUMP_VECTOR_CLASS(theOStream, "YDirection", 3, vydir.X(), vydir.Y(), vydir.Z())}

Standard_Boolean gp_Ax2::InitFromJson(const Standard_SStream& theSStream,
                                      Standard_Integer&       theStreamPos)
{
  Standard_Integer        aPos       = theStreamPos;
  TCollection_AsciiString aStreamStr = Standard_Dump::Text(theSStream);

  gp_XYZ anXYZLoc;
  OCCT_INIT_VECTOR_CLASS(aStreamStr,
                         "Location",
                         aPos,
                         3,
                         &anXYZLoc.ChangeCoord(1),
                         &anXYZLoc.ChangeCoord(2),
                         &anXYZLoc.ChangeCoord(3))
  SetLocation(anXYZLoc);

  gp_XYZ aDir;
  OCCT_INIT_VECTOR_CLASS(aStreamStr,
                         "Direction",
                         aPos,
                         3,
                         &aDir.ChangeCoord(1),
                         &aDir.ChangeCoord(2),
                         &aDir.ChangeCoord(3))
  gp_XYZ aXDir;
  OCCT_INIT_VECTOR_CLASS(aStreamStr,
                         "XDirection",
                         aPos,
                         3,
                         &aXDir.ChangeCoord(1),
                         &aXDir.ChangeCoord(2),
                         &aXDir.ChangeCoord(3))
  gp_XYZ anYDir;
  OCCT_INIT_VECTOR_CLASS(aStreamStr,
                         "YDirection",
                         aPos,
                         3,
                         &anYDir.ChangeCoord(1),
                         &anYDir.ChangeCoord(2),
                         &anYDir.ChangeCoord(3))

  axis.SetDirection(gp_Dir(aDir));
  vxdir = gp_Dir(aXDir);
  vydir = gp_Dir(anYDir);

  if (!Direction().IsEqual(aDir, Precision::Confusion()))
    return Standard_False;

  theStreamPos = aPos;
  return Standard_True;
}
