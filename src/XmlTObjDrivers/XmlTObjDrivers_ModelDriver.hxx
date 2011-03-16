// File      : XmlTObjDrivers_ModelDriver.hxx
// Created   : Wed Nov 24 11:15:32 2004
// Author    : Edward AGAPOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#ifndef XmlTObjDrivers_ModelDriver_HeaderFile
#define XmlTObjDrivers_ModelDriver_HeaderFile

#include <TObj_Common.hxx>
#include <XmlMDF_ADriver.hxx>

class XmlTObjDrivers_ModelDriver : public XmlMDF_ADriver 
{

 public:

  Standard_EXPORT XmlTObjDrivers_ModelDriver
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
  // Set CurrentModel of TObj_Persistence into Target TObj_TModel
  // if its GUID and GUID stored in Source are same

  Standard_EXPORT void Paste
                         (const Handle(TDF_Attribute)& Source,
                          XmlObjMgt_Persistent&        Target,
                          XmlObjMgt_SRelocationTable&  RelocTable) const;
  // Translate the contents of <aSource> and put it
  // into <aTarget>, using the relocation table
  // <aRelocTable> to keep the sharings.
  // a Model is stored as its GUID
  
 public:
  // CASCADE RTTI
  DEFINE_STANDARD_RTTI(XmlTObjDrivers_ModelDriver)
};

// Define handle class
DEFINE_STANDARD_HANDLE(XmlTObjDrivers_ModelDriver,XmlMDF_ADriver)


#endif

#ifdef _MSC_VER
#pragma once
#endif
