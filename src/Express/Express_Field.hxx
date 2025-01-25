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

#ifndef _Express_Field_HeaderFile
#define _Express_Field_HeaderFile

#include <Standard_Type.hxx>

class Express_Type;
class TCollection_HAsciiString;
class TCollection_AsciiString;

//! Represents field of the ENTITY item in the EXPRESS schema
class Express_Field : public Standard_Transient
{

public:
  //! Create object and initialize it
  Standard_EXPORT Express_Field(const Standard_CString      theName,
                                const Handle(Express_Type)& theType,
                                const Standard_Boolean      theOpt);

  //! Create object and initialize it
  Standard_EXPORT Express_Field(const Handle(TCollection_HAsciiString)& theName,
                                const Handle(Express_Type)&             theType,
                                const Standard_Boolean                  theOpt);

  //! Returns field name
  Standard_EXPORT const TCollection_AsciiString& Name() const;

  //! Returns a pointer to the field name to modify it
  Standard_EXPORT Handle(TCollection_HAsciiString) HName() const;

  //! Returns field type
  Standard_EXPORT const Handle(Express_Type)& Type() const;

  //! Returns True if field is optional
  Standard_EXPORT Standard_Boolean IsOptional() const;

  DEFINE_STANDARD_RTTIEXT(Express_Field, Standard_Transient)

protected:
private:
  Handle(TCollection_HAsciiString) myName;
  Handle(Express_Type)             myType;
  Standard_Boolean                 myOpt;
};

#endif // _Express_Field_HeaderFile
