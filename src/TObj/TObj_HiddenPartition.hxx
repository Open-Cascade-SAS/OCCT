// Created on: 2004-11-23
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

// Autho:     Pavel TELKOV
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
  virtual Standard_EXPORT Standard_Integer GetTypeFlags() const Standard_OVERRIDE;

 protected:
  //! Persistence of TObj object
  DECLARE_TOBJOCAF_PERSISTENCE(TObj_HiddenPartition,TObj_Partition)
 public:
  //! CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(TObj_HiddenPartition,TObj_Partition)
};

//! Define handle class for TObj_HiddenPartition
DEFINE_STANDARD_HANDLE(TObj_HiddenPartition,TObj_Partition)


#endif

#ifdef _MSC_VER
#pragma once
#endif
