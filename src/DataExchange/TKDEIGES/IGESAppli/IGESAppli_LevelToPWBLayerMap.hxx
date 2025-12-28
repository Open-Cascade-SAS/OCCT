// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Anand NATRAJAN )
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IGESAppli_LevelToPWBLayerMap_HeaderFile
#define _IGESAppli_LevelToPWBLayerMap_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TCollection_HAsciiString.hxx>
#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;

//! defines LevelToPWBLayerMap, Type <406> Form <24>
//! in package IGESAppli
//! Used to correlate an exchange file level number with
//! its corresponding native level identifier, physical PWB
//! layer number and predefined functional level
//! identification
class IGESAppli_LevelToPWBLayerMap : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESAppli_LevelToPWBLayerMap();

  //! This method is used to set the fields of the class
  //! LevelToPWBLayerMap
  //! - nbPropVal       : Number of property values
  //! - allExchLevels   : Exchange File Level Numbers
  //! - allNativeLevels : Native Level Identifications
  //! - allPhysLevels   : Physical Layer Numbers
  //! - allExchIdents   : Exchange File Level Identifications
  //! raises exception if allExchLevels, allNativeLevels, allPhysLevels
  //! and all ExchIdents are not of same dimensions
  Standard_EXPORT void Init(
    const int                                                                      nbPropVal,
    const occ::handle<NCollection_HArray1<int>>&                                   allExchLevels,
    const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& allNativeLevels,
    const occ::handle<NCollection_HArray1<int>>&                                   allPhysLevels,
    const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& allExchIdents);

  //! returns number of property values
  Standard_EXPORT int NbPropertyValues() const;

  //! returns number of level to layer definitions
  Standard_EXPORT int NbLevelToLayerDefs() const;

  //! returns Exchange File Level Number
  //! raises exception if Index <= 0 or Index > NbLevelToLayerDefs
  Standard_EXPORT int ExchangeFileLevelNumber(const int Index) const;

  //! returns Native Level Identification
  //! raises exception if Index <= 0 or Index > NbLevelToLayerDefs
  Standard_EXPORT occ::handle<TCollection_HAsciiString> NativeLevel(const int Index) const;

  //! returns Physical Layer Number
  //! raises exception if Index <= 0 or Index > NbLevelToLayerDefs
  Standard_EXPORT int PhysicalLayerNumber(const int Index) const;

  Standard_EXPORT occ::handle<TCollection_HAsciiString> ExchangeFileLevelIdent(
    const int Index) const;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_LevelToPWBLayerMap, IGESData_IGESEntity)

private:
  int                                   theNbPropertyValues;
  occ::handle<NCollection_HArray1<int>> theExchangeFileLevelNumber;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> theNativeLevel;
  occ::handle<NCollection_HArray1<int>>                                   thePhysicalLayerNumber;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> theExchangeFileLevelIdent;
};

#endif // _IGESAppli_LevelToPWBLayerMap_HeaderFile
