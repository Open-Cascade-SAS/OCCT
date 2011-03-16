// File      : BinTObjDrivers_IntSparseArrayDriver.hxx
// Created   : 30.03.2007
// Author    : Michael SAZONOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#ifndef BinTObjDrivers_IntSparseArrayDriver_HeaderFile
#define BinTObjDrivers_IntSparseArrayDriver_HeaderFile

#include <TObj_Common.hxx>
#include <BinMDF_ADriver.hxx>

class BinTObjDrivers_IntSparseArrayDriver : public BinMDF_ADriver 
{

 public:

  Standard_EXPORT BinTObjDrivers_IntSparseArrayDriver
                         (const Handle(CDM_MessageDriver)& theMessageDriver);
  // constructor

  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const;
  // Creates a new attribute

  Standard_EXPORT Standard_Boolean Paste
                         (const BinObjMgt_Persistent&  theSource,
                          const Handle(TDF_Attribute)& theTarget,
                          BinObjMgt_RRelocationTable&  theRelocTable) const;
  // Translate the contents of <theSource> and put it
  // into <theTarget>

  Standard_EXPORT void Paste
                         (const Handle(TDF_Attribute)& theSource,
                          BinObjMgt_Persistent&        theTarget,
                          BinObjMgt_SRelocationTable&  theRelocTable) const;
  // Translate the contents of <aSource> and put it
  // into <aTarget>

 public:
  // CASCADE RTTI
  DEFINE_STANDARD_RTTI(BinTObjDrivers_IntSparseArrayDriver)
};

// Define handle class
DEFINE_STANDARD_HANDLE(BinTObjDrivers_IntSparseArrayDriver,BinMDF_ADriver)

#endif

#ifdef _MSC_VER
#pragma once
#endif
