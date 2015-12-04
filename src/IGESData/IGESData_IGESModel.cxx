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

//pdn 11.01.99 modification for linking on NT
//#73 rln 10.03.99 S4135: "read.scale.unit" does not affect GlobalSection
//#13 smh 13.01.2000 : Parsing long year date

#include <IGESData_BasicEditor.hxx>
#include <IGESData_GlobalSection.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_Protocol.hxx>
#include <Interface_Check.hxx>
#include <Interface_InterfaceError.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IGESData_IGESModel,Interface_InterfaceModel)

static Standard_CString voidline = "";

// Routine interne utilisee pour VerifyCheck
void IGESData_VerifyDate
  (const Handle(TCollection_HAsciiString)& str,
   Handle(Interface_Check)& ach, const Standard_CString mess);



//=======================================================================
//function : IGESData_IGESModel
//purpose  : 
//=======================================================================

IGESData_IGESModel::IGESData_IGESModel ()
{
  thestart = new TColStd_HSequenceOfHAsciiString();
//  thecheckstx = new Interface_Check;
//  thechecksem = new Interface_Check;
}


//=======================================================================
//function : ClearHeader
//purpose  : 
//=======================================================================

void IGESData_IGESModel::ClearHeader ()
{
  IGESData_GlobalSection newheader;  // Un peu brutal, certes
  theheader = newheader;
  thestart = new TColStd_HSequenceOfHAsciiString();
}


//=======================================================================
//function : DumpHeader
//purpose  : 
//=======================================================================

