// Created on: 1993-01-11
// Created by: Christian CAILLET
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

#ifndef _IFSelect_SelectUnion_HeaderFile
#define _IFSelect_SelectUnion_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectCombine.hxx>
class Interface_EntityIterator;
class Interface_Graph;
class TCollection_AsciiString;

class IFSelect_SelectUnion;
DEFINE_STANDARD_HANDLE(IFSelect_SelectUnion, IFSelect_SelectCombine)

//! A SelectUnion cumulates the Entities issued from several other
//! Selections (union of results : "OR" operator)
class IFSelect_SelectUnion : public IFSelect_SelectCombine
{

public:
  //! Creates an empty SelectUnion
  Standard_EXPORT IFSelect_SelectUnion();

  //! Returns the list of selected Entities, which is the addition
  //! result from all input selections. Uniqueness is guaranteed.
  Standard_EXPORT Interface_EntityIterator
    RootResult(const Interface_Graph& G) const Standard_OVERRIDE;

  //! Returns a text defining the criterium : "Union (OR)"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectUnion, IFSelect_SelectCombine)

protected:
private:
};

#endif // _IFSelect_SelectUnion_HeaderFile
