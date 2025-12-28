// Created on: 1992-06-22
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _Units_Dimensions_HeaderFile
#define _Units_Dimensions_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>

//! This class includes all the methods to create and
//! manipulate the dimensions of the physical
//! quantities.
class Units_Dimensions : public Standard_Transient
{

public:
  //! Returns a Dimensions object which represents the
  //! dimension of a physical quantity. Each of the
  //! <amass>, <alength>, <atime>, <anelectriccurrent>,
  //! <athermodynamictemperature>, <anamountofsubstance>,
  //! <aluminousintensity>, <aplaneangle>, <asolidangle> are
  //! the powers for the 7 fundamental units of physical
  //! quantity and the 2 secondary fundamental units of
  //! physical quantity.
  Standard_EXPORT Units_Dimensions(const double amass,
                                   const double alength,
                                   const double atime,
                                   const double anelectriccurrent,
                                   const double athermodynamictemperature,
                                   const double anamountofsubstance,
                                   const double aluminousintensity,
                                   const double aplaneangle,
                                   const double asolidangle);

  //! Returns the power of mass stored in the dimensions.
  double Mass() const;

  //! Returns the power of length stored in the dimensions.
  double Length() const;

  //! Returns the power of time stored in the dimensions.
  double Time() const;

  //! Returns the power of electrical intensity (current)
  //! stored in the dimensions.
  double ElectricCurrent() const;

  //! Returns the power of temperature stored in the
  //! dimensions.
  double ThermodynamicTemperature() const;

  //! Returns the power of quantity of material (mole)
  //! stored in the dimensions.
  double AmountOfSubstance() const;

  //! Returns the power of light intensity stored in the
  //! dimensions.
  double LuminousIntensity() const;

  //! Returns the power of plane angle stored in the
  //! dimensions.
  double PlaneAngle() const;

  //! Returns the power of solid angle stored in the
  //! dimensions.
  double SolidAngle() const;

  //! Returns the quantity string of the dimension
  Standard_EXPORT const char* Quantity() const;

  //! Creates and returns a new Dimensions object which is
  //! the result of the multiplication of <me> and
  //! <adimensions>.
  Standard_EXPORT occ::handle<Units_Dimensions> Multiply(
    const occ::handle<Units_Dimensions>& adimensions) const;

  //! Creates and returns a new Dimensions object which is
  //! the result of the division of <me> by <adimensions>.
  Standard_EXPORT occ::handle<Units_Dimensions> Divide(
    const occ::handle<Units_Dimensions>& adimensions) const;

  //! Creates and returns a new Dimensions object which is
  //! the result of the power of <me> and <anexponent>.
  Standard_EXPORT occ::handle<Units_Dimensions> Power(const double anexponent) const;

  //! Returns true if <me> and <adimensions> have the same
  //! dimensions, false otherwise.
  Standard_EXPORT bool IsEqual(const occ::handle<Units_Dimensions>& adimensions) const;

  //! Returns false if <me> and <adimensions> have the same
  //! dimensions, true otherwise.
  Standard_EXPORT bool IsNotEqual(const occ::handle<Units_Dimensions>& adimensions) const;

  //! Useful for degugging.
  Standard_EXPORT void Dump(const int ashift) const;

  Standard_EXPORT static occ::handle<Units_Dimensions> ALess();

  Standard_EXPORT static occ::handle<Units_Dimensions> AMass();

  Standard_EXPORT static occ::handle<Units_Dimensions> ALength();

  Standard_EXPORT static occ::handle<Units_Dimensions> ATime();

  Standard_EXPORT static occ::handle<Units_Dimensions> AElectricCurrent();

  Standard_EXPORT static occ::handle<Units_Dimensions> AThermodynamicTemperature();

  Standard_EXPORT static occ::handle<Units_Dimensions> AAmountOfSubstance();

  Standard_EXPORT static occ::handle<Units_Dimensions> ALuminousIntensity();

  Standard_EXPORT static occ::handle<Units_Dimensions> APlaneAngle();

  //! Returns the basic dimensions.
  Standard_EXPORT static occ::handle<Units_Dimensions> ASolidAngle();

  DEFINE_STANDARD_RTTIEXT(Units_Dimensions, Standard_Transient)

private:
  double themass;
  double thelength;
  double thetime;
  double theelectriccurrent;
  double thethermodynamictemperature;
  double theamountofsubstance;
  double theluminousintensity;
  double theplaneangle;
  double thesolidangle;
};

#include <Units_Dimensions.lxx>

#endif // _Units_Dimensions_HeaderFile
