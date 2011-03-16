// File      : BinTObjDrivers_ModelDriver.hxx
// Created   : Wed Nov 24 11:28:12 2004
// Author    : Edward AGAPOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#ifndef BinTObjDrivers_ModelDriver_HeaderFile
#define BinTObjDrivers_ModelDriver_HeaderFile

#include <TObj_Common.hxx>
#include <BinMDF_ADriver.hxx>

class BinTObjDrivers_ModelDriver : public BinMDF_ADriver 
{

 public:

  Standard_EXPORT BinTObjDrivers_ModelDriver
                         (const Handle(CDM_MessageDriver)& theMessageDriver);
  // constructor

  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const;
  // Creates a new attribute

  Standard_EXPORT Standard_Boolean Paste
                         (const BinObjMgt_Persistent&  Source,
                          const Handle(TDF_Attribute)& Target,
                          BinObjMgt_RRelocationTable&  RelocTable) const;
  // Translate the contents of <aSource> and put it
  // into <aTarget>, using the relocation table
  // <aRelocTable> to keep the sharings.
  // Set CurrentModel of TObj_Persistence into Target TObj_TModel
  // if its GUID and GUID stored in Source are same

  Standard_EXPORT void Paste
                         (const Handle(TDF_Attribute)& Source,
                          BinObjMgt_Persistent&        Target,
                          BinObjMgt_SRelocationTable&  RelocTable) const;
  // Translate the contents of <aSource> and put it
  // into <aTarget>, using the relocation table
  // <aRelocTable> to keep the sharings.
  // a Model is stored as its GUID
  
 public:
  // CASCADE RTTI
  DEFINE_STANDARD_RTTI(BinTObjDrivers_ModelDriver)
};

// Define handle class
DEFINE_STANDARD_HANDLE(BinTObjDrivers_ModelDriver,BinMDF_ADriver)


#endif

#ifdef _MSC_VER
#pragma once
#endif
