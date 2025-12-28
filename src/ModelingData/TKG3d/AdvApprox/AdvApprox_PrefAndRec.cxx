// Created on: 1996-11-14
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
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

#include <AdvApprox_PrefAndRec.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>

AdvApprox_PrefAndRec::AdvApprox_PrefAndRec(const NCollection_Array1<double>& RecCut,
                                           const NCollection_Array1<double>& PrefCut,
                                           const double         Weight)
    : myRecCutting(1, RecCut.Length()),
      myPrefCutting(1, PrefCut.Length()),
      myWeight(Weight)
{
  myRecCutting  = RecCut;
  myPrefCutting = PrefCut;
  if (myWeight <= 1)
  {
    throw Standard_DomainError("PrefAndRec : Weight is too small");
  }
}

bool AdvApprox_PrefAndRec::Value(const double a,
                                             const double b,
                                             double&      cuttingvalue) const
{
  //  longueur minimum d'un intervalle parametrique : 10*PConfusion()
  constexpr double lgmin = 10 * Precision::PConfusion();
  int        i;
  double           cut, mil = (a + b) / 2, dist;
  bool        isfound = false;

  cut = mil;

  // Recheche d'une decoupe preferentiel
  dist = std::abs((a * myWeight + b) / (1 + myWeight) - mil);
  for (i = 1; i <= myPrefCutting.Length(); i++)
  {
    if (dist > std::abs(mil - myPrefCutting.Value(i)))
    {
      cut     = myPrefCutting.Value(i);
      dist    = std::abs(mil - cut);
      isfound = true;
    }
  }

  // Recheche d'une decoupe recommende
  if (!isfound)
  {
    dist = std::abs((a - b) / 2);
    for (i = 1; i <= myRecCutting.Length(); i++)
    {
      if ((dist - lgmin) > std::abs(mil - myRecCutting.Value(i)))
      {
        cut  = myRecCutting.Value(i);
        dist = std::abs(mil - cut);
      }
    }
  }

  // Resultat
  cuttingvalue = cut;
  return (std::abs(cut - a) >= lgmin && std::abs(b - cut) >= lgmin);
}
