// Created on: 1992-06-24
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

#include <Standard_Type.hxx>
#include <Units.hxx>
#include <Units_Dimensions.hxx>
#include <Units_Operators.hxx>
#include <Units_UnitsDictionary.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Units_Dimensions, Standard_Transient)

//=================================================================================================

Units_Dimensions::Units_Dimensions(const double amass,
                                   const double alength,
                                   const double atime,
                                   const double anelectriccurrent,
                                   const double athermodynamictemperature,
                                   const double anamountofsubstance,
                                   const double aluminousintensity,
                                   const double aplaneangle,
                                   const double asolidangle)
{
  themass                     = amass;
  thelength                   = alength;
  thetime                     = atime;
  theelectriccurrent          = anelectriccurrent;
  thethermodynamictemperature = athermodynamictemperature;
  theamountofsubstance        = anamountofsubstance;
  theluminousintensity        = aluminousintensity;
  theplaneangle               = aplaneangle;
  thesolidangle               = asolidangle;
}

//=======================================================================
// function : Quantity

// purpose  : gets the string quantity from dimension parameter
// Warning  : Bo Bo Tetete,take an aspirine !!
//	     If you found a more simple way ,phone to GG,thanks ...
//=======================================================================

static occ::handle<NCollection_HSequence<occ::handle<Units_Quantity>>> quantitySequence;
static TCollection_AsciiString                                         quantityName;

const char* Units_Dimensions::Quantity() const
{
  if (quantitySequence.IsNull())
  {
    quantitySequence = Units::DictionaryOfUnits(false)->Sequence();
  }
  occ::handle<Units_Dimensions> dim;
  for (int i = 1; i <= quantitySequence->Length(); i++)
  {
    dim = quantitySequence->Value(i)->Dimensions();
    if (themass == dim->Mass() && thelength == dim->Length() && thetime == dim->Time()
        && theelectriccurrent == dim->ElectricCurrent()
        && thethermodynamictemperature == dim->ThermodynamicTemperature()
        && theamountofsubstance == dim->AmountOfSubstance()
        && theluminousintensity == dim->LuminousIntensity() && theplaneangle == dim->PlaneAngle()
        && thesolidangle == dim->SolidAngle())
    {
      quantityName = quantitySequence->Value(i)->Name();
      return quantityName.ToCString();
    }
  }
  return nullptr;
}

//=================================================================================================

occ::handle<Units_Dimensions> Units_Dimensions::Multiply(
  const occ::handle<Units_Dimensions>& adimensions) const
{
  return new Units_Dimensions(themass + adimensions->Mass(),
                              thelength + adimensions->Length(),
                              thetime + adimensions->Time(),
                              theelectriccurrent + adimensions->ElectricCurrent(),
                              thethermodynamictemperature + adimensions->ThermodynamicTemperature(),
                              theamountofsubstance + adimensions->AmountOfSubstance(),
                              theluminousintensity + adimensions->LuminousIntensity(),
                              theplaneangle + adimensions->PlaneAngle(),
                              thesolidangle + adimensions->SolidAngle());
}

//=================================================================================================

occ::handle<Units_Dimensions> Units_Dimensions::Divide(
  const occ::handle<Units_Dimensions>& adimensions) const
{
  return new Units_Dimensions(themass - adimensions->Mass(),
                              thelength - adimensions->Length(),
                              thetime - adimensions->Time(),
                              theelectriccurrent - adimensions->ElectricCurrent(),
                              thethermodynamictemperature - adimensions->ThermodynamicTemperature(),
                              theamountofsubstance - adimensions->AmountOfSubstance(),
                              theluminousintensity - adimensions->LuminousIntensity(),
                              theplaneangle - adimensions->PlaneAngle(),
                              thesolidangle - adimensions->SolidAngle());
}

//=================================================================================================

occ::handle<Units_Dimensions> Units_Dimensions::Power(const double anexponent) const
{
  return new Units_Dimensions(themass * anexponent,
                              thelength * anexponent,
                              thetime * anexponent,
                              theelectriccurrent * anexponent,
                              thethermodynamictemperature * anexponent,
                              theamountofsubstance * anexponent,
                              theluminousintensity * anexponent,
                              theplaneangle * anexponent,
                              thesolidangle * anexponent);
}

//=================================================================================================

bool Units_Dimensions::IsEqual(const occ::handle<Units_Dimensions>& adimensions) const
{
  return (
    themass == adimensions->Mass() && thelength == adimensions->Length()
        && thetime == adimensions->Time() && theelectriccurrent == adimensions->ElectricCurrent()
        && thethermodynamictemperature == adimensions->ThermodynamicTemperature()
        && theamountofsubstance == adimensions->AmountOfSubstance()
        && theluminousintensity == adimensions->LuminousIntensity()
        && theplaneangle == adimensions->PlaneAngle() && thesolidangle == adimensions->SolidAngle());
}

//=================================================================================================

bool Units_Dimensions::IsNotEqual(const occ::handle<Units_Dimensions>& adimensions) const
{
  return !(IsEqual(adimensions));
}

//=================================================================================================

