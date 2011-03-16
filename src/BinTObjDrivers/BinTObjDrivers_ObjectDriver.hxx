// File      : BinTObjDrivers_ObjectDriver.hxx
// Created   : Wed Nov 24 11:31:42 2004
// Author    : Edward AGAPOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A


#ifndef BinTObjDrivers_ObjectDriver_HeaderFile
#define BinTObjDrivers_ObjectDriver_HeaderFile

#include <TObj_Common.hxx>
#include <BinMDF_ADriver.hxx>

class BinTObjDrivers_ObjectDriver : public BinMDF_ADriver 
{

 public:

  Standard_EXPORT BinTObjDrivers_ObjectDriver
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
  // an TObj_Object is restored by TObj_Persistence basing on class name
  // stored in Source

  Standard_EXPORT void Paste
                         (const Handle(TDF_Attribute)& Source,
                          BinObjMgt_Persistent&        Target,
                          BinObjMgt_SRelocationTable&  RelocTable) const;
  // Translate the contents of <aSource> and put it
  // into <aTarget>, using the relocation table
  // <aRelocTable> to keep the sharings.
  // anObject is stored as a Name of class derived from TObj_Object

 public:
  // CASCADE RTTI
  DEFINE_STANDARD_RTTI(BinTObjDrivers_ObjectDriver)
};

// Define handle class
DEFINE_STANDARD_HANDLE(BinTObjDrivers_ObjectDriver,BinMDF_ADriver)


#endif

#ifdef _MSC_VER
#pragma once
#endif
