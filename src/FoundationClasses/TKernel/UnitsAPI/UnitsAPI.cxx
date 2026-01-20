// Copyright (c) 1998-1999 Matra Datavision
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

#include <OSD_Environment.hxx>
#include <Resource_Manager.hxx>
#include <TCollection_AsciiString.hxx>
#include <Units.hxx>
#include <Units_UnitsSystem.hxx>
#include <UnitsAPI.hxx>

static occ::handle<Resource_Manager> CurrentUnits, SICurrentUnits, MDTVCurrentUnits;
static Units_UnitsSystem        LocalSystemUnits, SILocalSystemUnits, MDTVLocalSystemUnits;
static TCollection_AsciiString  rstring;
static UnitsAPI_SystemUnits     localSystem   = UnitsAPI_SI;
static UnitsAPI_SystemUnits     currentSystem = UnitsAPI_DEFAULT;

//=================================================================================================

void UnitsAPI::CheckLoading(const UnitsAPI_SystemUnits aSystemUnits)
{
  if (currentSystem != aSystemUnits || CurrentUnits.IsNull())
  {
    switch (aSystemUnits)
    {
      case UnitsAPI_DEFAULT:
        if (!CurrentUnits.IsNull())
          break;
        [[fallthrough]];
      case UnitsAPI_SI:
        currentSystem = UnitsAPI_SI;
        if (SICurrentUnits.IsNull())
        {
#ifdef _WIN32
          OSD_Environment         env3("CSF_CurrentUnits");
          TCollection_AsciiString csfcurrent(env3.Value());
          if (csfcurrent.Length() > 0)
            SICurrentUnits = new Resource_Manager(csfcurrent.ToCString());
          else
            SICurrentUnits = new Resource_Manager("CurrentUnits");
#else
          SICurrentUnits = new Resource_Manager("CurrentUnits");
#endif
        }
        CurrentUnits     = SICurrentUnits;
        LocalSystemUnits = SILocalSystemUnits;
        break;
      case UnitsAPI_MDTV:
        currentSystem = UnitsAPI_MDTV;
        if (MDTVCurrentUnits.IsNull())
        {
#ifdef _WIN32
          OSD_Environment         env4("CSF_MDTVCurrentUnits");
          TCollection_AsciiString csfmdtvcurrent(env4.Value());
          if (csfmdtvcurrent.Length() > 0)
            MDTVCurrentUnits = new Resource_Manager(csfmdtvcurrent.ToCString());
          else
            MDTVCurrentUnits = new Resource_Manager("MDTVCurrentUnits");
#else
          MDTVCurrentUnits = new Resource_Manager("MDTVCurrentUnits");
#endif
        }
        CurrentUnits = MDTVCurrentUnits;
        if (MDTVLocalSystemUnits.IsEmpty())
        {
          MDTVLocalSystemUnits.Specify("LENGTH", "mm");
          MDTVLocalSystemUnits.Specify("AREA", "mm\xB2");
          MDTVLocalSystemUnits.Specify("VOLUME", "mm\xB3");
          MDTVLocalSystemUnits.Specify("INERTIA", "mm**4");
          MDTVLocalSystemUnits.Specify("SPEED", "mm/s");
          MDTVLocalSystemUnits.Specify("ACCELERATION", "mm/s\xB2");
          MDTVLocalSystemUnits.Specify("VOLUMIC MASS", "kg/mm\xB3");
          MDTVLocalSystemUnits.Specify("VOLUME FLOW", "mm\xB3/s");
          MDTVLocalSystemUnits.Specify("CONSUMPTION", "mm\xB2");
          MDTVLocalSystemUnits.Specify("QUANTITY OF MOVEMENT", "kg*mm/s");
          MDTVLocalSystemUnits.Specify("KINETIC MOMENT", "kg*mm\xB2/s");
          MDTVLocalSystemUnits.Specify("MOMENT OF INERTIA", "kg*mm\xB2");
          MDTVLocalSystemUnits.Specify("FORCE", "kg*mm/s\xB2");
          MDTVLocalSystemUnits.Specify("LINEIC FORCE", "kg/s\xB2");
          MDTVLocalSystemUnits.Specify("MOMENT OF A FORCE", "kg*mm\xB2/s\xB2");
          MDTVLocalSystemUnits.Specify("PRESSURE", "kg/(mm*s\xB2)");
          MDTVLocalSystemUnits.Specify("DYNAMIC VISCOSITY", "kg/(mm*s)");
          MDTVLocalSystemUnits.Specify("KINETIC VISCOSITY", "mm\xB2/s");
          MDTVLocalSystemUnits.Specify("TENSION SUPERFICIELLE", "mm/s\xB2");
          MDTVLocalSystemUnits.Specify("ENERGY", "kg*mm\xB2/s\xB2");
          MDTVLocalSystemUnits.Specify("POWER", "kg*mm\xB2/s\xB3");
          MDTVLocalSystemUnits.Specify("LINEIC POWER", "kg*mm/s\xB3");
          MDTVLocalSystemUnits.Specify("SURFACIC POWER", "kg/s\xB3");
          MDTVLocalSystemUnits.Specify("VOLUMIC POWER", "kg/(mm*s\xB3)");
          MDTVLocalSystemUnits.Specify("THERMICAL CONDUCTIVITY", "kg*mm/(s\xB3*\xB0K)");
          MDTVLocalSystemUnits.Specify("THERMICAL CONVECTIVITY", "kg/(s\xB3*\xB0K)");
          MDTVLocalSystemUnits.Specify("THERMICAL MASSIC CAPACITY", "mm\xB2/(s\xB2*\xB0K)");
          MDTVLocalSystemUnits.Specify("ENTROPY", "kg*mm\xB2/(s\xB2*\xB0K)");
          MDTVLocalSystemUnits.Specify("ENTHALPY", "kg*mm\xB2/s\xB2");
          MDTVLocalSystemUnits.Specify("LUMINANCE", "cd/mm\xB2");
          MDTVLocalSystemUnits.Specify("LUMINOUS EFFICACITY", "s\xB3*Lu/(kg*mm\xB2)");
          MDTVLocalSystemUnits.Specify("ELECTRIC FIELD", "V/mm");
          MDTVLocalSystemUnits.Specify("ELECTRIC CAPACITANCE", "s**4*A\xB2/(kg*mm\xB2)");
          MDTVLocalSystemUnits.Specify("MAGNETIC FIELD", "A/mm");
          MDTVLocalSystemUnits.Specify("MAGNETIC FLUX", "kg*mm\xB2/(s\xB2*A)");
          MDTVLocalSystemUnits.Specify("INDUCTANCE", "kg*mm\xB2/(s\xB2*A\xB2)");
          MDTVLocalSystemUnits.Specify("RELUCTANCE", "s\xB2*A\xB2/(kg*mm\xB2)");
          MDTVLocalSystemUnits.Specify("RESISTIVITY", "O*mm");
          MDTVLocalSystemUnits.Specify("CONDUCTIVITY", "S/mm");
          MDTVLocalSystemUnits.Specify("MOLAR MASS", "kg/mol");
          MDTVLocalSystemUnits.Specify("MOLAR VOLUME", "mm\xB3/mol");
          MDTVLocalSystemUnits.Specify("CONCENTRATION", "kg/mm\xB3");
          MDTVLocalSystemUnits.Specify("MOLAR CONCENTRATION", "mol/mm\xB3");
          MDTVLocalSystemUnits.Specify("ACOUSTIC INTENSITY", "mm/A\xB2");
          MDTVLocalSystemUnits.Specify("DOSE EQUIVALENT", "mm\xB2/s\xB2");
          MDTVLocalSystemUnits.Specify("ABSORBED DOSE", "mm\xB2/s\xB2");
          MDTVLocalSystemUnits.Specify("FLUX OF MAGNETIC INDUCTION", "kg*mm\xB2/(s\xB2*A)");
          MDTVLocalSystemUnits.Specify("ROTATION ACCELERATION", "rad/s\xB2");
          MDTVLocalSystemUnits.Specify("TRANSLATION STIFFNESS", "kg/s\xB2");
          MDTVLocalSystemUnits.Specify("ROTATION STIFFNESS", "kg*mm\xB2/(s\xB2*rad)");
          MDTVLocalSystemUnits.Activates();
        }
        LocalSystemUnits = MDTVLocalSystemUnits;
        break;
    }
  }
}

