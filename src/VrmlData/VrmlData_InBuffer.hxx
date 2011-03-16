// File:      VrmlData_InBuffer.hxx
// Created:   08.10.06 23:19
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2006


#ifndef VrmlData_InBuffer_HeaderFile
#define VrmlData_InBuffer_HeaderFile

#include <Standard_IStream.hxx>
#include <Standard_Boolean.hxx>

/**
 * Structure passed to the methods dealing with input stream.
 */
struct VrmlData_InBuffer {
  Standard_IStream& Input;
  char              Line[8096];
  char *            LinePtr;
  Standard_Boolean  IsProcessed;
  Standard_Integer  LineCount;
  VrmlData_InBuffer (Standard_IStream& theStream)
    : Input       (theStream),
      LinePtr     (&Line[0]),
      IsProcessed (Standard_False),
      LineCount   (0) {}
};

#endif
