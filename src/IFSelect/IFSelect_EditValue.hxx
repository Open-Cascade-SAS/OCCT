// Created on: 1992-09-21
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IFSelect_EditValue_HeaderFile
#define _IFSelect_EditValue_HeaderFile

//! Controls access on Values by an Editor
//! EditOptional  : normal access, in addition may be removed
//! Editable      : normal access, must be present
//! EditProtected : access must be validated
//! EditComputed  : why write it ?  it will be recomputed
//! EditRead      : no way to write it, only for read
//! EditDynamic   : not a field, only to be displayed
enum IFSelect_EditValue
{
IFSelect_Optional,
IFSelect_Editable,
IFSelect_EditProtected,
IFSelect_EditComputed,
IFSelect_EditRead,
IFSelect_EditDynamic
};

#endif // _IFSelect_EditValue_HeaderFile
