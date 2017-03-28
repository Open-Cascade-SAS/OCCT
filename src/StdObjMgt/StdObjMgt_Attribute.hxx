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

#ifndef _StdObjMgt_Attribute_HeaderFile
#define _StdObjMgt_Attribute_HeaderFile

#include <StdObjMgt_Persistent.hxx>
#include <StdObjMgt_ReadData.hxx>
#include <StdObjMgt_WriteData.hxx>


//! Root class for a temporary persistent object corresponding to an attribute.
template <class Transient>
class StdObjMgt_Attribute : public Standard_Transient
{
  class base : public StdObjMgt_Persistent
  {
  public:
    //! Create an empty transient attribuite
    Standard_EXPORT virtual Handle(TDF_Attribute) CreateAttribute()
      { return myTransient = new Transient; }

    //! Get transient attribuite for the persistent data
    Standard_EXPORT virtual Handle(TDF_Attribute) GetAttribute() const
      { return Handle(TDF_Attribute)(myTransient); }

  protected:
    Handle(Transient) myTransient;
  };

public:
  class Static : public base {};

  template <class DataType>
  class Simple : public Static
  {
  public:
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData)
      { theReadData >> myData; }
    //! Write persistent data to a file.
    Standard_EXPORT virtual void Write (StdObjMgt_WriteData& theWriteData) const
      { theWriteData << myData; }
    Standard_EXPORT virtual void PChildren(StdObjMgt_Persistent::SequenceOfPersistent&) const { }
    Standard_EXPORT virtual Standard_CString PName() const { return "StdObjMgt_Attribute::undefined"; }

  protected:
    DataType myData;
  };

  struct SingleInt : Simple<Standard_Integer> {};
  struct SingleRef : Simple<Handle(StdObjMgt_Persistent)> {};

private:
  template <class Persistent>
  class container : public base
  {
  public:
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData)
    {
      myPersistent = new Persistent;
      myPersistent->Read (theReadData);
    }
    //! Write persistent data to a file.
    Standard_EXPORT virtual void Write(StdObjMgt_WriteData& theWriteData) const
      { myPersistent->Write(theWriteData); }
    Standard_EXPORT virtual void PChildren(StdObjMgt_Persistent::SequenceOfPersistent&) const { }
    Standard_EXPORT virtual Standard_CString PName() const 
      { return myPersistent->PName(); }

    //! Import transient attribuite from the persistent data
    Standard_EXPORT virtual void ImportAttribute()
    {
      if (myPersistent && this->myTransient)
      {
        myPersistent->Import (this->myTransient);
        myPersistent.Nullify();
      }
    }

  private:
    Handle(Persistent) myPersistent;
  };

public:
  template <class Persistent>
  static Handle(StdObjMgt_Persistent) Instantiate()
    { return new container<Persistent>; }
};

#endif // _StdObjMgt_Attribute_HeaderFile
