// Created on: 2006-10-08
// Created by: Alexander GRIGORIEV
// Copyright (c) 2006-2014 OPEN CASCADE SAS
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

#ifndef VrmlData_InBuffer_HeaderFile
#define VrmlData_InBuffer_HeaderFile

#include <Standard_IStream.hxx>

/**
 * Structure passed to the methods dealing with input stream.
 */
struct VrmlData_InBuffer
{
  Standard_IStream& Input;
  char              Line[8096];
  char*             LinePtr;
  Standard_Boolean  IsProcessed;
  Standard_Integer  LineCount;
  VrmlData_InBuffer(Standard_IStream& theStream)
      : Input(theStream),
        LinePtr(&Line[0]),
        IsProcessed(Standard_False),
        LineCount(0) {};

private:
  void operator=(const VrmlData_InBuffer&);
};

#endif
