// Created on: 2001-07-09
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _XmlMDF_ADriver_HeaderFile
#define _XmlMDF_ADriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>
#include <XmlObjMgt_SRelocationTable.hxx>
class Message_Messenger;
class TDF_Attribute;
class XmlObjMgt_Persistent;


class XmlMDF_ADriver;
DEFINE_STANDARD_HANDLE(XmlMDF_ADriver, Standard_Transient)

//! Attribute Storage/Retrieval Driver.
class XmlMDF_ADriver : public Standard_Transient
{

public:

  
  //! Returns the version number from which the driver
  //! is available.
  Standard_EXPORT virtual Standard_Integer VersionNumber() const;
  
  //! Creates a new attribute from TDF.
  Standard_EXPORT virtual Handle(TDF_Attribute) NewEmpty() const = 0;
  
  //! Returns the type of source object,
  //! inheriting from Attribute from TDF.
  Standard_EXPORT virtual Handle(Standard_Type) SourceType() const;
  
  //! Returns the full XML tag name (including NS prefix)
  Standard_EXPORT const TCollection_AsciiString& TypeName() const;

  //! Returns the namespace string
  const TCollection_AsciiString& Namespace() const { return myNamespace; }

  //! Translate the contents of <aSource> and put it
  //! into <aTarget>, using the relocation table
  //! <aRelocTable> to keep the sharings.
  Standard_EXPORT virtual Standard_Boolean Paste (const XmlObjMgt_Persistent& aSource, const Handle(TDF_Attribute)& aTarget, XmlObjMgt_RRelocationTable& aRelocTable) const = 0;
  
  //! Translate the contents of <aSource> and put it
  //! into <aTarget>, using the relocation table
  //! <aRelocTable> to keep the sharings.
  Standard_EXPORT virtual void Paste (const Handle(TDF_Attribute)& aSource, XmlObjMgt_Persistent& aTarget, XmlObjMgt_SRelocationTable& aRelocTable) const = 0;

  //! Returns the current message driver of this driver
  const Handle(Message_Messenger)& MessageDriver() const { return myMessageDriver; }

  DEFINE_STANDARD_RTTIEXT(XmlMDF_ADriver,Standard_Transient)

protected:

  Standard_EXPORT XmlMDF_ADriver(const Handle(Message_Messenger)& theMessageDriver, const Standard_CString theNamespace, const Standard_CString theName = NULL);

  TCollection_AsciiString myTypeName;
  TCollection_AsciiString myNamespace;
  Handle(Message_Messenger) myMessageDriver;

private:

friend class XmlMDF;

};







#endif // _XmlMDF_ADriver_HeaderFile
