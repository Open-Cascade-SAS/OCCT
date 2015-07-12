// Created on: 1995-03-08
// Created by: Mister rmi
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

#ifndef _StdSelect_SensitivityMode_HeaderFile
#define _StdSelect_SensitivityMode_HeaderFile

//! Selection sensitivity mode. SM_WINDOW mode uses the
//! specified pixel tolerance to compute the sensitivity value,
//! SM_VIEW mode allows to define the sensitivity manually.
enum StdSelect_SensitivityMode
{
StdSelect_SM_WINDOW,
StdSelect_SM_VIEW
};

#endif // _StdSelect_SensitivityMode_HeaderFile
