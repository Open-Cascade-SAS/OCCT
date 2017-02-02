// Created on: 1993-12-03
// Created by: Christophe MARION
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _BRepExtrema_UnCompatibleShape_HeaderFile
#define _BRepExtrema_UnCompatibleShape_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_DefineException.hxx>
#include <Standard_SStream.hxx>
#include <Standard_DomainError.hxx>

class BRepExtrema_UnCompatibleShape;
DEFINE_STANDARD_HANDLE(BRepExtrema_UnCompatibleShape, Standard_DomainError)

#if !defined No_Exception && !defined No_BRepExtrema_UnCompatibleShape
  #define BRepExtrema_UnCompatibleShape_Raise_if(CONDITION, MESSAGE) \
  if (CONDITION) throw BRepExtrema_UnCompatibleShape(MESSAGE);
#else
  #define BRepExtrema_UnCompatibleShape_Raise_if(CONDITION, MESSAGE)
#endif

DEFINE_STANDARD_EXCEPTION(BRepExtrema_UnCompatibleShape, Standard_DomainError)

#endif // _BRepExtrema_UnCompatibleShape_HeaderFile
