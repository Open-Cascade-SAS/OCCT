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

//abv 17.11.99: renamed from StepPDR_MakeUnitAndToleranceContext and merged with STEPControl_Unit
//abv 30.02.00: ability to write file in units other than MM

#include <Interface_Static.hxx>
#include <StepBasic_ConversionBasedUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndAreaUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndLengthUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndPlaneAngleUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndSolidAngleUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndVolumeUnit.hxx>
#include <StepBasic_DimensionalExponents.hxx>
#include <StepBasic_HArray1OfNamedUnit.hxx>
#include <StepBasic_HArray1OfUncertaintyMeasureWithUnit.hxx>
#include <StepBasic_LengthMeasureWithUnit.hxx>
#include <StepBasic_LengthUnit.hxx>
#include <StepBasic_MeasureValueMember.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepBasic_NamedUnit.hxx>
#include <StepBasic_PlaneAngleMeasureWithUnit.hxx>
#include <StepBasic_PlaneAngleUnit.hxx>
#include <StepBasic_SiPrefix.hxx>
#include <StepBasic_SiUnit.hxx>
#include <StepBasic_SiUnitAndAreaUnit.hxx>
#include <StepBasic_SiUnitAndLengthUnit.hxx>
#include <StepBasic_SiUnitAndPlaneAngleUnit.hxx>
#include <StepBasic_SiUnitAndSolidAngleUnit.hxx>
#include <StepBasic_SiUnitAndVolumeUnit.hxx>
#include <StepBasic_SolidAngleMeasureWithUnit.hxx>
#include <StepBasic_SolidAngleUnit.hxx>
#include <StepBasic_UncertaintyMeasureWithUnit.hxx>
#include <STEPConstruct_UnitContext.hxx>
#include <StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx.hxx>
#include <StepRepr_GlobalUncertaintyAssignedContext.hxx>
#include <StepRepr_GlobalUnitAssignedContext.hxx>
#include <TCollection_HAsciiString.hxx>
#include <UnitsMethods.hxx>

