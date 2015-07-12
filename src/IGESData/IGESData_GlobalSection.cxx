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

//#52 rln 06.01.99 writing value 1.e-07
//gka 19.01.99 changing size of ParamSet
//#65 rln 12.02.99 S4151 (explicitly force YYMMDD.HHMMSS before Y2000 and YYYYMMDD.HHMMSS after Y2000)
//#66 rln Setting IGES 5.3 by default
//#73 rln 10.03.99 S4135: "read.scale.unit" is applied after translation

#include <gp_XYZ.hxx>
#include <IGESData_BasicEditor.hxx>
#include <IGESData_GlobalSection.hxx>
#include <Interface_Check.hxx>
#include <Interface_FileParameter.hxx>
#include <Interface_FileReaderData.hxx>
#include <Interface_FloatWriter.hxx>
#include <Interface_ParamSet.hxx>
#include <Interface_ParamType.hxx>
#include <Interface_Static.hxx>
#include <Message_Msg.hxx>
#include <OSD_Process.hxx>
#include <Quantity_Date.hxx>
#include <TCollection_HAsciiString.hxx>
#include <UnitsMethods.hxx>

#include <stdio.h>
// MGE 21/07/98
//  valeurs en MILLIMETRE pardefaut, reajustable
//static Standard_Real convunit = 1000.;
//  Routines locales copiant une string [l`ideal serait : astr = astr->Copy()]
//    et transformant un CString (Hollerith ou non) en HAsciiString non Holl.
//    et l inverse
static void CopyString (Handle(TCollection_HAsciiString)& astr);

static void MakeHollerith(const Handle(TCollection_HAsciiString)& astr,
                          char* text, Standard_Integer& lt);

void CopyString (Handle(TCollection_HAsciiString)& astr)
{
  if (astr.IsNull()) return;   // ne rien faire si String pas definie !
  Handle(TCollection_HAsciiString) S = new TCollection_HAsciiString("");
  S->AssignCat(astr);  astr = S;
}

void MakeHollerith(const Handle(TCollection_HAsciiString)& astr,
   char* text, Standard_Integer& lt)
{
  lt = 0;  text[0] = '\0';
  if (astr.IsNull()) return;
  Standard_Integer ln = astr->Length();
  if (ln == 0) return;
  sprintf (text,"%dH%s",ln,astr->ToCString());
  lt = ln+2;  if (ln >= 10) lt ++;  if (ln >= 100) lt ++;    // strlen text
}


//=======================================================================
//function : IGESData_GlobalSection
//purpose  : 
//=======================================================================

IGESData_GlobalSection::IGESData_GlobalSection ()
{
//  convunit = 1000.;
  //#73 rln 10.03.99 S4135: "read.scale.unit" is applied after translation
  //if (Interface_Static::IVal("read.scale.unit") == 1) convunit = 1;
}


