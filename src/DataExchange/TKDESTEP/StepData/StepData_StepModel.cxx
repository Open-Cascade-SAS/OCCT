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

#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_GeneralModule.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <StepData.hxx>
#include <StepData_Protocol.hxx>
#include <StepData_StepModel.hxx>
#include <StepData_StepWriter.hxx>
#include <TCollection_HAsciiString.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(StepData_StepModel, Interface_InterfaceModel)

// File header: list of entities
// Default constructor for STEP data model
StepData_StepModel::StepData_StepModel() {}

occ::handle<Standard_Transient> StepData_StepModel::Entity(const int num) const
{
  return Value(num);
} // More user-friendly name for accessing entities

// Copy header entities from another STEP model
// This method transfers only the header section, not the data entities
void StepData_StepModel::GetFromAnother(const occ::handle<Interface_InterfaceModel>& other)
{
  theheader.Clear();
  DeclareAndCast(StepData_StepModel, another, other);
  if (another.IsNull())
    return;
  Interface_EntityIterator iter = another->Header();
  // Copy the header. Important: header is distinct from content...
  Interface_CopyTool TC(this, StepData::HeaderProtocol());
  for (; iter.More(); iter.Next())
  {
    occ::handle<Standard_Transient> newhead;
    if (!TC.Copy(iter.Value(), newhead, false, false))
      continue;
    if (!newhead.IsNull())
      theheader.Append(newhead);
  }
}

occ::handle<Interface_InterfaceModel> StepData_StepModel::NewEmptyModel() const
{
  return new StepData_StepModel;
}

Interface_EntityIterator StepData_StepModel::Header() const
{
  Interface_EntityIterator iter;
  theheader.FillIterator(iter);
  return iter;
}

// Check if exactly one header entity of specified type exists
// Returns true only if there is exactly one entity of the given type
bool StepData_StepModel::HasHeaderEntity(const occ::handle<Standard_Type>& atype) const
{
  return (theheader.NbTypedEntities(atype) == 1);
}

occ::handle<Standard_Transient> StepData_StepModel::HeaderEntity(
  const occ::handle<Standard_Type>& atype) const
{
  return theheader.TypedEntity(atype);
}

// Header population methods

void StepData_StepModel::ClearHeader()
{
  theheader.Clear();
}

void StepData_StepModel::AddHeaderEntity(const occ::handle<Standard_Transient>& ent)
{
  theheader.Append(ent);
}

void StepData_StepModel::VerifyCheck(occ::handle<Interface_Check>& ach) const
{
  Interface_GeneralLib                 lib(StepData::HeaderProtocol());
  occ::handle<StepData_StepModel>      me(this);
  occ::handle<Interface_Protocol>      aHP = StepData::HeaderProtocol();
  Interface_ShareTool                  sh(me, aHP);
  occ::handle<Interface_GeneralModule> module;
  int                                  CN;
  for (Interface_EntityIterator iter = Header(); iter.More(); iter.Next())
  {
    const occ::handle<Standard_Transient>& head = iter.Value();
    if (!lib.Select(head, module, CN))
      continue;
    module->CheckCase(CN, head, sh, ach);
  }
}

void StepData_StepModel::DumpHeader(Standard_OStream& S, const int /*level*/) const
{
  // Note: level parameter is not used in this implementation

  occ::handle<StepData_Protocol> stepro = StepData::HeaderProtocol();
  bool                           iapro  = !stepro.IsNull();
  if (!iapro)
    S << " -- WARNING : StepModel DumpHeader, Protocol not defined\n";

  Interface_EntityIterator iter = Header();
  int                      nb   = iter.NbEntities();
  S << " --  Step Model Header : " << iter.NbEntities() << " Entities :\n";
  for (iter.Start(); iter.More(); iter.Next())
  {
    S << "  " << iter.Value()->DynamicType()->Name() << "\n";
  }
  if (!iapro || nb == 0)
    return;
  S << " --  --        STEP MODEL    HEADER  CONTENT      --  --" << "\n";
  S << " --   Dumped with Protocol : " << stepro->DynamicType()->Name() << "   --\n";

  occ::handle<StepData_StepModel> me(this);
  StepData_StepWriter             SW(me);
  SW.SendModel(stepro, true); // Send HEADER only
  SW.Print(S);
}

void StepData_StepModel::ClearLabels()
{
  theidnums.Nullify();
}