//=================================================================================================

double UnitsAPI::CurrentToLS(const double aData, const char* const aQuantity)
{
  double aValue = aData;
  CheckLoading(localSystem);
  if (CurrentUnits->Find(aQuantity))
  {
    TCollection_AsciiString current(CurrentUnits->Value(aQuantity));
    aValue = Units::ToSI(aData, current.ToCString());
    aValue = LocalSystemUnits.ConvertSIValueToUserSystem(aQuantity, aValue);
  }
#ifdef OCCT_DEBUG
  else
  {
    std::cout << "Warning: UnitsAPI,the quantity '" << aQuantity
              << "' does not exist in the current units system" << std::endl;
  }
#endif

  return aValue;
}

//=================================================================================================

double UnitsAPI::CurrentToSI(const double aData, const char* const aQuantity)
{
  double aValue = aData;
  CheckLoading(UnitsAPI_DEFAULT);
  if (CurrentUnits->Find(aQuantity))
  {
    TCollection_AsciiString current(CurrentUnits->Value(aQuantity));
    aValue = Units::ToSI(aData, current.ToCString());
  }
#ifdef OCCT_DEBUG
  else
  {
    std::cout << "Warning: UnitsAPI,the quantity '" << aQuantity
              << "' does not exist in the current units system" << std::endl;
  }
#endif

  return aValue;
}

