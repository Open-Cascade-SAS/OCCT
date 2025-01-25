// Created on: 1992-11-17
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

#ifndef _IFSelect_SelectModelEntities_HeaderFile
#define _IFSelect_SelectModelEntities_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectBase.hxx>
class Interface_EntityIterator;
class Interface_Graph;
class TCollection_AsciiString;

class IFSelect_SelectModelEntities;
DEFINE_STANDARD_HANDLE(IFSelect_SelectModelEntities, IFSelect_SelectBase)

//! A SelectModelEntities gets all the Entities of an
//! InterfaceModel.
class IFSelect_SelectModelEntities : public IFSelect_SelectBase
{

public:
  //! Creates a SelectModelRoot
  Standard_EXPORT IFSelect_SelectModelEntities();

  //! Returns the list of selected entities : the Entities of the
  //! Model (note that this result assures naturally uniqueness)
  Standard_EXPORT Interface_EntityIterator
    RootResult(const Interface_Graph& G) const Standard_OVERRIDE;

  //! The complete list of Entities (including shared ones) ...
  //! is exactly identical to RootResults in this case
  Standard_EXPORT virtual Interface_EntityIterator CompleteResult(const Interface_Graph& G) const
    Standard_OVERRIDE;

  //! Returns a text defining the criterium : "Model Entities"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectModelEntities, IFSelect_SelectBase)

protected:
private:
};

#endif // _IFSelect_SelectModelEntities_HeaderFile
