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


#ifndef _StdObject_Shape_HeaderFile
#define _StdObject_Shape_HeaderFile

#include <StdObjMgt_ReadData.hxx>
#include <StdObject_Location.hxx>
#include <StdPersistent_TopoDS.hxx>

#include <TopoDS_Shape.hxx>


class StdObject_Shape
{
public:
  //! Import transient object from the persistent data.
  Standard_EXPORT TopoDS_Shape Import() const;

protected:
  //! Read persistent data from a file.
  inline void read (StdObjMgt_ReadData& theReadData)
    { theReadData >> myTShape >> myLocation >> myOrient; }

private:
  Handle(StdPersistent_TopoDS::TShape) myTShape;
  StdObject_Location                   myLocation;
  Standard_Integer                     myOrient;

  friend StdObjMgt_ReadData& operator >>
    (StdObjMgt_ReadData::Object, StdObject_Shape&);
};

//! Read persistent data from a file.
inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, StdObject_Shape& theShape)
{
  theShape.read (theReadData);
  return theReadData;
}

#endif