void IGESData_IGESModel::DumpHeader
  (const Handle(Message_Messenger)& S, const Standard_Integer ) const
{
  Standard_Integer ns = thestart->Length();
  S<<"****    Dump of IGES Model , Start and Global Sections   ****"<<endl;
  if (ns > 0) {
    S << "****    Start Section : "<<ns<<" Line(s)   ****\n";
    for (Standard_Integer i = 1; i <= ns; i ++)
      S<<"["<<(i<10 ? " ": "")<<i<<"]:"<<thestart->Value(i)->ToCString()<<endl;
  }
  S << "\n****    Global Section    ****\n";
  char sep = theheader.Separator();
  if (sep == ',')  S << "[ 1]      Default Separator : " << sep;
  else             S << "[ 1]  Non Default Separator : " << sep;
  char emk = theheader.EndMark();
  if (emk == ';')  S << "        [ 2]      Default End Mark  : " << emk;
  else             S << "        [ 2]  Non Default End Mark  : " << emk;
  S<<"\n";
  Handle(TCollection_HAsciiString) str;
  str = theheader.SendName();
  if (!str.IsNull()) S<<"[ 3]  Sender                : "<<str->ToCString()<<endl;
  str = theheader.FileName();
  if (!str.IsNull()) S<<"[ 4]  (recorded) File Name  : "<<str->ToCString()<<endl;
  str = theheader.SystemId();
  if (!str.IsNull()) S<<"[ 5]  System Identification : "<<str->ToCString()<<endl;
  str = theheader.InterfaceVersion();
  if (!str.IsNull()) S<<"[ 6]  Interface Version     : "<<str->ToCString()<<endl;
  S<<endl;
  S << "[ 7]  Integer Bits          : " << theheader.IntegerBits()
    << "          Features for Reals : " << endl;
  S << "[ 8]  Single Max.Power(10)  : " << theheader.MaxPower10Single();
  S << "         [ 9]  Digits   : " << theheader.MaxDigitsSingle()<<"\n";
  S << "[10]  Double Max.Power(10)  : " << theheader.MaxPower10Double();
  S << "         [11]  Digits   : " << theheader.MaxDigitsDouble() << "\n\n";
  str = theheader.ReceiveName();
  if (!str.IsNull()) S<<"[12]  Receiver              : "<<str->ToCString()<<"\n";
  S << "[13]  Scale                 : " << theheader.Scale()<<"\n";
  S << "[14]  Unit  Flag            : " << theheader.UnitFlag();
//  if (Interface_Static::IVal("read.scale.unit") == 1)
    //#73 rln 10.03.99 S4135: "read.scale.unit" does not affect GlobalSection
//    S    << "    -> Value (in Meter) = " << theheader.UnitValue() / 1000 <<"\n";
//  else S << "    -> Value (in Millimeter) = " << theheader.UnitValue()<<"\n";
  //abv 02 Mar 00: no unit parameter in OCC
  S << "    -> Value (in CASCADE units) = " << theheader.UnitValue() <<"\n";
  
  str = theheader.UnitName();
  if (!str.IsNull()) S<<"[15]  Unit  Name            : " << str->ToCString()<<"\n\n";
  S << "[16]  Line Weight  Gradient : " << theheader.LineWeightGrad()<<"\n";
  S << "[17]  Line Weight  Max Value: " << theheader.MaxLineWeight()<<"\n";

  str = theheader.Date();
  if (!str.IsNull()) S<<"[18]  (Creation) Date       : "<<str->ToCString()
    <<"  i.e. "<<IGESData_GlobalSection::NewDateString(str,1)->ToCString()<<"\n";
  S << "[19]  Resolution            : " << theheader.Resolution()<<"\n";
  if (theheader.HasMaxCoord())
    S<<"[20]  Maximum Coord         : " << theheader.MaxCoord() << "\n\n";
  else S<<"[20]  Maximum Coord           not defined\n\n";

  str = theheader.AuthorName();
  if (!str.IsNull()) S<<"[21]  Author                : "<<str->ToCString()<<"\n";
  str = theheader.CompanyName();
  if (!str.IsNull()) S<<"[22]  Company               : "<<str->ToCString()<<"\n";
  Standard_Integer num = theheader.IGESVersion();
  S << "[23]  IGES Version Number   : " << num << "   -> Name : " 
    << IGESData_BasicEditor::IGESVersionName(num);

  num = theheader.DraftingStandard();
  S << "\n[24]  Drafting Standard     : " << num;
  if (num > 0) S<< "   -> Name : " << IGESData_BasicEditor::DraftingName(num);
  S<<endl;

  if (theheader.HasLastChangeDate()) {
    str = theheader.LastChangeDate();
    S <<  "[25]  Last Change Date      : " << str->ToCString() 
      <<"  i.e. "<<IGESData_GlobalSection::NewDateString(str,1)->ToCString()<<endl;
  }
  else S<<"[25]  Last Change Date        not defined (version IGES < 5.1)" << endl;

  if (theheader.HasApplicationProtocol()) {
    str = theheader.ApplicationProtocol();
    S <<  "[26]  Application Protocol  : " << str->ToCString() <<endl;
  }

  S << " ****     End of Dump      ****"<<endl;
}


//=======================================================================
//function : StartSection
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfHAsciiString) IGESData_IGESModel::StartSection
       () const
{  return thestart;  }


//=======================================================================
//function : NbStartLines
//purpose  : 
//=======================================================================

Standard_Integer IGESData_IGESModel::NbStartLines () const
{  return thestart->Length();  }


//=======================================================================
//function : StartLine
//purpose  : 
//=======================================================================

Standard_CString IGESData_IGESModel::StartLine
  (const Standard_Integer num) const
{
  if (num > 0 && num <= thestart->Length()) return
    thestart->Value(num)->ToCString();
  return voidline;
}


//=======================================================================
//function : ClearStartSection
//purpose  : 
//=======================================================================

void   IGESData_IGESModel::ClearStartSection ()
      {  thestart->Clear();  }

    void   IGESData_IGESModel::SetStartSection
  (const Handle(TColStd_HSequenceOfHAsciiString)& list,
   const Standard_Boolean copy)
{
  if (copy) {
    thestart = new TColStd_HSequenceOfHAsciiString();
    if (list.IsNull()) return;
    Standard_Integer i, nb = list->Length();
    for (i = 1; i <= nb; i ++) thestart->Append
      (new TCollection_HAsciiString(list->Value(i)->ToCString()));
  }
  else if (list.IsNull()) thestart = new TColStd_HSequenceOfHAsciiString();
  else thestart = list;
}


