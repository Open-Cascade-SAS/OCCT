// Created on: 2002-10-29
// Created by: Michael SAZONOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _BinMDF_ADriver_HeaderFile
#define _BinMDF_ADriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_CString.hxx>
#include <Standard_Type.hxx>
#include <Standard_Boolean.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>
class CDM_MessageDriver;
class TDF_Attribute;
class TCollection_AsciiString;
class BinObjMgt_Persistent;
class TCollection_ExtendedString;


class BinMDF_ADriver;
DEFINE_STANDARD_HANDLE(BinMDF_ADriver, Standard_Transient)

//! Attribute Storage/Retrieval Driver.
class BinMDF_ADriver : public Standard_Transient
{

public:

  
  //! Creates a new attribute from TDF.
  Standard_EXPORT virtual Handle(TDF_Attribute) NewEmpty() const = 0;
  
  //! Returns the type of source object,
  //! inheriting from Attribute from TDF.
    const Handle(Standard_Type)& SourceType() const;
  
  //! Returns the type name of the attribute object
    const TCollection_AsciiString& TypeName() const;
  
  //! Translate the contents of <aSource> and put it
  //! into <aTarget>, using the relocation table
  //! <aRelocTable> to keep the sharings.
  Standard_EXPORT virtual Standard_Boolean Paste (const BinObjMgt_Persistent& aSource, const Handle(TDF_Attribute)& aTarget, BinObjMgt_RRelocationTable& aRelocTable) const = 0;
  
  //! Translate the contents of <aSource> and put it
  //! into <aTarget>, using the relocation table
  //! <aRelocTable> to keep the sharings.
  Standard_EXPORT virtual void Paste (const Handle(TDF_Attribute)& aSource, BinObjMgt_Persistent& aTarget, BinObjMgt_SRelocationTable& aRelocTable) const = 0;
  
  //! Send message to Application (usually when error occurres)
  Standard_EXPORT void WriteMessage (const TCollection_ExtendedString& theMessage) const;




  DEFINE_STANDARD_RTTIEXT(BinMDF_ADriver,Standard_Transient)

protected:

  
  Standard_EXPORT BinMDF_ADriver(const Handle(CDM_MessageDriver)& theMsgDriver, const Standard_CString theName = NULL);

  TCollection_AsciiString myTypeName;


private:


  Handle(CDM_MessageDriver) myMessageDriver;


};


#include <BinMDF_ADriver.lxx>





#endif // _BinMDF_ADriver_HeaderFile
