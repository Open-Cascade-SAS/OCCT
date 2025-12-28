// Created on: 1996-01-18
// Created by: Frederic MAUPAS
// Copyright (c) 1996-1999 Matra Datavision
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

// abv 17.11.99: renamed from StepPDR_MakeUnitAndToleranceContext and merged with STEPControl_Unit
// abv 30.02.00: ability to write file in units other than MM

#include <StepBasic_ConversionBasedUnitAndAreaUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndLengthUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndPlaneAngleUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndSolidAngleUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndVolumeUnit.hxx>
#include <StepBasic_DimensionalExponents.hxx>
#include <StepBasic_LengthMeasureWithUnit.hxx>
#include <StepBasic_MeasureValueMember.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepBasic_NamedUnit.hxx>
#include <StepBasic_SiUnitAndAreaUnit.hxx>
#include <StepBasic_SiUnitAndLengthUnit.hxx>
#include <StepBasic_SiUnitAndPlaneAngleUnit.hxx>
#include <StepBasic_SiUnitAndSolidAngleUnit.hxx>
#include <StepBasic_SiUnitAndVolumeUnit.hxx>
#include <STEPConstruct_UnitContext.hxx>
#include <StepData_Factors.hxx>
#include <StepData_StepModel.hxx>
#include <StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx.hxx>
#include <StepRepr_GlobalUncertaintyAssignedContext.hxx>
#include <StepRepr_GlobalUnitAssignedContext.hxx>
#include <StepRepr_ReprItemAndMeasureWithUnit.hxx>
#include <TCollection_HAsciiString.hxx>

//=================================================================================================

STEPConstruct_UnitContext::STEPConstruct_UnitContext()
    : done(false),
      lengthFactor(0.0),
      planeAngleFactor(0.0),
      solidAngleFactor(0.0),
      areaFactor(0.0),
      volumeFactor(0.0)
{
  lengthDone = planeAngleDone = solidAngleDone = hasUncertainty = areaDone = volumeDone = false;
  // pdn file r_47-sd.stp initialize field.
  theUncertainty = RealLast();
}

//=================================================================================================

