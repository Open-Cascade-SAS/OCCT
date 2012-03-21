// Created on: 2004-11-23
// Copyright (c) 2004-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