//=================================================================================================

double UnitsAPI::CurrentFromLS(const double aData, const char* const aQuantity)
{
  double aValue = aData;
  CheckLoading(localSystem);
  if (CurrentUnits->Find(aQuantity))
  {
    TCollection_AsciiString current(CurrentUnits->Value(aQuantity));
    aValue = LocalSystemUnits.ConvertUserSystemValueToSI(aQuantity, aData);
    aValue = Units::FromSI(aValue, current.ToCString());
  }
#ifdef OCCT_DEBUG
  else
  {
    std::cout << "Warning: UnitsAPI,the quantity '" << aQuantity
              << "' does not exist in the current units system" << std::endl;
  }
#endif

  return aValue;
}

//=================================================================================================

double UnitsAPI::CurrentFromSI(const double aData, const char* const aQuantity)
{
  double aValue = aData;
  CheckLoading(UnitsAPI_DEFAULT);
  if (CurrentUnits->Find(aQuantity))
  {
    TCollection_AsciiString current(CurrentUnits->Value(aQuantity));
    aValue = Units::FromSI(aData, current.ToCString());
  }
#ifdef OCCT_DEBUG
  else
  {
    std::cout << "Warning: UnitsAPI,the quantity '" << aQuantity
              << "' does not exist in the current units system" << std::endl;
  }
#endif

  return aValue;
}

//=================================================================================================

double UnitsAPI::CurrentToAny(const double    aData,
                                     const char* const aQuantity,
                                     const char* const aUnit)
{
  double aValue = aData;
  CheckLoading(UnitsAPI_DEFAULT);
  if (CurrentUnits->Find(aQuantity))
  {
    TCollection_AsciiString current(CurrentUnits->Value(aQuantity));
    aValue = AnyToAny(aData, current.ToCString(), aUnit);
  }
#ifdef OCCT_DEBUG
  else
  {
    std::cout << "Warning: UnitsAPI,the quantity '" << aQuantity
              << "' does not exist in the current units system" << std::endl;
  }
#endif

  return aValue;
}

//=================================================================================================

double UnitsAPI::CurrentFromAny(const double    aData,
                                       const char* const aQuantity,
                                       const char* const aUnit)
{
  double aValue = aData;
  CheckLoading(UnitsAPI_DEFAULT);
  if (CurrentUnits->Find(aQuantity))
  {
    TCollection_AsciiString current(CurrentUnits->Value(aQuantity));
    aValue = AnyToAny(aData, aUnit, current.ToCString());
  }
#ifdef OCCT_DEBUG
  else
  {
    std::cout << "Warning: UnitsAPI,the quantity '" << aQuantity
              << "' does not exist in the current units system" << std::endl;
  }
#endif

  return aValue;
}