void STEPConstruct_UnitContext::Init(const double                           Tol3d,
                                     const occ::handle<StepData_StepModel>& theModel,
                                     const StepData_Factors&                theLocalFactors)
{
  done = true;

  GRC = new StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx;
  occ::handle<TCollection_HAsciiString> contextID =
    new TCollection_HAsciiString("Context #1"); // ?????

  occ::handle<TCollection_HAsciiString> contextType =
    new TCollection_HAsciiString("3D Context with UNIT and UNCERTAINTY");

  // Units : LengthUnit and PlaneAngleUnit (no SolidAngleUnit applicable)

  occ::handle<StepBasic_NamedUnit> lengthUnit;
  const char*                      uName   = 0;
  bool                             hasPref = true;
  StepBasic_SiPrefix               siPref  = StepBasic_spMilli;
  double                           aScale  = 1.;
  switch (theModel->InternalParameters.WriteUnit)
  {
    case 1:
      uName  = "INCH";
      aScale = 25.4;
      break;
    default:
    case 2:
      break;
    case 4:
      uName  = "FOOT";
      aScale = 304.8;
      break;
    case 5:
      uName  = "MILE";
      aScale = 1609344.0;
      break;
    case 6:
      hasPref = false;
      aScale  = 1000.0;
      break;
    case 7:
      siPref = StepBasic_spKilo;
      aScale = 1000000.0;
      break;
    case 8:
      uName  = "MIL";
      aScale = 0.0254;
      break;
    case 9:
      siPref = StepBasic_spMicro;
      aScale = 0.001;
      break;
    case 10:
      siPref = StepBasic_spCenti;
      aScale = 10.0;
      break;
    case 11:
      uName  = "MICROINCH";
      aScale = 0.0000254;
      break;
  }

  occ::handle<StepBasic_SiUnitAndLengthUnit> siUnit = new StepBasic_SiUnitAndLengthUnit;
  siUnit->Init(hasPref, siPref, StepBasic_sunMetre);

  if (uName)
  { // for non-metric units, create conversion_based_unit
    occ::handle<StepBasic_MeasureValueMember> val = new StepBasic_MeasureValueMember;
    val->SetName("LENGTH_UNIT");
    val->SetReal(aScale);

    occ::handle<StepBasic_LengthMeasureWithUnit> measure = new StepBasic_LengthMeasureWithUnit;
    StepBasic_Unit                               Unit;
    Unit.SetValue(siUnit);
    measure->Init(val, Unit);

    occ::handle<StepBasic_DimensionalExponents> theDimExp = new StepBasic_DimensionalExponents;
    theDimExp->Init(1., 0., 0., 0., 0., 0., 0.);

    occ::handle<TCollection_HAsciiString> convName = new TCollection_HAsciiString(uName);
    occ::handle<StepBasic_ConversionBasedUnitAndLengthUnit> convUnit =
      new StepBasic_ConversionBasedUnitAndLengthUnit;
    convUnit->Init(theDimExp, convName, measure);

    lengthUnit = convUnit;
  }
  else
    lengthUnit = siUnit;

  occ::handle<StepBasic_SiUnitAndPlaneAngleUnit> radianUnit = new StepBasic_SiUnitAndPlaneAngleUnit;
  radianUnit->Init(false,
                   StepBasic_spMilli, // the unit is radian, no prefix
                   StepBasic_sunRadian);

  occ::handle<NCollection_HArray1<occ::handle<StepBasic_NamedUnit>>> units =
    new NCollection_HArray1<occ::handle<StepBasic_NamedUnit>>(1, 3);

  occ::handle<StepBasic_SiUnitAndSolidAngleUnit> sradUnit = new StepBasic_SiUnitAndSolidAngleUnit;
  sradUnit->Init(false,
                 StepBasic_spMilli, // the unit is steradian, no prefix
                 StepBasic_sunSteradian);

  units->SetValue(1, lengthUnit);
  units->SetValue(2, radianUnit);
  units->SetValue(3, sradUnit);

  // Uncertainty : 3D confusion Tolerance

  occ::handle<NCollection_HArray1<occ::handle<StepBasic_UncertaintyMeasureWithUnit>>> Tols =
    new NCollection_HArray1<occ::handle<StepBasic_UncertaintyMeasureWithUnit>>(1, 1);
  occ::handle<StepBasic_UncertaintyMeasureWithUnit> theTol3d =
    new StepBasic_UncertaintyMeasureWithUnit;

  occ::handle<TCollection_HAsciiString> TolName =
    new TCollection_HAsciiString("distance_accuracy_value");
  occ::handle<TCollection_HAsciiString> TolDesc =
    new TCollection_HAsciiString("confusion accuracy");

  occ::handle<StepBasic_MeasureValueMember> mvs = new StepBasic_MeasureValueMember;
  mvs->SetName("LENGTH_MEASURE");
  mvs->SetReal(Tol3d / theLocalFactors.LengthFactor());
  StepBasic_Unit Unit;
  Unit.SetValue(lengthUnit);
  theTol3d->Init(mvs, Unit, TolName, TolDesc);
  Tols->SetValue(1, theTol3d);

  GRC->Init(contextID, contextType, 3, units, Tols);
}

//=================================================================================================

bool STEPConstruct_UnitContext::IsDone() const
{
  return done;
}

//=================================================================================================

occ::handle<StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx>
  STEPConstruct_UnitContext::Value() const
{
  return GRC;
}

// ==========================================================================
// Method  : ConvertSiPrefix
// Purpose : Computes SiPrefix conversion
// ==========================================================================

