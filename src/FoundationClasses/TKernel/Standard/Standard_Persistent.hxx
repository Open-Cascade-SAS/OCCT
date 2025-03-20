// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Standard_Persistent_HeaderFile
#define _Standard_Persistent_HeaderFile

#include <Standard_Type.hxx>

//! Root of "persistent" classes, a legacy support of
//! object oriented databases, now outdated.
class Standard_Persistent : public Standard_Transient
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_Persistent()
      : _typenum(0),
        _refnum(0)
  {
  }

  DEFINE_STANDARD_RTTIEXT(Standard_Persistent, Standard_Transient)

  Standard_Integer& TypeNum() { return _typenum; }

private:
  Standard_Integer _typenum;
  Standard_Integer _refnum;

  friend class Storage_Schema;
};

#endif // _Standard_Persistent_HeaderFile
