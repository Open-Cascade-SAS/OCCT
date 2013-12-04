// Created on: 1997-03-04
// Created by: Mister rmi
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <FWOSDriver.ixx>
#include <FWOSDriver_DriverFactory.hxx>

#include <Plugin_Macro.hxx>

#ifdef _MSC_VER
#pragma warning(disable:4190) /* disable warning on C++ type returned by C function; should be OK for C++ usage */
#endif

PLUGIN(FWOSDriver)

Handle(Standard_Transient) FWOSDriver::Factory(const Standard_GUID& /*aGUID*/) {
  static Handle(FWOSDriver_DriverFactory) f;
  if(f.IsNull()) f = new FWOSDriver_DriverFactory;
  return f;
}
