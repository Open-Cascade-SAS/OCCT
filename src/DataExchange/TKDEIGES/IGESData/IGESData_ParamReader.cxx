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

// pdn S4135 05.04.99 comment uninitialized Interface_Static::IVal("iges.convert.read");

#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESType.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_Status.hxx>
#include <Interface_Check.hxx>
#include <Interface_EntityList.hxx>
#include <Interface_FileReaderData.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ParamList.hxx>
#include <Interface_Static.hxx>
#include <Message_Msg.hxx>
#include <TCollection_HAsciiString.hxx>

#include <stdio.h>
// MGE 03/08/98
static int testconv = -1; // see session parameter

//  ....              Gestion generale (etat, courant ...)              ....

//=================================================================================================

IGESData_ParamReader::IGESData_ParamReader(const occ::handle<Interface_ParamList>& list,
                                           const occ::handle<Interface_Check>&     ach,
                                           const int             base,
                                           const int             nbpar,
                                           const int /*num*/)
{
  Clear();
  theparams = list;
  thecheck  = ach;
  thelast   = true;
  thebase   = base;
  thenbpar  = (nbpar > 0 ? nbpar : list->Length());
  thenum    = 0;
  testconv  = -1;
}

//=================================================================================================

int IGESData_ParamReader::EntityNumber() const
{
  return thenum;
}

//=================================================================================================

void IGESData_ParamReader::Clear()
{
  thecurr   = 1;
  thestage  = IGESData_ReadOwn;
  pbrealint = pbrealform = 0;
}

//=================================================================================================

int IGESData_ParamReader::CurrentNumber() const
{
  return thecurr;
}

//=================================================================================================

void IGESData_ParamReader::SetCurrentNumber(const int num)
{
  // if (num <= NbParams() + 1) thecurr = num;  // NbParams+1 : "fin d'objet"
  // else thecurr = 0;
  thecurr = num;
}

//=================================================================================================

IGESData_ReadStage IGESData_ParamReader::Stage() const
{
  return thestage;
}

//=================================================================================================

void IGESData_ParamReader::NextStage()
{
  if (thestage != IGESData_ReadEnd)
    thestage = (IGESData_ReadStage)(((long)thestage) + 1);
}

//=================================================================================================

void IGESData_ParamReader::EndAll()
{
  thestage = IGESData_ReadEnd;
}

//  ....                  Basic parameter access                  ....

//=================================================================================================

int IGESData_ParamReader::NbParams() const
{
  return (thenbpar - 1);
}

//=================================================================================================

Interface_ParamType IGESData_ParamReader::ParamType(const int num) const
{
  return theparams->Value(num + thebase).ParamType();
}

//=================================================================================================

const char* IGESData_ParamReader::ParamValue(const int num) const
{
  return theparams->Value(num + thebase).CValue();
}

//=================================================================================================

bool IGESData_ParamReader::IsParamDefined(const int num) const
{
  if (num >= thenbpar)
    return false;
  return (theparams->Value(num + thebase).ParamType() != Interface_ParamVoid);
}

//=================================================================================================

bool IGESData_ParamReader::IsParamEntity(const int num) const
{
  return (ParamNumber(num) != 0);
}

//=================================================================================================

int IGESData_ParamReader::ParamNumber(const int num) const
{
  return theparams->Value(num + thebase).EntityNumber();
}

//=================================================================================================

occ::handle<IGESData_IGESEntity> IGESData_ParamReader::ParamEntity(
  const occ::handle<IGESData_IGESReaderData>& IR,
  const int                 num)
{
  int n = ParamNumber(num);
  if (n == 0)
    thecheck->AddFail("IGES ParamReader : ParamEntity, bad param");
  return GetCasted(IGESData_IGESEntity, IR->BoundEntity(n));
}

//  ....                    Reading assistance                    ....

//  The Read* functions offer the following services :
//  Error management : the Check is fed, by Fail or Corrected accordingly
//  If Fail, function returns False (may be useful), otherwise True
//  Furthermore, a Status is managed (of type enum DataState)
//    (can be queried following Read* call if True/False return too short)
//
//  Current pointer management (optional, but set by default) :
//  Parameters are designated via a ParmCursor, which can be manufactured by
//  the ad hoc methods Current and CurrentList, and which can request to advance
//  the current pointer once reading is done
//  Furthermore, for an HArray1, you can specify starting index

//=================================================================================================

IGESData_ParamCursor IGESData_ParamReader::Current() const
{
  return IGESData_ParamCursor(thecurr);
}

//=================================================================================================

IGESData_ParamCursor IGESData_ParamReader::CurrentList(const int nb,
                                                       const int size) const
{
  return IGESData_ParamCursor(thecurr, nb, size);
}

// PrepareRead for MoniTool

//=================================================================================================

bool IGESData_ParamReader::PrepareRead(const IGESData_ParamCursor& PC,
                                                   const bool      several,
                                                   const int      size)
{
  theindex  = PC.Start();
  themaxind = PC.Limit();
  thenbitem = PC.Count();
  theitemsz = PC.ItemSize();
  theoffset = PC.Offset();
  thetermsz = PC.TermSize();
  if (!several && thenbitem > 1)
  {
    // AddFail (mess," : List not allowed","");
    return false;
  }
  if (size > 1)
  {
    if (thetermsz % size != 0)
    {
      return false;
    }
  }
  if (theindex <= 0 || (themaxind - 1) > NbParams())
  {
    return false;
  }
  if (PC.Advance())
    SetCurrentNumber(themaxind); // themaxind : prochain index
  thelast = true;
  return true;
}

//=================================================================================================

