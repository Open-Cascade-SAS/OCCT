// Created on: 1992-02-03
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

#ifndef _Interface_GlobalNodeOfReaderLib_HeaderFile
#define _Interface_GlobalNodeOfReaderLib_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class Interface_ReaderModule;
class Interface_Protocol;
class Standard_Transient;
class Interface_ReaderLib;
class Interface_NodeOfReaderLib;

class Interface_GlobalNodeOfReaderLib;
DEFINE_STANDARD_HANDLE(Interface_GlobalNodeOfReaderLib, Standard_Transient)

class Interface_GlobalNodeOfReaderLib : public Standard_Transient
{

public:
  //! Creates an empty GlobalNode, with no Next
  Standard_EXPORT Interface_GlobalNodeOfReaderLib();

  //! Adds a Module bound with a Protocol to the list : does
  //! nothing if already in the list, THAT IS, Same Type (exact
  //! match) and Same State (that is, IsEqual is not required)
  //! Once added, stores its attached Protocol in correspondence
  Standard_EXPORT void Add(const Handle(Interface_ReaderModule)& amodule,
                           const Handle(Interface_Protocol)&     aprotocol);

  //! Returns the Module stored in a given GlobalNode
  Standard_EXPORT const Handle(Interface_ReaderModule)& Module() const;

  //! Returns the attached Protocol stored in a given GlobalNode
  Standard_EXPORT const Handle(Interface_Protocol)& Protocol() const;

  //! Returns the Next GlobalNode. If none is defined, returned
  //! value is a Null Handle
  Standard_EXPORT const Handle(Interface_GlobalNodeOfReaderLib)& Next() const;

  DEFINE_STANDARD_RTTI_INLINE(Interface_GlobalNodeOfReaderLib, Standard_Transient)

protected:
private:
  Handle(Interface_ReaderModule)          themod;
  Handle(Interface_Protocol)              theprot;
  Handle(Interface_GlobalNodeOfReaderLib) thenext;
};

#endif // _Interface_GlobalNodeOfReaderLib_HeaderFile
