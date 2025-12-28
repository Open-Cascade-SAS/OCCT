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
  Standard_EXPORT void Init(const int    nbPropVal,
                            const int    aSecTolFlag,
                            const int    aTolType,
                            const int    aTolPlaceFlag,
                            const double anUpperTol,
                            const double aLowerTol,
                            const bool   aSignFlag,
                            const int    aFracFlag,
                            const int    aPrecision);

  //! returns the number of property values, always = 8
  Standard_EXPORT int NbPropertyValues() const;

  //! returns the Secondary Tolerance Flag
  Standard_EXPORT int SecondaryToleranceFlag() const;

  //! returns the Tolerance Type
  Standard_EXPORT int ToleranceType() const;

  //! returns the Tolerance Placement Flag, default = 2
  Standard_EXPORT int TolerancePlacementFlag() const;

  //! returns the Upper or Bilateral Tolerance Value
  Standard_EXPORT double UpperTolerance() const;

  //! returns the Lower Tolerance Value
  Standard_EXPORT double LowerTolerance() const;

  //! returns the Sign Suppression Flag
  Standard_EXPORT bool SignSuppressionFlag() const;

  //! returns the Fraction Flag
  Standard_EXPORT int FractionFlag() const;

  //! returns the Precision for Value Display
  Standard_EXPORT int Precision() const;

  DEFINE_STANDARD_RTTIEXT(IGESDimen_DimensionTolerance, IGESData_IGESEntity)

private:
  int    theNbPropertyValues;
  int    theSecondaryToleranceFlag;
  int    theToleranceType;
  int    theTolerancePlacementFlag;
  double theUpperTolerance;
  double theLowerTolerance;
  bool   theSignSuppressionFlag;
  int    theFractionFlag;
  int    thePrecision;
};

#endif // _IGESDimen_DimensionTolerance_HeaderFile
