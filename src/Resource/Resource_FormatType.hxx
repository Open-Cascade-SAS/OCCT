// Created on: 1995-04-20
// Created by: Tony GEORGIADES
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

#ifndef _Resource_FormatType_HeaderFile
#define _Resource_FormatType_HeaderFile


//! List of non ASCII format types which may be
//! converted into the Unicode 16 bits format type.
//! Use the functions provided by the
//! Resource_Unicode class to convert a string
//! from one of these non ASCII format to Unicode, and vice versa.
enum Resource_FormatType
{
Resource_SJIS,
Resource_EUC,
Resource_ANSI,
Resource_GB
};

#endif // _Resource_FormatType_HeaderFile
