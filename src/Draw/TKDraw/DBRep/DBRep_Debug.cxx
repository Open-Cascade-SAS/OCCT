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

#include <DBRep.hxx>
#include <Standard_Failure.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TopTools_Array1OfShape.hxx>
#include <NCollection_Vector.hxx>
#include <BRep_Builder.hxx>

// This file defines global functions not declared in any public header,
// intended for use from debugger prompt (Command Window in Visual Studio)

//! Save shape identified by pointer
Standard_EXPORT const char* DBRep_Set(const char* theNameStr, void* theShapePtr)
{
  if (theNameStr == 0 || theShapePtr == 0)
  {
    return "Error: name or shape is null";
  }
  try
  {
    DBRep::Set(theNameStr, *(TopoDS_Shape*)theShapePtr);
    return theNameStr;
  }
  catch (Standard_Failure const& anException)
  {
    return anException.GetMessageString();
  }
}

//=======================================================================
// function : fromContainer
// purpose  : static function to copy shapes from container into compound
//=======================================================================
template <class T>
static Standard_Boolean fromContainer(void* theContainer, TopoDS_Compound& theShape)
{
  try
  {
    T* pContainer = (T*)theContainer;
    for (typename T::Iterator it(*pContainer); it.More(); it.Next())
    {
      BRep_Builder().Add(theShape, it.Value());
    }
    return true;
  }
  catch (Standard_Failure const&)
  {
    return false;
  }
}

//=======================================================================
// function : DBRep_SetComp
// purpose  : make compound from the given container of shapes
//=======================================================================
Standard_EXPORT const char* DBRep_SetComp(const char* theNameStr, void* theListPtr)
{
  if (theNameStr == 0 || theListPtr == 0)
  {
    return "Error: name or list of shapes is null";
  }

  TopoDS_Compound aC;
  BRep_Builder().MakeCompound(aC);

  if (fromContainer<TopTools_ListOfShape>(theListPtr, aC)
      || fromContainer<TopTools_MapOfShape>(theListPtr, aC)
      || fromContainer<TopTools_IndexedMapOfShape>(theListPtr, aC)
      || fromContainer<TopTools_SequenceOfShape>(theListPtr, aC)
      || fromContainer<TopTools_Array1OfShape>(theListPtr, aC)
      || fromContainer<NCollection_Vector<TopoDS_Shape>>(theListPtr, aC))
  {
    DBRep::Set(theNameStr, aC);
    return theNameStr;
  }
  else
  {
    return "Error: Invalid type";
  }
}

// MSVC debugger cannot deal correctly with functions whose argunments
// have non-standard types. Here we define alternative to the above functions
// with good types with the hope that GDB on Linux or other debugger could
// work with them (DBX could, on SUN Solaris).
#ifndef _MSC_VER

const char* DBRep_Set(char* theName, const TopoDS_Shape& theShape)
{
  return DBRep_Set(theName, (void*)&theShape);
}

#endif /* _MSC_VER */

// old function, perhaps too dangerous to be used
/*
void DBRep_Get(char* name, TopoDS_Shape& S)
{
  char n[255];
  strcpy(n,name);
  Standard_CString cs = (Standard_CString)n;
  S = DBRep::Get(cs);
  if (*name == '.')
    std::cout << "Name : " << n << std::endl;
}
*/
