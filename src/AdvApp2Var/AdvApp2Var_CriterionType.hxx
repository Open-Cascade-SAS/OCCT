// Created on: 1996-03-05
// Created by: Joelle CHAUVET
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

#ifndef _AdvApp2Var_CriterionType_HeaderFile
#define _AdvApp2Var_CriterionType_HeaderFile

//! influency of the criterion on cutting process//! cutting when criterion is not satisfied
//! desactivation of the compute of the error max//! cutting when error max is not good or if error
//! max is good and criterion is not satisfied
enum AdvApp2Var_CriterionType
{
AdvApp2Var_Absolute,
AdvApp2Var_Relative
};

#endif // _AdvApp2Var_CriterionType_HeaderFile
