// Created on: 1997-02-24
// Created by: Kernel
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

#ifndef _Storage_Root_HeaderFile
#define _Storage_Root_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
class Standard_Persistent;
class Storage_Schema;
class TCollection_AsciiString;


class Storage_Root;
DEFINE_STANDARD_HANDLE(Storage_Root, Standard_Transient)


//! A root object extracted from a Storage_Data object.
//! A Storage_Root encapsulates a persistent
//! object which is a root of a Storage_Data object.
//! It contains additional information: the name and
//! the data type of the persistent object.
//! When retrieving a Storage_Data object from a
//! container (for example, a file) you access its
//! roots with the function Roots which returns a
//! sequence of root objects. The provided functions
//! allow you to request information about each root of the sequence.
//! You do not create explicit roots: when inserting
//! data in a Storage_Data object, you just provide
//! the persistent object and optionally its name to the function AddRoot.
class Storage_Root : public Standard_Transient
{

public:

  
  Standard_EXPORT Storage_Root();
  
  Standard_EXPORT Storage_Root (const TCollection_AsciiString&     theName,
                                const Handle(Standard_Persistent)& theObject);

  Standard_EXPORT Storage_Root (const TCollection_AsciiString& theName,
                                const Standard_Integer         theRef,
                                const TCollection_AsciiString& theType);
  
  Standard_EXPORT void SetName (const TCollection_AsciiString& theName);
  

  //! Returns the name of this root object.
  //! The name may have been given explicitly when
  //! the root was inserted into the Storage_Data
  //! object. If not, the name is a reference number
  //! which was assigned automatically by the driver
  //! when writing the set of data into the container.
  //! When naming the roots, it is easier to retrieve
  //! objects by significant references rather than by
  //! references without any semantic values.
  //! Warning
  //! The returned string will be empty if you call this
  //! function before having named this root object,
  //! either explicitly, or when writing the set of data
  //! into the container.
  Standard_EXPORT TCollection_AsciiString Name() const;
  
  Standard_EXPORT void SetObject (const Handle(Standard_Persistent)& anObject);
  

  //! Returns the persistent object encapsulated by this root.
  Standard_EXPORT Handle(Standard_Persistent) Object() const;
  
  //! Returns the name of this root type.
  Standard_EXPORT TCollection_AsciiString Type() const;

  Standard_EXPORT void SetReference (const Standard_Integer aRef);

  Standard_EXPORT Standard_Integer Reference() const;

  Standard_EXPORT void SetType (const TCollection_AsciiString& aType);


friend class Storage_Schema;


  DEFINE_STANDARD_RTTIEXT(Storage_Root,Standard_Transient)

protected:




private:

  

  TCollection_AsciiString myName;
  TCollection_AsciiString myType;
  Handle(Standard_Persistent) myObject;
  Standard_Integer myRef;


};







#endif // _Storage_Root_HeaderFile
