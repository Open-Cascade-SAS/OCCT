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

// JCV 30/08/90 Modif passage version C++ 2.0 sur Sun
// JCV 1/10/90 Changement de nom du package vgeom -> gp
// JCV 12/12/90 Modif suite a la premiere revue de projet
// LPA, JCV  07/92 passage sur C1.
// JCV 07/92 Introduction de la method Dump
// LBO 08/93 Passage aux Ax3

#include <gp_Pln.hxx>

#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Dump.hxx>

//==================================================================================================

gp_Pln::gp_Pln(const gp_Pnt& theP, const gp_Dir& theV)
{
  const double A      = theV.X();
  const double B      = theV.Y();
  const double C      = theV.Z();
  const double anAbsA = std::abs(A);
  const double anAbsB = std::abs(B);
  const double anAbsC = std::abs(C);

  // To determine the X axis:
  // we require that the dot product Vx.V = 0.
  // and we search for the max(A,B,C) to perform the division.
  // one of the coordinates of the vector is zero.

  if (anAbsB <= anAbsA && anAbsB <= anAbsC)
  {
    if (anAbsA > anAbsC)
      myPosition = gp_Ax3(theP, theV, gp_Dir(-C, 0., A));
    else
      myPosition = gp_Ax3(theP, theV, gp_Dir(C, 0., -A));
  }
  else if (anAbsA <= anAbsB && anAbsA <= anAbsC)
  {
    if (anAbsB > anAbsC)
      myPosition = gp_Ax3(theP, theV, gp_Dir(0., -C, B));
    else
      myPosition = gp_Ax3(theP, theV, gp_Dir(0., C, -B));
  }
  else
  {
    if (anAbsA > anAbsB)
      myPosition = gp_Ax3(theP, theV, gp_Dir(-B, A, 0.));
    else
      myPosition = gp_Ax3(theP, theV, gp_Dir(B, -A, 0.));
  }
}

//==================================================================================================

gp_Pln::gp_Pln(const double theA, const double theB, const double theC, const double theD)
{
  const double anAbsA = std::abs(theA);
  const double anAbsB = std::abs(theB);
  const double anAbsC = std::abs(theC);
  if (anAbsB <= anAbsA && anAbsB <= anAbsC)
  {
    if (anAbsA > anAbsC)
      myPosition =
        gp_Ax3(gp_Pnt(-theD / theA, 0., 0.), gp_Dir(theA, theB, theC), gp_Dir(-theC, 0., theA));
    else
      myPosition =
        gp_Ax3(gp_Pnt(0., 0., -theD / theC), gp_Dir(theA, theB, theC), gp_Dir(theC, 0., -theA));
  }
  else if (anAbsA <= anAbsB && anAbsA <= anAbsC)
  {
    if (anAbsB > anAbsC)
      myPosition =
        gp_Ax3(gp_Pnt(0., -theD / theB, 0.), gp_Dir(theA, theB, theC), gp_Dir(0., -theC, theB));
    else
      myPosition =
        gp_Ax3(gp_Pnt(0., 0., -theD / theC), gp_Dir(theA, theB, theC), gp_Dir(0., theC, -theB));
  }
  else
  {
    if (anAbsA > anAbsB)
      myPosition =
        gp_Ax3(gp_Pnt(-theD / theA, 0., 0.), gp_Dir(theA, theB, theC), gp_Dir(-theB, theA, 0.));
    else
      myPosition =
        gp_Ax3(gp_Pnt(0., -theD / theB, 0.), gp_Dir(theA, theB, theC), gp_Dir(theB, -theA, 0.));
  }
}

//==================================================================================================

void gp_Pln::Mirror(const gp_Pnt& theP) noexcept
{
  myPosition.Mirror(theP);
}

//==================================================================================================

gp_Pln gp_Pln::Mirrored(const gp_Pnt& theP) const noexcept
{
  return gp_Pln(myPosition.Mirrored(theP));
}

//==================================================================================================

void gp_Pln::Mirror(const gp_Ax1& theA1) noexcept
{
  myPosition.Mirror(theA1);
}

//==================================================================================================

gp_Pln gp_Pln::Mirrored(const gp_Ax1& theA1) const noexcept
{
  return gp_Pln(myPosition.Mirrored(theA1));
}

//==================================================================================================

void gp_Pln::Mirror(const gp_Ax2& theA2) noexcept
{
  myPosition.Mirror(theA2);
}

//==================================================================================================

gp_Pln gp_Pln::Mirrored(const gp_Ax2& theA2) const noexcept
{
  return gp_Pln(myPosition.Mirrored(theA2));
}

//==================================================================================================

void gp_Pln::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myPosition);
}
