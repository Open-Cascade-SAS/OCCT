// Created on: 1995-03-14
// Created by: Jacques GOUSSARD
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

//=================================================================================================

inline GeomAPI_IntSS::GeomAPI_IntSS() = default;

//=================================================================================================

inline GeomAPI_IntSS::GeomAPI_IntSS(const occ::handle<Geom_Surface>& S1,
                                    const occ::handle<Geom_Surface>& S2,
                                    const double                     Tol)
    : myIntersec(S1, S2, Tol, true)
{
}

//=================================================================================================

inline void GeomAPI_IntSS::Perform(const occ::handle<Geom_Surface>& S1,
                                   const occ::handle<Geom_Surface>& S2,
                                   const double                     Tol)
{
  myIntersec.Perform(S1, S2, Tol, true);
}

//=================================================================================================

inline bool GeomAPI_IntSS::IsDone() const
{
  return myIntersec.IsDone();
}

//=================================================================================================

inline int GeomAPI_IntSS::NbLines() const
{
  return myIntersec.NbLines();
}

//=================================================================================================

inline const occ::handle<Geom_Curve>& GeomAPI_IntSS::Line(const int I) const
{
  return myIntersec.Line(I);
}
