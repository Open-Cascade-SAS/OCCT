// Created on: 1993-02-25
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepBuild_BlockIterator_HeaderFile
#define _TopOpeBRepBuild_BlockIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>

//! Iterator on the elements of a block.
class TopOpeBRepBuild_BlockIterator
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepBuild_BlockIterator();

  Standard_EXPORT TopOpeBRepBuild_BlockIterator(const Standard_Integer Lower,
                                                const Standard_Integer Upper);

  void Initialize();

  Standard_Boolean More() const;

  void Next();

  Standard_Integer Value() const;

  Standard_Integer Extent() const;

protected:
private:
  Standard_Integer myLower;
  Standard_Integer myUpper;
  Standard_Integer myValue;
};

#include <TopOpeBRepBuild_BlockIterator.lxx>

#endif // _TopOpeBRepBuild_BlockIterator_HeaderFile
