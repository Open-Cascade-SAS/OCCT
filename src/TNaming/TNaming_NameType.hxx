// Created on: 1997-03-17
// Created by: Yves FRICAUD
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _TNaming_NameType_HeaderFile
#define _TNaming_NameType_HeaderFile

//! to store naming characteristcs
enum TNaming_NameType
{
TNaming_UNKNOWN,
TNaming_IDENTITY,
TNaming_MODIFUNTIL,
TNaming_GENERATION,
TNaming_INTERSECTION,
TNaming_UNION,
TNaming_SUBSTRACTION,
TNaming_CONSTSHAPE,
TNaming_FILTERBYNEIGHBOURGS,
TNaming_ORIENTATION,
TNaming_WIREIN,
TNaming_SHELLIN
};

#endif // _TNaming_NameType_HeaderFile
