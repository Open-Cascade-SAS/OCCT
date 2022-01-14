// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen (Anand NATRAJAN)
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

#ifndef _IGESDimen_DimensionTolerance_HeaderFile
#define _IGESDimen_DimensionTolerance_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <IGESData_IGESEntity.hxx>


class IGESDimen_DimensionTolerance;
DEFINE_STANDARD_HANDLE(IGESDimen_DimensionTolerance, IGESData_IGESEntity)

//! defines Dimension Tolerance, Type <406>, Form <29>
//! in package IGESDimen
//! Provides tolerance information for a dimension which
//! can be used by the receiving system to regenerate the
//! dimension.
class IGESDimen_DimensionTolerance : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESDimen_DimensionTolerance();
  
  //! This method is used to set the fields of the class
  //! DimensionTolerance
  //! - nbPropVal     : Number of property values, default = 8
  //! - aSecTolFlag   : Secondary Tolerance Flag
  //! 0 = Applies to primary dimension
  //! 1 = Applies to secondary dimension
  //! 2 = Display values as fractions
  //! - aTolType      : Tolerance Type
  //! 1  = Bilateral
  //! 2  = Upper/Lower
  //! 3  = Unilateral Upper
  //! 4  = Unilateral Lower
  //! 5  = Range - min before max
  //! 6  = Range - min after max
  //! 7  = Range - min above max
  //! 8  = Range - min below max
  //! 9  = Nominal + Range - min above max
  //! 10 = Nominal + Range - min below max
  //! - aTolPlaceFlag : Tolerance Placement Flag
  //! 1 = Before nominal value
  //! 2 = After nominal value
  //! 3 = Above nominal value
  //! 4 = Below nominal value
  //! - anUpperTol    : Upper Tolerance
  //! - aLowerTol     : Lower Tolerance
  //! - aSignFlag     : Sign Suppression Flag
  //! - aFracFlag     : Fraction Flag
  //! 0 = Display values as decimal numbers
  //! 1 = Display values as mixed fractions
  //! 2 = Display values as fractions
  //! - aPrecision    : Precision Value
  Standard_EXPORT void Init (const Standard_Integer nbPropVal, const Standard_Integer aSecTolFlag, const Standard_Integer aTolType, const Standard_Integer aTolPlaceFlag, const Standard_Real anUpperTol, const Standard_Real aLowerTol, const Standard_Boolean aSignFlag, const Standard_Integer aFracFlag, const Standard_Integer aPrecision);
  
  //! returns the number of property values, always = 8
  Standard_EXPORT Standard_Integer NbPropertyValues() const;
  
  //! returns the Secondary Tolerance Flag
  Standard_EXPORT Standard_Integer SecondaryToleranceFlag() const;
  
  //! returns the Tolerance Type
  Standard_EXPORT Standard_Integer ToleranceType() const;
  
  //! returns the Tolerance Placement Flag, default = 2
  Standard_EXPORT Standard_Integer TolerancePlacementFlag() const;
  
  //! returns the Upper or Bilateral Tolerance Value
  Standard_EXPORT Standard_Real UpperTolerance() const;
  
  //! returns the Lower Tolerance Value
  Standard_EXPORT Standard_Real LowerTolerance() const;
  
  //! returns the Sign Suppression Flag
  Standard_EXPORT Standard_Boolean SignSuppressionFlag() const;
  
  //! returns the Fraction Flag
  Standard_EXPORT Standard_Integer FractionFlag() const;
  
  //! returns the Precision for Value Display
  Standard_EXPORT Standard_Integer Precision() const;




  DEFINE_STANDARD_RTTIEXT(IGESDimen_DimensionTolerance,IGESData_IGESEntity)

protected:




private:


  Standard_Integer theNbPropertyValues;
  Standard_Integer theSecondaryToleranceFlag;
  Standard_Integer theToleranceType;
  Standard_Integer theTolerancePlacementFlag;
  Standard_Real theUpperTolerance;
  Standard_Real theLowerTolerance;
  Standard_Boolean theSignSuppressionFlag;
  Standard_Integer theFractionFlag;
  Standard_Integer thePrecision;


};







#endif // _IGESDimen_DimensionTolerance_HeaderFile