double STEPConstruct_UnitContext::ConvertSiPrefix(const StepBasic_SiPrefix aPrefix)
{
  switch (aPrefix)
  {
    case StepBasic_spExa:
      return 1.E+18;
    case StepBasic_spPeta:
      return 1.E+15;
    case StepBasic_spTera:
      return 1.E+12;
    case StepBasic_spGiga:
      return 1.E+9;
    case StepBasic_spMega:
      return 1.E+6;
    case StepBasic_spKilo:
      return 1.E+3;
    case StepBasic_spHecto:
      return 1.E+2;
    case StepBasic_spDeca:
      return 1.E+1;
    case StepBasic_spDeci:
      return 1.E-1;
    case StepBasic_spCenti:
      return 1.E-2;
    case StepBasic_spMilli:
      return 1.E-3;
    case StepBasic_spMicro:
      return 1.E-6;
    case StepBasic_spNano:
      return 1.E-9;
    case StepBasic_spPico:
      return 1.E-12;
    case StepBasic_spFemto:
      return 1.E-15;
    case StepBasic_spAtto:
      return 1.E-18;
    default:
      break;
  }
  return 1.;
}

//=================================================================================================

bool STEPConstruct_UnitContext::SiUnitNameFactor(const occ::handle<StepBasic_SiUnit>& aSiUnit,
                                                 double& theSIUNFactor) const
{
  theSIUNFactor = 1.;
  switch (aSiUnit->Name())
  {
    case StepBasic_sunMetre:
    case StepBasic_sunRadian:
    case StepBasic_sunSteradian:
      return true;
    default:
      //	std::cout << "Unknown SiUnitName : " << aSiUnit->Name() << std::endl;
      return false;
  }
}

//=================================================================================================

int STEPConstruct_UnitContext::ComputeFactors(
  const occ::handle<StepRepr_GlobalUnitAssignedContext>& aContext,
  const StepData_Factors&                                theLocalFactors)
{
  int status = 0;

  // Initialise the default value
  // status : 0 if OK, else 1 2 3
  lengthFactor = solidAngleFactor = 1.;
  planeAngleFactor                = M_PI / 180.;
  //  double theLExp   = 1.;
  //  double thePAExp  = 0.;
  //  double theSAExp  = 0.;
  lengthDone = planeAngleDone = solidAngleDone = false;

  if (aContext.IsNull())
  {
    return 1;
  }

  // Start Computation
  occ::handle<NCollection_HArray1<occ::handle<StepBasic_NamedUnit>>> theUnits = aContext->Units();
  int                                                                nbU      = aContext->NbUnits();

  for (int i = 1; i <= nbU; i++)
  {
    occ::handle<StepBasic_NamedUnit> theNamedUnit = aContext->UnitsValue(i);
    status                                        = ComputeFactors(theNamedUnit, theLocalFactors);
  }
  return status;
}

//=================================================================================================

