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

#ifndef _IFSelect_SelectBase_HeaderFile
#define _IFSelect_SelectBase_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_Selection.hxx>
class IFSelect_SelectionIterator;

class IFSelect_SelectBase;
DEFINE_STANDARD_HANDLE(IFSelect_SelectBase, IFSelect_Selection)

//! SelectBase works directly from an InterfaceModel : it is the
//! first base for other Selections.
class IFSelect_SelectBase : public IFSelect_Selection
{

public:
  //! Puts in an Iterator the Selections from which "me" depends
  //! This list is empty for all SelectBase type Selections
  Standard_EXPORT void FillIterator(IFSelect_SelectionIterator& iter) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectBase, IFSelect_Selection)

protected:
private:
};

#endif // _IFSelect_SelectBase_HeaderFile
