// File:      TObj_HiddenPartition.hxx
// Created:   Tue Nov 23 11:38:32 2004
// Autho:     Pavel TELKOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#ifndef TObj_HiddenPartition_HeaderFile
#define TObj_HiddenPartition_HeaderFile

#include <TObj_Partition.hxx>

/**
 * This class is partition is predefined hidden flag
*/
  
class TObj_HiddenPartition : public TObj_Partition
{
 public:
  //! constructor
  Standard_EXPORT TObj_HiddenPartition (const TDF_Label& theLabel);

  //! Returns all flags of father except Visible
  virtual Standard_EXPORT Standard_Integer GetTypeFlags() const;

 protected:
  //! Persistence of TObj object
  DECLARE_TOBJOCAF_PERSISTENCE(TObj_HiddenPartition,TObj_Partition)
 public:
  //! CASCADE RTTI
  DEFINE_STANDARD_RTTI(TObj_HiddenPartition)
};

//! Define handle class for TObj_HiddenPartition
DEFINE_STANDARD_HANDLE(TObj_HiddenPartition,TObj_Partition)


#endif

#ifdef _MSC_VER
#pragma once
#endif
