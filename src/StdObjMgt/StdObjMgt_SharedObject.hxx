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

  template <class Transient, class Base = StdObjMgt_Persistent>
  class SharedBase : public Base
  {
  public:
    //! Import transient object from the persistent data.
    inline const Handle(Transient)& Import()  { return myTransient; }

  protected:
    Handle(Transient) myTransient;
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
    Standard_EXPORT virtual Handle(Transient) Import()
      { return myTransient; }

  protected:
    Handle(Transient) myTransient;
  };

  template <class Base,
            class PersistentData,
            class Transient = typename Base::TransientBase>
  class IgnoreData : public Base
  {
  public:
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData)
      { PersistentData().Read (theReadData); }

    //! Import transient object from the persistent data.
    Standard_EXPORT virtual Handle(Transient) Import()
      { return NULL; }
  };

private:
  template <class Base>
  class delayedSubBase : public Base
  {
  public:
    //! Import transient object from the persistent data.
    Standard_EXPORT virtual Handle(typename Base::TransientBase) Import()
    {
      if (Base::myTransient.IsNull() && !myPersistent.IsNull())
      {
        Base::myTransient = myPersistent->Import();
        myPersistent.Nullify();
      }

      return Base::myTransient;
    }

  protected:
    Handle(typename Base::PersistentBase) myPersistent;
  };

public:
  template <class Base, class Persistent = typename Base::PersistentBase>
  class Delayed : public delayedSubBase<Base>
  {
  public:
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData)
    {
      Handle(Persistent) aPersistent = new Persistent;
      aPersistent->Read (theReadData);
      this->myPersistent = aPersistent;
    }
  };
};

#endif
