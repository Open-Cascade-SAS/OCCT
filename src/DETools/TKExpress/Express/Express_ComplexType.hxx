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

#ifndef _Express_ComplexType_HeaderFile
#define _Express_ComplexType_HeaderFile

#include <Express_Type.hxx>
#include <Standard_Type.hxx>

class TCollection_AsciiString;

//! Base class for complex types (ARRAY, LIST, BAG, SET)
//! in EXPRESS schema
//! Stores type of elements and
class Express_ComplexType : public Express_Type
{

public:
  //! Creates an object and initializes fields
  Standard_EXPORT Express_ComplexType(const Standard_Integer      theImin,
                                      const Standard_Integer      theImax,
                                      const Handle(Express_Type)& theType);

  //! Returns type of complex type items
  Standard_EXPORT const Handle(Express_Type)& Type() const;

  //! Returns CPP-style name of the type
  Standard_EXPORT virtual const TCollection_AsciiString CPPName() const Standard_OVERRIDE;

  //! Declares type as used by some item being generated.
  //! Calls Use() for type of elements
  Standard_EXPORT virtual Standard_Boolean Use() const Standard_OVERRIDE;

  //! Declares type as used by some item being generated.
  //! Calls Use() for type of elements
  Standard_EXPORT virtual void Use2(const TCollection_AsciiString& theRefName,
                                    const TCollection_AsciiString& theRefPack) const
    Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Express_ComplexType, Express_Type)

protected:
private:
  Standard_Integer     myMin;
  Standard_Integer     myMax;
  Handle(Express_Type) myType;
};

#endif // _Express_ComplexType_HeaderFile
