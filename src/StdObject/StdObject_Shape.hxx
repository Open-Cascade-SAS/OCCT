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

#include <StdObjMgt_ContentTypes.hxx>
#include <StdObject_Location.hxx>
#include <StdPersistent_TopoDS.hxx>

#include <TopAbs_Orientation.hxx>
#include <TopoDS_Shape.hxx>


class StdObject_Shape : private StdObjMgt_ContentTypes
{
public:
  //! Read persistent data from a file.
  inline void Read (StdObjMgt_ReadData& theReadData)
    { theReadData >> myTShape >> myLocation >> myOrient; }

  //! Import transient object from the persistent data.
  Standard_EXPORT TopoDS_Shape Import() const;

private:
  Reference <StdPersistent_TopoDS::TShape> myTShape;
  Object    <StdObject_Location>           myLocation;
  Enum      <TopAbs_Orientation>           myOrient;
};

#endif
