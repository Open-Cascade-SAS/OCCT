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
#include <Standard_NoSuchObject.hxx>
#include <TCollection_AsciiString.hxx>
#include <Units.hxx>
#include <Units_Dimensions.hxx>
#include <Units_Quantity.hxx>
#include <Units_Unit.hxx>
#include <Units_UnitsSystem.hxx>
#include <UnitsAPI.hxx>

static Handle(Resource_Manager) CurrentUnits,SICurrentUnits,MDTVCurrentUnits;
static Units_UnitsSystem LocalSystemUnits,SILocalSystemUnits,MDTVLocalSystemUnits;
static TCollection_AsciiString rstring;
static UnitsAPI_SystemUnits localSystem = UnitsAPI_SI;
static UnitsAPI_SystemUnits currentSystem = UnitsAPI_DEFAULT;

//=======================================================================
//function : CheckLoading
//purpose  :
//=======================================================================

void UnitsAPI::CheckLoading (const UnitsAPI_SystemUnits aSystemUnits)
{
  if( currentSystem != aSystemUnits || CurrentUnits.IsNull()) {
    switch (aSystemUnits) {
      case UnitsAPI_DEFAULT :
        if( !CurrentUnits.IsNull() ) break;
        Standard_FALLTHROUGH
      case UnitsAPI_SI :  
        currentSystem = UnitsAPI_SI; 
        if( SICurrentUnits.IsNull() ) {
#ifdef _WIN32
          OSD_Environment env3("CSF_CurrentUnits");
          TCollection_AsciiString csfcurrent (env3.Value());
          if( csfcurrent.Length() > 0 )
                SICurrentUnits = new Resource_Manager(csfcurrent.ToCString());
          else
                SICurrentUnits = new Resource_Manager("CurrentUnits");
#else
		SICurrentUnits = new Resource_Manager("CurrentUnits");
#endif
        }
        CurrentUnits = SICurrentUnits;
        LocalSystemUnits = SILocalSystemUnits;
        break;
      case UnitsAPI_MDTV :  
        currentSystem = UnitsAPI_MDTV; 
        if( MDTVCurrentUnits.IsNull() )  {
#ifdef _WIN32
          OSD_Environment env4("CSF_MDTVCurrentUnits");
          TCollection_AsciiString csfmdtvcurrent (env4.Value());
          if( csfmdtvcurrent.Length() > 0 )
                MDTVCurrentUnits = new Resource_Manager(csfmdtvcurrent.ToCString());
          else
                MDTVCurrentUnits = new Resource_Manager("MDTVCurrentUnits");
#else
		MDTVCurrentUnits = new Resource_Manager("MDTVCurrentUnits");
#endif
        }
        CurrentUnits = MDTVCurrentUnits;
        if( MDTVLocalSystemUnits.IsEmpty() ) {
          MDTVLocalSystemUnits.Specify("LENGTH","mm");
          MDTVLocalSystemUnits.Specify("AREA","mm\xB2");
          MDTVLocalSystemUnits.Specify("VOLUME","mm\xB3");
          MDTVLocalSystemUnits.Specify("INERTIA","mm**4");
          MDTVLocalSystemUnits.Specify("SPEED","mm/s");
          MDTVLocalSystemUnits.Specify("ACCELERATION","mm/s\xB2");
          MDTVLocalSystemUnits.Specify("VOLUMIC MASS","kg/mm\xB3");
          MDTVLocalSystemUnits.Specify("VOLUME FLOW","mm\xB3/s");
          MDTVLocalSystemUnits.Specify("CONSUMPTION","mm\xB2");
          MDTVLocalSystemUnits.Specify("QUANTITY OF MOVEMENT","kg*mm/s");
          MDTVLocalSystemUnits.Specify("KINETIC MOMENT","kg*mm\xB2/s");
          MDTVLocalSystemUnits.Specify("MOMENT OF INERTIA","kg*mm\xB2");
          MDTVLocalSystemUnits.Specify("FORCE","kg*mm/s\xB2");
          MDTVLocalSystemUnits.Specify("LINEIC FORCE","kg/s\xB2");
          MDTVLocalSystemUnits.Specify("MOMENT OF A FORCE","kg*mm\xB2/s\xB2");
          MDTVLocalSystemUnits.Specify("PRESSURE","kg/(mm*s\xB2)");
          MDTVLocalSystemUnits.Specify("DYNAMIC VISCOSITY","kg/(mm*s)");
          MDTVLocalSystemUnits.Specify("KINETIC VISCOSITY","mm\xB2/s");
          MDTVLocalSystemUnits.Specify("TENSION SUPERFICIELLE","mm/s\xB2");
          MDTVLocalSystemUnits.Specify("ENERGY","kg*mm\xB2/s\xB2");
          MDTVLocalSystemUnits.Specify("POWER","kg*mm\xB2/s\xB3");
          MDTVLocalSystemUnits.Specify("LINEIC POWER","kg*mm/s\xB3");
          MDTVLocalSystemUnits.Specify("SURFACIC POWER","kg/s\xB3");
          MDTVLocalSystemUnits.Specify("VOLUMIC POWER","kg/(mm*s\xB3)");
          MDTVLocalSystemUnits.Specify("THERMICAL CONDUCTIVITY","kg*mm/(s\xB3*\xB0K)");
          MDTVLocalSystemUnits.Specify("THERMICAL CONVECTIVITY","kg/(s\xB3*\xB0K)");
          MDTVLocalSystemUnits.Specify("THERMICAL MASSIC CAPACITY","mm\xB2/(s\xB2*\xB0K)");
          MDTVLocalSystemUnits.Specify("ENTROPY","kg*mm\xB2/(s\xB2*\xB0K)");
          MDTVLocalSystemUnits.Specify("ENTHALPY","kg*mm\xB2/s\xB2");
          MDTVLocalSystemUnits.Specify("LUMINANCE","cd/mm\xB2");
          MDTVLocalSystemUnits.Specify("LUMINOUS EFFICACITY","s\xB3*Lu/(kg*mm\xB2)");
          MDTVLocalSystemUnits.Specify("ELECTRIC FIELD","V/mm");
          MDTVLocalSystemUnits.Specify("ELECTRIC CAPACITANCE","s**4*A\xB2/(kg*mm\xB2)");
          MDTVLocalSystemUnits.Specify("MAGNETIC FIELD","A/mm");
          MDTVLocalSystemUnits.Specify("MAGNETIC FLUX","kg*mm\xB2/(s\xB2*A)");
          MDTVLocalSystemUnits.Specify("INDUCTANCE","kg*mm\xB2/(s\xB2*A\xB2)");
          MDTVLocalSystemUnits.Specify("RELUCTANCE","s\xB2*A\xB2/(kg*mm\xB2)");
          MDTVLocalSystemUnits.Specify("RESISTIVITY","O*mm");
          MDTVLocalSystemUnits.Specify("CONDUCTIVITY","S/mm");
          MDTVLocalSystemUnits.Specify("MOLAR MASS","kg/mol");
          MDTVLocalSystemUnits.Specify("MOLAR VOLUME","mm\xB3/mol");
          MDTVLocalSystemUnits.Specify("CONCENTRATION","kg/mm\xB3");
          MDTVLocalSystemUnits.Specify("MOLAR CONCENTRATION","mol/mm\xB3");
          MDTVLocalSystemUnits.Specify("ACCOUSTIC INTENSITY","mm/A\xB2");
          MDTVLocalSystemUnits.Specify("DOSE EQUIVALENT","mm\xB2/s\xB2");
          MDTVLocalSystemUnits.Specify("ABSORBED DOSE","mm\xB2/s\xB2");
          MDTVLocalSystemUnits.Specify("FLUX OF MAGNETIC INDUCTION","kg*mm\xB2/(s\xB2*A)");
          MDTVLocalSystemUnits.Specify("ROTATION ACCELERATION","rad/s\xB2");
          MDTVLocalSystemUnits.Specify("TRANSLATION STIFFNESS","kg/s\xB2");
          MDTVLocalSystemUnits.Specify("ROTATION STIFFNESS","kg*mm\xB2/(s\xB2*rad)");
          MDTVLocalSystemUnits.Activates();
        }
        LocalSystemUnits = MDTVLocalSystemUnits;
        break;
    }
  }
}


