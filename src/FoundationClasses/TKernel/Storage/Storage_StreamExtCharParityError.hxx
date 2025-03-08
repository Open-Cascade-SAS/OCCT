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

#ifndef _Storage_StreamExtCharParityError_HeaderFile
#define _Storage_StreamExtCharParityError_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_DefineException.hxx>
#include <Standard_SStream.hxx>
#include <Storage_StreamReadError.hxx>

class Storage_StreamExtCharParityError;
DEFINE_STANDARD_HANDLE(Storage_StreamExtCharParityError, Storage_StreamReadError)

#if !defined No_Exception && !defined No_Storage_StreamExtCharParityError
  #define Storage_StreamExtCharParityError_Raise_if(CONDITION, MESSAGE)                            \
    if (CONDITION)                                                                                 \
      throw Storage_StreamExtCharParityError(MESSAGE);
#else
  #define Storage_StreamExtCharParityError_Raise_if(CONDITION, MESSAGE)
#endif

DEFINE_STANDARD_EXCEPTION(Storage_StreamExtCharParityError, Storage_StreamReadError)

#endif // _Storage_StreamExtCharParityError_HeaderFile
