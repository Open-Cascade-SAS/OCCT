// Created on: 1997-03-04
// Created by: Mister rmi
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

#ifndef _Plugin_Macro_HeaderFile
#define _Plugin_Macro_HeaderFile

#ifdef OCCT_NO_PLUGINS
  #define PLUGIN(name)
#else
//! Macro implementing C-style interface function to get factory object from the dynamically loaded library
#define PLUGIN(name) \
extern "C" Standard_EXPORT Standard_Transient* PLUGINFACTORY(const Standard_GUID& aGUID) { \
   return const_cast<Standard_Transient*>(name::Factory(aGUID).get()); \
}
#endif

#endif