int STEPConstruct_UnitContext::ComputeFactors(const occ::handle<StepBasic_NamedUnit>& aUnit,
                                              const StepData_Factors& theLocalFactors)
{

  //: f3 abv 8 Apr 98: ProSTEP TR8 tr8_as_sd_sw: the case of unrecognized entity
  if (aUnit.IsNull())
    return -1;

  int    status    = 0;
  double theFactor = 0.;
  double theSIUNF  = 0.;

  double parameter     = 0.;
  bool   parameterDone = false;
  if (aUnit->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnit)))
  {
    occ::handle<StepBasic_ConversionBasedUnit> theCBU =
      occ::down_cast<StepBasic_ConversionBasedUnit>(aUnit);

    occ::handle<StepBasic_MeasureWithUnit> aMWU;
    if (!theCBU.IsNull())
    {
      occ::handle<Standard_Transient> aConvFactor = theCBU->ConversionFactor();
      if (aConvFactor->IsKind(STANDARD_TYPE(StepBasic_MeasureWithUnit)))
      {
        aMWU = occ::down_cast<StepBasic_MeasureWithUnit>(aConvFactor);
      }
      else if (aConvFactor->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndMeasureWithUnit)))
      {
        occ::handle<StepRepr_ReprItemAndMeasureWithUnit> aReprMeasureItem =
          occ::down_cast<StepRepr_ReprItemAndMeasureWithUnit>(aConvFactor);
        aMWU = aReprMeasureItem->GetMeasureWithUnit();
      }

      // sln 8.10.2001: the case of unrecognized entity
      if (aMWU.IsNull())
      {
        return -1;
      }

      occ::handle<StepBasic_NamedUnit> theTargetUnit = aMWU->UnitComponent().NamedUnit();
      double                           theSIPFactor  = 1.;

      //: f5 abv 24 Apr 98: ProSTEP TR8 tr8_bv1_tc: INCHES
      occ::handle<StepBasic_SiUnit> theSIU = occ::down_cast<StepBasic_SiUnit>(theTargetUnit);

      if (!theSIU.IsNull())
      {
        if (theSIU->HasPrefix())
        {
          // Treat the prefix
          StepBasic_SiPrefix aPrefix = theSIU->Prefix();
          theSIPFactor               = ConvertSiPrefix(aPrefix);
        }
        // Treat the SiUnitName
        if (!SiUnitNameFactor(theSIU, theSIUNF))
          status = 11; // et continue
      }
      else
      {
        return 3;
      }
      double theMVAL = aMWU->ValueComponent();
      theFactor      = theSIPFactor * theMVAL; // * theSIUNF * pow(10.,theLExp)
    }
    parameter = theFactor;
    if (!parameterDone)
    {
      parameterDone = true;
    }
    else
    {
      status = 14;
    }
  }
  else if (aUnit->IsKind(STANDARD_TYPE(StepBasic_SiUnit)))
  {
    occ::handle<StepBasic_SiUnit> theSIU       = occ::down_cast<StepBasic_SiUnit>(aUnit);
    double                        theSIPFactor = 1.;
    if (theSIU->HasPrefix())
    {
      // Treat the prefix
      StepBasic_SiPrefix aPrefix = theSIU->Prefix();
      theSIPFactor               = ConvertSiPrefix(aPrefix);
    }

    // Treat the SiUnitName
    if (!SiUnitNameFactor(theSIU, theSIUNF))
      status = 11;

    // final computation for lengthFactor
    theFactor = theSIPFactor * theSIUNF;
    parameter = theFactor;
    if (!parameterDone)
    {
      parameterDone = true;
    }
    else
    {
      status = 14;
    }
  }

  // Defining a type of unit
  if (!parameterDone)
  {
    return 0;
  }

  const double aCascadeUnit = theLocalFactors.CascadeUnit();
  if (aUnit->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndLengthUnit))
      || aUnit->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndLengthUnit)))
  {
#ifdef METER
    lengthFactor = parameter;
#else
    lengthFactor = parameter * 1000. / aCascadeUnit;
#endif
    if (!lengthDone)
      lengthDone = true;
    else
    {
      status = 14;
    }
  } // end of LengthUnit
  else if (aUnit->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndPlaneAngleUnit))
           || aUnit->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndPlaneAngleUnit)))
  {
    planeAngleFactor = parameter;
    planeAngleDone   = true;
  } // end of PlaneAngleUnit
  else if (aUnit->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndSolidAngleUnit))
           || aUnit->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndSolidAngleUnit)))
  {
    solidAngleFactor = parameter;
    solidAngleDone   = true;
  } // end of SolidAngleUnit
  else if (aUnit->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndAreaUnit))
           || aUnit->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndAreaUnit)))
  {
    double af;
#ifdef METER
    af = parameter;
#else
    af = parameter * 1000. / aCascadeUnit;
#endif
    areaDone   = true;
    areaFactor = pow(af, 2);
  }
  else if (aUnit->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndVolumeUnit))
           || aUnit->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndVolumeUnit)))
  {
    double af;
#ifdef METER
    af = parameter;
#else
    af = parameter * 1000. / aCascadeUnit;
#endif
    volumeDone   = true;
    volumeFactor = pow(af, 3);
  }
  return status;
}

//=================================================================================================

