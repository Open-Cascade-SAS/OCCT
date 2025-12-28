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

#include <IGESData_GlobalSection.hxx>

#include <gp_XYZ.hxx>
#include <IGESData_BasicEditor.hxx>
#include <Interface_Check.hxx>
#include <Interface_FileParameter.hxx>
#include <Interface_FloatWriter.hxx>
#include <Interface_ParamSet.hxx>
#include <Interface_ParamType.hxx>
#include <Message_Msg.hxx>
#include <OSD_Process.hxx>
#include <Quantity_Date.hxx>
#include <TCollection_HAsciiString.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_ShapeProcessor.hxx>
#include <UnitsMethods.hxx>

#include <cstdio>

//  Local routines copying a string [the ideal would be : astr = astr->Copy()]
//    and transforming a CString (Hollerith or not) to non-Hollerith HAsciiString.
//    and the reverse
static void CopyString(occ::handle<TCollection_HAsciiString>& astr)
{
  if (astr.IsNull())
    return; // do nothing if String not defined !
  occ::handle<TCollection_HAsciiString> S = new TCollection_HAsciiString("");
  S->AssignCat(astr);
  astr = S;
}

static void MakeHollerith(const occ::handle<TCollection_HAsciiString>& astr, char* text, int& lt)
{
  lt      = 0;
  text[0] = '\0';
  if (astr.IsNull())
    return;
  int ln = astr->Length();
  if (ln == 0)
    return;
  Sprintf(text, "%dH%s", ln, astr->ToCString());
  lt = ln + 2;
  if (ln >= 10)
    lt++;
  if (ln >= 100)
    lt++; // strlen text
}

//=================================================================================================

