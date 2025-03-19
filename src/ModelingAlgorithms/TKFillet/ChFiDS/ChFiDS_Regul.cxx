// Created on: 1995-03-21
// Created by: Laurent BOURESCHE
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

#include <ChFiDS_Regul.hxx>

//=================================================================================================

ChFiDS_Regul::ChFiDS_Regul()
    : icurv(0),
      is1(0),
      is2(0)
{
}

//=================================================================================================

void ChFiDS_Regul::SetCurve(const Standard_Integer IC)
{
  icurv = Abs(IC);
}

//=================================================================================================

void ChFiDS_Regul::SetS1(const Standard_Integer IS1, const Standard_Boolean IsFace)
{
  if (IsFace)
    is1 = Abs(IS1);
  else
    is1 = -Abs(IS1);
}

//=================================================================================================

void ChFiDS_Regul::SetS2(const Standard_Integer IS2, const Standard_Boolean IsFace)
{
  if (IsFace)
    is2 = Abs(IS2);
  else
    is2 = -Abs(IS2);
}

//=================================================================================================

Standard_Boolean ChFiDS_Regul::IsSurface1() const
{
  return (is1 < 0);
}

//=================================================================================================

Standard_Boolean ChFiDS_Regul::IsSurface2() const
{
  return (is2 < 0);
}

//=================================================================================================

Standard_Integer ChFiDS_Regul::Curve() const
{
  return icurv;
}

//=================================================================================================

Standard_Integer ChFiDS_Regul::S1() const
{
  return Abs(is1);
}

//=================================================================================================

Standard_Integer ChFiDS_Regul::S2() const
{
  return Abs(is2);
}
