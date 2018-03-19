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


#ifndef _StdObjMgt_SharedObject_HeaderFile
#define _StdObjMgt_SharedObject_HeaderFile

#include <Standard_NoSuchObject.hxx>
#include <StdObjMgt_Persistent.hxx>

class StdObjMgt_SharedObject
{
public:
  template <class Transient>
  class AbstractPersistentBase : public Standard_Transient
  {
  public:
    virtual Handle(Transient) Import() const = 0;
  };

  template <class TransientT, class Base = StdObjMgt_Persistent>
  class SharedBase : public Base
  {
  public:
    //! Changes transient object
    inline void Transient(const Handle(TransientT)& theTransient)
      { myTransient = theTransient; }

    //! Import transient object from the persistent data.
    inline const Handle(TransientT)& Import()  { return myTransient; }

  protected:
    Handle(TransientT) myTransient;
  };

  template <class Base,
            class Transient,
            class Persistent = AbstractPersistentBase<Transient> >
  class DelayedBase : public Base
  {
  public:
    typedef Transient  TransientBase;
    typedef Persistent PersistentBase;

    //! Import transient object from the persistent data.
    virtual Handle(Transient) Import()
      { return myTransient; }

  public:
    Handle(Transient) myTransient;
  };

  template <class Base,
            class PersistentData,
            class Transient = typename Base::TransientBase>
  class IgnoreData : public Base
  {
  public:
    //! Read persistent data from a file.
    virtual void Read (StdObjMgt_ReadData& theReadData)
      { PersistentData().Read (theReadData); }
    //! Write persistent data to a file.
    virtual void Write (StdObjMgt_WriteData& theWriteData) const
      { PersistentData().Write (theWriteData); }
    //! Gets persistent child objects
    virtual void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
      { PersistentData().PChildren(theChildren); }
    //! Returns persistent type name
    virtual Standard_CString PName() const
      { return PersistentData().PName(); }

    //! Import transient object from the persistent data.
    virtual Handle(Transient) Import()
      { return NULL; }
  };

private:
  template <class Base>
  class delayedSubBase : public Base
  {
  public:
    //! Import transient object from the persistent data.
    virtual Handle(typename Base::TransientBase) Import()
    {
      if (Base::myTransient.IsNull() && !myPersistent.IsNull())
      {
        Base::myTransient = myPersistent->Import();
        myPersistent.Nullify();
      }

      return Base::myTransient;
    }

  public:
    Handle(typename Base::PersistentBase) myPersistent;
  };

public:
  template <class Base, class Persistent = typename Base::PersistentBase>
  class Delayed : public delayedSubBase<Base>
  {
  private:
    template <class T1, class T2>
    struct DownCast {
      static Handle(T1) make(const Handle(T2)& theT2)
        { return Handle(T1)::DownCast(theT2); }
    };

    template <class T>
    struct DownCast<T, T> {
      static Handle(T) make(const Handle(T)& theT)
        { return theT; }
    };

  public:
    //! Read persistent data from a file.
    virtual void Read (StdObjMgt_ReadData& theReadData)
    {
      Handle(Persistent) aPersistent = new Persistent;
      aPersistent->Read (theReadData);
      this->myPersistent = aPersistent;
    }
    //! Write persistent data to a file.
    virtual void Write(StdObjMgt_WriteData& theWriteData) const
    { 
      Handle(Persistent) aPersistent = 
        DownCast<Persistent, typename Base::PersistentBase>::make(this->myPersistent);
      Standard_NoSuchObject_Raise_if(aPersistent.IsNull(), 
        "StdObjMgt_SharedObject::Delayed::Write - persistent object wasn't set for writing!");
      aPersistent->Write(theWriteData);
    }
    //! Gets persistent child objects
    virtual void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
    {
      Handle(Persistent) aPersistent = 
        DownCast<Persistent, typename Base::PersistentBase>::make(this->myPersistent);
      Standard_NoSuchObject_Raise_if(aPersistent.IsNull(), 
        "StdObjMgt_SharedObject::Delayed::PChildren - persistent object wasn't set for writing!");
      aPersistent->PChildren(theChildren);
    }
    //! Returns persistent type name
    virtual Standard_CString PName() const 
    { 
      Handle(Persistent) aPersistent =
        DownCast<Persistent, typename Base::PersistentBase>::make(this->myPersistent);
      Standard_NoSuchObject_Raise_if(aPersistent.IsNull(), 
        "StdObjMgt_SharedObject::Delayed::PName - persistent object wasn't set for writing!");
      return aPersistent->PName();
    }
  };
};

#endif
