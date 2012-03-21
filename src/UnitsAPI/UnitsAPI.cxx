// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <UnitsAPI.ixx>
#include <Units.hxx>
#include <Units_Unit.hxx>
#include <Units_Quantity.hxx>
#include <Units_UnitsSystem.hxx>
#include <Resource_Manager.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_NoSuchObject.hxx>
#include <OSD_Environment.hxx>

static Handle(Resource_Manager) CurrentUnits,SICurrentUnits,MDTVCurrentUnits;
static Units_UnitsSystem LocalSystemUnits,SILocalSystemUnits,MDTVLocalSystemUnits;
static TCollection_AsciiString rstring;
static UnitsAPI_SystemUnits localSystem = UnitsAPI_SI;
static UnitsAPI_SystemUnits currentSystem = UnitsAPI_DEFAULT;
static OSD_Environment env1("CSF_UnitsLexicon");
static OSD_Environment env2("CSF_UnitsDefinition");
#ifdef WNT
static OSD_Environment env3("CSF_CurrentUnits");
static OSD_Environment env4("CSF_MDTVCurrentUnits");
#endif


//=======================================================================
//function : CheckLoading
//purpose  :
//=======================================================================

void UnitsAPI::CheckLoading (const UnitsAPI_SystemUnits aSystemUnits)
{
  if( currentSystem != aSystemUnits || CurrentUnits.IsNull()) {
    TCollection_AsciiString slexiconfile(env1.Value());
    if( slexiconfile.Length() > 0 )
      Units::LexiconFile(slexiconfile.ToCString());
    else {
      OSD_Environment CasRootEnv("CASROOT");
      TCollection_AsciiString CasRootString(CasRootEnv.Value());
      if (CasRootString.Length() > 0 )  {
	CasRootString += "/src/UnitsAPI/Lexi_Expr.dat" ;
	Units::LexiconFile(CasRootString.ToCString());
      }
      else {
	Standard_NoSuchObject::Raise("environment variable CSF_UnitsLexicon undefined");
      }
    }
    TCollection_AsciiString sunitsfile(env2.Value());
    if( sunitsfile.Length() > 0 )
      Units::UnitsFile(sunitsfile.ToCString());
    else {
      OSD_Environment CasRootEnv("CASROOT");
      TCollection_AsciiString CasRootString(CasRootEnv.Value());
      if (CasRootString.Length() > 0 ) {
	CasRootString += "/src/UnitsAPI/Units.dat";
	Units::UnitsFile(CasRootString.ToCString());
      }
      else {
	Standard_NoSuchObject::Raise("environment variable CSF_UnitsDefinition undefined");
      }
    }  
    switch (aSystemUnits) {
      case UnitsAPI_DEFAULT :
        if( !CurrentUnits.IsNull() ) break;
      case UnitsAPI_SI :  
        currentSystem = UnitsAPI_SI; 
        if( SICurrentUnits.IsNull() ) {
#ifdef WNT
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
#ifdef WNT
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
  else {
    cout <<"Warning: UnitsAPI,the quantity '" << aQuantity << "' does not exist in the current units system" << endl;
  }

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
  else {
    cout<<"Warning: UnitsAPI,the quantity '" << aQuantity << "' does not exist in the current units system" << endl;
  }

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
  else {
    cout<<"Warning: UnitsAPI,the quantity '" << aQuantity << "' does not exist in the current units system" << endl;
  }

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
  else {
    cout<<"Warning: UnitsAPI,the quantity '" << aQuantity << "' does not exist in the current units system" << endl;
  }

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
  else {
    cout<<"Warning: UnitsAPI,the quantity '" << aQuantity << "' does not exist in the current units system" << endl;
  }

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
  else {
    cout<<"Warning: UnitsAPI,the quantity '" << aQuantity << "' does not exist in the current units system" << endl;
  }

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
  else
    cout<<"Warning: BAD Quantity returns in UnitsAPI::AnyToLS(" << aData << "," << aUnit << ")" << endl;
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
  else
    cout<<"Warning: BAD Quantity returns in UnitsAPI::AnyToLS(" << aData << "," << aUnit << "," << aDim << ")" << endl;
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
  else
    cout<<"Warning: BAD Quantity returns in UnitsAPI::AnyToLS(" << aData << "," << aUnit << ")" << endl;

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
  else {
    cout<<"Warning: UnitsAPI,the quantity '" << aQuantity << "' does not exist in the current units system" << endl;
  }

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
  else {
    cout<<"Warning: UnitsAPI,the quantity '" << aQuantity << "' does not exist in the current units system" << endl;
  }

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
