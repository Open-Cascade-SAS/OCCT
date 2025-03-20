// Created on: 1997-08-01
// Created by: Jean-Louis Frenkel
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

#ifndef _PCDM_HeaderFile
#define _PCDM_HeaderFile

#include <Storage_BaseDriver.hxx>
#include <PCDM_TypeOfFileDriver.hxx>

class TCollection_AsciiString;

class PCDM
{
public:
  Standard_EXPORT static PCDM_TypeOfFileDriver FileDriverType(
    const TCollection_AsciiString& aFileName,
    Handle(Storage_BaseDriver)&    aBaseDriver);

  Standard_EXPORT static PCDM_TypeOfFileDriver FileDriverType(
    Standard_IStream&           theIStream,
    Handle(Storage_BaseDriver)& theBaseDriver);

  DEFINE_STANDARD_ALLOC
};

#endif // _PCDM_HeaderFile
