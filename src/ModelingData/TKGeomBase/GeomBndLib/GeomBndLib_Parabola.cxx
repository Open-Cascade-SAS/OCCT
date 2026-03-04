// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <GeomBndLib_Parabola.hxx>

#include <ElCLib.hxx>
#include <gp_Parab.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>

//=================================================================================================

Bnd_Box GeomBndLib_Parabola::Box(const gp_Parab& theParab,
                                  double          theU1,
                                  double          theU2,
                                  double          theTol)
{
  Bnd_Box aBox;
  if (Precision::IsNegativeInfinite(theU1))
  {
    if (Precision::IsNegativeInfinite(theU2))
    {
      throw Standard_Failure("GeomBndLib_Parabola::Box - bad parameter");
    }
    else if (Precision::IsPositiveInfinite(theU2))
    {
      aBox.OpenXmax();
      aBox.OpenYmax();
      aBox.OpenZmax();
    }
    else
    {
      aBox.Add(ElCLib::Value(theU2, theParab));
    }
    aBox.OpenXmin();
    aBox.OpenYmin();
    aBox.OpenZmin();
  }
  else if (Precision::IsPositiveInfinite(theU1))
  {
    if (Precision::IsNegativeInfinite(theU2))
    {
      aBox.OpenXmin();
      aBox.OpenYmin();
      aBox.OpenZmin();
    }
    else if (Precision::IsPositiveInfinite(theU2))
    {
      throw Standard_Failure("GeomBndLib_Parabola::Box - bad parameter");
    }
    else
    {
      aBox.Add(ElCLib::Value(theU2, theParab));
    }
    aBox.OpenXmax();
    aBox.OpenYmax();
    aBox.OpenZmax();
  }
  else
  {
    aBox.Add(ElCLib::Value(theU1, theParab));
    if (Precision::IsNegativeInfinite(theU2))
    {
      aBox.OpenXmin();
      aBox.OpenYmin();
      aBox.OpenZmin();
    }
    else if (Precision::IsPositiveInfinite(theU2))
    {
      aBox.OpenXmax();
      aBox.OpenYmax();
      aBox.OpenZmax();
    }
    else
    {
      aBox.Add(ElCLib::Value(theU2, theParab));
      if (theU1 * theU2 < 0)
        aBox.Add(ElCLib::Value(0., theParab));
    }
  }
  aBox.Enlarge(theTol);
  return aBox;
}
