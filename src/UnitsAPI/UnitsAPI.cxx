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

namespace
{
// Singleton class to manage units-related global state
class UnitsManager
{
public:
  static UnitsManager& Instance()
  {
    static UnitsManager theInstance;
    return theInstance;
  }

  Handle(Resource_Manager) CurrentUnits, SICurrentUnits, MDTVCurrentUnits;
  Units_UnitsSystem        LocalSystemUnits, SILocalSystemUnits, MDTVLocalSystemUnits;
  TCollection_AsciiString  rstring;
  TCollection_AsciiString  astring;
  UnitsAPI_SystemUnits     localSystem;
  UnitsAPI_SystemUnits     currentSystem;

private:
  // Private constructor to enforce singleton pattern
  UnitsManager()
      : localSystem(UnitsAPI_SI),
        currentSystem(UnitsAPI_DEFAULT)
  {
  }

  // Prevent copying
  UnitsManager(const UnitsManager&)            = delete;
  UnitsManager& operator=(const UnitsManager&) = delete;
};
} // namespace

//=================================================================================================

void UnitsAPI::CheckLoading(const UnitsAPI_SystemUnits aSystemUnits)
{
  UnitsManager& aUnitsManager = UnitsManager::Instance();

  if (aUnitsManager.currentSystem != aSystemUnits || aUnitsManager.CurrentUnits.IsNull())
  {
    switch (aSystemUnits)
    {
      case UnitsAPI_DEFAULT:
        if (!aUnitsManager.CurrentUnits.IsNull())
          break;
        Standard_FALLTHROUGH
      case UnitsAPI_SI:
        aUnitsManager.currentSystem = UnitsAPI_SI;
        if (aUnitsManager.SICurrentUnits.IsNull())
        {
#ifdef _WIN32
          OSD_Environment         env3("CSF_CurrentUnits");
          TCollection_AsciiString csfcurrent(env3.Value());
          if (csfcurrent.Length() > 0)
            aUnitsManager.SICurrentUnits = new Resource_Manager(csfcurrent.ToCString());
          else
            aUnitsManager.SICurrentUnits = new Resource_Manager("CurrentUnits");
#else
          aUnitsManager.SICurrentUnits = new Resource_Manager("CurrentUnits");
#endif
        }
        aUnitsManager.CurrentUnits     = aUnitsManager.SICurrentUnits;
        aUnitsManager.LocalSystemUnits = aUnitsManager.SILocalSystemUnits;
        break;
      case UnitsAPI_MDTV:
        aUnitsManager.currentSystem = UnitsAPI_MDTV;
        if (aUnitsManager.MDTVCurrentUnits.IsNull())
        {
#ifdef _WIN32
          OSD_Environment         env4("CSF_MDTVCurrentUnits");
          TCollection_AsciiString csfmdtvcurrent(env4.Value());
          if (csfmdtvcurrent.Length() > 0)
            aUnitsManager.MDTVCurrentUnits = new Resource_Manager(csfmdtvcurrent.ToCString());
          else
            aUnitsManager.MDTVCurrentUnits = new Resource_Manager("MDTVCurrentUnits");
#else
          aUnitsManager.MDTVCurrentUnits = new Resource_Manager("MDTVCurrentUnits");
#endif
        }
        aUnitsManager.CurrentUnits = aUnitsManager.MDTVCurrentUnits;
        if (aUnitsManager.MDTVLocalSystemUnits.IsEmpty())
        {
          aUnitsManager.MDTVLocalSystemUnits.Specify("LENGTH", "mm");
          aUnitsManager.MDTVLocalSystemUnits.Specify("AREA", "mm\xB2");
          aUnitsManager.MDTVLocalSystemUnits.Specify("VOLUME", "mm\xB3");
          aUnitsManager.MDTVLocalSystemUnits.Specify("INERTIA", "mm**4");
          aUnitsManager.MDTVLocalSystemUnits.Specify("SPEED", "mm/s");
          aUnitsManager.MDTVLocalSystemUnits.Specify("ACCELERATION", "mm/s\xB2");
          aUnitsManager.MDTVLocalSystemUnits.Specify("VOLUMIC MASS", "kg/mm\xB3");
          aUnitsManager.MDTVLocalSystemUnits.Specify("VOLUME FLOW", "mm\xB3/s");
          aUnitsManager.MDTVLocalSystemUnits.Specify("CONSUMPTION", "mm\xB2");
          aUnitsManager.MDTVLocalSystemUnits.Specify("QUANTITY OF MOVEMENT", "kg*mm/s");
          aUnitsManager.MDTVLocalSystemUnits.Specify("KINETIC MOMENT", "kg*mm\xB2/s");
          aUnitsManager.MDTVLocalSystemUnits.Specify("MOMENT OF INERTIA", "kg*mm\xB2");
          aUnitsManager.MDTVLocalSystemUnits.Specify("FORCE", "kg*mm/s\xB2");
          aUnitsManager.MDTVLocalSystemUnits.Specify("LINEIC FORCE", "kg/s\xB2");
          aUnitsManager.MDTVLocalSystemUnits.Specify("MOMENT OF A FORCE", "kg*mm\xB2/s\xB2");
          aUnitsManager.MDTVLocalSystemUnits.Specify("PRESSURE", "kg/(mm*s\xB2)");
          aUnitsManager.MDTVLocalSystemUnits.Specify("DYNAMIC VISCOSITY", "kg/(mm*s)");
          aUnitsManager.MDTVLocalSystemUnits.Specify("KINETIC VISCOSITY", "mm\xB2/s");
          aUnitsManager.MDTVLocalSystemUnits.Specify("TENSION SUPERFICIELLE", "mm/s\xB2");
          aUnitsManager.MDTVLocalSystemUnits.Specify("ENERGY", "kg*mm\xB2/s\xB2");
          aUnitsManager.MDTVLocalSystemUnits.Specify("POWER", "kg*mm\xB2/s\xB3");
          aUnitsManager.MDTVLocalSystemUnits.Specify("LINEIC POWER", "kg*mm/s\xB3");
          aUnitsManager.MDTVLocalSystemUnits.Specify("SURFACIC POWER", "kg/s\xB3");
          aUnitsManager.MDTVLocalSystemUnits.Specify("VOLUMIC POWER", "kg/(mm*s\xB3)");
          aUnitsManager.MDTVLocalSystemUnits.Specify("THERMICAL CONDUCTIVITY",
                                                     "kg*mm/(s\xB3*\xB0K)");
          aUnitsManager.MDTVLocalSystemUnits.Specify("THERMICAL CONVECTIVITY", "kg/(s\xB3*\xB0K)");
          aUnitsManager.MDTVLocalSystemUnits.Specify("THERMICAL MASSIC CAPACITY",
                                                     "mm\xB2/(s\xB2*\xB0K)");
          aUnitsManager.MDTVLocalSystemUnits.Specify("ENTROPY", "kg*mm\xB2/(s\xB2*\xB0K)");
          aUnitsManager.MDTVLocalSystemUnits.Specify("ENTHALPY", "kg*mm\xB2/s\xB2");
          aUnitsManager.MDTVLocalSystemUnits.Specify("LUMINANCE", "cd/mm\xB2");
          aUnitsManager.MDTVLocalSystemUnits.Specify("LUMINOUS EFFICACITY", "s\xB3*Lu/(kg*mm\xB2)");
          aUnitsManager.MDTVLocalSystemUnits.Specify("ELECTRIC FIELD", "V/mm");
          aUnitsManager.MDTVLocalSystemUnits.Specify("ELECTRIC CAPACITANCE",
                                                     "s**4*A\xB2/(kg*mm\xB2)");
          aUnitsManager.MDTVLocalSystemUnits.Specify("MAGNETIC FIELD", "A/mm");
          aUnitsManager.MDTVLocalSystemUnits.Specify("MAGNETIC FLUX", "kg*mm\xB2/(s\xB2*A)");
          aUnitsManager.MDTVLocalSystemUnits.Specify("INDUCTANCE", "kg*mm\xB2/(s\xB2*A\xB2)");
          aUnitsManager.MDTVLocalSystemUnits.Specify("RELUCTANCE", "s\xB2*A\xB2/(kg*mm\xB2)");
          aUnitsManager.MDTVLocalSystemUnits.Specify("RESISTIVITY", "O*mm");
          aUnitsManager.MDTVLocalSystemUnits.Specify("CONDUCTIVITY", "S/mm");
          aUnitsManager.MDTVLocalSystemUnits.Specify("MOLAR MASS", "kg/mol");
          aUnitsManager.MDTVLocalSystemUnits.Specify("MOLAR VOLUME", "mm\xB3/mol");
          aUnitsManager.MDTVLocalSystemUnits.Specify("CONCENTRATION", "kg/mm\xB3");
          aUnitsManager.MDTVLocalSystemUnits.Specify("MOLAR CONCENTRATION", "mol/mm\xB3");
          aUnitsManager.MDTVLocalSystemUnits.Specify("ACOUSTIC INTENSITY", "mm/A\xB2");
          aUnitsManager.MDTVLocalSystemUnits.Specify("DOSE EQUIVALENT", "mm\xB2/s\xB2");
          aUnitsManager.MDTVLocalSystemUnits.Specify("ABSORBED DOSE", "mm\xB2/s\xB2");
          aUnitsManager.MDTVLocalSystemUnits.Specify("FLUX OF MAGNETIC INDUCTION",
                                                     "kg*mm\xB2/(s\xB2*A)");
          aUnitsManager.MDTVLocalSystemUnits.Specify("ROTATION ACCELERATION", "rad/s\xB2");
          aUnitsManager.MDTVLocalSystemUnits.Specify("TRANSLATION STIFFNESS", "kg/s\xB2");
          aUnitsManager.MDTVLocalSystemUnits.Specify("ROTATION STIFFNESS", "kg*mm\xB2/(s\xB2*rad)");
          aUnitsManager.MDTVLocalSystemUnits.Activates();
        }
        aUnitsManager.LocalSystemUnits = aUnitsManager.MDTVLocalSystemUnits;
        break;
    }
  }
}