//=======================================================================
//function : AddStartLine
//purpose  : 
//=======================================================================

void   IGESData_IGESModel::AddStartLine
  (const Standard_CString line, const Standard_Integer atnum)
{
  if (atnum <= 0 || atnum > thestart->Length())
    thestart->Append (new TCollection_HAsciiString(line));
  else thestart->InsertBefore (atnum,new TCollection_HAsciiString(line));
}



//=======================================================================
//function : GlobalSection
//purpose  : 
//=======================================================================

const IGESData_GlobalSection& IGESData_IGESModel::GlobalSection () const
      {  return theheader;  }


//=======================================================================
//function : SetGlobalSection
//purpose  : 
//=======================================================================

void IGESData_IGESModel::SetGlobalSection
  (const IGESData_GlobalSection& header)
      {  theheader = header;  }


//=======================================================================
//function : ApplyStatic
//purpose  : 
//=======================================================================

Standard_Boolean  IGESData_IGESModel::ApplyStatic
  (const Standard_CString param)
{
  if (param[0] == '\0') {
    //Standard_Boolean ret = Standard_True; //szv#4:S4163:12Mar99 not needed
    ApplyStatic("receiver"); //szv#4:S4163:12Mar99 'ret =' not needed
    ApplyStatic("author"); //szv#4:S4163:12Mar99 'ret =' not needed
    ApplyStatic("company"); //szv#4:S4163:12Mar99 'ret =' not needed
    return Standard_True;
  }

  Standard_CString val;
  if (param[0] == 'r') {
    val = Interface_Static::CVal("write.iges.header.receiver");
    if (!val || val[0] == '\0') return Standard_False;
    theheader.SetReceiveName (new TCollection_HAsciiString(val));
  }
  if (param[0] == 'a') {
    val = Interface_Static::CVal("write.iges.header.author");
    if (!val || val[0] == '\0') return Standard_False;
    theheader.SetAuthorName (new TCollection_HAsciiString(val));
  }
  if (param[0] == 'c') {
    val = Interface_Static::CVal("write.iges.header.company");
    if (!val || val[0] == '\0') return Standard_False;
    theheader.SetCompanyName (new TCollection_HAsciiString(val));
  }
  return Standard_True;
}


//=======================================================================
//function : Entity
//purpose  : 
//=======================================================================

Handle(IGESData_IGESEntity) IGESData_IGESModel::Entity
  (const Standard_Integer num) const
{  return GetCasted(IGESData_IGESEntity,Value(num));  }


//=======================================================================
//function : DNum
//purpose  : 
//=======================================================================

Standard_Integer  IGESData_IGESModel::DNum
  (const Handle(IGESData_IGESEntity)& ent) const
{
  Standard_Integer num = Number(ent);
  if (num == 0) return 0;
  else return 2*num-1;
}


//=======================================================================
//function : GetFromAnother
//purpose  : 
//=======================================================================

void IGESData_IGESModel::GetFromAnother
  (const Handle(Interface_InterfaceModel)& other)
{
  DeclareAndCast(IGESData_IGESModel,another,other);
  theheader = another->GlobalSection();
  theheader.CopyRefs();
  SetStartSection (another->StartSection(),Standard_True);
}


//=======================================================================
//function : NewEmptyModel
//purpose  : 
//=======================================================================

Handle(Interface_InterfaceModel) IGESData_IGESModel::NewEmptyModel () const
      {  return new IGESData_IGESModel;  }


//=======================================================================
//function : VerifyCheck
//purpose  : 
//=======================================================================

