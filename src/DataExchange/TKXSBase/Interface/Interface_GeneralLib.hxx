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

#ifndef _Interface_GeneralLib_HeaderFile
#define _Interface_GeneralLib_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
class Interface_NodeOfGeneralLib;
class Standard_NoSuchObject;
class Standard_Transient;
class Interface_GeneralModule;
class Interface_Protocol;
class Interface_GlobalNodeOfGeneralLib;

class Interface_GeneralLib
{
public:
  DEFINE_STANDARD_ALLOC

  //! Adds a couple (Module-Protocol) into the global definition set
  //! for this class of Library.
  Standard_EXPORT static void SetGlobal(const Handle(Interface_GeneralModule)& amodule,
                                        const Handle(Interface_Protocol)&      aprotocol);

  //! Creates a Library which complies with a Protocol, that is :
  //! Same class (criterium IsInstance)
  //! This creation gets the Modules from the global set, those
  //! which are bound to the given Protocol and its Resources
  Standard_EXPORT Interface_GeneralLib(const Handle(Interface_Protocol)& aprotocol);

  //! Creates an empty Library : it will later by filled by method
  //! AddProtocol
  Standard_EXPORT Interface_GeneralLib();

  //! Adds a couple (Module-Protocol) to the Library, given the
  //! class of a Protocol. Takes Resources into account.
  //! (if <aprotocol> is not of type TheProtocol, it is not added)
  Standard_EXPORT void AddProtocol(const Handle(Standard_Transient)& aprotocol);

  //! Clears the list of Modules of a library (can be used to
  //! redefine the order of Modules before action : Clear then
  //! refill the Library by calls to AddProtocol)
  Standard_EXPORT void Clear();

  //! Sets a library to be defined with the complete Global list
  //! (all the couples Protocol/Modules recorded in it)
  Standard_EXPORT void SetComplete();

  //! Selects a Module from the Library, given an Object.
  //! Returns True if Select has succeeded, False else.
  //! Also Returns (as arguments) the selected Module and the Case
  //! Number determined by the associated Protocol.
  //! If Select has failed, <module> is Null Handle and CN is zero.
  //! (Select can work on any criterium, such as Object DynamicType)
  Standard_EXPORT Standard_Boolean Select(const Handle(Standard_Transient)& obj,
                                          Handle(Interface_GeneralModule)&  module,
                                          Standard_Integer&                 CN) const;

  //! Starts Iteration on the Modules (sets it on the first one)
  Standard_EXPORT void Start();

  //! Returns True if there are more Modules to iterate on
  Standard_EXPORT Standard_Boolean More() const;

  //! Iterates by getting the next Module in the list
  //! If there is none, the exception will be raised by Value
  Standard_EXPORT void Next();

  //! Returns the current Module in the Iteration
  Standard_EXPORT const Handle(Interface_GeneralModule)& Module() const;

  //! Returns the current Protocol in the Iteration
  Standard_EXPORT const Handle(Interface_Protocol)& Protocol() const;

protected:
private:
  Handle(Interface_NodeOfGeneralLib) thelist;
  Handle(Interface_NodeOfGeneralLib) thecurr;
};

#endif // _Interface_GeneralLib_HeaderFile