bool IGESData_ParamReader::PrepareRead(const IGESData_ParamCursor& PC,
                                                   const char* const      mess,
                                                   const bool      several,
                                                   const int      size)
{
  theindex  = PC.Start();
  themaxind = PC.Limit();
  thenbitem = PC.Count();
  theitemsz = PC.ItemSize();
  theoffset = PC.Offset();
  thetermsz = PC.TermSize();
  if (!several && thenbitem > 1)
  {
    AddFail(mess, " : List not allowed", "");
    return false;
  }
  if (size > 1)
  {
    if (thetermsz % size != 0)
    {
      AddFail(mess, " : term size mismatch", "");
      return false;
    }
  }
  if (theindex <= 0 || (themaxind - 1) > NbParams())
  {
    if (thenbitem == 1)
      AddFail(mess, " : Parameter number out of range", "");
    // else AddFail (mess," : too many values to read" ,"");
    else
      AddWarning(mess, " : too many values to read", "");
    return false;
  }
  if (PC.Advance())
    SetCurrentNumber(themaxind); // themaxind : prochain index
  thelast = true;
  return true;
}

//  theindex gives the start of reading; always aligned on item start
//  thenbterm gives start to read in the item
//  Thus, the true index is   theindex + thenbterm
//  thenbterm advances by +nb. When it has exceeded thetermsz, next item
//  theindex is itself limited (stop criterion) to themaxind

//=================================================================================================

int IGESData_ParamReader::FirstRead(const int nb)
{
  theindex += theoffset; // We automatically align on the start of the term to read
  int res = theindex;
  thenbterm            = nb;
  if (thenbterm >= thetermsz)
  {
    theindex += theitemsz;
    thenbterm = 0;
  }
  return res;
}

//=================================================================================================

int IGESData_ParamReader::NextRead(const int nb)
{
  int res = theindex;
  if (theindex >= themaxind)
    res = 0;
  thenbterm += nb; // Per Item: read thetermsz, then next item
  if (thenbterm >= thetermsz)
  {
    theindex += theitemsz;
    thenbterm = 0;
  }
  return res;
}

//=================================================================================================

bool IGESData_ParamReader::DefinedElseSkip()
{
  if (thecurr > NbParams())
    return false; // Skip en butee
  if (IsParamDefined(thecurr))
    return true;        // Defined
  SetCurrentNumber(thecurr + 1); // Skip
  return false;
}

// ReadInteger for MoniTool

//=================================================================================================

bool IGESData_ParamReader::ReadInteger(const IGESData_ParamCursor& PC,
                                                   int&           val)
{
  if (!PrepareRead(PC, false))
    return false;
  const Interface_FileParameter& FP = theparams->Value(theindex + thebase);
  if (FP.ParamType() != Interface_ParamInteger)
  {
    if (FP.ParamType() == Interface_ParamVoid)
    {
      val = 0;
      return true;
    } // DEFAULT
    return false;
  }
  val = atoi(FP.CValue());
  return true;
}

//=================================================================================================

bool IGESData_ParamReader::ReadInteger(const IGESData_ParamCursor& PC,
                                                   const char* const      mess,
                                                   int&           val)
{
  if (!PrepareRead(PC, mess, false))
    return false;
  const Interface_FileParameter& FP = theparams->Value(theindex + thebase);
  if (FP.ParamType() != Interface_ParamInteger)
  {
    if (FP.ParamType() == Interface_ParamVoid)
    {
      val = 0;
      return true;
    } // DEFAULT
    AddFail(mess, " : not given as an Integer", "");
    return false;
  }
  val = atoi(FP.CValue());
  return true;
}

// ReadBoolean for MoniTool

//=================================================================================================

bool IGESData_ParamReader::ReadBoolean(const IGESData_ParamCursor& PC,
                                                   const Message_Msg&          amsg,
                                                   bool&           val,
                                                   const bool      exact)
{
  if (!PrepareRead(PC, false))
    return false;
  const Interface_FileParameter& FP = theparams->Value(theindex + thebase);
  if (FP.ParamType() != Interface_ParamInteger)
  {
    if (FP.ParamType() == Interface_ParamVoid)
    {
      val = false;
      return true;
    } // DEFAULT
    SendFail(amsg);
    return false;
  }

  //  A Boolean is 0/1. But we can tolerate other values
  //  One can always consult LastReadStatus after reading to be sure
  int flag = atoi(FP.CValue());
  if (flag != 0 && flag != 1)
  {
    if (exact)
    {
      SendFail(amsg);
      thelast = true;
      return false;
    }
    else
    {
      SendWarning(amsg);
    }
  }
  val = (flag > 0);
  return true;
}

//=================================================================================================

bool IGESData_ParamReader::ReadBoolean(const IGESData_ParamCursor& PC,
                                                   const char* const      mess,
                                                   bool&           val,
                                                   const bool      exact)
{
  if (!PrepareRead(PC, mess, false))
    return false;
  const Interface_FileParameter& FP = theparams->Value(theindex + thebase);
  if (FP.ParamType() != Interface_ParamInteger)
  {
    if (FP.ParamType() == Interface_ParamVoid)
    {
      val = false;
      return true;
    } // DEFAULT
    AddFail(mess, " : not an Integer (for Boolean)", "");
    return false;
  }

  //  A Boolean is 0/1. But we can tolerate other values
  //  One can always consult LastReadStatus after reading to be sure
  int flag = atoi(FP.CValue());
  if (flag != 0 && flag != 1)
  {
    char ssem[100];
    Sprintf(ssem, " : Value is not 0/1, but %s", FP.CValue());
    if (exact)
    {
      AddFail(mess, ssem, " : Value is not 0/1, but %s");
      thelast = true;
      return false;
    }
    else
      AddWarning(mess, ssem, " : Value is not 0/1, but %s");
  }
  val = (flag > 0);
  return true;
}