IGESData_GlobalSection::IGESData_GlobalSection()
    : theSeparator(','),
      theEndMark(';'),
      theIntegerBits(32), // simple = integer = 32b, double = 64
      theMaxPower10Single(38),
      theMaxDigitsSingle(6),
      theMaxPower10Double(308),
      theMaxDigitsDouble(15),
      theScale(1.0),
      theCascadeUnit(1.0),
      theUnitFlag(0),
      theLineWeightGrad(1),
      theMaxLineWeight(0.0),
      theResolution(0.0),
      theMaxCoord(0.0),
      hasMaxCoord(false),
      theIGESVersion(11), // IGES 5.3 by default
      theDraftingStandard(0)
{
  //
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> IGESData_GlobalSection::TranslatedFromHollerith(
  const occ::handle<TCollection_HAsciiString>& astr) const
{
  occ::handle<TCollection_HAsciiString> res;
  if (astr.IsNull())
    return res;
  int n = astr->Search("H");
  if (n > 1)
  {
    if (!astr->Token("H")->IsIntegerValue())
      n = 0;
  }
  if (n > 1 && n < astr->Length())
    res = astr->SubString(n + 1, astr->Length());
  else if (astr->ToCString() == nullptr)
    res = new TCollection_HAsciiString;
  else
    res = new TCollection_HAsciiString(astr->ToCString());
  return res;
}

//=================================================================================================

void IGESData_GlobalSection::Init(const occ::handle<Interface_ParamSet>& params,
                                  occ::handle<Interface_Check>&          ach)
{
  // MGE 21/07/98
  // Building of messages
  //======================================
  // Message_Msg Msg39 ("XSTEP_39");
  // Message_Msg Msg48 ("XSTEP_48");
  // Message_Msg Msg49 ("XSTEP_49");
  //======================================
  XSAlgo_ShapeProcessor::PrepareForTransfer(); // update unit info
  theSeparator = ',';
  theEndMark   = ';';
  theSendName.Nullify();
  theFileName.Nullify();
  theSystemId.Nullify();
  theInterfaceVersion.Nullify();
  theIntegerBits      = 32; // by default, simple = integer = 32b, double = 64
  theMaxPower10Single = 38;
  theMaxDigitsSingle  = 6;
  theMaxPower10Double = 308;
  theMaxDigitsDouble  = 15;
  theReceiveName.Nullify();
  theScale    = 1.0;
  theUnitFlag = 0;
  theUnitName.Nullify();
  theLineWeightGrad = 1;
  theMaxLineWeight  = 0.;
  theDate.Nullify();
  theResolution = 0.;
  theMaxCoord   = 0.;
  hasMaxCoord   = false;
  theAuthorName.Nullify();
  theCompanyName.Nullify();
  // clang-format off
  theIGESVersion       = 11;//3 //#66 rln Setting IGES 5.3 by default(To avoid misleading fails below)
  // clang-format on
  theDraftingStandard = 0;
  theCascadeUnit      = UnitsMethods::GetCasCadeLengthUnit();
  theLastChangeDate.Nullify(); // new in 5.1 (may be absent)
  theAppliProtocol.Nullify();  // new in 5.3 (may be absent)

  int nbp = params->NbParams();

  for (int i = 1; i <= nbp; i++)
  {
    int                                   intval  = 0;
    double                                realval = 0.0;
    occ::handle<TCollection_HAsciiString> strval; // doit repartir a null
    // char message[80]; //szv#4:S4163:12Mar99 unused
    const char*         val = params->Param(i).CValue();
    Interface_ParamType fpt = params->Param(i).ParamType();
    if (fpt == Interface_ParamVoid)
      continue;

    // if the param is an Integer
    if (fpt == Interface_ParamInteger)
    {
      // but a real is expected
      if (i == 13 || i == 17 || i == 19 || i == 20)
        realval = Atof(val);
      intval = atoi(val);
    }

    // if the param is a Real
    else if (fpt == Interface_ParamReal || fpt == Interface_ParamEnum)
    {
      char text[50];
      int  k, j = 0;
      for (k = 0; k < 50; k++)
      {
        if (val[k] == 'D' || val[k] == 'd')
          text[j++] = 'e';
        else
          text[j++] = val[k];
        if (val[k] == '\0')
          break;
      }
      realval = Atof(text);
    }

    // if the param is a text
    else if (fpt == Interface_ParamText)
    {
      strval = new TCollection_HAsciiString(val);
      if (val[0] != '\0')
      {
        int nhol = strval->Search("H");
        int lhol = strval->Length();
        if (nhol > 1)
          if (!strval->Token("H")->IsIntegerValue())
            nhol = 0;
        if (nhol > 1 && nhol < lhol)
          strval = strval->SubString(nhol + 1, lhol);
      }
    }

    char sepend = '\0';
    if (i < 3)
    {
      if (val[0] != '\0')
        sepend = val[0];
      if (val[1] == 'H')
        sepend = val[2]; // prioritaire
    }

    switch (i)
    {
      case 1:
        if (sepend != '\0')
          theSeparator = sepend;
        break;
      case 2:
        if (sepend != '\0')
          theEndMark = sepend;
        break;
      case 3:
        theSendName = strval;
        break;
      case 4:
        theFileName = strval;
        break;
      case 5:
        theSystemId = strval;
        break;
      case 6:
        theInterfaceVersion = strval;
        break;
      case 7:
        theIntegerBits = intval;
        break;
      case 8:
        theMaxPower10Single = intval;
        break;
      case 9:
        theMaxDigitsSingle = intval;
        break;
      case 10:
        theMaxPower10Double = intval;
        break;
      case 11:
        theMaxDigitsDouble = intval;
        break;
      case 12:
        theReceiveName = strval;
        break;
      case 13:
        theScale = realval;
        break;
      case 14:
        theUnitFlag = intval;
        break;
      case 15:
        theUnitName = strval;
        break;
      case 16:
        theLineWeightGrad = intval;
        break;
      case 17:
        theMaxLineWeight = realval;
        break;
      case 18:
        theDate = strval;
        break;
      case 19:
        theResolution = realval;
        break;
      case 20:
        theMaxCoord = realval;
        hasMaxCoord = true;
        break;
      case 21:
        theAuthorName = strval;
        break;
      case 22:
        theCompanyName = strval;
        break;
      case 23:
        theIGESVersion = intval;
        break;
      case 24:
        theDraftingStandard = intval;
        break;
      case 25:
        theLastChangeDate = strval;
        break;
      case 26:
        theAppliProtocol = strval;
        break;
      default:
        break;
    }
  }

  // Sending of message : Incorrect number of parameters (following the IGES version)
  // Version less than 5.3
  if (theIGESVersion < 11)
  {
    if ((nbp < 24) || (nbp > 25))
    {
      // 24 or 25 parameters are expected (parameter 25 is not required)
      Message_Msg Msg39("XSTEP_39");
      Msg39.Arg(24);
      Msg39.Arg(25);
      if (nbp < 24)
        ach->SendFail(Msg39);
      else
        ach->SendWarning(Msg39);
    }
  }
  // Version 5.3
  else if ((nbp < 25) || (nbp > 26))
  {
    // 25 or 26 parameters are expected (parameter 25 is not required)
    Message_Msg Msg39("XSTEP_39");
    Msg39.Arg(25);
    Msg39.Arg(26);
    if (nbp < 25)
      ach->SendFail(Msg39);
    else
      ach->SendWarning(Msg39);
  }

  //: 45 by abv 11.12.97: if UnitFlag is not defined in the file,
  // restore it from UnitName. Repris par CKY 13-FEV-1998
  if (theUnitFlag == 0 || theUnitFlag == 3)
  {
    int corrected = 0;
    if (theUnitName.IsNull())
      // default (inches) value taken
      corrected = 1;
    else
      corrected = IGESData_BasicEditor::UnitNameFlag(theUnitName->ToCString());
    if (corrected > 0)
      theUnitFlag = corrected;
    else if (theUnitFlag == 3)
    {
      Message_Msg Msg49("XSTEP_49");
      ach->SendWarning(Msg49);
    }
    else
    {
      Message_Msg Msg48("XSTEP_48");
      ach->SendFail(Msg48);
    }
  }
}

//=================================================================================================

void IGESData_GlobalSection::CopyRefs()
{
  CopyString(theSendName);
  CopyString(theFileName);
  CopyString(theSystemId);
  CopyString(theInterfaceVersion);
  CopyString(theReceiveName);
  CopyString(theUnitName);
  CopyString(theDate);
  CopyString(theAuthorName);
  CopyString(theCompanyName);
  CopyString(theLastChangeDate);
  CopyString(theAppliProtocol);
}

//=================================================================================================

occ::handle<Interface_ParamSet> IGESData_GlobalSection::Params() const
{
  char vide[1];
  char uncar[2];
  char nombre[1024];
  char text[200];
  int  lt;
  vide[0] = uncar[1] = nombre[0]      = '\0';
  uncar[0]                            = ',';
  occ::handle<Interface_ParamSet> res = new Interface_ParamSet(26); // gka 19.01.99
  if (theSeparator == ',')
    res->Append(vide, 0, Interface_ParamVoid, 0);
  else
  {
    uncar[0] = theSeparator;
    res->Append(uncar, 1, Interface_ParamMisc, 0);
  }

  if (theEndMark == ';')
    res->Append(vide, 0, Interface_ParamVoid, 0);
  else
  {
    uncar[0] = theEndMark;
    res->Append(uncar, 1, Interface_ParamMisc, 0);
  }

  MakeHollerith(theSendName, text, lt);
  res->Append(text, lt, Interface_ParamText, 0);

  MakeHollerith(theFileName, text, lt);
  res->Append(text, lt, Interface_ParamText, 0);

  MakeHollerith(theSystemId, text, lt);
  res->Append(text, lt, Interface_ParamText, 0);

  MakeHollerith(theInterfaceVersion, text, lt);
  res->Append(text, lt, Interface_ParamText, 0);

  Sprintf(nombre, "%d", theIntegerBits);
  res->Append(nombre, (int)strlen(nombre), Interface_ParamInteger, 0);

  Sprintf(nombre, "%d", theMaxPower10Single);
  res->Append(nombre, (int)strlen(nombre), Interface_ParamInteger, 0);

  Sprintf(nombre, "%d", theMaxDigitsSingle);
  res->Append(nombre, (int)strlen(nombre), Interface_ParamInteger, 0);

  Sprintf(nombre, "%d", theMaxPower10Double);
  res->Append(nombre, (int)strlen(nombre), Interface_ParamInteger, 0);

  Sprintf(nombre, "%d", theMaxDigitsDouble);
  res->Append(nombre, (int)strlen(nombre), Interface_ParamInteger, 0);

  MakeHollerith(theReceiveName, text, lt);
  res->Append(text, lt, Interface_ParamText, 0);

  Interface_FloatWriter::Convert(theScale, nombre, true, 0., 0., "%f", "%f");
  //  Sprintf(nombre,"%f",theScale);
  res->Append(nombre, (int)strlen(nombre), Interface_ParamReal, 0);

  Sprintf(nombre, "%d", theUnitFlag);
  res->Append(nombre, (int)strlen(nombre), Interface_ParamInteger, 0);

  MakeHollerith(theUnitName, text, lt);
  res->Append(text, lt, Interface_ParamText, 0);

  Sprintf(nombre, "%d", theLineWeightGrad);
  res->Append(nombre, (int)strlen(nombre), Interface_ParamInteger, 0);

  Interface_FloatWriter::Convert(theMaxLineWeight, nombre, true, 0., 0., "%f", "%f");
  //  Sprintf(nombre,"%f",theMaxLineWeight);
  res->Append(nombre, (int)strlen(nombre), Interface_ParamReal, 0);

  MakeHollerith(theDate, text, lt);
  res->Append(text, lt, Interface_ParamText, 0);

  Interface_FloatWriter::Convert(theResolution, nombre, true, 0., 0., "%g", "%g");
  //  Sprintf(nombre,"%f",theResolution);
  res->Append(nombre, (int)strlen(nombre), Interface_ParamReal, 0);

  if (hasMaxCoord)
    Interface_FloatWriter::Convert(theMaxCoord, nombre, true, 0., 0., "%f", "%f");
  //  Sprintf(nombre,"%f",theMaxCoord);
  else
    nombre[0] = '\0';
  res->Append(nombre, (int)strlen(nombre), Interface_ParamReal, 0);

  MakeHollerith(theAuthorName, text, lt);
  res->Append(text, lt, Interface_ParamText, 0);

  MakeHollerith(theCompanyName, text, lt);
  res->Append(text, lt, Interface_ParamText, 0);

  Sprintf(nombre, "%d", theIGESVersion);
  res->Append(nombre, (int)strlen(nombre), Interface_ParamInteger, 0);

  Sprintf(nombre, "%d", theDraftingStandard);
  res->Append(nombre, (int)strlen(nombre), Interface_ParamInteger, 0);

  if (!theLastChangeDate.IsNull())
  {
    MakeHollerith(theLastChangeDate, text, lt);
    res->Append(text, lt, Interface_ParamText, 0);
  }

  if (!theAppliProtocol.IsNull())
  {
    MakeHollerith(theAppliProtocol, text, lt);
    res->Append(text, lt, Interface_ParamText, 0);
  }
  // Ici : parametre absent ignore

  return res;
}

// ###############           QUERIES           ###############

//=================================================================================================

char IGESData_GlobalSection::Separator() const
{
  return theSeparator;
}

//=================================================================================================

char IGESData_GlobalSection::EndMark() const
{
  return theEndMark;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> IGESData_GlobalSection::SendName() const
{
  return theSendName;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> IGESData_GlobalSection::FileName() const
{
  return theFileName;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> IGESData_GlobalSection::SystemId() const
{
  return theSystemId;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> IGESData_GlobalSection::InterfaceVersion() const
{
  return theInterfaceVersion;
}

//=================================================================================================

int IGESData_GlobalSection::IntegerBits() const
{
  return theIntegerBits;
}

//=================================================================================================

int IGESData_GlobalSection::MaxPower10Single() const
{
  return theMaxPower10Single;
}

//=================================================================================================

int IGESData_GlobalSection::MaxDigitsSingle() const
{
  return theMaxDigitsSingle;
}

//=================================================================================================

int IGESData_GlobalSection::MaxPower10Double() const
{
  return theMaxPower10Double;
}

//=================================================================================================

int IGESData_GlobalSection::MaxDigitsDouble() const
{
  return theMaxDigitsDouble;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> IGESData_GlobalSection::ReceiveName() const
{
  return theReceiveName;
}

//=================================================================================================

double IGESData_GlobalSection::Scale() const
{
  return theScale;
}

//=================================================================================================

double IGESData_GlobalSection::CascadeUnit() const
{
  return theCascadeUnit;
}

//=================================================================================================

int IGESData_GlobalSection::UnitFlag() const
{
  return theUnitFlag;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> IGESData_GlobalSection::UnitName() const
{
  return theUnitName;
}

//=================================================================================================

int IGESData_GlobalSection::LineWeightGrad() const
{
  return theLineWeightGrad;
}

//=================================================================================================

double IGESData_GlobalSection::MaxLineWeight() const
{
  return theMaxLineWeight;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> IGESData_GlobalSection::Date() const
{
  return theDate;
}

//=================================================================================================

double IGESData_GlobalSection::Resolution() const
{
  return theResolution;
}

//=================================================================================================

double IGESData_GlobalSection::MaxCoord() const
{
  return theMaxCoord;
}

//=================================================================================================

bool IGESData_GlobalSection::HasMaxCoord() const
{
  return hasMaxCoord;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> IGESData_GlobalSection::AuthorName() const
{
  return theAuthorName;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> IGESData_GlobalSection::CompanyName() const
{
  return theCompanyName;
}

//=================================================================================================

int IGESData_GlobalSection::IGESVersion() const
{
  return theIGESVersion;
}

//=================================================================================================

int IGESData_GlobalSection::DraftingStandard() const
{
  return theDraftingStandard;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> IGESData_GlobalSection::LastChangeDate() const
{
  return theLastChangeDate;
}

//=================================================================================================

bool IGESData_GlobalSection::HasLastChangeDate() const
{
  return (!theLastChangeDate.IsNull());
}

//=================================================================================================

void IGESData_GlobalSection::SetLastChangeDate()
{
  if (HasLastChangeDate())
    return;
  int           mois, jour, annee, heure, minute, seconde, millisec, microsec;
  OSD_Process   system;
  Quantity_Date ladate = system.SystemDate();
  ladate.Values(mois, jour, annee, heure, minute, seconde, millisec, microsec);
  if (annee < 2000)
    // #65 rln 12.02.99 S4151 (explicitly force YYMMDD.HHMMSS before Y2000)
    theLastChangeDate = NewDateString(annee, mois, jour, heure, minute, seconde, 0);
  else
    // #65 rln 12.02.99 S4151 (explicitly force YYYYMMDD.HHMMSS after Y2000)
    theLastChangeDate = NewDateString(annee, mois, jour, heure, minute, seconde, -1);
}

//=================================================================================================

bool IGESData_GlobalSection::HasApplicationProtocol() const
{
  return !theAppliProtocol.IsNull();
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> IGESData_GlobalSection::ApplicationProtocol() const
{
  return theAppliProtocol;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> IGESData_GlobalSection::NewDateString(const int annee,
                                                                            const int mois,
                                                                            const int jour,
                                                                            const int heure,
                                                                            const int minute,
                                                                            const int seconde,
                                                                            const int mode)
{
  //  0 : IGES annee a l ancienne 00-99    -1 IGES annee complete    1 lisible
  char madate[60];
  int  moi = mois, jou = jour, anne = annee;
  int  heur = heure, minut = minute, second = seconde;
  if (annee == 0)
  {
    int           millisec, microsec;
    OSD_Process   system;
    Quantity_Date ladate = system.SystemDate();
    ladate.Values(moi, jou, anne, heur, minut, second, millisec, microsec);
  }
  if (mode == 0 || mode == -1)
  {
    int  an      = anne % 100;
    bool dizaine = (an >= 10);
    if (!dizaine)
      an += 10;
    if (mode < 0)
    {
      an      = anne;
      dizaine = true;
    }
    int date1 = (an) * 10000 + moi * 100 + jou;
    int date2 = (heur + 100) * 10000 + minut * 100 + second;
    Sprintf(madate, "%d%d", date1, date2);
    madate[(mode == 0 ? 6 : 8)] = '.';
    if (!dizaine)
      madate[0] = '0';
  }
  else if (mode == 1)
  {
    Sprintf(madate, "%4.4d-%2.2d-%2.2d:%2.2d-%2.2d-%2.2d", anne, moi, jou, heur, minut, second);
  }
  return new TCollection_HAsciiString(madate);
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> IGESData_GlobalSection::NewDateString(
  const occ::handle<TCollection_HAsciiString>& date,
  const int                                    mode)
{
  int anne, moi, jou, heur, minut, second;
  if (date.IsNull())
    return date;
  int i0 = 0;
  if (date->Length() == 15)
    i0 = 2;
  else if (date->Length() != 13)
    return date;
  if (date->Value(i0 + 7) != '.')
    return date;
  anne = (date->Value(i0 + 1) - 48) * 10 + (date->Value(i0 + 2) - 48);
  if (i0 == 0)
  {
    anne = anne + 1900;
    if (anne < 1980)
      anne += 100;
  }
  else
  {
    anne = anne + (date->Value(1) - 48) * 1000 + (date->Value(2) - 48) * 100;
  }
  moi    = (date->Value(i0 + 3) - 48) * 10 + (date->Value(i0 + 4) - 48);
  jou    = (date->Value(i0 + 5) - 48) * 10 + (date->Value(i0 + 6) - 48);
  heur   = (date->Value(i0 + 8) - 48) * 10 + (date->Value(i0 + 9) - 48);
  minut  = (date->Value(i0 + 10) - 48) * 10 + (date->Value(i0 + 11) - 48);
  second = (date->Value(i0 + 12) - 48) * 10 + (date->Value(i0 + 13) - 48);

  return IGESData_GlobalSection::NewDateString(anne, moi, jou, heur, minut, second, mode);
}

//=================================================================================================

double IGESData_GlobalSection::UnitValue() const
{
  return IGESData_BasicEditor::UnitFlagValue(theUnitFlag) / theCascadeUnit;
}

// ###############           UPDATES           ###############

void IGESData_GlobalSection::SetSeparator(const char val)
{
  theSeparator = val;
}

void IGESData_GlobalSection::SetEndMark(const char val)
{
  theEndMark = val;
}

void IGESData_GlobalSection::SetSendName(const occ::handle<TCollection_HAsciiString>& val)
{
  theSendName = TranslatedFromHollerith(val);
}

void IGESData_GlobalSection::SetFileName(const occ::handle<TCollection_HAsciiString>& val)
{
  theFileName = TranslatedFromHollerith(val);
}

void IGESData_GlobalSection::SetSystemId(const occ::handle<TCollection_HAsciiString>& val)
{
  theSystemId = TranslatedFromHollerith(val);
}

void IGESData_GlobalSection::SetInterfaceVersion(const occ::handle<TCollection_HAsciiString>& val)
{
  theInterfaceVersion = TranslatedFromHollerith(val);
}

void IGESData_GlobalSection::SetIntegerBits(const int val)
{
  theIntegerBits = val;
}

void IGESData_GlobalSection::SetMaxPower10Single(const int val)
{
  theMaxPower10Single = val;
}

void IGESData_GlobalSection::SetMaxDigitsSingle(const int val)
{
  theMaxDigitsSingle = val;
}

void IGESData_GlobalSection::SetMaxPower10Double(const int val)
{
  theMaxPower10Double = val;
}

void IGESData_GlobalSection::SetMaxDigitsDouble(const int val)
{
  theMaxDigitsDouble = val;
}

void IGESData_GlobalSection::SetReceiveName(const occ::handle<TCollection_HAsciiString>& val)
{
  theReceiveName = TranslatedFromHollerith(val);
}

void IGESData_GlobalSection::SetScale(const double val)
{
  theScale = val;
}

void IGESData_GlobalSection::SetCascadeUnit(const double theUnit)
{
  theCascadeUnit = theUnit;
}

void IGESData_GlobalSection::SetUnitFlag(const int val)
{
  theUnitFlag = val;
}

void IGESData_GlobalSection::SetUnitName(const occ::handle<TCollection_HAsciiString>& val)
{
  theUnitName = TranslatedFromHollerith(val);
}

void IGESData_GlobalSection::SetLineWeightGrad(const int val)
{
  theLineWeightGrad = val;
}

void IGESData_GlobalSection::SetMaxLineWeight(const double val)
{
  theMaxLineWeight = val;
}

void IGESData_GlobalSection::SetDate(const occ::handle<TCollection_HAsciiString>& val)
{
  theDate = TranslatedFromHollerith(val);
}

void IGESData_GlobalSection::SetResolution(const double val)
{
  theResolution = val;
}

void IGESData_GlobalSection::SetMaxCoord(const double val)
{
  hasMaxCoord = (val > 0.);
  theMaxCoord = (hasMaxCoord ? val : 0.);
}

void IGESData_GlobalSection::MaxMaxCoord(const double val)
{
  double aval = std::abs(val);
  if (hasMaxCoord)
  {
    if (aval > theMaxCoord)
      theMaxCoord = aval;
  }
  else
    SetMaxCoord(aval);
}

void IGESData_GlobalSection::MaxMaxCoords(const gp_XYZ& xyz)
{
  MaxMaxCoord(xyz.X());
  MaxMaxCoord(xyz.Y());
  MaxMaxCoord(xyz.Z());
}

void IGESData_GlobalSection::SetAuthorName(const occ::handle<TCollection_HAsciiString>& val)
{
  theAuthorName = TranslatedFromHollerith(val);
}

void IGESData_GlobalSection::SetCompanyName(const occ::handle<TCollection_HAsciiString>& val)
{
  theCompanyName = TranslatedFromHollerith(val);
}

void IGESData_GlobalSection::SetIGESVersion(const int val)
{
  theIGESVersion = val;
}

void IGESData_GlobalSection::SetDraftingStandard(const int val)
{
  theDraftingStandard = val;
}

void IGESData_GlobalSection::SetLastChangeDate(const occ::handle<TCollection_HAsciiString>& val)
{
  theLastChangeDate = TranslatedFromHollerith(val);
}

void IGESData_GlobalSection::SetApplicationProtocol(
  const occ::handle<TCollection_HAsciiString>& val)
{
  theAppliProtocol = TranslatedFromHollerith(val);
}