void  IGESData_IGESModel::VerifyCheck (Handle(Interface_Check)& ach) const
{
  // MGE 23/07/98
  // =====================================
  //Message_Msg Msg40 ("XSTEP_40");
  //Message_Msg Msg41 ("XSTEP_41");
  //Message_Msg Msg42 ("XSTEP_42");
  //Message_Msg Msg43 ("XSTEP_43");
  //Message_Msg Msg44 ("XSTEP_44");
  //Message_Msg Msg45 ("XSTEP_45");
  //Message_Msg Msg46 ("XSTEP_46");
  //Message_Msg Msg47 ("XSTEP_47");
  //Message_Msg Msg48 ("XSTEP_48");
  //Message_Msg Msg50 ("XSTEP_50");
  //Message_Msg Msg51 ("XSTEP_51");
  //Message_Msg Msg52 ("XSTEP_52");
  //Message_Msg Msg53 ("XSTEP_53");
  //Message_Msg Msg54 ("XSTEP_54");
  //Message_Msg Msg55 ("XSTEP_55");
  // =====================================

  char del[2];
  del[0] = theheader.Separator();
  del[1] = theheader.EndMark();
  // Sending of message : Parameter Delimiter Character and Record Delimiter Character must be different.
  if (del[0] == del[1]) {
    Message_Msg Msg40 ("XSTEP_40");
    ach->SendFail(Msg40);
  }
  for (int i = 0; i <= 1; i ++) {
    if ( del[i] <= 32 || del[i] == 43  || del[i] == 45 || del[i] == 46 ||
	(del[i] >= 48 && del[i] <= 57) || del[i] == 68 || del[i] == 69 ||
	del [i] == 72 || del[i] >= 127) {
        // Sending of message : Parameter Delimiter Character is incorrect.
      if (i == 0) {
        Message_Msg Msg41 ("XSTEP_41");
        ach->SendFail(Msg41);
      }
      // Sending of message : Character Record Delimiter parameter is incorrect.
      else {
        Message_Msg Msg42 ("XSTEP_42");
        ach->SendFail (Msg42);
      }
    }
  }
  // Sending of message : Single Precision Magnitude parameter is incorrect. 
  if (theheader.MaxPower10Single() <= 0) {
    Message_Msg Msg43 ("XSTEP_43");
    ach->SendFail(Msg43);
  }

  // Sending of message : Precision Significance parameter is incorrect.
  if (theheader.MaxDigitsSingle()  <= 0) {
    Message_Msg Msg44 ("XSTEP_44");
    ach->SendFail(Msg44);
  }

  // Sending of messages : Double Precision Magnitude parameter is incorrect.
  if (theheader.MaxPower10Double() <= 0) {
    Message_Msg Msg45 ("XSTEP_45");
    ach->SendFail(Msg45);
  }

  // Sending of message : Double Precision Significance parameter is incorrect.
  if (theheader.MaxDigitsDouble()  <= 0) {
    Message_Msg Msg46 ("XSTEP_46");
    ach->SendFail(Msg46);
  }

  // Sending of message : Model Space Scale parameter is incorrect.
  if (theheader.Scale() <= 0.) {
    Message_Msg Msg47 ("XSTEP_47");
    ach->SendFail(Msg47);
  }

  Standard_Integer unf = theheader.UnitFlag();

  // Sending of message : Unit Flag parameter is incorrect.
  if (unf  < 1 || unf > 11) {
    Message_Msg Msg48 ("XSTEP_48");
    ach->SendFail(Msg48);
  }

// ..  verifie-t-on UnitName en accord avec UnitFlag ?
  if (theheader.UnitName().IsNull()) {
    // Sending of message : Unit Name parameter is undefined.
    if (unf == 3) {
      Message_Msg Msg50 ("XSTEP_50");
      ach->SendFail(Msg50);
    }
  }
  else {
    Standard_CString unm  = theheader.UnitName()->ToCString();
    Standard_Boolean unok = Standard_True;
    switch (unf) {
      case  1 : unok = (!strcmp(unm,"IN") || !strcmp(unm,"INCH"));  break;
      case  2 : unok = !strcmp(unm,"MM");   break;
      case  3 : unok = Standard_True;       break;  // nom libre
      case  4 : unok = !strcmp(unm,"FT");   break;
      case  5 : unok = !strcmp(unm,"MI");   break;
      case  6 : unok = !strcmp(unm,"M");    break;
      case  7 : unok = !strcmp(unm,"KM");   break;
      case  8 : unok = !strcmp(unm,"MIL");  break;
      case  9 : unok = !strcmp(unm,"UM");   break;
      case 10 : unok = !strcmp(unm,"CM");   break;
      case 11 : unok = !strcmp(unm,"UIN");  break;
      default : Message_Msg Msg48 ("XSTEP_48"); ach->SendFail(Msg48);
	break;
    }
    // Sending of message : Flag parameter doesn`t correspond to the Unit Name parameter.
    if (!unok) {
      Message_Msg Msg51 ("XSTEP_51");
      ach->SendFail(Msg51);
    }
  }

  IGESData_VerifyDate (theheader.Date(),ach,"Creation Date");
  // Sending of message : Minimum Resolution parameter is incorrect.
  if (theheader.Resolution() <= 0.) {
    Message_Msg Msg52 ("XSTEP_52");
    ach->SendFail(Msg52);
  }
  // ..  comment verifier les coordonnees max ?

  // Sending of message : Version Flag parameter is incorrect.
  if (theheader.IGESVersion() < 1 ||
      theheader.IGESVersion() > IGESData_BasicEditor::IGESVersionMax()) {
    Message_Msg Msg53 ("XSTEP_53");
    ach->SendWarning(Msg53);
  }

  // Sending of message : Drafting Standard Flag parameter is incorrect.
  if (theheader.DraftingStandard() < 0 ||
      theheader.DraftingStandard() > IGESData_BasicEditor::DraftingMax()) {
    Message_Msg Msg54 ("XSTEP_54");
    ach->SendWarning(Msg54);
  }

  // Sending of message : 
  if (theheader.IGESVersion() >= 9) {
    // Sending of message : Last change Date parameter is undefined.
    if (!theheader.HasLastChangeDate()) {
      Message_Msg Msg55 ("XSTEP_55");
      ach->SendWarning(Msg55);
    }
    else IGESData_VerifyDate (theheader.LastChangeDate(),ach,"Last Change Date");
  }
}


