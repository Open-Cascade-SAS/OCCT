// Created on: 1995-11-15
// Created by: Jean-Louis Frenkel <rmi@pernox>
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _UnitsAPI_HeaderFile
#define _UnitsAPI_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_CString.hxx>
#include <UnitsAPI_SystemUnits.hxx>
class Units_Dimensions;


//! The UnitsAPI global functions are used to
//! convert a value from any unit into another unit.
//! Principles
//! Conversion is executed among three unit systems:
//! -   the SI System
//! -   the user's Local System
//! -   the user's Current System.
//! The SI System is the standard international unit
//! system. It is indicated by SI in the synopses of
//! the UnitsAPI functions.
//! The MDTV System corresponds to the SI
//! international standard but the length unit and all
//! its derivatives use millimeters instead of the meters.
//! Both systems are proposed by Open CASCADE;
//! the SI System is the standard option. By
//! selecting one of these two systems, the user
//! defines his Local System through the
//! SetLocalSystem function. The Local System is
//! indicated by LS in the synopses of the UnitsAPI functions.
//! The user's Local System units can be modified in
//! the working environment. The user defines his
//! Current System by modifying its units through
//! the SetCurrentUnit function. The Current
//! System is indicated by Current in the synopses
//! of the UnitsAPI functions.
class UnitsAPI 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Converts the current unit value to the local system units value.
  //! Example: CurrentToLS(1.,"LENGTH") returns 1000. if the current length unit
  //! is meter and LocalSystem is MDTV.
  Standard_EXPORT static Standard_Real CurrentToLS (const Standard_Real aData, const Standard_CString aQuantity);
  
  //! Converts the current unit value to the SI system units value.
  //! Example: CurrentToSI(1.,"LENGTH") returns 0.001 if current length unit
  //! is millimeter.
  Standard_EXPORT static Standard_Real CurrentToSI (const Standard_Real aData, const Standard_CString aQuantity);
  
  //! Converts the local system units value to the current unit value.
  //! Example: CurrentFromLS(1000.,"LENGTH") returns 1. if current length unit
  //! is meter and LocalSystem is MDTV.
  Standard_EXPORT static Standard_Real CurrentFromLS (const Standard_Real aData, const Standard_CString aQuantity);
  
  //! Converts the SI system units value to the current unit value.
  //! Example: CurrentFromSI(0.001,"LENGTH") returns 1 if current length unit
  //! is millimeter.
  Standard_EXPORT static Standard_Real CurrentFromSI (const Standard_Real aData, const Standard_CString aQuantity);
  
  //! Converts the local unit value to the local system units value.
  //! Example: AnyToLS(1.,"in.") returns 25.4 if the LocalSystem is MDTV.
  Standard_EXPORT static Standard_Real AnyToLS (const Standard_Real aData, const Standard_CString aUnit);
  
  //! Converts the local unit value to the local system units value.
  //! and gives the associated dimension of the unit
  Standard_EXPORT static Standard_Real AnyToLS (const Standard_Real aData, const Standard_CString aUnit, Handle(Units_Dimensions)& aDim);
  
  //! Converts the local unit value to the SI system units value.
  //! Example: AnyToSI(1.,"in.") returns 0.0254
  Standard_EXPORT static Standard_Real AnyToSI (const Standard_Real aData, const Standard_CString aUnit);
  
  //! Converts the local unit value to the SI system units value.
  //! and gives the associated dimension of the unit
  Standard_EXPORT static Standard_Real AnyToSI (const Standard_Real aData, const Standard_CString aUnit, Handle(Units_Dimensions)& aDim);
  
  //! Converts the local system units value to the local unit value.
  //! Example: AnyFromLS(25.4,"in.") returns 1. if the LocalSystem is MDTV.
  //! Note: aUnit is also used to identify the type of physical quantity to convert.
  Standard_EXPORT static Standard_Real AnyFromLS (const Standard_Real aData, const Standard_CString aUnit);
  
  //! Converts the SI system units value to the local unit value.
  //! Example: AnyFromSI(0.0254,"in.") returns 0.001
  //! Note: aUnit is also used to identify the type of physical quantity to convert.
  Standard_EXPORT static Standard_Real AnyFromSI (const Standard_Real aData, const Standard_CString aUnit);
  
  //! Converts the aData value expressed in the
  //! current unit for the working environment, as
  //! defined for the physical quantity aQuantity by the
  //! last call to the SetCurrentUnit function, into the unit aUnit.
  Standard_EXPORT static Standard_Real CurrentToAny (const Standard_Real aData, const Standard_CString aQuantity, const Standard_CString aUnit);
  
  //! Converts the aData value expressed in the unit
  //! aUnit, into the current unit for the working
  //! environment, as defined for the physical quantity
  //! aQuantity by the last call to the SetCurrentUnit function.
  Standard_EXPORT static Standard_Real CurrentFromAny (const Standard_Real aData, const Standard_CString aQuantity, const Standard_CString aUnit);
  
  //! Converts the local unit value to another local unit value.
  //! Example: AnyToAny(0.0254,"in.","millimeter") returns 1. ;
  Standard_EXPORT static Standard_Real AnyToAny (const Standard_Real aData, const Standard_CString aUnit1, const Standard_CString aUnit2);
  
  //! Converts the local system units value to the SI system unit value.
  //! Example: LSToSI(1.,"LENGTH") returns 0.001 if the local system
  //! length unit is millimeter.
  Standard_EXPORT static Standard_Real LSToSI (const Standard_Real aData, const Standard_CString aQuantity);
  
  //! Converts the SI system unit value to the local system units value.
  //! Example: SIToLS(1.,"LENGTH") returns 1000. if the local system
  //! length unit is millimeter.
  Standard_EXPORT static Standard_Real SIToLS (const Standard_Real aData, const Standard_CString aQuantity);
  
  //! Sets the local system units.
  //! Example: SetLocalSystem(UnitsAPI_MDTV)
  Standard_EXPORT static void SetLocalSystem (const UnitsAPI_SystemUnits aSystemUnit = UnitsAPI_SI);
  
  //! Returns the current local system units.
  Standard_EXPORT static UnitsAPI_SystemUnits LocalSystem();
  
  //! Sets the current unit dimension <aUnit> to the unit quantity <aQuantity>.
  //! Example: SetCurrentUnit("LENGTH","millimeter")
  Standard_EXPORT static void SetCurrentUnit (const Standard_CString aQuantity, const Standard_CString aUnit);
  
  //! Returns the current unit dimension <aUnit> from the unit quantity <aQuantity>.
  Standard_EXPORT static Standard_CString CurrentUnit (const Standard_CString aQuantity);
  
  //! saves the units in the file .CurrentUnits of the directory pointed by the
  //! CSF_CurrentUnitsUserDefaults environment variable.
  Standard_EXPORT static void Save();
  
  Standard_EXPORT static void Reload();
  
  //! return the dimension associated to the quantity
  Standard_EXPORT static Handle(Units_Dimensions) Dimensions (const Standard_CString aQuantity);
  
  Standard_EXPORT static Handle(Units_Dimensions) DimensionLess();
  
  Standard_EXPORT static Handle(Units_Dimensions) DimensionMass();
  
  Standard_EXPORT static Handle(Units_Dimensions) DimensionLength();
  
  Standard_EXPORT static Handle(Units_Dimensions) DimensionTime();
  
  Standard_EXPORT static Handle(Units_Dimensions) DimensionElectricCurrent();
  
  Standard_EXPORT static Handle(Units_Dimensions) DimensionThermodynamicTemperature();
  
  Standard_EXPORT static Handle(Units_Dimensions) DimensionAmountOfSubstance();
  
  Standard_EXPORT static Handle(Units_Dimensions) DimensionLuminousIntensity();
  
  Standard_EXPORT static Handle(Units_Dimensions) DimensionPlaneAngle();
  
  //! Returns the basic dimensions.
  Standard_EXPORT static Handle(Units_Dimensions) DimensionSolidAngle();
  
  //! Checks the coherence between the quantity <aQuantity>
  //! and the unit <aUnits> in the current system and
  //! returns FALSE when it's WRONG.
  Standard_EXPORT static Standard_Boolean Check (const Standard_CString aQuantity, const Standard_CString aUnit);




protected:





private:

  
  Standard_EXPORT static void CheckLoading (const UnitsAPI_SystemUnits aSystemUnit);




};







#endif // _UnitsAPI_HeaderFile