//=================================================================================================

double UnitsAPI::AnyToLS(const double aData, const char* const aUnit)
{
  double aValue = aData;
  CheckLoading(localSystem);
  occ::handle<Units_Dimensions> aDim;
  aValue = Units::ToSI(aValue, aUnit, aDim);
  if (aDim.IsNull())
    return aValue;
  const char* quantity = aDim->Quantity();
  if (quantity)
  {
    aValue = LocalSystemUnits.ConvertSIValueToUserSystem(quantity, aValue);
  }
#ifdef OCCT_DEBUG
  else
    std::cout << "Warning: BAD Quantity returns in UnitsAPI::AnyToLS(" << aData << "," << aUnit
              << ")" << std::endl;
#endif
  return aValue;
}

//=================================================================================================

double UnitsAPI::AnyToLS(const double       aData,
                                const char* const    aUnit,
                                occ::handle<Units_Dimensions>& aDim)
{
  double aValue = aData;
  CheckLoading(localSystem);
  aValue                    = Units::ToSI(aValue, aUnit, aDim);
  const char* quantity = aDim->Quantity();
  if (aDim.IsNull())
    return aValue;
  if (quantity)
  {
    aValue = LocalSystemUnits.ConvertSIValueToUserSystem(quantity, aValue);
  }
#ifdef OCCT_DEBUG
  else
    std::cout << "Warning: BAD Quantity returns in UnitsAPI::AnyToLS(" << aData << "," << aUnit
              << "," << aDim.get() << ")" << std::endl;
#endif
  return aValue;
}

//=================================================================================================

double UnitsAPI::AnyToSI(const double aData, const char* const aUnit)
{
  double aValue;
  CheckLoading(UnitsAPI_DEFAULT);
  aValue = Units::ToSI(aData, aUnit);
  return aValue;
}

//=================================================================================================

double UnitsAPI::AnyToSI(const double       aData,
                                const char* const    aUnit,
                                occ::handle<Units_Dimensions>& aDim)
{
  double aValue;
  CheckLoading(UnitsAPI_DEFAULT);
  aValue = Units::ToSI(aData, aUnit, aDim);
  return aValue;
}

//=================================================================================================

double UnitsAPI::AnyFromLS(const double aData, const char* const aUnit)
{
  double aValue = aData;
  CheckLoading(localSystem);
  occ::handle<Units_Dimensions> aDim;
  aValue                    = Units::FromSI(aValue, aUnit, aDim);
  const char* quantity = aDim->Quantity();
  if (quantity)
  {
    aValue = LocalSystemUnits.ConvertUserSystemValueToSI(quantity, aValue);
  }
#ifdef OCCT_DEBUG
  else
    std::cout << "Warning: BAD Quantity returns in UnitsAPI::AnyToLS(" << aData << "," << aUnit
              << ")" << std::endl;
#endif

  return aValue;
}

//=================================================================================================

double UnitsAPI::AnyFromSI(const double aData, const char* const aUnit)
{
  double aValue;
  CheckLoading(UnitsAPI_DEFAULT);
  aValue = Units::FromSI(aData, aUnit);
  return aValue;
}

//=================================================================================================

double UnitsAPI::AnyToAny(const double    aData,
                                 const char* const aUnit1,
                                 const char* const aUnit2)
{
  double aValue = aData;
  CheckLoading(UnitsAPI_DEFAULT);
  aValue = Units::Convert(aValue, aUnit1, aUnit2);
  return aValue;
}

//=================================================================================================

double UnitsAPI::LSToSI(const double aData, const char* const aQuantity)
{
  double aValue = aData;
  CheckLoading(localSystem);
  if (CurrentUnits->Find(aQuantity))
  {
    aValue = LocalSystemUnits.ConvertUserSystemValueToSI(aQuantity, aData);
  }
#ifdef OCCT_DEBUG
  else
  {
    std::cout << "Warning: UnitsAPI,the quantity '" << aQuantity
              << "' does not exist in the current units system" << std::endl;
  }
#endif

  return aValue;
}

