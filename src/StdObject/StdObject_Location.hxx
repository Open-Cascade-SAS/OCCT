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


#ifndef _StdObject_Location_HeaderFile
#define _StdObject_Location_HeaderFile

#include <StdObjMgt_ReadData.hxx>
#include <StdObjMgt_WriteData.hxx>
#include <StdObjMgt_Persistent.hxx>
#include <StdObjMgt_TransientPersistentMap.hxx>

#include <TopLoc_Location.hxx>

class StdObject_Location
{
public:

  //! Gets persistent child objects
  Standard_EXPORT void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const;

  //! Import transient object from the persistent data.
  TopLoc_Location Import() const;

  //! Creates a persistent wrapper object for a location
  Standard_EXPORT static StdObject_Location Translate (const TopLoc_Location& theLoc,
                                                       StdObjMgt_TransientPersistentMap& theMap);

private:
  Handle(StdObjMgt_Persistent) myData;

  friend StdObjMgt_ReadData& operator >>
    (StdObjMgt_ReadData::Object, StdObject_Location&);
  friend StdObjMgt_WriteData& operator <<
    (StdObjMgt_WriteData::Object, const StdObject_Location&);
};

//! Read persistent data from a file.
inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, StdObject_Location& theLocation)
{
  return theReadData >> theLocation.myData;
}

//! Write persistent data to a file.
inline StdObjMgt_WriteData& operator <<
(StdObjMgt_WriteData::Object theWriteData, const StdObject_Location& theLocation)
{
  return theWriteData << theLocation.myData;
}

#endif