//=======================================================================
//function : CurrentToLS
//purpose  :
//=======================================================================

Standard_Real UnitsAPI::CurrentToLS(const Standard_Real aData,
                                    const Standard_CString aQuantity)
{
  Standard_Real aValue = aData;
  CheckLoading (localSystem); 
  if( CurrentUnits->Find(aQuantity) ) {
    TCollection_AsciiString current(CurrentUnits->Value(aQuantity));
    aValue = Units::ToSI(aData,current.ToCString());
    aValue = LocalSystemUnits.ConvertSIValueToUserSystem(aQuantity,aValue);
  }
#ifdef OCCT_DEBUG
  else {
    cout <<"Warning: UnitsAPI,the quantity '" << aQuantity << "' does not exist in the current units system" << endl;
  }
#endif

  return aValue;
}


//=======================================================================
//function : CurrentToSI
//purpose  :
//=======================================================================

Standard_Real UnitsAPI::CurrentToSI(const Standard_Real aData,
                                    const Standard_CString aQuantity)
{
  Standard_Real aValue = aData;
  CheckLoading (UnitsAPI_DEFAULT); 
  if( CurrentUnits->Find(aQuantity) ) {
    TCollection_AsciiString current(CurrentUnits->Value(aQuantity));
    aValue = Units::ToSI(aData,current.ToCString());
  }
#ifdef OCCT_DEBUG
  else {
    cout<<"Warning: UnitsAPI,the quantity '" << aQuantity << "' does not exist in the current units system" << endl;
  }
#endif

  return aValue;
}