//=======================================================================
//function : STEPConstruct_UnitContext
//purpose  : 
//=======================================================================
STEPConstruct_UnitContext::STEPConstruct_UnitContext() : done(Standard_False)
{
  lengthDone = planeAngleDone = solidAngleDone = hasUncertainty = 
    areaDone = volumeDone = Standard_False;
  //pdn file r_47-sd.stp initalize field.
  theUncertainty = RealLast();
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void STEPConstruct_UnitContext::Init(const Standard_Real Tol3d) 
{
  done = Standard_True;

  GRC = new StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx;
  Handle(TCollection_HAsciiString) contextID =
    new TCollection_HAsciiString("Context #1"); // ?????
  
  Handle(TCollection_HAsciiString) contextType =
   new TCollection_HAsciiString("3D Context with UNIT and UNCERTAINTY");

  // Units : LengthUnit and PlaneAngleUnit (no SolidAngleUnit appliable)

  Handle(StepBasic_NamedUnit) lengthUnit;
  Standard_CString uName = 0;
  Standard_Boolean hasPref = Standard_True;
  StepBasic_SiPrefix siPref = StepBasic_spMilli;
  switch ( Interface_Static::IVal ( "write.step.unit" ) ) {
  case  1 : uName = "INCH";             break;
  default :
  case  2 :                             break;
  case  4 : uName = "FOOT";             break;
  case  5 : uName = "MILE";             break;
  case  6 : hasPref = Standard_False;   break;
  case  7 : siPref = StepBasic_spKilo;  break;
  case  8 : uName = "MIL";              break;
  case  9 : siPref = StepBasic_spMicro; break;
  case 10 : siPref = StepBasic_spCenti; break;
  case 11 : uName = "MICROINCH";        break;
  }
  
  Handle(StepBasic_SiUnitAndLengthUnit) siUnit =
    new StepBasic_SiUnitAndLengthUnit;
  siUnit->Init(hasPref,siPref,StepBasic_sunMetre);

  if ( uName ) { // for non-metric units, create conversion_based_unit
    Handle(StepBasic_MeasureValueMember) val = new StepBasic_MeasureValueMember;
    val->SetName("LENGTH_UNIT");
    val->SetReal ( UnitsMethods::GetLengthFactorValue ( Interface_Static::IVal ( "write.step.unit" ) ) );

    Handle(StepBasic_LengthMeasureWithUnit) measure = new StepBasic_LengthMeasureWithUnit;
    StepBasic_Unit Unit;
    Unit.SetValue ( siUnit );
    measure->Init ( val, Unit );

    Handle(StepBasic_DimensionalExponents) theDimExp = new StepBasic_DimensionalExponents;
    theDimExp->Init ( 1., 0., 0., 0., 0., 0., 0. );
    
    Handle(TCollection_HAsciiString) convName = new TCollection_HAsciiString ( uName );
    Handle(StepBasic_ConversionBasedUnitAndLengthUnit) convUnit =
      new StepBasic_ConversionBasedUnitAndLengthUnit;
    convUnit->Init ( theDimExp, convName, measure );
    
    lengthUnit = convUnit;
  }
  else lengthUnit = siUnit;
  
  Handle(StepBasic_SiUnitAndPlaneAngleUnit) radianUnit =
    new StepBasic_SiUnitAndPlaneAngleUnit;
  radianUnit ->Init(Standard_False,
		    StepBasic_spMilli,    // the unit is radian, no prefix
		    StepBasic_sunRadian);

  Handle(StepBasic_HArray1OfNamedUnit) units =
    new StepBasic_HArray1OfNamedUnit(1, 3);

  Handle(StepBasic_SiUnitAndSolidAngleUnit) sradUnit =
    new StepBasic_SiUnitAndSolidAngleUnit;
  sradUnit ->Init(Standard_False,
		  StepBasic_spMilli,    // the unit is steradian, no prefix
		  StepBasic_sunSteradian);

  units->SetValue(1, lengthUnit);
  units->SetValue(2, radianUnit);
  units->SetValue(3, sradUnit);

  // Uncertainty : 3D confusion Tolerance

  Handle(StepBasic_HArray1OfUncertaintyMeasureWithUnit) Tols = 
    new StepBasic_HArray1OfUncertaintyMeasureWithUnit(1,1);
  Handle(StepBasic_UncertaintyMeasureWithUnit) theTol3d = 
    new StepBasic_UncertaintyMeasureWithUnit;
  
  Handle(TCollection_HAsciiString) TolName = 
    new TCollection_HAsciiString("distance_accuracy_value");
  Handle(TCollection_HAsciiString) TolDesc =
    new TCollection_HAsciiString("confusion accuracy");
  
  Handle(StepBasic_MeasureValueMember) mvs = new StepBasic_MeasureValueMember;
  mvs->SetName("LENGTH_MEASURE");
  mvs->SetReal ( Tol3d / UnitsMethods::LengthFactor() );
  StepBasic_Unit Unit;
  Unit.SetValue ( lengthUnit );
  theTol3d->Init(mvs, Unit, TolName, TolDesc);
  Tols->SetValue(1, theTol3d);

  GRC->Init(contextID, contextType, 3, units, Tols);
}
  
//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean STEPConstruct_UnitContext::IsDone() const
{
  return done;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Handle(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx) STEPConstruct_UnitContext::Value() const
{
  return GRC;
}

// ==========================================================================
// Method  : ConvertSiPrefix
// Purpose : Computes SiPrefix conversion
// ==========================================================================

Standard_Real STEPConstruct_UnitContext::ConvertSiPrefix (const StepBasic_SiPrefix aPrefix)
{
  switch(aPrefix)
    {
    case StepBasic_spExa:      return 1.E+18;
    case StepBasic_spPeta:     return 1.E+15;
    case StepBasic_spTera:     return 1.E+12;
    case StepBasic_spGiga:     return 1.E+9;
    case StepBasic_spMega:     return 1.E+6;
    case StepBasic_spKilo:     return 1.E+3;
    case StepBasic_spHecto:    return 1.E+2;
    case StepBasic_spDeca:     return 1.E+1;
    case StepBasic_spDeci:     return 1.E-1;
    case StepBasic_spCenti:    return 1.E-2;
    case StepBasic_spMilli:    return 1.E-3;
    case StepBasic_spMicro:    return 1.E-6;
    case StepBasic_spNano:     return 1.E-9;
    case StepBasic_spPico:     return 1.E-12;
    case StepBasic_spFemto:    return 1.E-15;
    case StepBasic_spAtto:     return 1.E-18;
    default:
      break;
    }
  return 1.;
}


// ==========================================================================
// Method  : STEPConstruct_UnitContext::SiUnitNameFactor
// Purpose : 
// ==========================================================================

Standard_Boolean STEPConstruct_UnitContext::SiUnitNameFactor(const Handle(StepBasic_SiUnit)& aSiUnit,
						    Standard_Real& theSIUNFactor) const
{
  theSIUNFactor = 1.;
  switch ( aSiUnit->Name() )
  {
  case StepBasic_sunMetre:
  case StepBasic_sunRadian:
  case StepBasic_sunSteradian: 
    return Standard_True;
  default:
//	cout << "Unknown SiUnitName : " << aSiUnit->Name() << endl;
    return Standard_False;
  }
}

// ==========================================================================
// Method  : STEPConstruct_UnitContext::ComputeFactors
// Purpose : 
// ==========================================================================

Standard_Integer STEPConstruct_UnitContext::ComputeFactors(const Handle(StepRepr_GlobalUnitAssignedContext)& aContext)
{
  Standard_Integer status = 0;

  // Initialise the default value
  // status : 0 if OK, else 1 2 3
  lengthFactor = solidAngleFactor = 1.;
  planeAngleFactor = M_PI/180.;
//  Standard_Real theLExp   = 1.;
//  Standard_Real thePAExp  = 0.;
//  Standard_Real theSAExp  = 0.;
  lengthDone = planeAngleDone = solidAngleDone = Standard_False;
  
  if (aContext.IsNull()) {
#ifdef OCCT_DEBUG
    cout<<" -- STEPConstruct_UnitContext:ComputeFactor, Context undefined -> default"<<endl;
#endif
    return 1;
  }

  // Start Computation
  Handle(StepBasic_HArray1OfNamedUnit) theUnits = aContext->Units();
  Standard_Integer nbU = aContext->NbUnits();
  
  for (Standard_Integer i = 1; i <= nbU; i++) {
    Handle(StepBasic_NamedUnit) theNamedUnit = aContext->UnitsValue(i);
    status = ComputeFactors(theNamedUnit);
#ifdef OCCT_DEBUG
    if(status == -1)
      cout << " -- STEPConstruct_UnitContext:ComputeFactor: Unit item no." << i << " is not recognized" << endl;
#endif    
  }
  return status;
} 


Standard_Integer STEPConstruct_UnitContext::ComputeFactors(const Handle(StepBasic_NamedUnit)& aUnit)
{

  //:f3 abv 8 Apr 98: ProSTEP TR8 tr8_as_sd_sw: the case of unrecognized entity
  if ( aUnit.IsNull() ) 
    return -1;

  Standard_Integer status = 0;
  Standard_Real theFactor= 0.; 
  Standard_Real theSIUNF  = 0.;
  
  Standard_Real parameter= 0.;
  Standard_Boolean parameterDone = Standard_False;
  if(aUnit->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnit))) {
    Handle(StepBasic_ConversionBasedUnit) theCBU =
      Handle(StepBasic_ConversionBasedUnit)::DownCast(aUnit);
//    Handle(StepBasic_DimensionalExponents) theDimExp = theCBU->Dimensions();
    Handle(StepBasic_MeasureWithUnit) theMWU;
    if(!theCBU.IsNull()) {
       theMWU = theCBU->ConversionFactor();
       // sln 8.10.2001: the case of unrecognized entity
       if(theMWU.IsNull())
         return -1;
      //if (!theMWU->IsKind(STANDARD_TYPE(StepBasic_LengthMeasureWithUnit))) { gka
      //	return 2;
      //}
      Handle(StepBasic_NamedUnit) theTargetUnit = theMWU->UnitComponent().NamedUnit();
      //StepBasic_Unit theTargetUnit = theMWU->UnitComponent();
      Standard_Real theSIPFactor = 1.;
	
      //:f5 abv 24 Apr 98: ProSTEP TR8 tr8_bv1_tc: INCHES
//gka   Handle(StepBasic_SiUnitAndLengthUnit) theSUALU =
//	Handle(StepBasic_SiUnitAndLengthUnit)::DownCast(theTargetUnit);
//      Handle(StepBasic_SiUnit) theSIU;
//      if ( ! theSUALU.IsNull() ) theSIU = Handle(StepBasic_SiUnit)::DownCast(theSUALU);
    Handle(StepBasic_SiUnit) theSIU =  //f5
      Handle(StepBasic_SiUnit)::DownCast(theTargetUnit);//f5
      
      if (!theSIU.IsNull()) {
	if (theSIU->HasPrefix()) {
	  // Treat the prefix
	  StepBasic_SiPrefix aPrefix = theSIU->Prefix();
	  theSIPFactor = ConvertSiPrefix(aPrefix); 
	}
	// Treat the SiUnitName
	if (!SiUnitNameFactor(theSIU,theSIUNF)) status = 11; // et continue
	//cout << "The SiUnitNameFactor is :";
	//cout << theSIUNF << endl;
      }
      else {
	//      cout << "Recursive algo required - Aborted" << endl;
	return 3;
      }
      Standard_Real theMVAL = theMWU->ValueComponent();
      theFactor = theSIPFactor * theMVAL; // * theSIUNF * pow(10.,theLExp)
    }
    parameter = theFactor;
    if(!parameterDone) {
      parameterDone = Standard_True;
    }
    else {
      status = 14;
#ifdef OCCT_DEBUG
      cout << "Error in the file : parameter double defined" << endl;
#endif
    }
  }    
  else if (aUnit->IsKind(STANDARD_TYPE(StepBasic_SiUnit))) {
    Handle(StepBasic_SiUnit) theSIU = Handle(StepBasic_SiUnit)::DownCast(aUnit);
    Standard_Real theSIPFactor = 1.;
    if (theSIU->HasPrefix()) {
      // Treat the prefix
      StepBasic_SiPrefix aPrefix = theSIU->Prefix();
      theSIPFactor = ConvertSiPrefix(aPrefix); 
    }
    
    // Treat the SiUnitName
    if (!SiUnitNameFactor(theSIU,theSIUNF)) status = 11;
    
    // final computation for lengthFactor
    theFactor = theSIPFactor * theSIUNF;
    parameter = theFactor;
    if (!parameterDone) {
      parameterDone = Standard_True;
    }
    else {
      status = 14;
#ifdef OCCT_DEBUG
      cout << "Error in the file : parameter double defined" << endl;
#endif
    }
  }
  
  // Defining a type of unit
  if(!parameterDone) {
#ifdef OCCT_DEBUG
    cout << "Unit Type not implemented" << endl;
#endif 
    return 0;
  }
  
  if (aUnit->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndLengthUnit))||
      aUnit->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndLengthUnit))) {
#ifdef METER
    lengthFactor = parameter;
#else
    lengthFactor = parameter * 1000. / UnitsMethods::GetCasCadeLengthUnit();
#endif    
    if(!lengthDone) 
      lengthDone = Standard_True;
    else {
      status = 14;
#ifdef OCCT_DEBUG
      cout << "Error in the file : LengthFactor double defined" << endl;
#endif    
    }
  }  // end of LengthUnit
  else if (aUnit->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndPlaneAngleUnit))||
	   aUnit->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndPlaneAngleUnit))) {
    planeAngleFactor = parameter;
    planeAngleDone = Standard_True;	
  }  // end of PlaneAngleUnit
  else if (aUnit->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndSolidAngleUnit))||
	   aUnit->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndSolidAngleUnit))) {
    solidAngleFactor = parameter;
    solidAngleDone = Standard_True;
  }  // end of SolidAngleUnit 
  else if (aUnit->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndAreaUnit)) ||
	   aUnit->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndAreaUnit))) {
    Standard_Real af;
#ifdef METER   
    af = parameter;
#else
    af = parameter * 1000. / UnitsMethods::GetCasCadeLengthUnit();
#endif    
    areaDone = Standard_True;
    areaFactor = pow(af,2);
  }
  else if (aUnit->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndVolumeUnit)) ||
	   aUnit->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndVolumeUnit))) {
    Standard_Real af;
#ifdef METER   
    af = parameter;
#else
    af = parameter * 1000. / UnitsMethods::GetCasCadeLengthUnit();
#endif
    volumeDone = Standard_True;
    volumeFactor = pow(af,3);
  }   
  return status;
}


