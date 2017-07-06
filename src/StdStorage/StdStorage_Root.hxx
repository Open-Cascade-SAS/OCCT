// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _StdStorage_Root_HeaderFile
#define _StdStorage_Root_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
class StdObjMgt_Persistent;
class Storage_Schema;
class TCollection_AsciiString;

class Storage_Root;
DEFINE_STANDARD_HANDLE(StdStorage_Root, Standard_Transient)

//! Describes a named persistent root 
class StdStorage_Root
  : public Standard_Transient
{
  friend class StdStorage_RootData;

public:

  DEFINE_STANDARD_RTTIEXT(StdStorage_Root, Standard_Transient)

  //! Creates an empty root
  Standard_EXPORT StdStorage_Root();

  //! Creates a root for writing
  Standard_EXPORT StdStorage_Root(const TCollection_AsciiString&      theName,
                                  const Handle(StdObjMgt_Persistent)& theObject);

  //! Returns a name of the root
  Standard_EXPORT TCollection_AsciiString Name() const;

  //! Sets a name to the root object
  Standard_EXPORT void SetName(const TCollection_AsciiString& theName);

  //! Returns a root's persistent object
  Standard_EXPORT Handle(StdObjMgt_Persistent) Object() const;

  //! Sets a root's persistent object
  Standard_EXPORT void SetObject(const Handle(StdObjMgt_Persistent)& anObject);

  //! Returns a root's persistent type
  Standard_EXPORT TCollection_AsciiString Type() const;

  //! Sets a root's persistent type
  Standard_EXPORT void SetType(const TCollection_AsciiString& aType);

  //! Returns root's position in the root data section
  Standard_EXPORT Standard_Integer Reference() const;

private:

  Standard_EXPORT StdStorage_Root(const TCollection_AsciiString& theName,
                                  const Standard_Integer         theRef,
                                  const TCollection_AsciiString& theType);

  Standard_EXPORT void SetReference(const Standard_Integer aRef);

  TCollection_AsciiString myName;
  TCollection_AsciiString myType;
  Handle(StdObjMgt_Persistent) myObject;
  Standard_Integer myRef;

};

#endif // _StdStorage_Root_HeaderFile