// pourrait etre une methode generale au package en fait
//=======================================================================
//function : TranslatedFromHollerith
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IGESData_GlobalSection::TranslatedFromHollerith
       (const Handle(TCollection_HAsciiString)& astr) const
{
  Handle(TCollection_HAsciiString) res;
  if (astr.IsNull()) return res;
  Standard_Integer n = astr->Search("H");
  if (n > 1) {
    if (!astr->Token("H")->IsIntegerValue()) n = 0;
  }
  if (n > 1 && n < astr->Length()) res = astr->SubString(n+1,astr->Length());
  else if(astr->ToCString()== NULL)
    res = new TCollection_HAsciiString;
  else res = new TCollection_HAsciiString(astr->ToCString());
  return res;
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void IGESData_GlobalSection::Init(const Handle(Interface_ParamSet)& params,
                                  Handle(Interface_Check)& ach)
{
  // MGE 21/07/98
  // Building of messages
  //======================================
  //Message_Msg Msg39 ("XSTEP_39");
  //Message_Msg Msg48 ("XSTEP_48");
  //Message_Msg Msg49 ("XSTEP_49");
  //======================================

  theSeparator = ',';       theEndMark = ';';
  theSendName.Nullify();    theFileName.Nullify();  theSystemId.Nullify();
  theInterfaceVersion.Nullify();
  theIntegerBits       = 32;  // par defaut, simple = entier = 32b, double = 64
  theMaxPower10Single  =  38;  theMaxDigitsSingle   =  6;
  theMaxPower10Double  = 308;  theMaxDigitsDouble   = 15;
  theReceiveName.Nullify();
  theScale             = 1.0;
  theUnitFlag          = 0;  theUnitName.Nullify();
  theLineWeightGrad    = 1;  theMaxLineWeight     = 0.;
  theDate.Nullify();
  theResolution        = 0.; theMaxCoord          = 0.; hasMaxCoord = Standard_False;
  theAuthorName.Nullify();  theCompanyName.Nullify();
  theIGESVersion       = 11;//3 //#66 rln Setting IGES 5.3 by default(To avoid misleading fails below)
  theDraftingStandard  = 0;
  theLastChangeDate.Nullify();  // nouveaute 5.1 (peut etre absente)
  theAppliProtocol.Nullify();   // nouveaute 5.3 (peut etre absente)

  Standard_Integer nbp = params->NbParams();

  for (Standard_Integer i = 1; i <= nbp; i ++) {
    Standard_Integer intval = 0;  Standard_Real realval = 0.0;
    Handle(TCollection_HAsciiString) strval;  // doit repartir a null
    //char message[80]; //szv#4:S4163:12Mar99 unused
    Standard_CString    val = params->Param(i).CValue();
    Interface_ParamType fpt = params->Param(i).ParamType();
    if (fpt == Interface_ParamVoid) continue;
    
    // if the param is an Integer
    if (fpt == Interface_ParamInteger) {
       // but a real is expected 
      if ( i == 13 || i == 17 || i == 19 || i == 20)
	realval = Atof(val);
      intval  = atoi(val);
    }

    // if the param is a Real
    else if (fpt == Interface_ParamReal || fpt == Interface_ParamEnum) {
      char text[50];
      Standard_Integer k , j = 0;
      for (k = 0; k < 50; k ++) {
        if (val[k] ==  'D' || val[k] == 'd')
          text[j++] = 'e';
        else
          text[j++] = val[k];  
        if (val[k] == '\0') break;
      }
      realval = Atof(text);
    }

    // if the param is a text
    else if (fpt == Interface_ParamText) {
      strval = new TCollection_HAsciiString (val);
      if (val[0] != '\0') {
	Standard_Integer nhol = strval->Search("H");
	Standard_Integer lhol = strval->Length();
	if (nhol > 1) 
	  if (!strval->Token("H")->IsIntegerValue()) nhol = 0;
	if (nhol > 1 && nhol < lhol) 
	  strval = strval->SubString(nhol+1,lhol);
      }
    }

    char sepend = '\0';
    if (i < 3) {
      if (val[0] != '\0') sepend = val[0];
      if (val[1] == 'H')  sepend = val[2];  // prioritaire
    }

    switch (i) 
     {
      case  1 : if (sepend != '\0') theSeparator = sepend;    break;
      case  2 : if (sepend != '\0') theEndMark   = sepend;    break;
      case  3 : theSendName                      = strval;    break;
      case  4 : theFileName                      = strval;    break;
      case  5 : theSystemId                      = strval;    break;
      case  6 : theInterfaceVersion              = strval;    break;
      case  7 : theIntegerBits                   = intval;    break;
      case  8 : theMaxPower10Single              = intval;    break;
      case  9 : theMaxDigitsSingle               = intval;    break;
      case 10 : theMaxPower10Double              = intval;    break;
      case 11 : theMaxDigitsDouble               = intval;    break;
      case 12 : theReceiveName                   = strval;    break;
      case 13 : theScale                         = realval;   break;
      case 14 : theUnitFlag                      = intval;    break;
      case 15 : theUnitName                      = strval;    break;
      case 16 : theLineWeightGrad                = intval;    break;
      case 17 : theMaxLineWeight                 = realval;   break;
      case 18 : theDate                          = strval;    break;
      case 19 : theResolution                    = realval;   break;
      case 20 : theMaxCoord                      = realval;
		hasMaxCoord                      = Standard_True;  break;
      case 21 : theAuthorName                    = strval;    break;
      case 22 : theCompanyName                   = strval;    break;
      case 23 : theIGESVersion                   = intval;    break;
      case 24 : theDraftingStandard              = intval;    break;
      case 25 : theLastChangeDate                = strval;    break;
      case 26 : theAppliProtocol                 = strval;    break;
      default : break;
    }
  }

  // Sending of message : Incorrect number of parameters (following the IGES version)
  // Version less than 5.3 
  if  (theIGESVersion < 11)
  {
    if ((nbp < 24) || (nbp > 25)) {
       // 24 or 25 parameters are expected (parameter 25 is not required)
      Message_Msg Msg39 ("XSTEP_39");
      Msg39.Arg(24);
      Msg39.Arg(25);
      if (nbp < 24) ach->SendFail(Msg39);
      else          ach->SendWarning(Msg39);
    }
  }
  // Version 5.3 
  else if ((nbp < 25) || (nbp > 26)) {
    // 25 or 26 parameters are expected (parameter 25 is not required)
    Message_Msg Msg39 ("XSTEP_39");
    Msg39.Arg(25);
    Msg39.Arg(26);
    if (nbp < 25) ach->SendFail(Msg39);
    else          ach->SendWarning(Msg39);
  }
  
  //:45 by abv 11.12.97: if UnitFlag is not defined in the file, 
  // restore it from UnitName. Repris par CKY 13-FEV-1998
  if ( theUnitFlag == 0 || theUnitFlag == 3 ) {
    Standard_Integer corrected = 0 ;
    if(theUnitName.IsNull())
      //default (inches) value taken
      corrected = 1;
    else
      corrected = IGESData_BasicEditor::UnitNameFlag (theUnitName->ToCString());
    if (corrected > 0) theUnitFlag = corrected;
    else if (theUnitFlag == 3) {
      Message_Msg Msg49 ("XSTEP_49");
      ach->SendWarning (Msg49);
    }
    else {
      Message_Msg Msg48 ("XSTEP_48");
      ach->SendFail (Msg48);
    }
  }
}


//=======================================================================
//function : CopyRefs
//purpose  : 
//=======================================================================

void IGESData_GlobalSection::CopyRefs ()
{
  CopyString(theSendName);   CopyString(theFileName);  CopyString(theSystemId);
  CopyString(theInterfaceVersion);  CopyString(theReceiveName);
  CopyString(theUnitName);          CopyString(theDate);
  CopyString(theAuthorName);        CopyString(theCompanyName);
  CopyString(theLastChangeDate);    CopyString(theAppliProtocol);
}


//=======================================================================
//function : Params
//purpose  : 
//=======================================================================

Handle(Interface_ParamSet) IGESData_GlobalSection::Params () const
{
  char vide[1];  char uncar[2];  char nombre[1024];  char text[200];
  Standard_Integer lt;
  vide[0] = uncar[1] = nombre[0] = '\0';  uncar[0] = ',';
  Handle(Interface_ParamSet) res  = new Interface_ParamSet(26);  //gka 19.01.99
  if (theSeparator == ',') res->Append (vide,0,Interface_ParamVoid,0);
  else { uncar[0] = theSeparator; res->Append (uncar,1,Interface_ParamMisc,0); }

  if (theEndMark == ';') res->Append (vide,0,Interface_ParamVoid,0);
  else { uncar[0] = theEndMark; res->Append (uncar,1,Interface_ParamMisc,0); }

  MakeHollerith (theSendName,text,lt);
  res->Append (text,lt, Interface_ParamText,0);

  MakeHollerith (theFileName,text,lt);
  res->Append (text,lt, Interface_ParamText,0);

  MakeHollerith (theSystemId,text,lt);
  res->Append (text,lt, Interface_ParamText,0);

  MakeHollerith (theInterfaceVersion,text,lt);
  res->Append (text,lt, Interface_ParamText,0);

  sprintf(nombre,"%d",theIntegerBits);
  res->Append (nombre,(Standard_Integer)strlen(nombre),Interface_ParamInteger,0);

  sprintf(nombre,"%d",theMaxPower10Single);
  res->Append (nombre,(Standard_Integer)strlen(nombre),Interface_ParamInteger,0);

  sprintf(nombre,"%d",theMaxDigitsSingle);
  res->Append (nombre,(Standard_Integer)strlen(nombre),Interface_ParamInteger,0);

  sprintf(nombre,"%d",theMaxPower10Double);
  res->Append (nombre,(Standard_Integer)strlen(nombre),Interface_ParamInteger,0);

  sprintf(nombre,"%d",theMaxDigitsDouble);
  res->Append (nombre,(Standard_Integer)strlen(nombre),Interface_ParamInteger,0);

  MakeHollerith (theReceiveName,text,lt);
  res->Append (text,lt, Interface_ParamText,0);

  Interface_FloatWriter::Convert (theScale,nombre,Standard_True,0.,0.,"%f","%f");
//  sprintf(nombre,"%f",theScale);
  res->Append (nombre,(Standard_Integer)strlen(nombre),Interface_ParamReal,0);

  sprintf(nombre,"%d",theUnitFlag);
  res->Append (nombre,(Standard_Integer)strlen(nombre),Interface_ParamInteger,0);

  MakeHollerith (theUnitName,text,lt);
  res->Append (text,lt, Interface_ParamText,0);

  sprintf(nombre,"%d",theLineWeightGrad);
  res->Append (nombre,(Standard_Integer)strlen(nombre),Interface_ParamInteger,0);

  Interface_FloatWriter::Convert (theMaxLineWeight,nombre,Standard_True,0.,0.,"%f","%f");
//  sprintf(nombre,"%f",theMaxLineWeight);
  res->Append (nombre,(Standard_Integer)strlen(nombre),Interface_ParamReal,0);

  MakeHollerith (theDate,text,lt);
  res->Append (text,lt, Interface_ParamText,0);

  Interface_FloatWriter::Convert (theResolution,nombre,Standard_True,0.,0.,"%g","%g");
//  sprintf(nombre,"%f",theResolution);
  res->Append (nombre,(Standard_Integer)strlen(nombre),Interface_ParamReal,0);

  if (hasMaxCoord)
    Interface_FloatWriter::Convert (theMaxCoord,nombre,Standard_True,0.,0.,"%f","%f");
  //  sprintf(nombre,"%f",theMaxCoord);
  else nombre[0] = '\0';
  res->Append (nombre,(Standard_Integer)strlen(nombre),Interface_ParamReal,0);

  MakeHollerith (theAuthorName,text,lt);
  res->Append (text,lt, Interface_ParamText,0);

  MakeHollerith (theCompanyName,text,lt);
  res->Append (text,lt, Interface_ParamText,0);

  sprintf(nombre,"%d",theIGESVersion);
  res->Append (nombre,(Standard_Integer)strlen(nombre),Interface_ParamInteger,0);

  sprintf(nombre,"%d",theDraftingStandard);
  res->Append (nombre,(Standard_Integer)strlen(nombre),Interface_ParamInteger,0);

  if (!theLastChangeDate.IsNull()) {
    MakeHollerith (theLastChangeDate,text,lt);
    res->Append (text,lt, Interface_ParamText,0);
  }

  if (!theAppliProtocol.IsNull()) {
    MakeHollerith (theAppliProtocol,text,lt);
    res->Append (text,lt, Interface_ParamText,0);
  }
// Ici : parametre absent ignore

  return res;
}


// ###############           QUERIES           ###############


//=======================================================================
//function : Separator
//purpose  : 
//=======================================================================

Standard_Character IGESData_GlobalSection::Separator () const
{
  return theSeparator;
}


//=======================================================================
//function : EndMark
//purpose  : 
//=======================================================================

Standard_Character IGESData_GlobalSection::EndMark () const
{
  return theEndMark;
}


//=======================================================================
//function : SendName
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IGESData_GlobalSection::SendName () const
{
  return theSendName;
}


//=======================================================================
//function : FileName
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IGESData_GlobalSection::FileName () const
{
  return theFileName;
}


//=======================================================================
//function : SystemId
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IGESData_GlobalSection::SystemId () const
{
  return theSystemId;
}


//=======================================================================
//function : InterfaceVersion
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IGESData_GlobalSection::InterfaceVersion () const
{
  return theInterfaceVersion;
}


//=======================================================================
//function : IntegerBits
//purpose  : 
//=======================================================================

Standard_Integer IGESData_GlobalSection::IntegerBits () const
{
  return theIntegerBits;
}


//=======================================================================
//function : MaxPower10Single
//purpose  : 
//=======================================================================

Standard_Integer IGESData_GlobalSection::MaxPower10Single () const
{
  return theMaxPower10Single;
}


//=======================================================================
//function : MaxDigitsSingle
//purpose  : 
//=======================================================================

Standard_Integer IGESData_GlobalSection::MaxDigitsSingle () const
{
  return theMaxDigitsSingle;
}


//=======================================================================
//function : MaxPower10Double
//purpose  : 
//=======================================================================

Standard_Integer IGESData_GlobalSection::MaxPower10Double () const
{
  return theMaxPower10Double;
}


//=======================================================================
//function : MaxDigitsDouble
//purpose  : 
//=======================================================================

Standard_Integer IGESData_GlobalSection::MaxDigitsDouble () const
{
  return theMaxDigitsDouble;
}


//=======================================================================
//function : ReceiveName
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IGESData_GlobalSection::ReceiveName () const
{
  return theReceiveName;
}


//=======================================================================
//function : Scale
//purpose  : 
//=======================================================================

Standard_Real IGESData_GlobalSection::Scale () const
{
  return theScale;
}


//=======================================================================
//function : UnitFlag
//purpose  : 
//=======================================================================

Standard_Integer IGESData_GlobalSection::UnitFlag () const
{
  return theUnitFlag;
}


//=======================================================================
//function : UnitName
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IGESData_GlobalSection::UnitName () const
{
  return theUnitName;
}


//=======================================================================
//function : LineWeightGrad
//purpose  : 
//=======================================================================

Standard_Integer IGESData_GlobalSection::LineWeightGrad () const
{
  return theLineWeightGrad;
}


//=======================================================================
//function : MaxLineWeight
//purpose  : 
//=======================================================================

Standard_Real IGESData_GlobalSection::MaxLineWeight () const
{
  return theMaxLineWeight;
}


//=======================================================================
//function : Date
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IGESData_GlobalSection::Date () const
{
  return theDate;
}


//=======================================================================
//function : Resolution
//purpose  : 
//=======================================================================

Standard_Real IGESData_GlobalSection::Resolution () const
{
  return theResolution;
}


//=======================================================================
//function : MaxCoord
//purpose  : 
//=======================================================================

Standard_Real IGESData_GlobalSection::MaxCoord () const
{
  return theMaxCoord;
}


//=======================================================================
//function : HasMaxCoord
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_GlobalSection::HasMaxCoord () const
{
  return hasMaxCoord;
}


//=======================================================================
//function : AuthorName
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IGESData_GlobalSection::AuthorName () const
{
  return theAuthorName;
}


//=======================================================================
//function : CompanyName
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IGESData_GlobalSection::CompanyName () const
{
  return theCompanyName;
}


//=======================================================================
//function : IGESVersion
//purpose  : 
//=======================================================================

Standard_Integer IGESData_GlobalSection::IGESVersion () const
{
  return theIGESVersion;
}


//=======================================================================
//function : DraftingStandard
//purpose  : 
//=======================================================================

Standard_Integer IGESData_GlobalSection::DraftingStandard () const
{
  return theDraftingStandard;
}


//=======================================================================
//function : LastChangeDate
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IGESData_GlobalSection::LastChangeDate () const
{
  return theLastChangeDate;
}


//=======================================================================
//function : HasLastChangeDate
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_GlobalSection::HasLastChangeDate () const
{
  return (!theLastChangeDate.IsNull());
}


//=======================================================================
//function : SetLastChangeDate
//purpose  : 
//=======================================================================

void IGESData_GlobalSection::SetLastChangeDate ()
{
  if (HasLastChangeDate()) return;
  Standard_Integer mois,jour,annee,heure,minute,seconde,millisec,microsec;
  OSD_Process system;
  Quantity_Date ladate = system.SystemDate();
  ladate.Values (mois,jour,annee,heure,minute,seconde,millisec,microsec);
  if (annee < 2000)
    //#65 rln 12.02.99 S4151 (explicitly force YYMMDD.HHMMSS before Y2000)
    theLastChangeDate = NewDateString (annee,mois,jour,heure,minute,seconde,0);
  else 
    //#65 rln 12.02.99 S4151 (explicitly force YYYYMMDD.HHMMSS after Y2000)
    theLastChangeDate = NewDateString (annee,mois,jour,heure,minute,seconde, -1);
}


//=======================================================================
//function : HasApplicationProtocol
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_GlobalSection::HasApplicationProtocol () const
{
  return !theAppliProtocol.IsNull();
}


//=======================================================================
//function : ApplicationProtocol
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IGESData_GlobalSection::ApplicationProtocol () const
{
  return theAppliProtocol;
}


//=======================================================================
//function : NewDateString
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IGESData_GlobalSection::NewDateString
       (const Standard_Integer annee, const Standard_Integer mois,
        const Standard_Integer jour, const Standard_Integer heure,
        const Standard_Integer minute, const Standard_Integer seconde,
        const Standard_Integer mode)
{
//  0 : IGES annee a l ancienne 00-99    -1 IGES annee complete    1 lisible
  char madate[60];
  Standard_Integer moi  = mois  , jou   = jour   , anne   = annee;
  Standard_Integer heur = heure , minut = minute , second = seconde;
  if (annee == 0) {
    Standard_Integer millisec,microsec;
    OSD_Process system;
    Quantity_Date ladate = system.SystemDate();
    ladate.Values (moi,jou,anne,heur,minut,second,millisec,microsec);
  }
  if (mode == 0 || mode == -1) {
    Standard_Integer an = anne % 100;
    Standard_Boolean dizaine = (an >= 10);
    if (!dizaine) an += 10;
    if (mode < 0) { an = anne; dizaine = Standard_True; }
    Standard_Integer date1 = (an)         * 10000 + moi   * 100 + jou;
    Standard_Integer date2 = (heur + 100) * 10000 + minut * 100 + second;
    sprintf (madate,"%d%d",date1,date2);
    madate[(mode == 0 ? 6: 8)] = '.';
    if (!dizaine) madate[0] = '0';
  } else if (mode == 1) {
    sprintf (madate,"%4.4d-%2.2d-%2.2d:%2.2d-%2.2d-%2.2d",
	     anne,moi,jou,heur,minut,second);
  }
  return new TCollection_HAsciiString(madate);
}


//=======================================================================
//function : NewDateString
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IGESData_GlobalSection::NewDateString
       (const Handle(TCollection_HAsciiString)& date, const Standard_Integer mode)
{
  Standard_Integer anne,moi,jou,heur,minut,second;
  if (date.IsNull()) return date;
  Standard_Integer i0 = 0;
  if (date->Length() == 15) i0 = 2;
  else if (date->Length() != 13) return date;
  if (date->Value(i0+7) != '.') return date;
  anne   = (date->Value(i0+ 1) - 48) * 10 + (date->Value(i0+ 2) - 48);
  if (i0 == 0) {
    anne = anne + 1900;
    if (anne < 1980) anne += 100;
  } else {
    anne = anne + (date->Value(1) - 48) *1000 + (date->Value(2) - 48) * 100;
  }
  moi    = (date->Value(i0+ 3) - 48) * 10 + (date->Value(i0+ 4) - 48);
  jou    = (date->Value(i0+ 5) - 48) * 10 + (date->Value(i0+ 6) - 48);
  heur   = (date->Value(i0+ 8) - 48) * 10 + (date->Value(i0+ 9) - 48);
  minut  = (date->Value(i0+10) - 48) * 10 + (date->Value(i0+11) - 48);
  second = (date->Value(i0+12) - 48) * 10 + (date->Value(i0+13) - 48);

  return IGESData_GlobalSection::NewDateString (anne,moi,jou,heur,minut,second,mode);
}


//=======================================================================
//function : UnitValue
//purpose  : 
//=======================================================================

Standard_Real IGESData_GlobalSection::UnitValue () const
{
  return UnitsMethods::GetLengthFactorValue ( theUnitFlag ) /
         UnitsMethods::GetCasCadeLengthUnit(); //abv 22 Feb 00: adding cascade unit factor
}


// ###############           UPDATES           ###############

    void  IGESData_GlobalSection::SetSeparator   (const Standard_Character val)
      {  theSeparator = val;  }

    void  IGESData_GlobalSection::SetEndMark     (const Standard_Character val)
      {  theEndMark = val;  }

    void  IGESData_GlobalSection::SetSendName (const Handle(TCollection_HAsciiString)& val)
      {  theSendName = TranslatedFromHollerith(val);  }

    void  IGESData_GlobalSection::SetFileName (const Handle(TCollection_HAsciiString)& val)
      {  theFileName = TranslatedFromHollerith(val);  }

    void  IGESData_GlobalSection::SetSystemId    (const Handle(TCollection_HAsciiString)& val)
      {  theSystemId = TranslatedFromHollerith(val);  }

    void  IGESData_GlobalSection::SetInterfaceVersion (const Handle(TCollection_HAsciiString)& val)
      {  theInterfaceVersion = TranslatedFromHollerith(val);  }

    void  IGESData_GlobalSection::SetIntegerBits (const Standard_Integer val)
      {  theIntegerBits = val;  }

    void  IGESData_GlobalSection::SetMaxPower10Single (const Standard_Integer val)
      {  theMaxPower10Single = val;  }

    void  IGESData_GlobalSection::SetMaxDigitsSingle (const Standard_Integer val)
      {  theMaxDigitsSingle = val;  }

    void  IGESData_GlobalSection::SetMaxPower10Double (const Standard_Integer val)
      {  theMaxPower10Double = val;  }

    void  IGESData_GlobalSection::SetMaxDigitsDouble (const Standard_Integer val)
      {  theMaxDigitsDouble = val;  }

    void  IGESData_GlobalSection::SetReceiveName (const Handle(TCollection_HAsciiString)& val)
      {  theReceiveName = TranslatedFromHollerith(val);  }

    void  IGESData_GlobalSection::SetScale       (const Standard_Real val)
      {  theScale = val;  }

    void  IGESData_GlobalSection::SetUnitFlag    (const Standard_Integer val)
      {  theUnitFlag = val;  }

    void  IGESData_GlobalSection::SetUnitName (const Handle(TCollection_HAsciiString)& val)
      {  theUnitName = TranslatedFromHollerith(val);  }

    void  IGESData_GlobalSection::SetLineWeightGrad (const Standard_Integer val)
      {  theLineWeightGrad = val;  }

    void  IGESData_GlobalSection::SetMaxLineWeight (const Standard_Real val)
      {  theMaxLineWeight = val;  }

    void  IGESData_GlobalSection::SetDate (const Handle(TCollection_HAsciiString)& val)
      {  theDate = TranslatedFromHollerith(val);  }

    void  IGESData_GlobalSection::SetResolution  (const Standard_Real val)
      {  theResolution = val;  }

    void  IGESData_GlobalSection::SetMaxCoord    (const Standard_Real val)
{
  hasMaxCoord = (val > 0.);
  theMaxCoord = (hasMaxCoord ? val : 0.);
}

    void  IGESData_GlobalSection::MaxMaxCoord    (const Standard_Real val)
{
  Standard_Real aval = Abs(val);
  if (hasMaxCoord)  {  if (aval > theMaxCoord) theMaxCoord = aval;  }
  else SetMaxCoord (aval);
}

    void  IGESData_GlobalSection::MaxMaxCoords   (const gp_XYZ& xyz)
{  MaxMaxCoord (xyz.X());  MaxMaxCoord (xyz.Y());  MaxMaxCoord (xyz.Z());  }


    void  IGESData_GlobalSection::SetAuthorName (const Handle(TCollection_HAsciiString)& val)
      {  theAuthorName = TranslatedFromHollerith(val);  }

    void  IGESData_GlobalSection::SetCompanyName (const Handle(TCollection_HAsciiString)& val)
      {  theCompanyName = TranslatedFromHollerith(val);  }

    void  IGESData_GlobalSection::SetIGESVersion (const Standard_Integer val)
      {  theIGESVersion = val;  }

    void  IGESData_GlobalSection::SetDraftingStandard (const Standard_Integer val)
      {  theDraftingStandard = val;  }

    void  IGESData_GlobalSection::SetLastChangeDate (const Handle(TCollection_HAsciiString)& val)
      {  theLastChangeDate = TranslatedFromHollerith(val);  }

    void  IGESData_GlobalSection::SetApplicationProtocol (const Handle(TCollection_HAsciiString)& val)
      {  theAppliProtocol = TranslatedFromHollerith(val);  }