// ReadReal for MoniTool

//=================================================================================================

bool IGESData_ParamReader::ReadReal(const IGESData_ParamCursor& PC, double& val)
{
  if (!PrepareRead(PC, false))
    return false;
  //  return ReadingReal (theindex,amsg,val);
  return ReadingReal(theindex, val);
}

//=================================================================================================

bool IGESData_ParamReader::ReadReal(const IGESData_ParamCursor& PC,
                                                const char* const      mess,
                                                double&              val)
{
  if (!PrepareRead(PC, mess, false))
    return false;
  return ReadingReal(theindex, mess, val);
}

// ReadXY for MoniTool

//=================================================================================================

bool IGESData_ParamReader::ReadXY(const IGESData_ParamCursor& PC,
                                              Message_Msg& /*amsg*/,
                                              gp_XY& val)
{
  if (!PrepareRead(PC, false, 2))
    return false;
  double    X, Y = 0.;
  bool stat = (ReadingReal(theindex, X) && ReadingReal(theindex + 1, Y));
  if (stat)
    val.SetCoord(X, Y);
  return stat;
}

//=================================================================================================

bool IGESData_ParamReader::ReadXY(const IGESData_ParamCursor& PC,
                                              const char* const      mess,
                                              gp_XY&                      val)
{
  if (!PrepareRead(PC, mess, false, 2))
    return false;
  double    X, Y = 0.;
  bool stat = (ReadingReal(theindex, mess, X) && ReadingReal(theindex + 1, mess, Y));
  if (stat)
    val.SetCoord(X, Y);
  return stat;
}

// ReadXYZ for MoniTool

//=================================================================================================

bool IGESData_ParamReader::ReadXYZ(const IGESData_ParamCursor& PC,
                                               Message_Msg& /*amsg*/,
                                               gp_XYZ& val)
{
  if (!PrepareRead(PC, false, 3))
    return false;
  double    X, Y = 0., Z = 0.;
  bool stat =
    (ReadingReal(theindex, X) && ReadingReal(theindex + 1, Y) && ReadingReal(theindex + 2, Z));
  if (stat)
    val.SetCoord(X, Y, Z);
  return true;
}

//=================================================================================================

bool IGESData_ParamReader::ReadXYZ(const IGESData_ParamCursor& PC,
                                               const char* const      mess,
                                               gp_XYZ&                     val)
{
  if (!PrepareRead(PC, mess, false, 3))
    return false;
  double    X, Y = 0., Z = 0.;
  bool stat = (ReadingReal(theindex, mess, X) && ReadingReal(theindex + 1, mess, Y)
                           && ReadingReal(theindex + 2, mess, Z));
  if (stat)
    val.SetCoord(X, Y, Z);
  return true;
}

// ReadText for MoniTool

//=================================================================================================

bool IGESData_ParamReader::ReadText(const IGESData_ParamCursor&       thePC,
                                                const Message_Msg&                theMsg,
                                                occ::handle<TCollection_HAsciiString>& theVal)
{
  if (!PrepareRead(thePC, false))
  {
    return false;
  }
  const Interface_FileParameter& aFP = theparams->Value(theindex + thebase);
  if (aFP.ParamType() != Interface_ParamText)
  {
    theVal = new TCollection_HAsciiString("");
    if (aFP.ParamType() == Interface_ParamVoid)
    {
      return true;
    }
    SendFail(theMsg);
    return false;
  }
  const occ::handle<TCollection_HAsciiString> aBaseValue  = new TCollection_HAsciiString(aFP.CValue());
  const int                 aBaseLength = aBaseValue->Length();
  const int aSymbolLocation             = aBaseValue->Location(1, 'H', 1, aBaseLength);
  if (aSymbolLocation <= 1 || aSymbolLocation > aBaseLength)
  {
    theVal = new TCollection_HAsciiString("");
    SendFail(theMsg);
    return false;
  }
  const TCollection_AsciiString aSpecialSubString =
    aBaseValue->String().SubString(1, aSymbolLocation - 1);
  if (!aSpecialSubString.IsIntegerValue())
  {
    theVal = new TCollection_HAsciiString("");
    SendFail(theMsg);
    return false;
  }
  int aResLength = aSpecialSubString.IntegerValue();
  if (aResLength != (aBaseLength - aSymbolLocation))
  {
    SendWarning(theMsg);
    aResLength = aBaseLength - aSymbolLocation;
  }
  TCollection_AsciiString aResString;
  if (aResLength > 0)
  {
    aResString = aBaseValue->String().SubString(aSymbolLocation + 1, aBaseLength);
  }
  theVal = new TCollection_HAsciiString(aResString);
  return true;
}

//=================================================================================================

bool IGESData_ParamReader::ReadText(const IGESData_ParamCursor&       PC,
                                                const char* const            mess,
                                                occ::handle<TCollection_HAsciiString>& val)
{
  if (!PrepareRead(PC, mess, false))
    return false;
  const Interface_FileParameter& FP = theparams->Value(theindex + thebase);
  if (FP.ParamType() != Interface_ParamText)
  {
    if (FP.ParamType() == Interface_ParamVoid)
    {
      val = new TCollection_HAsciiString("");
      return true;
    }
    AddFail(mess, " : not given as a Text", "");
    return false;
  }
  occ::handle<TCollection_HAsciiString> tval = new TCollection_HAsciiString(FP.CValue());
  int                 lnt  = tval->Length();
  int                 lnh  = tval->Location(1, 'H', 1, lnt);
  if (lnh <= 1 || lnh >= lnt)
  {
    AddFail(mess, " : not in Hollerith Form", "");
    return false;
  }
  else
  {
    int hol = atoi(tval->SubString(1, lnh - 1)->ToCString());
    if (hol != (lnt - lnh))
      AddWarning(mess, " : bad Hollerith count ", "");
  }
  val = new TCollection_HAsciiString(tval->SubString(lnh + 1, lnt)->ToCString());
  return true;
}