void IGESData_VerifyDate(const Handle(TCollection_HAsciiString)& str,
                         Handle(Interface_Check)& ach,
                         const Standard_CString mess)
{
  // MGE 23/07/98
  // =====================================
  Message_Msg Msg57 ("XSTEP_57");
  // =====================================

  //  Attention c est du Hollerith
  if (str.IsNull())
    {  ach->SendFail(Msg57);  return;  }

  Handle(TCollection_HAsciiString) stdvar = str;
  if (strcmp(mess,"Last Change Date")==0)
    Msg57.Arg(25);
  else
    Msg57.Arg(18);
  if (((stdvar->Length() != 13) && (stdvar->Length() != 15)) || !stdvar->IsRealValue()) ach->SendFail(Msg57); 
  //smh#13 For short year date
  else if ((stdvar->Value(3) > '1' || 
	    (stdvar->Value(3) == '1' && stdvar->Value(4) > '2'))&&(stdvar->Length() == 13)) ach->SendFail(Msg57);  
  
  else if ((stdvar->Value(5) > '3' ||
	    (stdvar->Value(5) == '3' && stdvar->Value(6) > '1'))&&(stdvar->Length() == 13)) ach->SendFail(Msg57);  
  else if ((stdvar->Value(7) != '.' || stdvar->Value(10) > '5' || stdvar->Value(12) > '5')&&(stdvar->Length() == 13))
    ach->SendFail(Msg57);
  else if ((stdvar->Value(8) > '2' || (stdvar->Value(8) == '2' && stdvar->Value(9) > '3')) && 
           (stdvar->Length() == 13)) ach->SendFail(Msg57);  
  //smh#13 For long year date 
  else if ( (stdvar->Value(5) > '1' ||
	     (stdvar->Value(5) == '1' && stdvar->Value(6) > '2'))&&(stdvar->Length() == 15)) ach->SendFail(Msg57);  
  else if ((stdvar->Value(7) > '3' ||
	    (stdvar->Value(7) == '3' && stdvar->Value(8) > '1'))&&(stdvar->Length() == 15)) ach->SendFail(Msg57);  
  else if ((stdvar->Value(9) != '.' || stdvar->Value(12) > '5' || stdvar->Value(14) > '5')&&(stdvar->Length() == 15))
    ach->SendFail(Msg57);
  else if ((stdvar->Value(10) > '2' ||
	    (stdvar->Value(10) == '2' && stdvar->Value(11) > '3'))&&(stdvar->Length() == 15)) ach->SendFail(Msg57);  
  
}


