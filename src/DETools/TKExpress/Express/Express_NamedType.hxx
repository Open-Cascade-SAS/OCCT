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

#ifndef _Express_NamedType_HeaderFile
#define _Express_NamedType_HeaderFile

#include <Express_Type.hxx>
#include <Standard_Type.hxx>

class Express_Item;
class TCollection_AsciiString;
class TCollection_HAsciiString;

//! Base class for complex types (ARRAY, LIST, BAG, SET)
//! in EXPRESS schema
//! Stores type of elements and
class Express_NamedType : public Express_Type
{

public:
  //! Creates an object and initializes by name
  Standard_EXPORT Express_NamedType(const Standard_CString theName);

  //! Creates an object and initializes by name
  Standard_EXPORT Express_NamedType(const Handle(TCollection_HAsciiString)& theName);

  //! Returns name of type (item in schema)
  Standard_EXPORT const TCollection_AsciiString& Name() const;

  //! Returns a pointer to the type name to modify it
  Standard_EXPORT Handle(TCollection_HAsciiString) HName() const;

  //! Returns handle to referred item in schema
  Standard_EXPORT const Handle(Express_Item)& Item() const;

  //! Sets handle to referred item in schema
  Standard_EXPORT void SetItem(const Handle(Express_Item)& theItem);

  //! Returns CPP-style name of the type
  Standard_EXPORT virtual const TCollection_AsciiString CPPName() const Standard_OVERRIDE;

  //! Return True if type is defined in package Standard
  Standard_EXPORT virtual Standard_Boolean IsStandard() const Standard_OVERRIDE;

  //! Return True if type is simple (not a class)
  Standard_EXPORT virtual Standard_Boolean IsSimple() const Standard_OVERRIDE;

  //! Return True if type is inherited from Transient
  Standard_EXPORT virtual Standard_Boolean IsHandle() const Standard_OVERRIDE;

  //! Declares type as used by some item being generated.
  //! Calls Use() for referred item (found by name).
  Standard_EXPORT virtual Standard_Boolean Use() const Standard_OVERRIDE;

  //! Declares type as used by some item being generated.
  //! Calls Use() for referred item (found by name).
  Standard_EXPORT virtual void Use2(const TCollection_AsciiString& theRefName,
                                    const TCollection_AsciiString& theRefPack) const
    Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Express_NamedType, Express_Type)

protected:
private:
  Handle(TCollection_HAsciiString) myName;
  Handle(Express_Item)             myItem;
};

#endif // _Express_NamedType_HeaderFile
