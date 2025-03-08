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

#ifndef _IFSelect_SelectIntersection_HeaderFile
#define _IFSelect_SelectIntersection_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectCombine.hxx>
class Interface_EntityIterator;
class Interface_Graph;
class TCollection_AsciiString;

class IFSelect_SelectIntersection;
DEFINE_STANDARD_HANDLE(IFSelect_SelectIntersection, IFSelect_SelectCombine)

//! A SelectIntersection filters the Entities issued from several
//! other Selections as Intersection of results : "AND" operator
class IFSelect_SelectIntersection : public IFSelect_SelectCombine
{

public:
  //! Creates an empty SelectIntersection
  Standard_EXPORT IFSelect_SelectIntersection();

  //! Returns the list of selected Entities, which is the common part
  //! of results from all input selections. Uniqueness is guaranteed.
  Standard_EXPORT Interface_EntityIterator
    RootResult(const Interface_Graph& G) const Standard_OVERRIDE;

  //! Returns a text defining the criterium : "Intersection (AND)"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectIntersection, IFSelect_SelectCombine)

protected:
private:
};

#endif // _IFSelect_SelectIntersection_HeaderFile
