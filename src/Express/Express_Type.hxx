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

#ifndef _Express_Type_HeaderFile
#define _Express_Type_HeaderFile

#include <Standard_Type.hxx>

class TCollection_AsciiString;

//! Provides basis for identification (reference) to some type
//! in express schema
class Express_Type : public Standard_Transient
{

public:

  //! Returns CPP-style name of the type
  Standard_EXPORT virtual const TCollection_AsciiString CPPName() const = 0;

  //! Return True if type is defined in package Standard (False by default)
  Standard_EXPORT virtual Standard_Boolean IsStandard() const;

  //! Return True if type is simple (not a class)
  //! (by default returns IsStandard())
  Standard_EXPORT virtual Standard_Boolean IsSimple() const;

  //! Return True if type is Transient
  //! (by default returns ! IsSimple())
  Standard_EXPORT virtual Standard_Boolean IsHandle() const;

  //! Declares type as used by some item being generated.
  //! Calls Use() for all referred types and schema items.
  //! Default instantiation does nothing
  Standard_EXPORT virtual Standard_Boolean Use() const;

  //! Declares type as used by some item being generated.
  //! Calls Use() for all referred types and schema items.
  //! Default instantiation does nothing
  Standard_EXPORT virtual void Use2 (const TCollection_AsciiString& theRefName, const TCollection_AsciiString& theRefPack) const;

  DEFINE_STANDARD_RTTIEXT(Express_Type, Standard_Transient)

protected:

  //! Empty constructor
  Standard_EXPORT Express_Type();

private:

};

#endif // _Express_Type_HeaderFile
