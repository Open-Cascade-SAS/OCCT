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
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <IGESData_IGESEntity.hxx>
class Standard_DimensionMismatch;
class Standard_OutOfRange;
class TCollection_HAsciiString;


class IGESAppli_LevelToPWBLayerMap;
DEFINE_STANDARD_HANDLE(IGESAppli_LevelToPWBLayerMap, IGESData_IGESEntity)

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
  Standard_EXPORT void Init (const Standard_Integer nbPropVal, const Handle(TColStd_HArray1OfInteger)& allExchLevels, const Handle(Interface_HArray1OfHAsciiString)& allNativeLevels, const Handle(TColStd_HArray1OfInteger)& allPhysLevels, const Handle(Interface_HArray1OfHAsciiString)& allExchIdents);
  
  //! returns number of property values
  Standard_EXPORT Standard_Integer NbPropertyValues() const;
  
  //! returns number of level to layer definitions
  Standard_EXPORT Standard_Integer NbLevelToLayerDefs() const;
  
  //! returns Exchange File Level Number
  //! raises exception if Index <= 0 or Index > NbLevelToLayerDefs
  Standard_EXPORT Standard_Integer ExchangeFileLevelNumber (const Standard_Integer Index) const;
  
  //! returns Native Level Identification
  //! raises exception if Index <= 0 or Index > NbLevelToLayerDefs
  Standard_EXPORT Handle(TCollection_HAsciiString) NativeLevel (const Standard_Integer Index) const;
  
  //! returns Physical Layer Number
  //! raises exception if Index <= 0 or Index > NbLevelToLayerDefs
  Standard_EXPORT Standard_Integer PhysicalLayerNumber (const Standard_Integer Index) const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) ExchangeFileLevelIdent (const Standard_Integer Index) const;




  DEFINE_STANDARD_RTTIEXT(IGESAppli_LevelToPWBLayerMap,IGESData_IGESEntity)

protected:




private:


  Standard_Integer theNbPropertyValues;
  Handle(TColStd_HArray1OfInteger) theExchangeFileLevelNumber;
  Handle(Interface_HArray1OfHAsciiString) theNativeLevel;
  Handle(TColStd_HArray1OfInteger) thePhysicalLayerNumber;
  Handle(Interface_HArray1OfHAsciiString) theExchangeFileLevelIdent;


};







#endif // _IGESAppli_LevelToPWBLayerMap_HeaderFile