//=================================================================================================

double UnitsAPI::SIToLS(const double aData, const char* const aQuantity)
{
  double aValue = aData;
  CheckLoading(localSystem);
  if (CurrentUnits->Find(aQuantity))
  {
    aValue = LocalSystemUnits.ConvertSIValueToUserSystem(aQuantity, aValue);
  }
#ifdef OCCT_DEBUG
  else
  {
    std::cout << "Warning: UnitsAPI,the quantity '" << aQuantity
              << "' does not exist in the current units system" << std::endl;
  }
#endif

  return aValue;
}

//=================================================================================================

void UnitsAPI::SetLocalSystem(const UnitsAPI_SystemUnits aSystemUnits)
{
  CheckLoading(aSystemUnits);
  localSystem = currentSystem;
}

//=================================================================================================

UnitsAPI_SystemUnits UnitsAPI::LocalSystem()
{
  return localSystem;
}

//=================================================================================================

void UnitsAPI::SetCurrentUnit(const char* const aQuantity, const char* const anUnit)
{
  CheckLoading(localSystem);
  CurrentUnits->SetResource(aQuantity, anUnit);
}

//=================================================================================================

void UnitsAPI::Save()
{
  CheckLoading(localSystem);
  CurrentUnits->Save();
}

//=================================================================================================

void UnitsAPI::Reload()
{
  currentSystem = UnitsAPI_DEFAULT;
  CheckLoading(localSystem);
}

//=================================================================================================

static TCollection_AsciiString astring;

const char* UnitsAPI::CurrentUnit(const char* const aQuantity)
{
  CheckLoading(localSystem);
  astring = CurrentUnits->Value(aQuantity);
  return astring.ToCString();
}

//=================================================================================================

occ::handle<Units_Dimensions> UnitsAPI::Dimensions(const char* const aType)
{
  return Units::Dimensions(aType);
}

//=================================================================================================

occ::handle<Units_Dimensions> UnitsAPI::DimensionLess()
{
  return Units_Dimensions::ALess();
}

//=================================================================================================

occ::handle<Units_Dimensions> UnitsAPI::DimensionMass()
{
  return Units_Dimensions::AMass();
}

//=================================================================================================

occ::handle<Units_Dimensions> UnitsAPI::DimensionLength()
{
  return Units_Dimensions::ALength();
}

//=================================================================================================

occ::handle<Units_Dimensions> UnitsAPI::DimensionTime()
{
  return Units_Dimensions::ATime();
}

//=================================================================================================

occ::handle<Units_Dimensions> UnitsAPI::DimensionElectricCurrent()
{
  return Units_Dimensions::AElectricCurrent();
}

//=================================================================================================

occ::handle<Units_Dimensions> UnitsAPI::DimensionThermodynamicTemperature()
{
  return Units_Dimensions::AThermodynamicTemperature();
}

//=================================================================================================

occ::handle<Units_Dimensions> UnitsAPI::DimensionAmountOfSubstance()
{
  return Units_Dimensions::AAmountOfSubstance();
}

//=================================================================================================

occ::handle<Units_Dimensions> UnitsAPI::DimensionLuminousIntensity()
{
  return Units_Dimensions::ALuminousIntensity();
}

//=================================================================================================

occ::handle<Units_Dimensions> UnitsAPI::DimensionPlaneAngle()
{
  return Units_Dimensions::APlaneAngle();
}

//=================================================================================================

occ::handle<Units_Dimensions> UnitsAPI::DimensionSolidAngle()
{
  return Units_Dimensions::ASolidAngle();
}

//=================================================================================================

bool UnitsAPI::Check(const char* const aQuantity, const char* const /*aUnit*/)
{
  bool status = false;
  CheckLoading(UnitsAPI_DEFAULT);
  if (CurrentUnits->Find(aQuantity))
  {
    TCollection_AsciiString current(CurrentUnits->Value(aQuantity));
    //    aValue = AnyToAny(aData,current.ToCString(),aUnit);
    //    aValue = Units::Convert(aValue,aUnit1,aUnit2);
  }

  return status;
}
