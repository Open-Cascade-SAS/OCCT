// File      : XmlTObjDrivers_ReferenceDriver.hxx
// Created   : Wed Nov 24 11:19:22 2004
// Author    : Edward AGAPOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A


#ifndef XmlTObjDrivers_ReferenceDriver_HeaderFile
#define XmlTObjDrivers_ReferenceDriver_HeaderFile

#include <TObj_Common.hxx>
#include <XmlMDF_ADriver.hxx>

class XmlTObjDrivers_ReferenceDriver : public XmlMDF_ADriver 
{

 public:

  Standard_EXPORT XmlTObjDrivers_ReferenceDriver
                         (const Handle(CDM_MessageDriver)& theMessageDriver);
  // constructor

  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const;
  // Creates a new attribute

  Standard_EXPORT Standard_Boolean Paste
                         (const XmlObjMgt_Persistent&  Source,
                          const Handle(TDF_Attribute)& Target,
                          XmlObjMgt_RRelocationTable&  RelocTable) const;
  // Translate the contents of <aSource> and put it
  // into <aTarget>, using the relocation table
  // <aRelocTable> to keep the sharings.

  Standard_EXPORT void Paste
                         (const Handle(TDF_Attribute)& Source,
                          XmlObjMgt_Persistent&        Target,
                          XmlObjMgt_SRelocationTable&  RelocTable) const;
  // Translate the contents of <aSource> and put it
  // into <aTarget>, using the relocation table
  // <aRelocTable> to keep the sharings.
  // Store master and referred labels as entry, the other model referred 
  // as entry in model-container
  // The reference pointing nowhere is not stored 

 public:
  // CASCADE RTTI
  DEFINE_STANDARD_RTTI(XmlTObjDrivers_ReferenceDriver)
};

// Define handle class
DEFINE_STANDARD_HANDLE(XmlTObjDrivers_ReferenceDriver,XmlMDF_ADriver)

#endif

#ifdef _MSC_VER
#pragma once
#endif