// ==========================================================================
// Method  : STEPConstruct_UnitContext::ComputeTolerance
// Purpose : 
// ==========================================================================

Standard_Integer STEPConstruct_UnitContext::ComputeTolerance
(const Handle(StepRepr_GlobalUncertaintyAssignedContext)& aContext)
{
  Standard_Integer status = 0;
    // Decode the Uncertainty information (geometric accuracy)

  hasUncertainty = Standard_False;
  Standard_Integer nbUncertainty = 0;
    
  if (!aContext.IsNull()) nbUncertainty = aContext->NbUncertainty();
  else return 40;

  for (Standard_Integer un = 1 ; un <= nbUncertainty ; un ++) {
    Handle(StepBasic_UncertaintyMeasureWithUnit) aUMWU = aContext->UncertaintyValue(un);
    if (aUMWU.IsNull()) {
#ifdef OCCT_DEBUG
      cout<<"BAD Uncertainty Measure with Units, n0."<<un<<endl;
#endif
      continue;
    }
    // Decode the associated Unit
    Handle(StepBasic_SiUnitAndLengthUnit) aUnit = 
      Handle(StepBasic_SiUnitAndLengthUnit)::DownCast(aUMWU->UnitComponent().NamedUnit());
    if (!aUnit.IsNull()) {
      // Extract Uncertainty value
      Standard_Real LengthUncertainty = aUMWU->ValueComponent();
      // Update it according to the Length Unit Factor
      //pdn r_47-sd.stp to choose minimal uncertainty
      if(theUncertainty > LengthUncertainty) theUncertainty =  LengthUncertainty;
      hasUncertainty = Standard_True;

    } else {
      Handle(StepBasic_ConversionBasedUnitAndLengthUnit) aCBULU =
	Handle(StepBasic_ConversionBasedUnitAndLengthUnit)::DownCast(aUMWU->UnitComponent().NamedUnit());
      if (!aCBULU.IsNull()) {
	// Extract Uncertainty value
	Standard_Real LengthUncertainty = aUMWU->ValueComponent();
	// Update it according to the Length Unit Factor
	//pdn r_47-sd.stp to choose minimal uncertainty
	if(theUncertainty > LengthUncertainty) theUncertainty =  LengthUncertainty; // *lengthFactor; fait par appelant
	hasUncertainty = Standard_True;
      }
    }
  }

#ifdef OCCT_DEBUG
  if (hasUncertainty) cout << "UNCERTAINTY read as " << theUncertainty << endl;
#endif
  return status;
}