//=======================================================================
//function : CurrentFromLS
//purpose  :
//=======================================================================

Standard_Real UnitsAPI::CurrentFromLS(const Standard_Real aData,
                                      const Standard_CString aQuantity)
{
  Standard_Real aValue = aData;
  CheckLoading (localSystem); 
  if( CurrentUnits->Find(aQuantity) ) {
    TCollection_AsciiString current(CurrentUnits->Value(aQuantity));
    aValue = LocalSystemUnits.ConvertUserSystemValueToSI(aQuantity,aData);
    aValue = Units::FromSI(aValue,current.ToCString());
  }
#ifdef OCCT_DEBUG
  else {
    cout<<"Warning: UnitsAPI,the quantity '" << aQuantity << "' does not exist in the current units system" << endl;
  }
#endif

  return aValue;
}


//=======================================================================
//function : CurrentFromSI
//purpose  :
//=======================================================================

Standard_Real UnitsAPI::CurrentFromSI(const Standard_Real aData,
                                      const Standard_CString aQuantity)
{
  Standard_Real aValue = aData;
  CheckLoading (UnitsAPI_DEFAULT); 
  if( CurrentUnits->Find(aQuantity) ) {
    TCollection_AsciiString current(CurrentUnits->Value(aQuantity));
    aValue = Units::FromSI(aData,current.ToCString());
  }
#ifdef OCCT_DEBUG
  else {
    cout<<"Warning: UnitsAPI,the quantity '" << aQuantity << "' does not exist in the current units system" << endl;
  }
#endif

  return aValue;
}


//=======================================================================
//function : CurrentToAny
//purpose  :
//=======================================================================