// ReadEntity for MoniTool

//=================================================================================================

bool IGESData_ParamReader::ReadEntity(const occ::handle<IGESData_IGESReaderData>& IR,
                                                  const IGESData_ParamCursor&            PC,
                                                  IGESData_Status&                       aStatus,
                                                  occ::handle<IGESData_IGESEntity>&           val,
                                                  const bool                 canbenul)
{
  aStatus = IGESData_EntityError;
  if (!PrepareRead(PC, false))
    return false;
  int nval;
  //  if (!ReadingEntityNumber(theindex,amsg,nval)) return false;
  if (!ReadingEntityNumber(theindex, nval))
    return false;
  if (nval == 0)
  {
    val.Nullify();
    if (!canbenul)
    {
      aStatus = IGESData_ReferenceError;
      // Message_Msg Msg216 ("IGESP_216");
      // amsg.Arg(amsg.Value());
      // SendFail (amsg);

      thelast = true;
    }
    else
      aStatus = IGESData_EntityOK;
    return canbenul;
  }
  else
    val = GetCasted(IGESData_IGESEntity, IR->BoundEntity(nval));
  if (val.IsNull())
    return canbenul;
  //    Case of "Null IGES"
  if (val->TypeNumber() == 0)
  { // Null or not yet filled ...
    if (IR->DirType(nval).Type() == 0)
    { // the real criterion (a bit expensive)
      val.Nullify();
      if (!canbenul)
      {
        aStatus = IGESData_EntityError;
        // Message_Msg Msg217 ("IGES_217");
        // amsg.Arg(Msg217.Value());
        // SendFail (amsg);
        thelast = true;
      }
      else
        aStatus = IGESData_EntityOK;
      return canbenul;
    }
  }
  aStatus = IGESData_EntityOK;
  return true;
}

//=================================================================================================

bool IGESData_ParamReader::ReadEntity(const occ::handle<IGESData_IGESReaderData>& IR,
                                                  const IGESData_ParamCursor&            PC,
                                                  const char* const                 mess,
                                                  occ::handle<IGESData_IGESEntity>&           val,
                                                  const bool                 canbenul)
{
  if (!PrepareRead(PC, mess, false))
    return false;
  int nval;
  if (!ReadingEntityNumber(theindex, mess, nval))
    return false;
  if (nval == 0)
  {
    val.Nullify();
    if (!canbenul)
    {
      AddFail(mess, " : Null Reference", "");
      thelast = true;
    }
    return canbenul;
  }
  else
    val = GetCasted(IGESData_IGESEntity, IR->BoundEntity(nval));
  if (val.IsNull())
    return canbenul;
  //    Case of "Null IGES"
  if (val->TypeNumber() == 0)
  { // Null or not yet filled ...
    if (IR->DirType(nval).Type() == 0)
    { // the real criterion (a bit expensive)
      val.Nullify();
      if (!canbenul)
      {
        AddFail(mess, " : IGES Null Entity", "");
        thelast = true;
      }
      return canbenul;
    }
  }
  return true;
}

// ReadEntity for MoniTool

//=================================================================================================

bool IGESData_ParamReader::ReadEntity(const occ::handle<IGESData_IGESReaderData>& IR,
                                                  const IGESData_ParamCursor&            PC,
                                                  IGESData_Status&                       aStatus,
                                                  const occ::handle<Standard_Type>&           type,
                                                  occ::handle<IGESData_IGESEntity>&           val,
                                                  const bool                 canbenul)
{
  bool res = ReadEntity(IR, PC, aStatus, val, canbenul);
  if (!res)
  {
    return res;
  }
  if (val.IsNull())
    return res;
  if (!val->IsKind(type))
  {
    aStatus = IGESData_TypeError;
    // Message_Msg Msg218 ("IGES_218");
    // amsg.Arg(Msg218.Value());
    // SendFail(amsg);
    thelast = true;
    val.Nullify();
    return false;
  }
  return true;
}

//=================================================================================================

bool IGESData_ParamReader::ReadEntity(const occ::handle<IGESData_IGESReaderData>& IR,
                                                  const IGESData_ParamCursor&            PC,
                                                  const char* const                 mess,
                                                  const occ::handle<Standard_Type>&           type,
                                                  occ::handle<IGESData_IGESEntity>&           val,
                                                  const bool                 canbenul)
{
  bool res = ReadEntity(IR, PC, mess, val, canbenul);
  if (!res)
    return res;
  if (val.IsNull())
    return res;
  if (!val->IsKind(type))
  {
    AddFail(mess, " : Incorrect Type", "");
    thelast = true;
    val.Nullify();
    return false;
  }
  return true;
}

// ReadInts for MoniTool

//=================================================================================================

