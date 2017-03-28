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


#ifndef _StdPersistent_TopLoc_HeaderFile
#define _StdPersistent_TopLoc_HeaderFile

#include <StdObjMgt_SharedObject.hxx>
#include <StdObjMgt_Persistent.hxx>
#include <StdObject_Location.hxx>
#include <StdObjMgt_TransientPersistentMap.hxx>

#include <TopLoc_Datum3D.hxx>
#include <TopLoc_Location.hxx>


class StdPersistent_TopLoc
{
public:
  class Datum3D : public StdObjMgt_SharedObject::SharedBase<TopLoc_Datum3D>
  {
  public:
    //! Read persistent data from a file.
    void Read (StdObjMgt_ReadData& theReadData);
    //! Write persistent data to a file.
    void Write (StdObjMgt_WriteData& theWriteData) const;
    //! Gets persistent child objects
    Standard_EXPORT virtual void PChildren(SequenceOfPersistent&) const { }
    //! Returns persistent type name
    Standard_EXPORT virtual Standard_CString PName () const
      { return "PTopLoc_Datum3D"; }
  };

  class ItemLocation : public StdObjMgt_Persistent
  {
    friend class StdPersistent_TopLoc;

  public:
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData);
    //! Write persistent data to a file.
    Standard_EXPORT virtual void Write (StdObjMgt_WriteData& theWriteData) const;
    //! Gets persistent child objects
    Standard_EXPORT virtual void PChildren(SequenceOfPersistent& theChildren) const;
    //! Returns persistent type name
    Standard_EXPORT virtual Standard_CString PName () const
      { return "PTopLoc_ItemLocation"; }

    //! Import transient object from the persistent data.
    Standard_EXPORT TopLoc_Location Import() const;

  private:
    Handle(Datum3D)    myDatum;
    Standard_Integer   myPower;
    StdObject_Location myNext;
  };

public:
  Standard_EXPORT static Handle(ItemLocation) Translate (const TopLoc_Location& theLoc,
                                                         StdObjMgt_TransientPersistentMap& theMap);
  Standard_EXPORT static Handle(Datum3D) Translate (const Handle(TopLoc_Datum3D)& theDatum,
                                                    StdObjMgt_TransientPersistentMap& theMap);
};

#endif
