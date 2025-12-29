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

// pmn -> modified 17/01/1996 : utilisation de Curve() et SetCurve()

#include <gp_Pnt2d.hxx>
#include <Law_Interpol.hxx>
#include <Law_Interpolate.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Law_Interpol, Law_BSpFunc)

#ifdef OCCT_DEBUG
#endif

//=================================================================================================

Law_Interpol::Law_Interpol() = default;

//=================================================================================================

void Law_Interpol::Set(const NCollection_Array1<gp_Pnt2d>& ParAndRad, const bool Periodic)
{
  int l   = ParAndRad.Lower();
  int nbp = ParAndRad.Length();

  occ::handle<NCollection_HArray1<double>> par = new NCollection_HArray1<double>(1, nbp);
  occ::handle<NCollection_HArray1<double>> rad;
  if (Periodic)
    rad = new NCollection_HArray1<double>(1, nbp - 1);
  else
    rad = new NCollection_HArray1<double>(1, nbp);
  double x, y;
  int    i;
  for (i = 1; i <= nbp; i++)
  {
    ParAndRad(l + i - 1).Coord(x, y);
    par->SetValue(i, x);
    if (!Periodic || i != nbp)
      rad->SetValue(i, y);
  }
  Law_Interpolate inter(rad, par, Periodic, Precision::Confusion());
  inter.Perform();
  SetCurve(inter.Curve());
#ifdef OCCT_DEBUG
#endif
}

//=================================================================================================

void Law_Interpol::SetInRelative(const NCollection_Array1<gp_Pnt2d>& ParAndRad,
                                 const double                        Ud,
                                 const double                        Uf,
                                 const bool                          Periodic)
{
  int                                      l = ParAndRad.Lower(), u = ParAndRad.Upper();
  double                                   wd = ParAndRad(l).X(), wf = ParAndRad(u).X();
  int                                      nbp = ParAndRad.Length();
  occ::handle<NCollection_HArray1<double>> par = new NCollection_HArray1<double>(1, nbp);
  occ::handle<NCollection_HArray1<double>> rad;
  if (Periodic)
    rad = new NCollection_HArray1<double>(1, nbp - 1);
  else
    rad = new NCollection_HArray1<double>(1, nbp);
  double x, y;
  int    i;
  for (i = 1; i <= nbp; i++)
  {
    ParAndRad(l + i - 1).Coord(x, y);
    par->SetValue(i, (Uf * (x - wd) + Ud * (wf - x)) / (wf - wd));
    if (!Periodic || i != nbp)
      rad->SetValue(i, y);
  }
  Law_Interpolate inter(rad, par, Periodic, Precision::Confusion());
  inter.Perform();
  SetCurve(inter.Curve());
#ifdef OCCT_DEBUG
#endif
}

//=================================================================================================

void Law_Interpol::Set(const NCollection_Array1<gp_Pnt2d>& ParAndRad,
                       const double                        Dd,
                       const double                        Df,
                       const bool                          Periodic)
{
  int l   = ParAndRad.Lower();
  int nbp = ParAndRad.Length();

  occ::handle<NCollection_HArray1<double>> par = new NCollection_HArray1<double>(1, nbp);
  occ::handle<NCollection_HArray1<double>> rad;
  if (Periodic)
    rad = new NCollection_HArray1<double>(1, nbp - 1);
  else
    rad = new NCollection_HArray1<double>(1, nbp);
  double x, y;
  int    i;
  for (i = 1; i <= nbp; i++)
  {
    ParAndRad(l + i - 1).Coord(x, y);
    par->SetValue(i, x);
    if (!Periodic || i != nbp)
      rad->SetValue(i, y);
  }
  Law_Interpolate inter(rad, par, Periodic, Precision::Confusion());
  inter.Load(Dd, Df);
  inter.Perform();
  SetCurve(inter.Curve());
}

//=================================================================================================

void Law_Interpol::SetInRelative(const NCollection_Array1<gp_Pnt2d>& ParAndRad,
                                 const double                        Ud,
                                 const double                        Uf,
                                 const double                        Dd,
                                 const double                        Df,
                                 const bool                          Periodic)
{
  int                                      l = ParAndRad.Lower(), u = ParAndRad.Upper();
  double                                   wd = ParAndRad(l).X(), wf = ParAndRad(u).X();
  int                                      nbp = ParAndRad.Length();
  occ::handle<NCollection_HArray1<double>> par = new NCollection_HArray1<double>(1, nbp);
  occ::handle<NCollection_HArray1<double>> rad;
  if (Periodic)
    rad = new NCollection_HArray1<double>(1, nbp - 1);
  else
    rad = new NCollection_HArray1<double>(1, nbp);
  double x, y;
  int    i;
  for (i = 1; i <= nbp; i++)
  {
    ParAndRad(l + i - 1).Coord(x, y);
    par->SetValue(i, (Uf * (x - wd) + Ud * (wf - x)) / (wf - wd));
    if (!Periodic || i != nbp)
      rad->SetValue(i, y);
  }
  Law_Interpolate inter(rad, par, Periodic, Precision::Confusion());
  inter.Load(Dd, Df);
  inter.Perform();
  SetCurve(inter.Curve());
}
