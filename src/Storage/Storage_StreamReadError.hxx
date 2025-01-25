// Created on: 1996-04-30
// Created by: cle
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

#ifndef _Storage_StreamReadError_HeaderFile
#define _Storage_StreamReadError_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_DefineException.hxx>
#include <Standard_SStream.hxx>
#include <Standard_Failure.hxx>

class Storage_StreamReadError;
DEFINE_STANDARD_HANDLE(Storage_StreamReadError, Standard_Failure)

#if !defined No_Exception && !defined No_Storage_StreamReadError
  #define Storage_StreamReadError_Raise_if(CONDITION, MESSAGE)                                     \
    if (CONDITION)                                                                                 \
      throw Storage_StreamReadError(MESSAGE);
#else
  #define Storage_StreamReadError_Raise_if(CONDITION, MESSAGE)
#endif

DEFINE_STANDARD_EXCEPTION(Storage_StreamReadError, Standard_Failure)

#endif // _Storage_StreamReadError_HeaderFile
