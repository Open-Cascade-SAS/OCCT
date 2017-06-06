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

#include <Quantity_Convert.hxx>
#include <TCollection_AsciiString.hxx>

Standard_EXPORT NCollection_Array1<Standard_Real> ConvertTable(1,68);
static Standard_CString theEnums[] = {      // En attendant la methode "Elements" de Standard_Type
"Quantity_MASS", 
"Quantity_PLANEANGLE", 
"Quantity_SOLIDANGLE", 
"Quantity_LENGTH", 
"Quantity_AREA", 
"Quantity_VOLUME", 
"Quantity_SPEED", 
"Quantity_VELOCITY", 
"Quantity_ACCELERATION", 
"Quantity_ANGULARVELOCITY", 
"Quantity_FREQUENCY", 
"Quantity_TEMPERATURE", 
"Quantity_AMOUNTOFSUBSTANCE", 
"Quantity_DENSITY", 
"Quantity_MASSFLOW", 
"Quantity_VOLUMEFLOW", 
"Quantity_CONSUMPTION", 
"Quantity_MOMENTUM", 
"Quantity_KINETICMOMENT", 
"Quantity_MOMENTOFINERTIA", 
"Quantity_FORCE", 
"Quantity_MOMENTOFAFORCE", 
"Quantity_TORQUE", 
"Quantity_WEIGHT", 
"Quantity_PRESSURE", 
"Quantity_VISCOSITY", 
"Quantity_KINEMATICVISCOSITY", 
"Quantity_ENERGY", 
"Quantity_WORK", 
"Quantity_POWER", 
"Quantity_SURFACETENSION", 
"Quantity_COEFFICIENTOFEXPANSION", 
"Quantity_THERMALCONDUCTIVITY", 
"Quantity_SPECIFICHEATCAPACITY", 
"Quantity_ENTROPY", 
"Quantity_ENTHALPY", 
"Quantity_LUMINOUSINTENSITY", 
"Quantity_LUMINOUSFLUX", 
"Quantity_LUMINANCE", 
"Quantity_ILLUMINANCE", 
"Quantity_LUMINOUSEXPOSITION", 
"Quantity_LUMINOUSEFFICACITY", 
"Quantity_ELECTRICCHARGE", 
"Quantity_ELECTRICCURRENT", 
"Quantity_ELECTRICFIELDSTRENGTH", 
"Quantity_ELECTRICPOTENTIAL", 
"Quantity_ELECTRICCAPACITANCE", 
"Quantity_MAGNETICFLUX", 
"Quantity_MAGNETICFLUXDENSITY", 
"Quantity_MAGNETICFIELDSTRENGTH", 
"Quantity_RELUCTANCE", 
"Quantity_RESISTANCE", 
"Quantity_INDUCTANCE", 
"Quantity_CAPACITANCE", 
"Quantity_IMPEDANCE", 
"Quantity_ADMITTANCE", 
"Quantity_RESISTIVITY", 
"Quantity_CONDUCTIVITY", 
"Quantity_MOLARMASS", 
"Quantity_MOLARVOLUME", 
"Quantity_CONCENTRATION", 
"Quantity_MOLARCONCENTRATION", 
"Quantity_MOLARITY", 
"Quantity_SOUNDINTENSITY", 
"Quantity_ACOUSTICINTENSITY", 
"Quantity_ACTIVITY", 
"Quantity_ABSORBEDDOSE", 
"Quantity_DOSEEQUIVALENT" };

//----------------------------------------------------------------------------
//  Create
//----------------------------------------------------------------------------
Quantity_Convert::Quantity_Convert () {
// ...On se fixe sur un nombre de grandeurs physiques egal a 68 (temporaire)
// ...Initialisation de la table de correspondance a 1 (Coefficient de conversion
// ...par defaut)
   for (Standard_Integer i = 1 ; i <= 68 ; i++) ConvertTable(i) = 1;
}

//----------------------------------------------------------------------------
//  IsPhysicalQuantity
//----------------------------------------------------------------------------
Standard_Boolean Quantity_Convert::IsPhysicalQuantity 
        (const TCollection_AsciiString& aTypeName, TCollection_AsciiString& anEnum) 
{
//     ... Fabriquer le nom de l'enumeration (Quantity_LENGTH par exemple).
   TCollection_AsciiString aPrefixe("Quantity_");
   anEnum = aTypeName;
   anEnum.UpperCase();
   anEnum.Prepend(aPrefixe);
// ... Rechercher si il existe existe une valeur d'enum correspondante a <aTypeName>
   Standard_Integer i = 1;
   Standard_Boolean Find = Standard_False; 
   while (i <= 68 && !Find) 
   {
      if (IsEqual(anEnum.ToCString(),theEnums[i-1])) 
         Find = Standard_True;
      else 
         i++;
   }
   return Find; 

}