Standard_Real UnitsAPI::CurrentToAny(const Standard_Real aData,
                                     const Standard_CString aQuantity,
                                     const Standard_CString aUnit) {
  Standard_Real aValue = aData;
  CheckLoading (UnitsAPI_DEFAULT); 
  if( CurrentUnits->Find(aQuantity) ) {
    TCollection_AsciiString current(CurrentUnits->Value(aQuantity));
    aValue = AnyToAny(aData,current.ToCString(),aUnit);
  }
#ifdef OCCT_DEBUG
  else {
    cout<<"Warning: UnitsAPI,the quantity '" << aQuantity << "' does not exist in the current units system" << endl;
  }
#endif

  return aValue;
}


//=======================================================================
//function : CurrentFromAny
//purpose  :
//=======================================================================

Standard_Real UnitsAPI::CurrentFromAny(const Standard_Real aData,
                                       const Standard_CString aQuantity,
                                       const Standard_CString aUnit)
{
  Standard_Real aValue = aData;
  CheckLoading (UnitsAPI_DEFAULT); 
  if( CurrentUnits->Find(aQuantity) ) {
    TCollection_AsciiString current(CurrentUnits->Value(aQuantity));
    aValue = AnyToAny(aData,aUnit,current.ToCString());
  }
#ifdef OCCT_DEBUG
  else {
    cout<<"Warning: UnitsAPI,the quantity '" << aQuantity << "' does not exist in the current units system" << endl;
  }
#endif

  return aValue;
}


//=======================================================================
//function : AnyToLS
//purpose  :
//=======================================================================

Standard_Real UnitsAPI::AnyToLS(const Standard_Real aData,
                                const Standard_CString aUnit)
{
  Standard_Real aValue = aData;
  CheckLoading (localSystem); 
  Handle(Units_Dimensions) aDim;
  aValue = Units::ToSI(aValue,aUnit,aDim);
  if(aDim.IsNull())
    return aValue;
  Standard_CString quantity = aDim->Quantity();
  if( quantity ) {
    aValue = LocalSystemUnits.ConvertSIValueToUserSystem(quantity,aValue);
  }
#ifdef OCCT_DEBUG
  else
    cout<<"Warning: BAD Quantity returns in UnitsAPI::AnyToLS(" << aData << "," << aUnit << ")" << endl;
#endif
  return aValue;
}


//=======================================================================
//function : AnyToLS
//purpose  :
//=======================================================================

Standard_Real UnitsAPI::AnyToLS(const Standard_Real aData,
                                const Standard_CString aUnit,
                                Handle(Units_Dimensions) &aDim)
{
  Standard_Real aValue = aData;
  CheckLoading (localSystem); 
  aValue = Units::ToSI(aValue,aUnit,aDim);
  Standard_CString quantity = aDim->Quantity();
  if(aDim.IsNull())
    return aValue;
  if( quantity ) {
    aValue = LocalSystemUnits.ConvertSIValueToUserSystem(quantity,aValue);
  }
#ifdef OCCT_DEBUG
  else
    cout<<"Warning: BAD Quantity returns in UnitsAPI::AnyToLS(" << aData << "," << aUnit << "," << aDim.get() << ")" << endl;
#endif
  return aValue;
}


//=======================================================================
//function : AnyToSI
//purpose  :
//=======================================================================

Standard_Real UnitsAPI::AnyToSI(const Standard_Real aData,
                                const Standard_CString aUnit)
{
  Standard_Real aValue;
  CheckLoading (UnitsAPI_DEFAULT); 
  aValue = Units::ToSI(aData,aUnit);
  return aValue;
}


//=======================================================================
//function : AnyToSI
//purpose  :
//=======================================================================

Standard_Real UnitsAPI::AnyToSI(const Standard_Real aData,
                                const Standard_CString aUnit,
                                Handle(Units_Dimensions) &aDim)
{
  Standard_Real aValue;
  CheckLoading (UnitsAPI_DEFAULT);
  aValue = Units::ToSI(aData,aUnit,aDim);
  return aValue;
}