// ==========================================================================
// Method  : STEPConstruct_UnitContext::LengthFactor
// Purpose : 
// ==========================================================================
Standard_Real STEPConstruct_UnitContext::LengthFactor() const 
{  return lengthFactor;  }

// ==========================================================================
// Method  : STEPConstruct_UnitContext::PlaneAngleFactor
// Purpose : 
// ==========================================================================
Standard_Real STEPConstruct_UnitContext::PlaneAngleFactor() const 
{  return planeAngleFactor;  }

// ==========================================================================
// Method  : STEPConstruct_UnitContext::SolidAngleFactor
// Purpose : 
// ==========================================================================
Standard_Real STEPConstruct_UnitContext::SolidAngleFactor() const 
{  return solidAngleFactor;  }

// ==========================================================================
// Method  : STEPConstruct_UnitContext::Uncertainty
// Purpose : 
// ==========================================================================
Standard_Real STEPConstruct_UnitContext::Uncertainty () const 
{  return theUncertainty;  }

// ==========================================================================
// Method  : STEPConstruct_UnitContext::HasUncertainty
// Purpose : 
// ==========================================================================
Standard_Boolean STEPConstruct_UnitContext::HasUncertainty () const 
{  return hasUncertainty;  }

// ==========================================================================
// Method  : STEPConstruct_UnitContext::LengthDone
// Purpose : 
// ==========================================================================
Standard_Boolean STEPConstruct_UnitContext::LengthDone() const
{  return lengthDone;  }