//=================================================================================================

Standard_Real UnitsAPI::CurrentToLS(const Standard_Real aData, const Standard_CString aQuantity)
{
  Standard_Real aValue        = aData;
  UnitsManager& aUnitsManager = UnitsManager::Instance();

  CheckLoading(aUnitsManager.localSystem);
  if (aUnitsManager.CurrentUnits->Find(aQuantity))
  {
    TCollection_AsciiString current(aUnitsManager.CurrentUnits->Value(aQuantity));
    aValue = Units::ToSI(aData, current.ToCString());
    aValue = aUnitsManager.LocalSystemUnits.ConvertSIValueToUserSystem(aQuantity, aValue);
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

Standard_Real UnitsAPI::CurrentToSI(const Standard_Real aData, const Standard_CString aQuantity)
{
  Standard_Real aValue        = aData;
  UnitsManager& aUnitsManager = UnitsManager::Instance();

  CheckLoading(UnitsAPI_DEFAULT);
  if (aUnitsManager.CurrentUnits->Find(aQuantity))
  {
    TCollection_AsciiString current(aUnitsManager.CurrentUnits->Value(aQuantity));
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

Standard_Real UnitsAPI::CurrentFromLS(const Standard_Real aData, const Standard_CString aQuantity)
{
  Standard_Real aValue        = aData;
  UnitsManager& aUnitsManager = UnitsManager::Instance();

  CheckLoading(aUnitsManager.localSystem);
  if (aUnitsManager.CurrentUnits->Find(aQuantity))
  {
    TCollection_AsciiString current(aUnitsManager.CurrentUnits->Value(aQuantity));
    aValue = aUnitsManager.LocalSystemUnits.ConvertUserSystemValueToSI(aQuantity, aData);
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

Standard_Real UnitsAPI::CurrentFromSI(const Standard_Real aData, const Standard_CString aQuantity)
{
  Standard_Real aValue        = aData;
  UnitsManager& aUnitsManager = UnitsManager::Instance();

  CheckLoading(UnitsAPI_DEFAULT);
  if (aUnitsManager.CurrentUnits->Find(aQuantity))
  {
    TCollection_AsciiString current(aUnitsManager.CurrentUnits->Value(aQuantity));
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

Standard_Real UnitsAPI::CurrentToAny(const Standard_Real    aData,
                                     const Standard_CString aQuantity,
                                     const Standard_CString aUnit)
{
  Standard_Real aValue        = aData;
  UnitsManager& aUnitsManager = UnitsManager::Instance();

  CheckLoading(UnitsAPI_DEFAULT);
  if (aUnitsManager.CurrentUnits->Find(aQuantity))
  {
    TCollection_AsciiString current(aUnitsManager.CurrentUnits->Value(aQuantity));
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

Standard_Real UnitsAPI::CurrentFromAny(const Standard_Real    aData,
                                       const Standard_CString aQuantity,
                                       const Standard_CString aUnit)
{
  Standard_Real aValue        = aData;
  UnitsManager& aUnitsManager = UnitsManager::Instance();

  CheckLoading(UnitsAPI_DEFAULT);
  if (aUnitsManager.CurrentUnits->Find(aQuantity))
  {
    TCollection_AsciiString current(aUnitsManager.CurrentUnits->Value(aQuantity));
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

Standard_Real UnitsAPI::AnyToLS(const Standard_Real aData, const Standard_CString aUnit)
{
  Standard_Real aValue        = aData;
  UnitsManager& aUnitsManager = UnitsManager::Instance();

  CheckLoading(aUnitsManager.localSystem);
  Handle(Units_Dimensions) aDim;
  aValue = Units::ToSI(aValue, aUnit, aDim);
  if (aDim.IsNull())
    return aValue;
  Standard_CString quantity = aDim->Quantity();
  if (quantity)
  {
    aValue = aUnitsManager.LocalSystemUnits.ConvertSIValueToUserSystem(quantity, aValue);
  }
#ifdef OCCT_DEBUG
  else
    std::cout << "Warning: BAD Quantity returns in UnitsAPI::AnyToLS(" << aData << "," << aUnit
              << ")" << std::endl;
#endif
  return aValue;
}

//=================================================================================================

Standard_Real UnitsAPI::AnyToLS(const Standard_Real       aData,
                                const Standard_CString    aUnit,
                                Handle(Units_Dimensions)& aDim)
{
  Standard_Real aValue        = aData;
  UnitsManager& aUnitsManager = UnitsManager::Instance();

  CheckLoading(aUnitsManager.localSystem);
  aValue                    = Units::ToSI(aValue, aUnit, aDim);
  Standard_CString quantity = aDim->Quantity();
  if (aDim.IsNull())
    return aValue;
  if (quantity)
  {
    aValue = aUnitsManager.LocalSystemUnits.ConvertSIValueToUserSystem(quantity, aValue);
  }
#ifdef OCCT_DEBUG
  else
    std::cout << "Warning: BAD Quantity returns in UnitsAPI::AnyToLS(" << aData << "," << aUnit
              << "," << aDim.get() << ")" << std::endl;
#endif
  return aValue;
}

//=================================================================================================

Standard_Real UnitsAPI::AnyToSI(const Standard_Real aData, const Standard_CString aUnit)
{
  CheckLoading(UnitsAPI_DEFAULT);
  return Units::ToSI(aData, aUnit);
}

//=================================================================================================

Standard_Real UnitsAPI::AnyToSI(const Standard_Real       aData,
                                const Standard_CString    aUnit,
                                Handle(Units_Dimensions)& aDim)
{
  CheckLoading(UnitsAPI_DEFAULT);
  return Units::ToSI(aData, aUnit, aDim);
}

//=================================================================================================

Standard_Real UnitsAPI::AnyFromLS(const Standard_Real aData, const Standard_CString aUnit)
{
  Standard_Real aValue        = aData;
  UnitsManager& aUnitsManager = UnitsManager::Instance();

  CheckLoading(aUnitsManager.localSystem);
  Handle(Units_Dimensions) aDim;
  aValue                    = Units::FromSI(aValue, aUnit, aDim);
  Standard_CString quantity = aDim->Quantity();
  if (quantity)
  {
    aValue = aUnitsManager.LocalSystemUnits.ConvertUserSystemValueToSI(quantity, aValue);
  }
#ifdef OCCT_DEBUG
  else
    std::cout << "Warning: BAD Quantity returns in UnitsAPI::AnyToLS(" << aData << "," << aUnit
              << ")" << std::endl;
#endif

  return aValue;
}

//=================================================================================================

Standard_Real UnitsAPI::AnyFromSI(const Standard_Real aData, const Standard_CString aUnit)
{
  CheckLoading(UnitsAPI_DEFAULT);
  return Units::FromSI(aData, aUnit);
}

//=================================================================================================

Standard_Real UnitsAPI::AnyToAny(const Standard_Real    aData,
                                 const Standard_CString aUnit1,
                                 const Standard_CString aUnit2)
{
  CheckLoading(UnitsAPI_DEFAULT);
  return Units::Convert(aData, aUnit1, aUnit2);
}

//=================================================================================================

Standard_Real UnitsAPI::LSToSI(const Standard_Real aData, const Standard_CString aQuantity)
{
  Standard_Real aValue        = aData;
  UnitsManager& aUnitsManager = UnitsManager::Instance();

  CheckLoading(aUnitsManager.localSystem);
  if (aUnitsManager.CurrentUnits->Find(aQuantity))
  {
    aValue = aUnitsManager.LocalSystemUnits.ConvertUserSystemValueToSI(aQuantity, aData);
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

Standard_Real UnitsAPI::SIToLS(const Standard_Real aData, const Standard_CString aQuantity)
{
  Standard_Real aValue        = aData;
  UnitsManager& aUnitsManager = UnitsManager::Instance();

  CheckLoading(aUnitsManager.localSystem);
  if (aUnitsManager.CurrentUnits->Find(aQuantity))
  {
    aValue = aUnitsManager.LocalSystemUnits.ConvertSIValueToUserSystem(aQuantity, aValue);
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
  UnitsManager& aUnitsManager = UnitsManager::Instance();

  CheckLoading(aSystemUnits);
  aUnitsManager.localSystem = aUnitsManager.currentSystem;
}

//=================================================================================================

UnitsAPI_SystemUnits UnitsAPI::LocalSystem()
{
  UnitsManager& aUnitsManager = UnitsManager::Instance();

  return aUnitsManager.localSystem;
}

//=================================================================================================

void UnitsAPI::SetCurrentUnit(const Standard_CString aQuantity, const Standard_CString anUnit)
{
  UnitsManager& aUnitsManager = UnitsManager::Instance();

  CheckLoading(aUnitsManager.localSystem);
  aUnitsManager.CurrentUnits->SetResource(aQuantity, anUnit);
}

//=================================================================================================

void UnitsAPI::Save()
{
  UnitsManager& aUnitsManager = UnitsManager::Instance();

  CheckLoading(aUnitsManager.localSystem);
  aUnitsManager.CurrentUnits->Save();
}

//=================================================================================================

void UnitsAPI::Reload()
{
  UnitsManager& aUnitsManager = UnitsManager::Instance();

  aUnitsManager.currentSystem = UnitsAPI_DEFAULT;
  CheckLoading(aUnitsManager.localSystem);
}

//=================================================================================================

Standard_CString UnitsAPI::CurrentUnit(const Standard_CString aQuantity)
{
  UnitsManager& aUnitsManager = UnitsManager::Instance();

  CheckLoading(aUnitsManager.localSystem);
  aUnitsManager.astring = aUnitsManager.CurrentUnits->Value(aQuantity);
  return aUnitsManager.astring.ToCString();
}

//=================================================================================================

Handle(Units_Dimensions) UnitsAPI::Dimensions(const Standard_CString aType)
{
  return Units::Dimensions(aType);
}

//=================================================================================================

Handle(Units_Dimensions) UnitsAPI::DimensionLess()
{
  return Units_Dimensions::ALess();
}

//=================================================================================================

Handle(Units_Dimensions) UnitsAPI::DimensionMass()
{
  return Units_Dimensions::AMass();
}

//=================================================================================================

Handle(Units_Dimensions) UnitsAPI::DimensionLength()
{
  return Units_Dimensions::ALength();
}

//=================================================================================================

Handle(Units_Dimensions) UnitsAPI::DimensionTime()
{
  return Units_Dimensions::ATime();
}

//=================================================================================================

Handle(Units_Dimensions) UnitsAPI::DimensionElectricCurrent()
{
  return Units_Dimensions::AElectricCurrent();
}

//=================================================================================================

Handle(Units_Dimensions) UnitsAPI::DimensionThermodynamicTemperature()
{
  return Units_Dimensions::AThermodynamicTemperature();
}

//=================================================================================================

Handle(Units_Dimensions) UnitsAPI::DimensionAmountOfSubstance()
{
  return Units_Dimensions::AAmountOfSubstance();
}

//=================================================================================================

Handle(Units_Dimensions) UnitsAPI::DimensionLuminousIntensity()
{
  return Units_Dimensions::ALuminousIntensity();
}

//=================================================================================================

Handle(Units_Dimensions) UnitsAPI::DimensionPlaneAngle()
{
  return Units_Dimensions::APlaneAngle();
}

//=================================================================================================

Handle(Units_Dimensions) UnitsAPI::DimensionSolidAngle()
{
  return Units_Dimensions::ASolidAngle();
}

//=================================================================================================

Standard_Boolean UnitsAPI::Check(const Standard_CString aQuantity, const Standard_CString /*aUnit*/)
{
  Standard_Boolean status        = Standard_False;
  UnitsManager&    aUnitsManager = UnitsManager::Instance();

  CheckLoading(UnitsAPI_DEFAULT);
  if (aUnitsManager.CurrentUnits->Find(aQuantity))
  {
    TCollection_AsciiString current(aUnitsManager.CurrentUnits->Value(aQuantity));
    //    aValue = AnyToAny(aData,current.ToCString(),aUnit);
    //    aValue = Units::Convert(aValue,aUnit1,aUnit2);
  }

  return status;
}