//=======================================================================
//function : SetLineWeights
//purpose  : 
//=======================================================================

void IGESData_IGESModel::SetLineWeights (const Standard_Real defw)
{
  Standard_Integer nb  = NbEntities();
  Standard_Integer lwg = theheader.LineWeightGrad();
  Standard_Real maxw   = theheader.MaxLineWeight();
  if (lwg > 0) {  maxw = maxw/lwg; lwg = 1;  }
   for (Standard_Integer i = 1; i <= nb; i ++)
     Entity(i)->SetLineWeight(defw,maxw,lwg);
}


//=======================================================================
//function : ClearLabels
//purpose  : 
//=======================================================================

void  IGESData_IGESModel::ClearLabels ()  
{
}


//=======================================================================
//function : PrintLabel
//purpose  : 
//=======================================================================

void  IGESData_IGESModel::PrintLabel
  (const Handle(Standard_Transient)& ent, const Handle(Message_Messenger)& S) const
{ 
  DeclareAndCast(IGESData_IGESEntity,igesent,ent);
  if (igesent.IsNull()) S<<"Null";
   else {
     Standard_Integer num = Number(ent);
     if (num == 0) S<<"??";
     else          S<<"D"<<(2*num-1);
   }
}


//=======================================================================
//function : PrintToLog
//purpose  : 
//=======================================================================

void  IGESData_IGESModel::PrintToLog
  (const Handle(Standard_Transient)& ent, const Handle(Message_Messenger)& S) const
{
  DeclareAndCast(IGESData_IGESEntity,igesent,ent);
  if (!igesent.IsNull()) {
    Standard_Integer num = Number(ent);
    if (num == 0) S<<"??";
    else {
      S<<" DE : "<<(2*num-1) << " type : " << igesent->TypeNumber();
//      Standard_Integer num2 = igesent->TypeNumber();
    }
  }
}


//=======================================================================
//function : PrintInfo
//purpose  : 
//=======================================================================

void  IGESData_IGESModel::PrintInfo
  (const Handle(Standard_Transient)& ent, const Handle(Message_Messenger)& S) const
{
  DeclareAndCast(IGESData_IGESEntity,igesent,ent);
  if (igesent.IsNull()) S<<"(NOT IGES)";
  else {
    Standard_Integer num = Number(ent);
    if (num == 0) S<<"??";
    else  {
      S<<(2*num-1) << "type " << Type(ent)->Name();
    }
  }
}


//=======================================================================
//function : StringLabel
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IGESData_IGESModel::StringLabel(const Handle(Standard_Transient)& ent) const
{
  Handle(TCollection_HAsciiString) label;
  DeclareAndCast(IGESData_IGESEntity,igesent,ent);
  if (igesent.IsNull()) return new TCollection_HAsciiString("(NOT IGES)");
  else {
    char text[20];
    Standard_Integer num = Number(ent);
    if (num > 0) sprintf(text,"D%d",2*num-1);
    else         sprintf(text,"D0...");
    label = new TCollection_HAsciiString(text);
  }
  return label;
}
