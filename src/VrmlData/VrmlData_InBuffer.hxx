// Created on: 2006-10-08
// Created by: Alexander GRIGORIEV
// Copyright (c) 2006-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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
