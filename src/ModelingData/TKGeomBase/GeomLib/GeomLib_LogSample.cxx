// Created on: 1998-09-23
// Created by: Philippe MANGIN
// Copyright (c) 1998-1999 Matra Datavision
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

#include <GeomLib_LogSample.hxx>
#include <Standard_OutOfRange.hxx>

GeomLib_LogSample::GeomLib_LogSample(const double A, const double B, const int N)
    : math_FunctionSample(A, B, N)
{
  myF   = A - 1;
  myexp = std::log(B - A) / N;
}

double GeomLib_LogSample::GetParameter(const int Index) const
{
  int aN = NbPoints();

  if ((Index >= aN) || (Index <= 1))
  {
    double aA, aB;
    Bounds(aA, aB);
    if (Index == 1)
      return aA;
    else if (Index == aN)
      return aB;
    else
      throw Standard_OutOfRange("GeomLib_LogSample::GetParameter");
  }

  double v = myF + std::exp(myexp * Index);
  return v;
}
