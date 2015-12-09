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

class StdObjMgt_ReadData;
class TDF_Attribute;


//! Root class for a temporary object that reads data from a file and then
//! creates transient object using the data.
class StdObjMgt_Persistent : public Standard_Transient
{
public:
  //! Create a derived class instance.
  typedef Handle(StdObjMgt_Persistent) (*Instantiator)();

  //! Read persistent data from a file.
  Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData) = 0;

  //! Import transient attribuite from the persistent data
  //! (to be overriden by attribute classes;
  //! returns a null handle by default for non-attribute classes).
  Standard_EXPORT virtual Handle(TDF_Attribute) ImportAttribute() const;
};

#endif // _StdObjMgt_Persistent_HeaderFile