int STEPConstruct_UnitContext::ComputeTolerance(
  const occ::handle<StepRepr_GlobalUncertaintyAssignedContext>& aContext)
{
  int status = 0;
  // Decode the Uncertainty information (geometric accuracy)

  hasUncertainty    = false;
  int nbUncertainty = 0;

  if (!aContext.IsNull())
    nbUncertainty = aContext->NbUncertainty();
  else
    return 40;

  for (int un = 1; un <= nbUncertainty; un++)
  {
    occ::handle<StepBasic_UncertaintyMeasureWithUnit> aUMWU = aContext->UncertaintyValue(un);
    if (aUMWU.IsNull())
    {
      continue;
    }
    // Decode the associated Unit
    occ::handle<StepBasic_SiUnitAndLengthUnit> aUnit =
      occ::down_cast<StepBasic_SiUnitAndLengthUnit>(aUMWU->UnitComponent().NamedUnit());
    if (!aUnit.IsNull())
    {
      // Extract Uncertainty value
      double LengthUncertainty = aUMWU->ValueComponent();
      // Update it according to the Length Unit Factor
      // pdn r_47-sd.stp to choose minimal uncertainty
      if (theUncertainty > LengthUncertainty)
        theUncertainty = LengthUncertainty;
      hasUncertainty = true;
    }
    else
    {
      occ::handle<StepBasic_ConversionBasedUnitAndLengthUnit> aCBULU =
        occ::down_cast<StepBasic_ConversionBasedUnitAndLengthUnit>(
          aUMWU->UnitComponent().NamedUnit());
      if (!aCBULU.IsNull())
      {
        // Extract Uncertainty value
        double LengthUncertainty = aUMWU->ValueComponent();
        // Update it according to the Length Unit Factor
        // pdn r_47-sd.stp to choose minimal uncertainty
        // clang-format off
	if(theUncertainty > LengthUncertainty) theUncertainty =  LengthUncertainty; // *lengthFactor; fait par appelant
        // clang-format on
        hasUncertainty = true;
      }
    }
  }

  return status;
}

//=================================================================================================

double STEPConstruct_UnitContext::LengthFactor() const
{
  return lengthFactor;
}

//=================================================================================================

double STEPConstruct_UnitContext::PlaneAngleFactor() const
{
  return planeAngleFactor;
}

//=================================================================================================

double STEPConstruct_UnitContext::SolidAngleFactor() const
{
  return solidAngleFactor;
}

//=================================================================================================

double STEPConstruct_UnitContext::Uncertainty() const
{
  return theUncertainty;
}

//=================================================================================================

bool STEPConstruct_UnitContext::HasUncertainty() const
{
  return hasUncertainty;
}

//=================================================================================================

bool STEPConstruct_UnitContext::LengthDone() const
{
  return lengthDone;
}

//=================================================================================================

bool STEPConstruct_UnitContext::PlaneAngleDone() const
{
  return planeAngleDone;
}

//=================================================================================================

bool STEPConstruct_UnitContext::SolidAngleDone() const
{
  return solidAngleDone;
}

//=================================================================================================

const char* STEPConstruct_UnitContext::StatusMessage(const int status) const
{
  switch (status)
  {
    case 0:
      return "";
    case 1:
      return "No GlobalUnitAssignedContext, default taken";
    case 2:
      return "No LengthMeasureWithUnit, default taken";
    case 3:
      return "No SiUnit for LengthMeasure undefined, default taken";
    case 4:
      return "No PlaneAngleMeasureWithUnit, default taken";
    case 5:
      return "No SiUnit for PlaneAngleMeasure undefined, default taken";
    case 6:
      return "No SolidAngleMeasureWithUnit, default taken";
    case 7:
      return "No SiUnit for SolidAngleMeasure undefined, default taken";

    case 11:
      return "Length Unit not recognized, default taken";
    case 12:
      return "Plane Angle Unit not recognized, default taken";
    case 13:
      return "Solid Angle Unit not recognized, default taken";
    case 14:
      return "At least one unit is twice defined";

    case 40:
      return "Bad GlobalUncertaintyAssignedContext, default unit taken";

    default:
      break;
  }

  return "Badly defined units, default taken";
}

double STEPConstruct_UnitContext::AreaFactor() const
{
  return areaFactor;
}

double STEPConstruct_UnitContext::VolumeFactor() const
{
  return volumeFactor;
}

bool STEPConstruct_UnitContext::AreaDone() const
{
  return areaDone;
}

bool STEPConstruct_UnitContext::VolumeDone() const
{
  return volumeDone;
}
