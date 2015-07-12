// Created on: 1996-07-24
// Created by: Herve LOUESSARD
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

#ifndef _LocalAnalysis_StatusErrorType_HeaderFile
#define _LocalAnalysis_StatusErrorType_HeaderFile


enum LocalAnalysis_StatusErrorType
{
LocalAnalysis_NullFirstDerivative,
LocalAnalysis_NullSecondDerivative,
LocalAnalysis_TangentNotDefined,
LocalAnalysis_NormalNotDefined,
LocalAnalysis_CurvatureNotDefined
};

#endif // _LocalAnalysis_StatusErrorType_HeaderFile
