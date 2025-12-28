// Created on: 1996-01-22
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

inline int FairCurve_Energy::NbVariables() const
{
  return MyNbVar;
}

inline const occ::handle<NCollection_HArray1<gp_Pnt2d>>& FairCurve_Energy::Poles() const
{
  return MyPoles;
}

inline int FairCurve_Energy::Indice(const int i, const int j) const
{
  return MyNbValues + j + i * (i - 1) / 2;
}
