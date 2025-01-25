// Created on: 2007-03-30
// Created by: Michael SAZONOV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#ifndef XmlTObjDrivers_IntSparseArrayDriver_HeaderFile
#define XmlTObjDrivers_IntSparseArrayDriver_HeaderFile

#include <XmlMDF_ADriver.hxx>

class XmlTObjDrivers_IntSparseArrayDriver : public XmlMDF_ADriver
{

public:
  Standard_EXPORT XmlTObjDrivers_IntSparseArrayDriver(
    const Handle(Message_Messenger)& theMessageDriver);
  // constructor

  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  // Creates a new attribute

  Standard_EXPORT Standard_Boolean
    Paste(const XmlObjMgt_Persistent&  theSource,
          const Handle(TDF_Attribute)& theTarget,
          XmlObjMgt_RRelocationTable&  theRelocTable) const Standard_OVERRIDE;
  // Translate the contents of <theSource> and put it
  // into <theTarget>

  Standard_EXPORT void Paste(const Handle(TDF_Attribute)& theSource,
                             XmlObjMgt_Persistent&        theTarget,
                             XmlObjMgt_SRelocationTable&  theRelocTable) const Standard_OVERRIDE;
  // Translate the contents of <aSource> and put it
  // into <aTarget>

public:
  // CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(XmlTObjDrivers_IntSparseArrayDriver, XmlMDF_ADriver)
};

// Define handle class
DEFINE_STANDARD_HANDLE(XmlTObjDrivers_IntSparseArrayDriver, XmlMDF_ADriver)

#endif

#ifdef _MSC_VER
#pragma once
#endif
