// Created on: 1997-02-12
// Created by: Alexander BRIVIN
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _Vrml_WWWAnchor_HeaderFile
#define _Vrml_WWWAnchor_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TCollection_AsciiString.hxx>
#include <Vrml_WWWAnchorMap.hxx>
#include <Standard_OStream.hxx>

//! defines a WWWAnchor node of VRML specifying group properties.
//! The  WWWAnchor  group  node  loads  a  new  scene  into  a  VRML  browser
//! when  one  of  its  children  is  closen.  Exactly  how  a  user  "chooses"
//! a  child  of  the  WWWAnchor  is  up  to  the  VRML browser.
//! WWWAnchor  with  an  empty  ("")  name  does  nothing  when  its
//! children  are  chosen.
//! WWWAnchor  behaves  like  a  Separator,  pushing  the  traversal  state
//! before  traversing  its  children  and  popping  it  afterwards.
class Vrml_WWWAnchor
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Vrml_WWWAnchor(const TCollection_AsciiString& aName        = "",
                                 const TCollection_AsciiString& aDescription = "",
                                 const Vrml_WWWAnchorMap        aMap         = Vrml_MAP_NONE);

  Standard_EXPORT void SetName(const TCollection_AsciiString& aName);

  Standard_EXPORT TCollection_AsciiString Name() const;

  Standard_EXPORT void SetDescription(const TCollection_AsciiString& aDescription);

  Standard_EXPORT TCollection_AsciiString Description() const;

  Standard_EXPORT void SetMap(const Vrml_WWWAnchorMap aMap);

  Standard_EXPORT Vrml_WWWAnchorMap Map() const;

  Standard_EXPORT Standard_OStream& Print(Standard_OStream& anOStream) const;

protected:
private:
  TCollection_AsciiString myName;
  TCollection_AsciiString myDescription;
  Vrml_WWWAnchorMap       myMap;
};

#endif // _Vrml_WWWAnchor_HeaderFile