// ==========================================================================
// Method  : STEPConstruct_UnitContext::PlaneAngleDone
// Purpose : 
// ==========================================================================
Standard_Boolean STEPConstruct_UnitContext::PlaneAngleDone() const
{  return planeAngleDone;  }

// ==========================================================================
// Method  : STEPConstruct_UnitContext::SolidAngleDone
// Purpose : 
// ==========================================================================
Standard_Boolean STEPConstruct_UnitContext::SolidAngleDone() const
{  return solidAngleDone;  }

// ==========================================================================
// Method  : STEPConstruct_UnitContext::StatusMessage
// Purpose : 
// ==========================================================================
Standard_CString  STEPConstruct_UnitContext::StatusMessage (const Standard_Integer status) const
{
  switch (status) {
  case  0 : return "";
  case  1 : return "No GlobalUnitAssignedContext, default taken";
  case  2 : return "No LengthMeasureWithUnit, default taken";
  case  3 : return "No SiUnit for LengthMeasure undefined, default taken";
  case  4 : return "No PlaneAngleMeasureWithUnit, default taken";
  case  5 : return "No SiUnit for PlaneAngleMeasure undefined, default taken";
  case  6 : return "No SolidAngleMeasureWithUnit, default taken";
  case  7 : return "No SiUnit for SolidAngleMeasure undefined, default taken";

  case 11 : return "Length Unit not recognized, default taken";
  case 12 : return "Plane Angle Unit not recognized, default taken";
  case 13 : return "Solid Angle Unit not recognized, default taken";
  case 14 : return "At least one unit is twice defined";

  case 40 : return "Bad GlobalUncertaintyAssignedContext, default unit taken";

  default : break;
  }

  return "Badly defined units, default taken";
}

Standard_Real STEPConstruct_UnitContext::AreaFactor() const
{
  return areaFactor;
}

Standard_Real STEPConstruct_UnitContext::VolumeFactor() const
{
  return volumeFactor;
}

Standard_Boolean STEPConstruct_UnitContext::AreaDone() const
{
  return areaDone;
}

Standard_Boolean STEPConstruct_UnitContext::VolumeDone() const
{
  return volumeDone;
}
