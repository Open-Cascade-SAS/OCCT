// Created on: 1994-07-25
// Created by: Remi LEQUETTE
// Copyright (c) 1994-1999 Matra Datavision
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

#include <BRepTools.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TopTools_LocationSet.hxx>

// This file defines global functions not declared in any public header,
// intended for use from debugger prompt (Command Window in Visual Studio)

//! Save shape to file
const char* BRepTools_Write (const char* theFileStr, void* theShapePtr)
{
  if (theFileStr == 0 || theShapePtr == 0)
  {
    return "Error: name or shape is null";
  }
  try {
    OCC_CATCH_SIGNALS
    if (BRepTools::Write (*(TopoDS_Shape*)theShapePtr, theFileStr))
      return theFileStr;
    else
      return "Error: write failed";
  }
  catch (Standard_Failure const& anException)
  {
    return anException.GetMessageString();
  }
}

//! Dump shape to cout
const char* BRepTools_Dump (void* theShapePtr)
{
  if (theShapePtr == 0)
  {
    return "Error: name or shape is null";
  }
  try {
    OCC_CATCH_SIGNALS

    cout <<"\n\n";
    BRepTools::Dump (*(TopoDS_Shape*)theShapePtr, cout);
    cout << endl;

    return "Shape dumped to cout";
  }
  catch (Standard_Failure const& anException)
  {
    return anException.GetMessageString();
  }
}

//! Dump shape location to cout
const char* BRepTools_DumpLoc (void* theLocationPtr)
{
  if (theLocationPtr == 0)
  {
    return "Error: name or shape is null";
  }
  try {
    OCC_CATCH_SIGNALS

      cout <<"\n\n";
    TopTools_LocationSet LS;
    LS.Add(*(TopLoc_Location*)theLocationPtr);
    LS.Dump(cout);
    cout <<endl;

    return "Location dumped to cout";
  }
  catch (Standard_Failure const& anException)
  {
    return anException.GetMessageString();
  }
}

// MSVC debugger cannot deal correctly with functions whose argunments 
// have non-standard types. Here we define alternative to the above functions
// with good types with the hope that GDB on Linux or other debugger could
// work with them (DBX could, on SUN Solaris).
#ifndef _MSC_VER

const char* BRepTools_Write (const char* theFileNameStr, const TopoDS_Shape& theShape)
{
  return BRepTools_Write (theFileNameStr, (void*)&theShape);
}

const char* BRepTools_Dump (const TopoDS_Shape& theShape)
{
  return BRepTools_Dump ((void*)&theShape);
}

const char* BRepTools_DumpLoc (const TopoDS_Shape& theShape)
{
  return BRepTools_DumpLoc ((void*)&theShape);
}

#endif
