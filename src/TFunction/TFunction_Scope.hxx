// Created on: 2008-06-22
// Created by: Vladislav ROMASHKO
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#ifndef _TFunction_Scope_HeaderFile
#define _TFunction_Scope_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TFunction_DoubleMapOfIntegerLabel.hxx>
#include <TFunction_Logbook.hxx>
#include <Standard_Integer.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_OStream.hxx>
class TDF_Label;
class Standard_GUID;
class TFunction_Logbook;
class TDF_RelocationTable;

class TFunction_Scope;
DEFINE_STANDARD_HANDLE(TFunction_Scope, TDF_Attribute)

//! Keeps a scope of functions.
class TFunction_Scope : public TDF_Attribute
{

public:
  //! Static methods
  //! ==============
  //! Finds or Creates a TFunction_Scope attribute at the root label accessed by <Access>.
  //! Returns the attribute.
  Standard_EXPORT static Handle(TFunction_Scope) Set(const TDF_Label& Access);

  //! Returns the GUID for Scope attribute.
  //! Instant methods
  //! ===============
  //! Constructor (empty).
  Standard_EXPORT static const Standard_GUID& GetID();

  Standard_EXPORT TFunction_Scope();

  //! Adds a function to the scope of functions.
  Standard_EXPORT Standard_Boolean AddFunction(const TDF_Label& L);

  //! Removes a function from the scope of functions.
  Standard_EXPORT Standard_Boolean RemoveFunction(const TDF_Label& L);

  //! Removes a function from the scope of functions.
  Standard_EXPORT Standard_Boolean RemoveFunction(const Standard_Integer ID);

  //! Removes all functions from the scope of functions.
  Standard_EXPORT void RemoveAllFunctions();

  //! Returns true if the function exists with such an ID.
  Standard_EXPORT Standard_Boolean HasFunction(const Standard_Integer ID) const;

  //! Returns true if the label contains a function of this scope.
  Standard_EXPORT Standard_Boolean HasFunction(const TDF_Label& L) const;

  //! Returns an ID of the function.
  Standard_EXPORT Standard_Integer GetFunction(const TDF_Label& L) const;

  //! Returns the label of the function with this ID.
  Standard_EXPORT const TDF_Label& GetFunction(const Standard_Integer ID) const;

  //! Returns the Logbook used in TFunction_Driver methods.
  //! Implementation of Attribute methods
  //! ===================================
  Standard_EXPORT Handle(TFunction_Logbook) GetLogbook() const;

  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  Standard_EXPORT virtual void Restore(const Handle(TDF_Attribute)& with) Standard_OVERRIDE;

  Standard_EXPORT virtual void Paste(const Handle(TDF_Attribute)&       into,
                                     const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;

  Standard_EXPORT virtual Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_OStream& Dump(Standard_OStream& anOS) const Standard_OVERRIDE;

  //! Returns the scope of functions.
  Standard_EXPORT const TFunction_DoubleMapOfIntegerLabel& GetFunctions() const;

  //! Returns the scope of functions for modification.
  //! Warning: Don't use this method if You are not sure what You do!
  Standard_EXPORT TFunction_DoubleMapOfIntegerLabel& ChangeFunctions();

  Standard_EXPORT void SetFreeID(const Standard_Integer ID);

  Standard_EXPORT Standard_Integer GetFreeID() const;

  DEFINE_STANDARD_RTTIEXT(TFunction_Scope, TDF_Attribute)

protected:
private:
  TFunction_DoubleMapOfIntegerLabel myFunctions;
  Standard_Integer                  myFreeID;
};

#endif // _TFunction_Scope_HeaderFile