bool IGESData_ParamReader::ReadInts(const IGESData_ParamCursor&       PC,
                                                const Message_Msg&                amsg,
                                                occ::handle<NCollection_HArray1<int>>& val,
                                                const int            index)
{
  if (!PrepareRead(PC, true))
    return false;
  if (thenbitem == 0)
    return true; // vide : retour Null ...
  val                  = new NCollection_HArray1<int>(index, index + thenbitem * thetermsz - 1);
  int ind = index;

  for (int i = FirstRead(); i > 0; i = NextRead())
  {
    const Interface_FileParameter& FP = theparams->Value(i + thebase);
    if (FP.ParamType() == Interface_ParamInteger)
    {
      val->SetValue(ind, atoi(FP.CValue()));
      ind++;
    }
    else if (FP.ParamType() == Interface_ParamVoid)
    {
      val->SetValue(ind, 0);
      ind++; // DEFAULT : rien a dire
    }
    else
    {
      SendFail(amsg);
      return false;
    }
  }
  return true;
}

//=================================================================================================

bool IGESData_ParamReader::ReadInts(const IGESData_ParamCursor&       PC,
                                                const char* const            mess,
                                                occ::handle<NCollection_HArray1<int>>& val,
                                                const int            index)
{
  if (!PrepareRead(PC, mess, true))
    return false;
  if (thenbitem == 0)
    return true; // vide : retour Null ...
  val                  = new NCollection_HArray1<int>(index, index + thenbitem * thetermsz - 1);
  int ind = index;

  for (int i = FirstRead(); i > 0; i = NextRead())
  {
    const Interface_FileParameter& FP = theparams->Value(i + thebase);
    if (FP.ParamType() == Interface_ParamInteger)
    {
      val->SetValue(ind, atoi(FP.CValue()));
      ind++;
    }
    else if (FP.ParamType() == Interface_ParamVoid)
    {
      val->SetValue(ind, 0);
      ind++; // DEFAULT : rien a dire
    }
    else
    {
      char ssem[100];
      Sprintf(ssem, " : not an Integer, rank %d", i);
      AddFail(mess, ssem, " : not an Integer, rank %d");
      return false;
    }
  }
  return true;
}

// ReadReals for MoniTool

//=================================================================================================

bool IGESData_ParamReader::ReadReals(const IGESData_ParamCursor& PC,
                                                 Message_Msg& /*amsg*/,
                                                 occ::handle<NCollection_HArray1<double>>& val,
                                                 const int         index)
{
  if (!PrepareRead(PC, true))
    return false;
  if (thenbitem == 0)
    return true; // vide : retour Null ...
  val                  = new NCollection_HArray1<double>(index, index + thenbitem * thetermsz - 1);
  int ind = index;

  for (int i = FirstRead(); i > 0; i = NextRead())
  {
    double rval;
    if (!ReadingReal(i, rval))
      return false;
    val->SetValue(ind, rval);
    ind++;
  }
  return true;
}

//=================================================================================================

bool IGESData_ParamReader::ReadReals(const IGESData_ParamCursor&    PC,
                                                 const char* const         mess,
                                                 occ::handle<NCollection_HArray1<double>>& val,
                                                 const int         index)
{
  if (!PrepareRead(PC, mess, true))
    return false;
  if (thenbitem == 0)
    return true; // vide : retour Null ...
  val                  = new NCollection_HArray1<double>(index, index + thenbitem * thetermsz - 1);
  int ind = index;

  for (int i = FirstRead(); i > 0; i = NextRead())
  {
    double rval;
    if (!ReadingReal(i, mess, rval))
      return false;
    val->SetValue(ind, rval);
    ind++;
  }
  return true;
}

// ReadTexts for MoniTool

//=================================================================================================

bool IGESData_ParamReader::ReadTexts(const IGESData_ParamCursor&              PC,
                                                 const Message_Msg&                       amsg,
                                                 occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& val,
                                                 const int                   index)
{
  if (!PrepareRead(PC, true))
    return false;
  if (thenbitem == 0)
    return true; // vide : retour Null ...
  val = new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(index, index + thenbitem * thetermsz - 1);
  int ind = index;

  for (int i = FirstRead(); i > 0; i = NextRead())
  {
    const Interface_FileParameter& FP = theparams->Value(i + thebase);
    if (FP.ParamType() != Interface_ParamText)
    {
      if (FP.ParamType() == Interface_ParamVoid)
      {
        val->SetValue(ind, new TCollection_HAsciiString(""));
        ind++;
        // AddWarning (mess," : empty text","");  DEFAULT : rien a dire
        continue;
      }
      SendFail(amsg);
      return false;
    }
    occ::handle<TCollection_HAsciiString> tval = new TCollection_HAsciiString(FP.CValue());
    // IGESFile_Read a filtre
    int lnt = tval->Length();
    int lnh = tval->Location(1, 'H', 1, lnt);
    if (lnh <= 1 || lnh >= lnt)
    {
      SendFail(amsg);
      return false;
    }
    else
    {
      int hol = atoi(tval->SubString(1, lnh - 1)->ToCString());
      if (hol != (lnt - lnh))
        SendWarning(amsg);
    }
    val->SetValue(ind, new TCollection_HAsciiString(tval->SubString(lnh + 1, lnt)->ToCString()));
    ind++;
  }
  return true;
}

//=================================================================================================

