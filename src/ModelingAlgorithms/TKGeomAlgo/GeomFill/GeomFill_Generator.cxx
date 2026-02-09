// Created on: 1994-02-25
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
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

#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Surface.hxx>
#include <GeomFill_Generator.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard_Integer.hxx>

//=================================================================================================

GeomFill_Generator::GeomFill_Generator() = default;

//=================================================================================================

void GeomFill_Generator::Perform(const double PTol)
{
  // Perform the profile of the sections.
  GeomFill_Profiler::Perform(PTol);

  // Create the surface.

  int  i, j;
  int  NbUPoles    = NbPoles();
  int  NbVPoles    = mySequence.Length();
  int  NbUKnots    = NbKnots();
  int  NbVKnots    = NbVPoles;
  bool isUPeriodic = IsPeriodic();
  bool isVPeriodic = false;

  NCollection_Array2<gp_Pnt> Poles(1, NbUPoles, 1, NbVPoles);
  NCollection_Array2<double> Weights(1, NbUPoles, 1, NbVPoles);
  NCollection_Array1<double> UKnots(1, NbUKnots);
  NCollection_Array1<double> VKnots(1, NbVKnots);
  NCollection_Array1<int>    UMults(1, NbUKnots);
  NCollection_Array1<int>    VMults(1, NbVKnots);
  VMults.Init(1);

  VMults(1) = VMults(NbVKnots) = 2;

  KnotsAndMults(UKnots, UMults);

  for (j = 1; j <= NbVPoles; j++)
  {
    occ::handle<Geom_BSplineCurve>    Cj       = occ::down_cast<Geom_BSplineCurve>(mySequence(j));
    const NCollection_Array1<gp_Pnt>& aPoles   = Cj->Poles();
    const NCollection_Array1<double>& aWeights = Cj->WeightsArray();
    VKnots(j) = (double)(j - 1);
    for (i = 1; i <= NbUPoles; i++)
    {
      Poles(i, j)   = aPoles(i);
      Weights(i, j) = aWeights(i);
    }
  }

  mySurface = new Geom_BSplineSurface(Poles,
                                      Weights,
                                      UKnots,
                                      VKnots,
                                      UMults,
                                      VMults,
                                      Degree(),
                                      1,
                                      isUPeriodic,
                                      isVPeriodic);
}
