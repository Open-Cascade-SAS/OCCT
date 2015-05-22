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

#ifndef _Standard_Persistent_proto_HeaderFile
#define _Standard_Persistent_proto_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <Standard_OStream.hxx>

class Standard_Type;
class Handle_Standard_Type;
class Handle_Standard_Persistent;
class Standard_Type;

class Storage_stCONSTclCOM;

class Standard_Persistent : public Standard_Transient
{
friend class Handle(Standard_Persistent);
friend class Storage_Schema;

private:
  Standard_Integer _typenum;
  Standard_Integer _refnum;
public:
  
  Standard_Persistent& operator= (const Standard_Persistent&) { return *this; }
  Standard_Persistent() : _typenum(0),_refnum(0) {}
  Standard_Persistent(const Standard_Persistent&) : _typenum(0),_refnum(0) {}
  Standard_Persistent(const Storage_stCONSTclCOM&) : _typenum(0),_refnum(0) {}

  DEFINE_STANDARD_RTTI(Standard_Persistent,Standard_Transient)
};

#include <Handle_Standard_Persistent.hxx>

#endif
