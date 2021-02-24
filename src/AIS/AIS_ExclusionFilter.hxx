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

#include <Standard_Boolean.hxx>
#include <TColStd_DataMapOfIntegerListOfInteger.hxx>
#include <SelectMgr_Filter.hxx>
#include <AIS_KindOfInteractive.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_ListOfInteger.hxx>
class SelectMgr_EntityOwner;

class AIS_ExclusionFilter;
DEFINE_STANDARD_HANDLE(AIS_ExclusionFilter, SelectMgr_Filter)

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
//! -   the funciton IsOk answers true for all objects
//! which have the same type and signature as the stored ones.
class AIS_ExclusionFilter : public SelectMgr_Filter
{

public:

  
  //! Constructs an empty exclusion filter object defined by
  //! the flag setting ExclusionFlagOn.
  //! By default, the flag is set to true.
  Standard_EXPORT AIS_ExclusionFilter(const Standard_Boolean ExclusionFlagOn = Standard_True);
  
  //! All the AIS objects of <TypeToExclude>
  //! Will be rejected by the IsOk Method.
  Standard_EXPORT AIS_ExclusionFilter(const AIS_KindOfInteractive TypeToExclude, const Standard_Boolean ExclusionFlagOn = Standard_True);
  
  //! Constructs an exclusion filter object defined by the
  //! enumeration value TypeToExclude, the signature
  //! SignatureInType, and the flag setting ExclusionFlagOn.
  //! By default, the flag is set to true.
  Standard_EXPORT AIS_ExclusionFilter(const AIS_KindOfInteractive TypeToExclude, const Standard_Integer SignatureInType, const Standard_Boolean ExclusionFlagOn = Standard_True);
  
  Standard_EXPORT virtual Standard_Boolean IsOk (const Handle(SelectMgr_EntityOwner)& anObj) const Standard_OVERRIDE;
  
  //! Adds the type TypeToExclude to the list of types.
  Standard_EXPORT Standard_Boolean Add (const AIS_KindOfInteractive TypeToExclude);
  
  Standard_EXPORT Standard_Boolean Add (const AIS_KindOfInteractive TypeToExclude, const Standard_Integer SignatureInType);
  
  Standard_EXPORT Standard_Boolean Remove (const AIS_KindOfInteractive TypeToExclude);
  
  Standard_EXPORT Standard_Boolean Remove (const AIS_KindOfInteractive TypeToExclude, const Standard_Integer SignatureInType);
  
  Standard_EXPORT void Clear();

  Standard_Boolean IsExclusionFlagOn() const { return myIsExclusionFlagOn; }

  void SetExclusionFlag (const Standard_Boolean theStatus) { myIsExclusionFlagOn = theStatus; }

  Standard_EXPORT Standard_Boolean IsStored (const AIS_KindOfInteractive aType) const;
  
  Standard_EXPORT void ListOfStoredTypes (TColStd_ListOfInteger& TheList) const;
  
  Standard_EXPORT void ListOfSignature (const AIS_KindOfInteractive aType, TColStd_ListOfInteger& TheStoredList) const;

  DEFINE_STANDARD_RTTIEXT(AIS_ExclusionFilter,SelectMgr_Filter)

private:

  Standard_EXPORT Standard_Boolean IsSignatureIn (const AIS_KindOfInteractive aType, const Standard_Integer aSignature) const;

  Standard_Boolean myIsExclusionFlagOn;
  TColStd_DataMapOfIntegerListOfInteger myStoredTypes;

};

#endif // _AIS_ExclusionFilter_HeaderFile
