// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _StdLPersistent_HString_HeaderFile
#define _StdLPersistent_HString_HeaderFile

#include <StdObjMgt_Persistent.hxx>

#include <Standard_TypeDef.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TCollection_HExtendedString.hxx>

class StdLPersistent_HString
{
  template <class StringClass, typename CharType>
  class instance : public StdObjMgt_Persistent
  {
  public:
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read(StdObjMgt_ReadData& theReadData);
    //! Write persistent data to a file.
    Standard_EXPORT virtual void Write(StdObjMgt_WriteData& theWriteData) const;

    virtual void PChildren(StdObjMgt_Persistent::SequenceOfPersistent&) const {}

    //! Get/create a label defined by referenced string.
    Standard_EXPORT virtual TDF_Label Label(const Handle(TDF_Data)& theDF) const;

    //! Get the value.
    const Handle(StringClass)& Value() const { return myValue; }

  protected:
    Handle(StringClass) myValue;
  };

public:
  class Ascii : public instance<TCollection_HAsciiString, Standard_Character>
  {
  public:
    //! Get referenced ASCII string.
    Standard_EXPORT virtual Handle(TCollection_HAsciiString) AsciiString() const;

    inline Standard_CString PName() const { return "PCollection_HAsciiString"; }
  };

  class Extended : public instance<TCollection_HExtendedString, Standard_ExtCharacter>
  {
  public:
    //! Get referenced extended string.
    Standard_EXPORT virtual Handle(TCollection_HExtendedString) ExtString() const;

    inline Standard_CString PName() const { return "PCollection_HExtendedString"; }
  };
};

#endif