//=======================================================================
//function : AnyFromLS
//purpose  :
//=======================================================================

Standard_Real UnitsAPI::AnyFromLS(const Standard_Real aData,
                                  const Standard_CString aUnit)
{
  Standard_Real aValue = aData;
  CheckLoading (localSystem); 
  Handle(Units_Dimensions) aDim;
  aValue = Units::FromSI(aValue,aUnit,aDim);
  Standard_CString quantity = aDim->Quantity();
  if( quantity ) {
    aValue = LocalSystemUnits.ConvertUserSystemValueToSI(quantity,aValue);
  }
#ifdef OCCT_DEBUG
  else
    cout<<"Warning: BAD Quantity returns in UnitsAPI::AnyToLS(" << aData << "," << aUnit << ")" << endl;
#endif

  return aValue;
}


//=======================================================================
//function : AnyFromSI
//purpose  :
//=======================================================================

Standard_Real UnitsAPI::AnyFromSI(const Standard_Real aData,
                                  const Standard_CString aUnit)
{
  Standard_Real aValue;
  CheckLoading (UnitsAPI_DEFAULT); 
  aValue = Units::FromSI(aData,aUnit);
  return aValue;
}


//=======================================================================
//function : AnyToAny
//purpose  :
//=======================================================================

Standard_Real UnitsAPI::AnyToAny(const Standard_Real aData,
                                 const Standard_CString aUnit1,
                                 const Standard_CString aUnit2)
{
  Standard_Real aValue = aData;
  CheckLoading (UnitsAPI_DEFAULT); 
  aValue = Units::Convert(aValue,aUnit1,aUnit2);
  return aValue;
}


//=======================================================================
//function : LSToSI
//purpose  :
//=======================================================================

Standard_Real UnitsAPI::LSToSI(const Standard_Real aData,
                               const Standard_CString aQuantity)
{
  Standard_Real aValue = aData;
  CheckLoading (localSystem); 
  if( CurrentUnits->Find(aQuantity) ) {
    aValue = LocalSystemUnits.ConvertUserSystemValueToSI(aQuantity,aData);
  }
#ifdef OCCT_DEBUG
  else {
    cout<<"Warning: UnitsAPI,the quantity '" << aQuantity << "' does not exist in the current units system" << endl;
  }
#endif

  return aValue;
}


//=======================================================================
//function : SIToLS
//purpose  :
//=======================================================================

Standard_Real UnitsAPI::SIToLS(const Standard_Real aData,
                               const Standard_CString aQuantity)
{
  Standard_Real aValue = aData;
  CheckLoading (localSystem); 
  if( CurrentUnits->Find(aQuantity) ) {
    aValue = LocalSystemUnits.ConvertSIValueToUserSystem(aQuantity,aValue);
  }
#ifdef OCCT_DEBUG
  else {
    cout<<"Warning: UnitsAPI,the quantity '" << aQuantity << "' does not exist in the current units system" << endl;
  }
#endif

  return aValue;
}


//=======================================================================
//function : SetLocalSystem
//purpose  :
//=======================================================================

void UnitsAPI::SetLocalSystem(const UnitsAPI_SystemUnits aSystemUnits)
{
  CheckLoading (aSystemUnits); 
  localSystem = currentSystem; 
}


//=======================================================================
//function : LocalSystem
//purpose  :
//=======================================================================

UnitsAPI_SystemUnits UnitsAPI::LocalSystem()
{
  return localSystem; 
}


//=======================================================================
//function : SetCurrentUnit
//purpose  :
//=======================================================================

void UnitsAPI::SetCurrentUnit(const Standard_CString aQuantity,
                              const Standard_CString anUnit)
{
  CheckLoading(localSystem);
  CurrentUnits->SetResource(aQuantity,anUnit);
}


//=======================================================================
//function : Save
//purpose  :
//=======================================================================

