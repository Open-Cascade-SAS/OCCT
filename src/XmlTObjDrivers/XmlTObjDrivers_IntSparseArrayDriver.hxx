// File      : XmlTObjDrivers_IntSparseArrayDriver.hxx
// Created   : 30.03.2007
// Author    : Michael SAZONOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#ifndef XmlTObjDrivers_IntSparseArrayDriver_HeaderFile
#define XmlTObjDrivers_IntSparseArrayDriver_HeaderFile

#include <TObj_Common.hxx>
#include <XmlMDF_ADriver.hxx>

class XmlTObjDrivers_IntSparseArrayDriver : public XmlMDF_ADriver 
{

 public:

  Standard_EXPORT XmlTObjDrivers_IntSparseArrayDriver
                         (const Handle(CDM_MessageDriver)& theMessageDriver);
  // constructor

  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const;
  // Creates a new attribute

  Standard_EXPORT Standard_Boolean Paste
                         (const XmlObjMgt_Persistent&  theSource,
                          const Handle(TDF_Attribute)& theTarget,
                          XmlObjMgt_RRelocationTable&  theRelocTable) const;
  // Translate the contents of <theSource> and put it
  // into <theTarget>

  Standard_EXPORT void Paste
                         (const Handle(TDF_Attribute)& theSource,
                          XmlObjMgt_Persistent&        theTarget,
                          XmlObjMgt_SRelocationTable&  theRelocTable) const;
  // Translate the contents of <aSource> and put it
  // into <aTarget>

 public:
  // CASCADE RTTI
  DEFINE_STANDARD_RTTI(XmlTObjDrivers_IntSparseArrayDriver)
};

// Define handle class
DEFINE_STANDARD_HANDLE(XmlTObjDrivers_IntSparseArrayDriver,XmlMDF_ADriver)

#endif

#ifdef _MSC_VER
#pragma once
#endif
