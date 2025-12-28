// Created on: 1995-08-23
// Created by: Xavier BENVENISTE
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

#ifndef Convert_CosAndSinEvalFunction_HeaderFile
#define Convert_CosAndSinEvalFunction_HeaderFile

#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>

typedef void Convert_CosAndSinEvalFunction(double,
                                           const int,
                                           const NCollection_Array1<gp_Pnt2d>&,
                                           const NCollection_Array1<double>&,
                                           const NCollection_Array1<int>*,
                                           double Result[2]);

#endif