// Set identifier label for an entity (used in STEP file format)
// The identifier is typically a number like #123 that appears in STEP files
void StepData_StepModel::SetIdentLabel(const occ::handle<Standard_Transient>& ent, const int ident)
{
  int num = Number(ent);
  if (!num)
    return; // Entity not found in model
  int nbEnt = NbEntities();

  // Initialize identifier array if not yet created
  if (theidnums.IsNull())
  {
    theidnums = new NCollection_HArray1<int>(1, nbEnt);
    theidnums->Init(0); // Initialize all values to 0
  }
  // Resize array if model has grown since last allocation
  else if (nbEnt > theidnums->Length())
  {
    int                                   prevLength = theidnums->Length();
    occ::handle<NCollection_HArray1<int>> idnums1    = new NCollection_HArray1<int>(1, nbEnt);
    idnums1->Init(0);
    // Copy existing identifier mappings
    int k = 1;
    for (; k <= prevLength; k++)
      idnums1->SetValue(k, theidnums->Value(k));
    theidnums = idnums1;
  }
  theidnums->SetValue(num, ident);
}

int StepData_StepModel::IdentLabel(const occ::handle<Standard_Transient>& ent) const
{
  if (theidnums.IsNull())
    return 0;
  int num = Number(ent);
  return (!num ? 0 : theidnums->Value(num));
}

void StepData_StepModel::PrintLabel(const occ::handle<Standard_Transient>& ent,
                                    Standard_OStream&                      S) const
{
  int num = (theidnums.IsNull() ? 0 : Number(ent));
  int nid = (!num ? 0 : theidnums->Value(num));
  if (nid > 0)
    S << "#" << nid;
  else if (num > 0)
    S << "(#" << num << ")";
  else
    S << "(#0..)";
}

occ::handle<TCollection_HAsciiString> StepData_StepModel::StringLabel(
  const occ::handle<Standard_Transient>& ent) const
{
  occ::handle<TCollection_HAsciiString> label;
  char                                  text[20];
  int                                   num = (theidnums.IsNull() ? 0 : Number(ent));
  int                                   nid = (!num ? 0 : theidnums->Value(num));

  if (nid > 0)
    Sprintf(text, "#%d", nid);
  else if (num > 0)
    Sprintf(text, "(#%d)", num);
  else
    Sprintf(text, "(#0..)");

  label = new TCollection_HAsciiString(text);
  return label;
}

//=================================================================================================

void StepData_StepModel::SetLocalLengthUnit(const double theUnit)
{
  myLocalLengthUnit       = theUnit;
  myReadUnitIsInitialized = true;
}

//=================================================================================================

double StepData_StepModel::LocalLengthUnit() const
{
  return myLocalLengthUnit;
}

//=================================================================================================

void StepData_StepModel::SetWriteLengthUnit(const double theUnit)
{
  myWriteUnit              = theUnit;
  myWriteUnitIsInitialized = true;
}

//=================================================================================================

// Get the length unit for writing STEP files
// Returns the conversion factor from millimeters to the target unit
double StepData_StepModel::WriteLengthUnit() const
{
  // Lazy initialization of write unit from global parameters
  if (!myWriteUnitIsInitialized)
  {
    myWriteUnitIsInitialized = true;
    switch (InternalParameters.WriteUnit)
    {
      case UnitsMethods_LengthUnit_Inch:
        myWriteUnit = 25.4;
        break;
      case UnitsMethods_LengthUnit_Millimeter:
        myWriteUnit = 1.;
        break;
      case UnitsMethods_LengthUnit_Foot:
        myWriteUnit = 304.8;
        break;
      case UnitsMethods_LengthUnit_Mile:
        myWriteUnit = 1609344.0;
        break;
      case UnitsMethods_LengthUnit_Meter:
        myWriteUnit = 1000.0;
        break;
      case UnitsMethods_LengthUnit_Kilometer:
        myWriteUnit = 1000000.0;
        break;
      case UnitsMethods_LengthUnit_Mil:
        myWriteUnit = 0.0254;
        break;
      case UnitsMethods_LengthUnit_Micron:
        myWriteUnit = 0.001;
        break;
      case UnitsMethods_LengthUnit_Centimeter:
        myWriteUnit = 10.0;
        break;
      case UnitsMethods_LengthUnit_Microinch:
        myWriteUnit = 0.0000254;
        break;
      default: {
        myWriteUnitIsInitialized = false;
        GlobalCheck()->AddWarning("Incorrect write.step.unit parameter, use default value");
      }
    }
  }
  return myWriteUnit;
}