bool IGESData_ParamReader::ReadTexts(const IGESData_ParamCursor&              PC,
                                                 const char* const                   mess,
                                                 occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& val,
                                                 const int                   index)
{
  if (!PrepareRead(PC, mess, true))
    return false;
  if (thenbitem == 0)
    return true; // vide : retour Null ...
  val = new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(index, index + thenbitem * thetermsz - 1);
  int ind = index;

  for (int i = FirstRead(); i > 0; i = NextRead())
  {
    const Interface_FileParameter& FP = theparams->Value(i + thebase);
    if (FP.ParamType() != Interface_ParamText)
    {
      if (FP.ParamType() == Interface_ParamVoid)
      {
        val->SetValue(ind, new TCollection_HAsciiString(""));
        ind++;
        // AddWarning (mess," : empty text","");  DEFAULT : rien a dire
        continue;
      }
      AddFail(mess, " : not given as a Text", "");
      return false;
    }
    occ::handle<TCollection_HAsciiString> tval = new TCollection_HAsciiString(FP.CValue());
    // IGESFile_Read a filtre
    int lnt = tval->Length();
    int lnh = tval->Location(1, 'H', 1, lnt);
    if (lnh <= 1 || lnh >= lnt)
    {
      AddFail(mess, " : not in Hollerith Form", "");
      return false;
    }
    else
    {
      int hol = atoi(tval->SubString(1, lnh - 1)->ToCString());
      if (hol != (lnt - lnh))
        AddWarning(mess, " : bad Hollerith count ", "");
    }
    val->SetValue(ind, new TCollection_HAsciiString(tval->SubString(lnh + 1, lnt)->ToCString()));
    ind++;
  }
  return true;
}

// ReadEnts for MoniTool

//=================================================================================================

bool IGESData_ParamReader::ReadEnts(const occ::handle<IGESData_IGESReaderData>& IR,
                                                const IGESData_ParamCursor&            PC,
                                                const Message_Msg&                     amsg,
                                                occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>&  val,
                                                const int                 index)
{
  if (!PrepareRead(PC, true))
    return false;
  if (thenbitem == 0)
    return true; // vide : retour Null ...
  int indmax = index + thenbitem * thetermsz - 1;
  val                     = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(index, indmax);
  int ind    = index;
  int nbnul  = 0;

  int i; // svv Jan11 2000 : porting on DEC
  for (i = FirstRead(); i > 0; i = NextRead())
  {
    int nval;
    if (!ReadingEntityNumber(i, nval))
      nval = 0; // return false;
    if (nval > 0)
    {
      DeclareAndCast(IGESData_IGESEntity, anent, IR->BoundEntity(nval));
      if (anent.IsNull())
        nbnul++;
      else if (IR->DirType(nval).Type() == 0)
        nbnul++;
      else
      {
        val->SetValue(ind, anent);
        ind++;
      }
    }
  }
  if (ind == indmax + 1)
  {
  } // complete array
  else if (ind == index)
    val.Nullify(); // empty array
  else
  {
    // Gaps: they have been eliminated, but the array needs to be resized
    occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tab = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(index, ind - 1);
    for (i = index; i < ind; i++)
      tab->SetValue(i, val->Value(i));
    val = tab;
  }
  if (nbnul > 0)
  {
    SendWarning(amsg);
  }
  return true;
}

//=================================================================================================

bool IGESData_ParamReader::ReadEnts(const occ::handle<IGESData_IGESReaderData>& IR,
                                                const IGESData_ParamCursor&            PC,
                                                const char* const                 mess,
                                                occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>&  val,
                                                const int                 index)
{
  if (!PrepareRead(PC, mess, true))
    return false;
  if (thenbitem == 0)
    return true; // vide : retour Null ...
  int indmax = index + thenbitem * thetermsz - 1;
  val                     = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(index, indmax);
  int ind    = index;
  int nbneg = 0, nbnul = 0;

  int i; // svv Jan11 2000 : porting on DEC
  for (i = FirstRead(); i > 0; i = NextRead())
  {
    int nval;
    if (!ReadingEntityNumber(i, mess, nval))
      nval = 0; // return false;
    if (nval < 0)
      nbneg++;
    if (nval > 0)
    {
      DeclareAndCast(IGESData_IGESEntity, anent, IR->BoundEntity(nval));
      if (anent.IsNull())
        nbnul++;
      else if (IR->DirType(nval).Type() == 0)
        nbnul++;
      else
      {
        val->SetValue(ind, anent);
        ind++;
      }
    }
  }
  if (ind == indmax + 1)
  {
  } // complete array
  else if (ind == index)
    val.Nullify(); // empty array
  else
  {
    // Gaps: they have been eliminated, but the array needs to be resized
    occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tab = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(index, ind - 1);
    for (i = index; i < ind; i++)
      tab->SetValue(i, val->Value(i));
    val = tab;
  }
  //  Messages ?
  char mest[80];
  if (nbneg > 0)
  {
    Sprintf(mest, "Skipped Negative Pointer(s), count %d", nbneg);
    AddWarning(mest, "Skipped Negative Pointer(s), count %d");
  }
  if (nbnul > 0)
  {
    Sprintf(mest, "Skipped Null Type Entity(ies), count %d", nbnul);
    AddWarning(mest, "Skipped Null Type Entity(ies), count %d");
  }
  return true;
}

// ReadEntList for MoniTool

//=================================================================================================

bool IGESData_ParamReader::ReadEntList(const occ::handle<IGESData_IGESReaderData>& IR,
                                                   const IGESData_ParamCursor&            PC,
                                                   Message_Msg&                           amsg,
                                                   Interface_EntityList&                  val,
                                                   const bool                 ord)
{

  if (!PrepareRead(PC, true))
    return false;
  val.Clear();
  if (thenbitem == 0)
    return true; // vide : retour Null ...
  for (int i = FirstRead(); i > 0; i = NextRead())
  {
    int nval;
    if (!ReadingEntityNumber(i, nval))
      return false;
    if (nval < 0)
    {
      Message_Msg Msg219("XSTEP_219");
      amsg.Arg(Msg219.Value());
      SendWarning(amsg);
    }
    if (nval <= 0)
      continue;
    DeclareAndCast(IGESData_IGESEntity, anent, IR->BoundEntity(nval));
    if (anent.IsNull())
    {
      Message_Msg Msg216("XSTEP_216");
      amsg.Arg(Msg216.Value());
      SendWarning(amsg);
    }
    else if (IR->DirType(nval).Type() == 0)
    {
      Message_Msg Msg217("XSTEP_217");
      SendWarning(TCollection_AsciiString(Msg217.Value()).ToCString());
    }
    else if (ord)
      val.Append(anent);
    else
      val.Add(anent);
  }
  return true;
}

