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

#ifndef _StdObjMgt_Persistent_HeaderFile
#define _StdObjMgt_Persistent_HeaderFile


#include <Standard.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>

#include <TDF_Label.hxx>

class StdObjMgt_ReadData;
class StdObjMgt_WriteData;
class TDocStd_Document;
class TDF_Attribute;
class TDF_Data;
class TCollection_HAsciiString;
class TCollection_HExtendedString;


//! Root class for a temporary persistent object that reads data from a file
//! and then creates transient object using the data.
class StdObjMgt_Persistent : public Standard_Transient
{
public:
  Standard_EXPORT StdObjMgt_Persistent();

  //! Derived class instance create function.
  typedef Handle(StdObjMgt_Persistent) (*Instantiator)();

  //! Create a derived class instance.
  template <class Persistent>
  static Handle(StdObjMgt_Persistent) Instantiate()
    { return new Persistent; }

  //! Read persistent data from a file.
  virtual void Read (StdObjMgt_ReadData& theReadData) = 0;

  //! Write persistent data to a file.
  virtual void Write (StdObjMgt_WriteData& theWriteData) const = 0;

  typedef NCollection_Sequence<Handle(StdObjMgt_Persistent)> SequenceOfPersistent;

  //! Gets persistent child objects
  virtual void PChildren (SequenceOfPersistent&) const = 0;

  //! Returns persistent type name
  virtual Standard_CString PName() const = 0;

  //! Import transient document from the persistent data
  //! (to be overridden by document class;
  //! does nothing by default for other classes).
  Standard_EXPORT virtual void ImportDocument
    (const Handle(TDocStd_Document)& theDocument) const;

  //! Create an empty transient attribute
  //! (to be overridden by attribute classes;
  //! does nothing and returns a null handle by default for other classes).
  Standard_EXPORT virtual Handle(TDF_Attribute) CreateAttribute();

  //! Get transient attribute for the persistent data
  //! (to be overridden by attribute classes;
  //! returns a null handle by default for non-attribute classes).
  Standard_EXPORT virtual Handle(TDF_Attribute) GetAttribute() const;

  //! Import transient attribute from the persistent data
  //! (to be overridden by attribute classes;
  //! does nothing by default for non-attribute classes).
  Standard_EXPORT virtual void ImportAttribute();

  //! Get referenced ASCII string
  //! (to be overridden by ASCII string class;
  //! returns a null handle by default for other classes).
  Standard_EXPORT virtual Handle(TCollection_HAsciiString) AsciiString() const;

  //! Get referenced extended string
  //! (to be overridden by extended string class;
  //! returns a null handle by default for other classes).
  Standard_EXPORT virtual Handle(TCollection_HExtendedString) ExtString() const;

  //! Get a label expressed by referenced extended string
  //! (to be overridden by extended string class;
  //! returns a null label by default for other classes).
  Standard_EXPORT virtual TDF_Label Label (const Handle(TDF_Data)& theDF) const;

  //! Returns the assigned persistent type number
  Standard_Integer TypeNum() const { return myTypeNum; }

  //! Assigns a persistent type number to the object
  void TypeNum(Standard_Integer theTypeNum) { myTypeNum = theTypeNum; }

  //! Returns the object reference number
  Standard_Integer RefNum() const { return myRefNum; }

  //! Sets an object reference number
  void RefNum(Standard_Integer theRefNum) { myRefNum = theRefNum; }

private:
  Standard_Integer myTypeNum;
  Standard_Integer myRefNum;
};

#endif // _StdObjMgt_Persistent_HeaderFile
