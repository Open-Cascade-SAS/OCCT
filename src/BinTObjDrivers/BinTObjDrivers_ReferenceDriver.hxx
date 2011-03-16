// File      : BinTObjDrivers_ReferenceDriver.hxx
// Created   : Wed Nov 24 11:32:18 2004
// Author    : Edward AGAPOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A


#ifndef BinTObjDrivers_ReferenceDriver_HeaderFile
#define BinTObjDrivers_ReferenceDriver_HeaderFile

#include <TObj_Common.hxx>
#include <BinMDF_ADriver.hxx>

class BinTObjDrivers_ReferenceDriver : public BinMDF_ADriver 
{

 public:

  Standard_EXPORT BinTObjDrivers_ReferenceDriver
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

  Standard_EXPORT void Paste
                         (const Handle(TDF_Attribute)& Source,
                          BinObjMgt_Persistent&        Target,
                          BinObjMgt_SRelocationTable&  RelocTable) const;
  // Translate the contents of <aSource> and put it
  // into <aTarget>, using the relocation table
  // <aRelocTable> to keep the sharings.
  // Store master and referred labels as entry, the other model referred 
  // as entry in model-container
  // The reference pointing nowhere is not stored 

 public:
  // CASCADE RTTI
  DEFINE_STANDARD_RTTI(BinTObjDrivers_ReferenceDriver)
};

// Define handle class
DEFINE_STANDARD_HANDLE(BinTObjDrivers_ReferenceDriver,BinMDF_ADriver)



#endif

#ifdef _MSC_VER
#pragma once
#endif