//=================================================================================================

bool IGESData_ParamReader::ReadEntList(const occ::handle<IGESData_IGESReaderData>& IR,
                                                   const IGESData_ParamCursor&            PC,
                                                   const char* const                 mess,
                                                   Interface_EntityList&                  val,
                                                   const bool                 ord)
{
  if (!PrepareRead(PC, mess, true))
    return false;
  val.Clear();
  if (thenbitem == 0)
    return true; // vide : retour Null ...
  for (int i = FirstRead(); i > 0; i = NextRead())
  {
    int nval;
    if (!ReadingEntityNumber(i, mess, nval))
      return false;
    if (nval < 0)
      AddWarning(" Negative Pointer, skipped", "");
    if (nval <= 0)
      continue;
    DeclareAndCast(IGESData_IGESEntity, anent, IR->BoundEntity(nval));
    if (anent.IsNull())
      AddWarning(" Null Pointer, skipped", "");
    else if (IR->DirType(nval).Type() == 0)
      AddWarning(" Pointer to IGES Null Entity, skipped", "");
    else if (ord)
      val.Append(anent);
    else
      val.Add(anent);
  }
  return true;
}

// ReadingReal for MoniTool

//=================================================================================================

bool IGESData_ParamReader::ReadingReal(const int num, double& val)
{
  const Interface_FileParameter& FP = theparams->Value(num + thebase);
  if (FP.ParamType() == Interface_ParamInteger)
  {
    if (!pbrealint)
    {
      if (testconv < 0)
        testconv = 0; // Interface_Static::IVal("iges.convert.read");
      if (testconv > 0)
      {
        //   char ssem[100];
        pbrealint = num;
        //   Sprintf(ssem,": Integer converted to Real, 1st rank=%d",num);
        //   AddWarning (mess,ssem,"At least one Integer converted to Real, 1st rank=%d");
      }
    }
    int ival = atoi(FP.CValue());
    val                   = ival;
    return true;
  }
  char             text[50];
  const char* orig = FP.CValue();
  int i, j = 0;
  for (i = 0; i < 50; i++)
  {
    if (orig[i] == 'D' || orig[i] == 'd')
      text[j++] = 'e';
    else
      text[j++] = orig[i];
    if (orig[i] == '\0')
      break;
  }
  if (FP.ParamType() == Interface_ParamReal)
    val = Atof(text);
  else if (FP.ParamType() == Interface_ParamEnum)
  { // convention
    if (!pbrealform)
    {
      if (testconv < 0)
        testconv = 0; // Interface_Static::IVal("iges.convert.read");
      if (testconv > 0)
      {
        // char ssem[100];
        pbrealform = num;
        //  Sprintf(ssem,"Real with no decimal point (added), 1st rank=%d",num);
        //  AddWarning (mess,ssem,"Real with no decimal point (added), 1st rank=%d");
      }
    }
    // By convention (no explicit enum in IGES), means
    // "recognized as floating but not clean" i.e. without decimal point
    // but with exponent (otherwise it would be an integer)
    // -> a warning message + we add the point then convert

    val = Atof(text);
  }
  else if (FP.ParamType() == Interface_ParamVoid)
  {
    val = 0.0; // DEFAULT
  }
  else
  {
    // char ssem[100];
    //  Sprintf(ssem,": not given as Real, rank %d",num);
    //  AddFail (mess,ssem,": not given as Real, rank %d");
    /*  TCollection_AsciiString mess = amsg.Value();
      if ((mess.Search("ter %d"))||(mess.Search("tre %d")))
         amsg.AddInteger(num); // Parameter index
    */
    return false;
  }
  return true;
}

//=================================================================================================

bool IGESData_ParamReader::ReadingReal(const int num,
                                                   const char* const mess,
                                                   double&         val)
{
  const Interface_FileParameter& FP = theparams->Value(num + thebase);
  if (FP.ParamType() == Interface_ParamInteger)
  {
    if (!pbrealint)
    {
      if (testconv < 0)
        testconv = 0; // Interface_Static::IVal("iges.convert.read");
      if (testconv > 0)
      {
        char ssem[100];
        pbrealint = num;
        Sprintf(ssem, ": Integer converted to Real, 1st rank=%d", num);
        AddWarning(mess, ssem, "At least one Integer converted to Real, 1st rank=%d");
      }
    }
    int ival = atoi(FP.CValue());
    val                   = ival;
    return true;
  }
  char             text[50];
  const char* orig = FP.CValue();
  int i, j = 0;
  for (i = 0; i < 50; i++)
  {
    if (orig[i] == 'D' || orig[i] == 'd')
      text[j++] = 'e';
    else
      text[j++] = orig[i];
    if (orig[i] == '\0')
      break;
  }
  if (FP.ParamType() == Interface_ParamReal)
    val = Atof(text);
  else if (FP.ParamType() == Interface_ParamEnum)
  { // convention
    if (!pbrealform)
    {
      if (testconv < 0)
        testconv = 0; // Interface_Static::IVal("iges.convert.read");
      if (testconv > 0)
      {
        char ssem[100];
        pbrealform = num;
        Sprintf(ssem, "Real with no decimal point (added), 1st rank=%d", num);
        AddWarning(mess, ssem, "Real with no decimal point (added), 1st rank=%d");
      }
    }
    // By convention (no explicit enum in IGES), means
    // "recognized as floating but not clean" i.e. without decimal point
    // but with exponent (otherwise it would be an integer)
    // -> a warning message + we add the point then convert

    val = Atof(text);
  }
  else if (FP.ParamType() == Interface_ParamVoid)
  {
    val = 0.0; // DEFAULT
  }
  else
  {
    val = 0.0; // DEFAULT
    char ssem[100];
    Sprintf(ssem, ": not given as Real, rank %d", num);
    AddFail(mess, ssem, ": not given as Real, rank %d");
    return false;
  }
  return true;
}