void Units_Dimensions::Dump(const int ashift) const
{
  int i;
  for (i = 0; i < ashift; i++)
    std::cout << "  ";
  std::cout << " with the physical dimensions : " << std::endl;
  for (i = 0; i < ashift; i++)
    std::cout << "  ";
  std::cout << "         mass                      : " << themass << std::endl;
  for (i = 0; i < ashift; i++)
    std::cout << "  ";
  std::cout << "         length                    : " << thelength << std::endl;
  for (i = 0; i < ashift; i++)
    std::cout << "  ";
  std::cout << "         time                      : " << thetime << std::endl;
  for (i = 0; i < ashift; i++)
    std::cout << "  ";
  std::cout << "         electric current          : " << theelectriccurrent << std::endl;
  for (i = 0; i < ashift; i++)
    std::cout << "  ";
  std::cout << "         thermodynamic temperature : " << thethermodynamictemperature << std::endl;
  for (i = 0; i < ashift; i++)
    std::cout << "  ";
  std::cout << "         amount of substance       : " << theamountofsubstance << std::endl;
  for (i = 0; i < ashift; i++)
    std::cout << "  ";
  std::cout << "         luminous intensity        : " << theluminousintensity << std::endl;
  for (i = 0; i < ashift; i++)
    std::cout << "  ";
  std::cout << "         plane angle               : " << theplaneangle << std::endl;
  for (i = 0; i < ashift; i++)
    std::cout << "  ";
  std::cout << "         solid angle               : " << thesolidangle << std::endl;
}

//=======================================================================
// function : operator *
// purpose  :
//=======================================================================

occ::handle<Units_Dimensions> operator*(const occ::handle<Units_Dimensions>& adimension1,
                                        const occ::handle<Units_Dimensions>& adimension2)
{
  return adimension1->Multiply(adimension2);
}

//=======================================================================
// function : operator /
// purpose  :
//=======================================================================

occ::handle<Units_Dimensions> operator/(const occ::handle<Units_Dimensions>& adimension1,
                                        const occ::handle<Units_Dimensions>& adimension2)
{
  return adimension1->Divide(adimension2);
}

//=================================================================================================

occ::handle<Units_Dimensions> pow(const occ::handle<Units_Dimensions>& adimension,
                                  const double                         areal)
{
  return adimension->Power(areal);
}

//=======================================================================
// function : operator ==
// purpose  :
//=======================================================================

// bool operator ==(const occ::handle<Units_Dimensions>& adimension1,
//			     const occ::handle<Units_Dimensions>& adimension2)
//{
//   return adimension1->IsEqual(adimension2);
// }

//=======================================================================
// function : operator !=
// purpose  :
//=======================================================================

// bool operator !=(const occ::handle<Units_Dimensions>& adimension1,
//			     const occ::handle<Units_Dimensions>& adimension2)
//{
//   return adimension1->IsNotEqual(adimension2);
// }

occ::handle<Units_Dimensions> Units_Dimensions::ALess()
{
  static occ::handle<Units_Dimensions> aDim;
  if (aDim.IsNull())
    aDim = new Units_Dimensions(0., 0., 0., 0., 0., 0., 0., 0., 0.);
  return aDim;
}

occ::handle<Units_Dimensions> Units_Dimensions::AMass()
{
  static occ::handle<Units_Dimensions> aDim;
  if (aDim.IsNull())
    aDim = new Units_Dimensions(1., 0., 0., 0., 0., 0., 0., 0., 0.);
  return aDim;
}

occ::handle<Units_Dimensions> Units_Dimensions::ALength()
{
  static occ::handle<Units_Dimensions> aDim;
  if (aDim.IsNull())
    aDim = new Units_Dimensions(0., 1., 0., 0., 0., 0., 0., 0., 0.);
  return aDim;
}

occ::handle<Units_Dimensions> Units_Dimensions::ATime()
{
  static occ::handle<Units_Dimensions> aDim;
  if (aDim.IsNull())
    aDim = new Units_Dimensions(0., 0., 1., 0., 0., 0., 0., 0., 0.);
  return aDim;
}

occ::handle<Units_Dimensions> Units_Dimensions::AElectricCurrent()
{
  static occ::handle<Units_Dimensions> aDim;
  if (aDim.IsNull())
    aDim = new Units_Dimensions(0., 0., 0., 1., 0., 0., 0., 0., 0.);
  return aDim;
}

occ::handle<Units_Dimensions> Units_Dimensions::AThermodynamicTemperature()
{
  static occ::handle<Units_Dimensions> aDim;
  if (aDim.IsNull())
    aDim = new Units_Dimensions(0., 0., 0., 0., 1., 0., 0., 0., 0.);
  return aDim;
}

occ::handle<Units_Dimensions> Units_Dimensions::AAmountOfSubstance()
{
  static occ::handle<Units_Dimensions> aDim;
  if (aDim.IsNull())
    aDim = new Units_Dimensions(0., 0., 0., 0., 0., 1., 0., 0., 0.);
  return aDim;
}

occ::handle<Units_Dimensions> Units_Dimensions::ALuminousIntensity()
{
  static occ::handle<Units_Dimensions> aDim;
  if (aDim.IsNull())
    aDim = new Units_Dimensions(0., 0., 0., 0., 0., 0., 1., 0., 0.);
  return aDim;
}

occ::handle<Units_Dimensions> Units_Dimensions::APlaneAngle()
{
  static occ::handle<Units_Dimensions> aDim;
  if (aDim.IsNull())
    aDim = new Units_Dimensions(0., 0., 0., 0., 0., 0., 0., 1., 0.);
  return aDim;
}

occ::handle<Units_Dimensions> Units_Dimensions::ASolidAngle()
{
  static occ::handle<Units_Dimensions> aDim;
  if (aDim.IsNull())
    aDim = new Units_Dimensions(0., 0., 0., 0., 0., 0., 0., 0., 1.);
  return aDim;
}
