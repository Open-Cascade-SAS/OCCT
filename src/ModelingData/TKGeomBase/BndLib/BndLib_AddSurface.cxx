// Created on: 1995-07-24
// Created by: Modelistation
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

#include <Adaptor3d_Surface.hxx>
#include <Bnd_Box.hxx>
#include <BndLib_AddSurface.hxx>
#include "BndLib_LegacyCheck.hxx"
#include <GeomBndLib_Surface.hxx>

//=================================================================================================

void BndLib_AddSurface::Add(const Adaptor3d_Surface& S, const double Tol, Bnd_Box& B)
{
  BndLib_AddSurface::Add(S,
                         S.FirstUParameter(),
                         S.LastUParameter(),
                         S.FirstVParameter(),
                         S.LastVParameter(),
                         Tol,
                         B);
}

//=================================================================================================

void BndLib_AddSurface::Add(const Adaptor3d_Surface& S,
                            const double             UMin,
                            const double             UMax,
                            const double             VMin,
                            const double             VMax,
                            const double             Tol,
                            Bnd_Box&                 B)
{
  Bnd_Box aNewBox;
  GeomBndLib_Surface(S).Add(UMin, UMax, VMin, VMax, Tol, aNewBox);
  BndLib_LegacyCheck::CompareSurfaceAdd(S, UMin, UMax, VMin, VMax, Tol, aNewBox);
  B.Add(aNewBox);
}

//=================================================================================================

void BndLib_AddSurface::AddOptimal(const Adaptor3d_Surface& S, const double Tol, Bnd_Box& B)
{
  BndLib_AddSurface::AddOptimal(S,
                                S.FirstUParameter(),
                                S.LastUParameter(),
                                S.FirstVParameter(),
                                S.LastVParameter(),
                                Tol,
                                B);
}

//=================================================================================================

void BndLib_AddSurface::AddOptimal(const Adaptor3d_Surface& S,
                                   const double             UMin,
                                   const double             UMax,
                                   const double             VMin,
                                   const double             VMax,
                                   const double             Tol,
                                   Bnd_Box&                 B)
{
  Bnd_Box aNewBox;
  GeomBndLib_Surface(S).AddOptimal(UMin, UMax, VMin, VMax, Tol, aNewBox);
  BndLib_LegacyCheck::CompareSurfaceAddOptimal(S, UMin, UMax, VMin, VMax, Tol, aNewBox);
  B.Add(aNewBox);
}
