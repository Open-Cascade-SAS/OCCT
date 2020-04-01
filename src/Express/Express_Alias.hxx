// Copyright (c) 1999-2020 OPEN CASCADE SAS
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

#ifndef _Express_Alias_HeaderFile
#define _Express_Alias_HeaderFile

#include <Express_Item.hxx>
#include <Express_Type.hxx>
#include <Standard_Type.hxx>

class TCollection_AsciiString;

//! Implements TYPE = type (alias) item of the EXPRESS
//! schema, with interface for deferred Item class.
class Express_Alias : public Express_Item
{

public:

  //! Create ALIAS item and initialize it
  Standard_EXPORT Express_Alias (const Standard_CString theName, const Handle(Express_Type)& theType);

  //! Returns aliased type
  Standard_EXPORT const Handle(Express_Type)& Type() const;

  //! Returns name of aliased type
  Standard_EXPORT virtual const TCollection_AsciiString CPPName() const Standard_OVERRIDE;

  //! Create HXX/CXX files from item
  Standard_EXPORT virtual Standard_Boolean GenerateClass() const Standard_OVERRIDE;

  //! Propagates the calls of Use function
  Standard_EXPORT virtual void PropagateUse() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Express_Alias, Express_Item)

protected:

private:

  Handle(Express_Type) myType;

};

#endif // _Express_Alias_HeaderFile