void UnitsAPI::Save()
{
  CheckLoading(localSystem);
  CurrentUnits->Save();
}


//=======================================================================
//function : Reload
//purpose  :
//=======================================================================

void UnitsAPI::Reload()
{
  currentSystem = UnitsAPI_DEFAULT;
  CheckLoading(localSystem);
}


//=======================================================================
//function : CurrentUnit
//purpose  :
//=======================================================================

static TCollection_AsciiString astring;
Standard_CString UnitsAPI::CurrentUnit(const Standard_CString aQuantity)
{
  CheckLoading(localSystem);
  astring = CurrentUnits->Value(aQuantity);
  return astring.ToCString();
}


//=======================================================================
//function : Dimensions
//purpose  :
//=======================================================================

Handle(Units_Dimensions) UnitsAPI::Dimensions(const Standard_CString aType)
{
 return  Units::Dimensions(aType);
}


//=======================================================================
//function : DimensionLess
//purpose  :
//=======================================================================

Handle(Units_Dimensions) UnitsAPI::DimensionLess()
{
  return Units_Dimensions::ALess();
}


//=======================================================================
//function : DimensionMass
//purpose  :
//=======================================================================

Handle(Units_Dimensions) UnitsAPI::DimensionMass()
{
 return Units_Dimensions::AMass();
}


//=======================================================================
//function : DimensionLength
//purpose  :
//=======================================================================

Handle(Units_Dimensions) UnitsAPI::DimensionLength()
{
 return  Units_Dimensions::ALength();
}


//=======================================================================
//function : DimensionTime
//purpose  :
//=======================================================================

Handle(Units_Dimensions) UnitsAPI::DimensionTime()
{
 return Units_Dimensions::ATime() ;
}


//=======================================================================
//function : DimensionElectricCurrent
//purpose  :
//=======================================================================

Handle(Units_Dimensions) UnitsAPI::DimensionElectricCurrent()
{
 return Units_Dimensions::AElectricCurrent() ;
}


//=======================================================================
//function : DimensionThermodynamicTemperature
//purpose  :
//=======================================================================

Handle(Units_Dimensions) UnitsAPI::DimensionThermodynamicTemperature()
{
 return Units_Dimensions::AThermodynamicTemperature();
}


//=======================================================================
//function : DimensionAmountOfSubstance
//purpose  :
//=======================================================================

Handle(Units_Dimensions) UnitsAPI::DimensionAmountOfSubstance()
{
 return Units_Dimensions::AAmountOfSubstance();
}


//=======================================================================
//function : DimensionLuminousIntensity
//purpose  :
//=======================================================================

Handle(Units_Dimensions) UnitsAPI::DimensionLuminousIntensity()
{
 return Units_Dimensions::ALuminousIntensity();
}


//=======================================================================
//function : DimensionPlaneAngle
//purpose  :
//=======================================================================

Handle(Units_Dimensions) UnitsAPI::DimensionPlaneAngle()
{
 return Units_Dimensions::APlaneAngle();
}


//=======================================================================
//function : DimensionSolidAngle
//purpose  :
//=======================================================================

Handle(Units_Dimensions) UnitsAPI::DimensionSolidAngle()
{
 return Units_Dimensions::ASolidAngle();
}


//=======================================================================
//function : Check
//purpose  :
//=======================================================================

Standard_Boolean UnitsAPI::Check(const Standard_CString aQuantity, 
				 const Standard_CString /*aUnit*/)
{
  Standard_Boolean status = Standard_False;
  CheckLoading (UnitsAPI_DEFAULT); 
  if( CurrentUnits->Find(aQuantity) ) {
    TCollection_AsciiString current(CurrentUnits->Value(aQuantity));
//    aValue = AnyToAny(aData,current.ToCString(),aUnit);
//    aValue = Units::Convert(aValue,aUnit1,aUnit2);
  }

  return status;
}
