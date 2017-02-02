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

#ifndef _Resource_NoSuchResource_HeaderFile
#define _Resource_NoSuchResource_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_DefineException.hxx>
#include <Standard_SStream.hxx>
#include <Standard_NoSuchObject.hxx>

class Resource_NoSuchResource;
DEFINE_STANDARD_HANDLE(Resource_NoSuchResource, Standard_NoSuchObject)

#if !defined No_Exception && !defined No_Resource_NoSuchResource
  #define Resource_NoSuchResource_Raise_if(CONDITION, MESSAGE) \
  if (CONDITION) throw Resource_NoSuchResource(MESSAGE);
#else
  #define Resource_NoSuchResource_Raise_if(CONDITION, MESSAGE)
#endif

DEFINE_STANDARD_EXCEPTION(Resource_NoSuchResource, Standard_NoSuchObject)

#endif // _Resource_NoSuchResource_HeaderFile
