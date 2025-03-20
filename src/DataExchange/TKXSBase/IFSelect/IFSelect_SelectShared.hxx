// Created on: 1992-11-18
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IFSelect_SelectShared_HeaderFile
#define _IFSelect_SelectShared_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectDeduct.hxx>
class Interface_EntityIterator;
class Interface_Graph;
class TCollection_AsciiString;

class IFSelect_SelectShared;
DEFINE_STANDARD_HANDLE(IFSelect_SelectShared, IFSelect_SelectDeduct)

//! A SelectShared selects Entities which are directly Shared
//! by the Entities of the Input list
class IFSelect_SelectShared : public IFSelect_SelectDeduct
{

public:
  //! Creates a SelectShared;
  Standard_EXPORT IFSelect_SelectShared();

  //! Returns the list of selected entities (list of entities
  //! shared by those of input list)
  Standard_EXPORT Interface_EntityIterator
    RootResult(const Interface_Graph& G) const Standard_OVERRIDE;

  //! Returns a text defining the criterium : "Shared (one level)"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectShared, IFSelect_SelectDeduct)

protected:
private:
};

#endif // _IFSelect_SelectShared_HeaderFile
