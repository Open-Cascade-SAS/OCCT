// Created on: 1997-11-28
// Created by: Robert COUBLANC
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _AIS_ExclusionFilter_HeaderFile
#define _AIS_ExclusionFilter_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>
#include <NCollection_DataMap.hxx>
#include <SelectMgr_Filter.hxx>
#include <AIS_KindOfInteractive.hxx>
class SelectMgr_EntityOwner;

//! A framework to reject or to accept only objects of
//! given types and/or signatures.
//! Objects are stored, and the stored objects - along
//! with the flag settings - are used to define the filter.
//! Objects to be filtered are compared with the stored
//! objects added to the filter, and are accepted or
//! rejected according to the exclusion flag setting.
//! -   Exclusion flag on
//! -   the function IsOk answers true for all objects,
//! except those of the types and signatures stored
//! in the filter framework
//! -   Exclusion flag off
//! -   the function IsOk answers true for all objects
//! which have the same type and signature as the stored ones.
class AIS_ExclusionFilter : public SelectMgr_Filter
{

public:
  //! Constructs an empty exclusion filter object defined by
  //! the flag setting ExclusionFlagOn.
  //! By default, the flag is set to true.
  Standard_EXPORT AIS_ExclusionFilter(const bool ExclusionFlagOn = true);

  //! All the AIS objects of <TypeToExclude>
  //! Will be rejected by the IsOk Method.
  Standard_EXPORT AIS_ExclusionFilter(const AIS_KindOfInteractive TypeToExclude,
                                      const bool                  ExclusionFlagOn = true);

  //! Constructs an exclusion filter object defined by the
  //! enumeration value TypeToExclude, the signature
  //! SignatureInType, and the flag setting ExclusionFlagOn.
  //! By default, the flag is set to true.
  Standard_EXPORT AIS_ExclusionFilter(const AIS_KindOfInteractive TypeToExclude,
                                      const int                   SignatureInType,
                                      const bool                  ExclusionFlagOn = true);

  Standard_EXPORT bool IsOk(const occ::handle<SelectMgr_EntityOwner>& anObj) const override;

  //! Adds the type TypeToExclude to the list of types.
  Standard_EXPORT bool Add(const AIS_KindOfInteractive TypeToExclude);

  Standard_EXPORT bool Add(const AIS_KindOfInteractive TypeToExclude, const int SignatureInType);

  Standard_EXPORT bool Remove(const AIS_KindOfInteractive TypeToExclude);

  Standard_EXPORT bool Remove(const AIS_KindOfInteractive TypeToExclude, const int SignatureInType);

  Standard_EXPORT void Clear();

  bool IsExclusionFlagOn() const { return myIsExclusionFlagOn; }

  void SetExclusionFlag(const bool theStatus) { myIsExclusionFlagOn = theStatus; }

  Standard_EXPORT bool IsStored(const AIS_KindOfInteractive aType) const;

  Standard_EXPORT void ListOfStoredTypes(NCollection_List<int>& TheList) const;

  Standard_EXPORT void ListOfSignature(const AIS_KindOfInteractive aType,
                                       NCollection_List<int>&      TheStoredList) const;

  DEFINE_STANDARD_RTTIEXT(AIS_ExclusionFilter, SelectMgr_Filter)

private:
  Standard_EXPORT bool IsSignatureIn(const AIS_KindOfInteractive aType, const int aSignature) const;

  bool                                            myIsExclusionFlagOn;
  NCollection_DataMap<int, NCollection_List<int>> myStoredTypes;
};

#endif // _AIS_ExclusionFilter_HeaderFile