// ReadingEntityNumber for MoniTool

//=================================================================================================

bool IGESData_ParamReader::ReadingEntityNumber(const int num,
                                                           int&      val)
{
  const Interface_FileParameter& FP = theparams->Value(num + thebase);
  val                               = ParamNumber(num);
  if (val == 0)
  {
    bool nulref = false;
    if (FP.ParamType() == Interface_ParamInteger)
      nulref = (atoi(FP.CValue()) == 0);
    else if (FP.ParamType() == Interface_ParamVoid)
      nulref = true;
    if (!nulref)
    {
      //   AddFail (mess," : cannot refer to an Entity","");
      thelast = true;
      return false;
    }
  }
  return true;
}

//=================================================================================================

bool IGESData_ParamReader::ReadingEntityNumber(const int num,
                                                           const char* const mess,
                                                           int&      val)
{
  const Interface_FileParameter& FP = theparams->Value(num + thebase);
  val                               = ParamNumber(num);
  if (val == 0)
  {
    bool nulref = false;
    if (FP.ParamType() == Interface_ParamInteger)
      nulref = (atoi(FP.CValue()) == 0);
    else if (FP.ParamType() == Interface_ParamVoid)
      nulref = true;
    if (!nulref)
    {
      AddFail(mess, " : cannot refer to an Entity", "");
      thelast = true;
      return false;
    }
  }
  return true;
}

//=================================================================================================

void IGESData_ParamReader::SendFail(const Message_Msg& amsg)
{
  thecheck->SendFail(amsg);
  thelast = false;
}

//=================================================================================================

void IGESData_ParamReader::SendWarning(const Message_Msg& amsg)
{
  thecheck->SendWarning(amsg);
  thelast = false;
}

//  ....              Current reading status management              ....

//=================================================================================================

void IGESData_ParamReader::AddFail(const char* const                  idm,
                                   const occ::handle<TCollection_HAsciiString>& afail,
                                   const occ::handle<TCollection_HAsciiString>& bfail)
{
  afail->Insert(1, idm);
  if (bfail != afail)
    bfail->Insert(1, idm);
  thecheck->AddFail(afail, bfail);
  thelast = false;
}

//=================================================================================================

void IGESData_ParamReader::AddFail(const char* const idm,
                                   const char* const afail,
                                   const char* const bfail)
{
  occ::handle<TCollection_HAsciiString> af = new TCollection_HAsciiString(afail);
  occ::handle<TCollection_HAsciiString> bf = af;
  if (bfail[0] != '\0')
    bf = new TCollection_HAsciiString(bfail);
  AddFail(idm, af, bf);
}

//=================================================================================================

void IGESData_ParamReader::AddWarning(const char* const                  idm,
                                      const occ::handle<TCollection_HAsciiString>& aw,
                                      const occ::handle<TCollection_HAsciiString>& bw)
{
  aw->Insert(1, idm);
  if (bw != aw)
    bw->Insert(1, idm);
  thecheck->AddWarning(aw, bw);
}

//=================================================================================================

void IGESData_ParamReader::AddWarning(const char* const idm,
                                      const char* const awarn,
                                      const char* const bwarn)
{
  occ::handle<TCollection_HAsciiString> aw = new TCollection_HAsciiString(awarn);
  occ::handle<TCollection_HAsciiString> bw = aw;
  if (bwarn[0] != '\0')
    bw = new TCollection_HAsciiString(bwarn);
  AddWarning(idm, aw, bw);
}

//=================================================================================================

void IGESData_ParamReader::AddFail(const char* const afail, const char* const bfail)
{
  thelast = false;
  thecheck->AddFail(afail, bfail);
}

//=================================================================================================

void IGESData_ParamReader::AddFail(const occ::handle<TCollection_HAsciiString>& afail,
                                   const occ::handle<TCollection_HAsciiString>& bfail)
{
  thelast = false;
  thecheck->AddFail(afail, bfail);
}

//=================================================================================================

void IGESData_ParamReader::AddWarning(const char* const amess, const char* const bmess)
{
  thecheck->AddWarning(amess, bmess);
}

//=================================================================================================

void IGESData_ParamReader::AddWarning(const occ::handle<TCollection_HAsciiString>& amess,
                                      const occ::handle<TCollection_HAsciiString>& bmess)
{
  thecheck->AddWarning(amess, bmess);
}

//=================================================================================================

void IGESData_ParamReader::Mend(const char* const pref)
{
  thecheck->Mend(pref);
  thelast = true;
}

//=================================================================================================

bool IGESData_ParamReader::HasFailed() const
{
  return !thelast;
} // thecheck.HasFailed();

//=================================================================================================

const occ::handle<Interface_Check>& IGESData_ParamReader::Check() const
{
  return thecheck;
}

//=================================================================================================

occ::handle<Interface_Check>& IGESData_ParamReader::CCheck()
{
  return thecheck;
}

//=================================================================================================

bool IGESData_ParamReader::IsCheckEmpty() const
{
  return (!thecheck->HasFailed() && !thecheck->HasWarnings());
}
