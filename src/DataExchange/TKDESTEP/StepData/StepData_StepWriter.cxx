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

// List of changes:
// skl 29.01.2003 - deleted one space symbol at the beginning
//                 of strings from Header Section

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_InterfaceMismatch.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ReportEntity.hxx>
#include <Standard_Transient.hxx>
#include <StepData_ESDescr.hxx>
#include <StepData_FieldList.hxx>
#include <StepData_PDescr.hxx>
#include <StepData_Protocol.hxx>
#include <StepData_ReadWriteModule.hxx>
#include <StepData_SelectArrReal.hxx>
#include <StepData_SelectMember.hxx>
#include <StepData_StepModel.hxx>
#include <StepData_StepWriter.hxx>
#include <StepData_UndefinedEntity.hxx>
#include <StepData_WriterLib.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

#include <cstdio>
#define StepLong 72
// StepLong: maximum length of a Step file line

// Literal constants (useful for performance AND MEMORY)

static TCollection_AsciiString textscope(" &SCOPE");
static TCollection_AsciiString textendscope("        ENDSCOPE");
static TCollection_AsciiString textcomm("  /*  ");
static TCollection_AsciiString textendcomm("  */");
static TCollection_AsciiString textlist("(");
static TCollection_AsciiString textendlist(")");
static TCollection_AsciiString textendent(");");
static TCollection_AsciiString textparam(",");
static TCollection_AsciiString textundef("$");
static TCollection_AsciiString textderived("*");
static TCollection_AsciiString texttrue(".T.");
static TCollection_AsciiString textfalse(".F.");
static TCollection_AsciiString textunknown(".U.");

//=================================================================================================

StepData_StepWriter::StepData_StepWriter(const occ::handle<StepData_StepModel>& amodel)
    : thecurr(StepLong),
      thefloatw(12)
{
  themodel   = amodel;
  thelabmode = thetypmode = 0;
  thefile                 = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
  thesect                 = false;
  thefirst                = true;
  themult                 = false;
  thecomm                 = false;
  thelevel = theindval = 0;
  theindent            = false;
  // Floating point format: delegated to FloatWriter
}

//  ....                Float Sending Control                ....

//=================================================================================================

Interface_FloatWriter& StepData_StepWriter::FloatWriter()
{
  return thefloatw;
} // refer to it

//=================================================================================================

int& StepData_StepWriter::LabelMode()
{
  return thelabmode;
}

//=================================================================================================

int& StepData_StepWriter::TypeMode()
{
  return thetypmode;
}

//  ....                Scope Description (BEFORE Sending)               ....

//=================================================================================================

void StepData_StepWriter::SetScope(const int numscope, const int numin)
{
  int nb = themodel->NbEntities();
  if (numscope <= 0 || numscope > nb || numin <= 0 || numin > nb)
    throw Interface_InterfaceMismatch("StepWriter : SetScope, out of range");
  if (thescopenext.IsNull())
  {
    thescopebeg = new NCollection_HArray1<int>(1, nb);
    thescopebeg->Init(0);
    thescopeend = new NCollection_HArray1<int>(1, nb);
    thescopeend->Init(0);
    thescopenext = new NCollection_HArray1<int>(1, nb);
    thescopenext->Init(0);
  }
  else if (thescopenext->Value(numin) != 0)
  {
#ifdef OCCT_DEBUG
    std::cout << "StepWriter : SetScope (scope : " << numscope << " entity : " << numin
              << "), Entity already in a Scope" << std::endl;
#endif
    throw Interface_InterfaceMismatch("StepWriter : SetScope, already set");
  }
  thescopenext->SetValue(numin, -1); // new end of scope
  if (thescopebeg->Value(numscope) == 0)
    thescopebeg->SetValue(numscope, numin);
  int lastin = thescopeend->Value(numscope);
  if (lastin > 0)
    thescopenext->SetValue(lastin, numin);
  thescopeend->SetValue(numscope, numin);
}

//=================================================================================================

bool StepData_StepWriter::IsInScope(const int num) const
{
  if (thescopenext.IsNull())
    return false;
  return (thescopenext->Value(num) != 0);
}

//  ###########################################################################
//  ##    ##    ##    ##        SENDING SECTIONS        ##    ##    ##    ##

//  ....                      Sending Complete Model                      ....

//=================================================================================================

void StepData_StepWriter::SendModel(const occ::handle<StepData_Protocol>& protocol,
                                    const bool                            headeronly)
{
  StepData_WriterLib lib(protocol);

  if (!headeronly)
    thefile->Append(new TCollection_HAsciiString("ISO-10303-21;"));
  SendHeader();

  //  ....                Header: sequence of entities without Ident                ....

  Interface_EntityIterator header = themodel->Header();
  thenum                          = 0;
  for (header.Start(); header.More(); header.Next())
  {
    const occ::handle<Standard_Transient>& anent = header.Value();

    //   Write Entity via Lib  (similaire a SendEntity)
    occ::handle<StepData_ReadWriteModule> module;
    int                                   CN;
    if (lib.Select(anent, module, CN))
    {
      if (module->IsComplex(CN))
        StartComplex();
      else
      {
        TCollection_AsciiString styp;
        if (thetypmode > 0)
          styp = module->ShortType(CN);
        if (styp.Length() == 0)
          styp = module->StepType(CN);
        StartEntity(styp);
      }
      module->WriteStep(CN, *this, anent);
      if (module->IsComplex(CN))
        EndComplex();
    }
    else
    {
      //    Not found above ... try UndefinedEntity
      DeclareAndCast(StepData_UndefinedEntity, und, anent);
      if (und.IsNull())
        continue;
      if (und->IsComplex())
        StartComplex();
      und->WriteParams(*this);
      if (und->IsComplex())
        EndComplex();
    }
    EndEntity();
  }
  EndSec();
  if (headeronly)
    return;

  //  Data: Like Header but with Idents ... otherwise the code is the same
  SendData();

  // ....                    Global Errors (if any)                    ....

  occ::handle<Interface_Check> achglob = themodel->GlobalCheck();
  int                          nbfails = achglob->NbFails();
  if (nbfails > 0)
  {
    Comment(true);
    SendComment("GLOBAL FAIL MESSAGES,  recorded at Read time :");
    for (int ifail = 1; ifail <= nbfails; ifail++)
    {
      SendComment(achglob->Fail(ifail));
    }
    Comment(false);
    NewLine(false);
  }

  //  ....                Output Entities one by one                ....

  int nb = themodel->NbEntities();
  for (int i = 1; i <= nb; i++)
  {
    //    Main list: we don't send Entities that are in a Scope
    //    They will be sent through the Scope that contains them
    if (!thescopebeg.IsNull())
    {
      if (thescopenext->Value(i) != 0)
        continue;
    }
    SendEntity(i, lib);
  }

  EndSec();
  EndFile();
}

//  ....                FILE DIVISION INTO SECTIONS                ....

//=================================================================================================

void StepData_StepWriter::SendHeader()
{
  NewLine(false);
  thefile->Append(new TCollection_HAsciiString("HEADER;"));
  thesect = true;
}

//=================================================================================================

void StepData_StepWriter::SendData()
{
  if (thesect)
    throw Interface_InterfaceMismatch("StepWriter : Data section");
  NewLine(false);
  thefile->Append(new TCollection_HAsciiString("DATA;"));
  thesect = true;
}

//=================================================================================================

void StepData_StepWriter::EndSec()
{
  thefile->Append(new TCollection_HAsciiString("ENDSEC;"));
  thesect = false;
}

//=================================================================================================

void StepData_StepWriter::EndFile()
{
  if (thesect)
    throw Interface_InterfaceMismatch("StepWriter : EndFile");
  NewLine(false);
  thefile->Append(new TCollection_HAsciiString("END-ISO-10303-21;"));
  thesect = false;
}

//  ....                        SENDING AN ENTITY                        ....

//=================================================================================================

void StepData_StepWriter::SendEntity(const int num, const StepData_WriterLib& lib)
{
  char                            lident[20];
  occ::handle<Standard_Transient> anent = themodel->Entity(num);
  int                             idnum = num, idtrue = 0;

  //   themodel->Number(anent) and/or IdentLabel(anent)
  if (thelabmode > 0)
    idtrue = themodel->IdentLabel(anent);
  if (thelabmode == 1)
    idnum = idtrue;
  if (idnum == 0)
    idnum = num;
  if (thelabmode < 2 || idnum == idtrue)
    Sprintf(lident, "#%d = ", idnum); // skl 29.01.2003
  else
    Sprintf(lident, "%d:#%d = ", idnum, idtrue); // skl 29.01.2003

  //  SendIdent reused, lident has just been calculated
  thecurr.Clear();
  thecurr.Add(lident);
  themult = false;

  //  ....        Processing of Potential Scope
  if (!thescopebeg.IsNull())
  {
    int numin = thescopebeg->Value(num);
    if (numin != 0)
    {
      SendScope();
      for (int nument = numin; numin > 0; nument = numin)
      {
        SendEntity(nument, lib);
        numin = thescopenext->Value(nument);
      }
      SendEndscope();
    }
  }

  //  ....        Sending the Entity proper

  //   Write Entity via Lib
  thenum = num;
  occ::handle<StepData_ReadWriteModule> module;
  int                                   CN;
  if (themodel->IsRedefinedContent(num))
  {
    //    Error Entity: Write the Content + Errors as Comments
    occ::handle<Interface_ReportEntity> rep = themodel->ReportEntity(num);
    DeclareAndCast(StepData_UndefinedEntity, und, rep->Content());
    if (und.IsNull())
    {
      thechecks.CCheck(num)->AddFail("Erroneous Entity, Content lost");
      StartEntity(TCollection_AsciiString("!?LOST_DATA"));
    }
    else
    {
      thechecks.CCheck(num)->AddWarning("Erroneous Entity, equivalent content");
      if (und->IsComplex())
        AddString(" (", 2);
      und->WriteParams(*this);
      if (und->IsComplex())
      {
        AddString(") ", 2);
      } // thelevel --; }
    }
    EndEntity(); // BEFORE Comments
    NewLine(false);
    Comment(true);
    if (und.IsNull())
      SendComment("   ERRONEOUS ENTITY, DATA LOST");
    SendComment("On Entity above, Fail Messages recorded at Read time :");
    occ::handle<Interface_Check> ach     = rep->Check();
    int                          nbfails = ach->NbFails();
    for (int ifail = 1; ifail <= nbfails; ifail++)
    {
      SendComment(ach->Fail(ifail));
    }
    Comment(false);
    NewLine(false);

    //    Normal case
  }
  else if (lib.Select(anent, module, CN))
  {
    if (module->IsComplex(CN))
      StartComplex();
    else
    {
      TCollection_AsciiString styp;
      if (thetypmode > 0)
        styp = module->ShortType(CN);
      if (styp.Length() == 0)
        styp = module->StepType(CN);
      StartEntity(styp);
    }
    module->WriteStep(CN, *this, anent);
    if (module->IsComplex(CN))
      EndComplex();
    EndEntity();
  }
  else
  {
    //    Not found above ... try UndefinedEntity
    DeclareAndCast(StepData_UndefinedEntity, und, anent);
    if (und.IsNull())
      return;
    if (und->IsComplex())
      StartComplex();
    und->WriteParams(*this);
    if (und->IsComplex())
      EndComplex();
    EndEntity();
  }
}

//  ###########################################################################
//  ##    ##    ##        TEXT CONSTITUTION FOR SENDING        ##    ##    ##

//  Go to next line. Empty line not counted unless evenempty == true

//=================================================================================================

void StepData_StepWriter::NewLine(const bool evenempty)
{
  if (evenempty || thecurr.Length() > 0)
  {
    thefile->Append(thecurr.Moved());
  }
  int indst = thelevel * 2;
  if (theindent)
    indst += theindval;
  thecurr.SetInitial(indst);
  thecurr.Clear();
}

//  Group current line with previous; remains current except if newline
//  == true, in which case we start a new line
//  Does nothing if: corresponding total > StepLong or start or end of entity

//=================================================================================================

void StepData_StepWriter::JoinLast(const bool)
{
  thecurr.SetKeep();
}

//=================================================================================================

void StepData_StepWriter::Indent(const bool onent)
{
  theindent = onent;
}

//=================================================================================================

void StepData_StepWriter::SendIdent(const int ident)
{
  char lident[12];
  Sprintf(lident, "#%d =", ident);
  thecurr.Clear();
  thecurr.Add(lident);
  themult = false;
}

//=================================================================================================

void StepData_StepWriter::SendScope()
{
  AddString(textscope);
}

//=================================================================================================

void StepData_StepWriter::SendEndscope()
{
  NewLine(false);
  thefile->Append(new TCollection_HAsciiString(textendscope));
}

//=================================================================================================

void StepData_StepWriter::Comment(const bool mode)
{
  if (mode && !thecomm)
    AddString(textcomm, 20);
  if (!mode && thecomm)
    AddString(textendcomm);
  thecomm = mode;
}

//=================================================================================================

void StepData_StepWriter::SendComment(const occ::handle<TCollection_HAsciiString>& text)
{
  if (!thecomm)
    throw Interface_InterfaceMismatch("StepWriter : Comment");
  AddString(text->ToCString(), text->Length());
}

//=================================================================================================

void StepData_StepWriter::SendComment(const char* const text)
{
  if (!thecomm)
    throw Interface_InterfaceMismatch("StepWriter : Comment");
  AddString(text, (int)strlen(text));
}

//=================================================================================================

void StepData_StepWriter::StartEntity(const TCollection_AsciiString& atype)
{
  if (atype.Length() == 0)
    return;
  if (themult)
  {
    // clang-format off
    if (thelevel != 1) throw Interface_InterfaceMismatch("StepWriter : StartEntity");   // bad parentheses count...
    // clang-format on
    AddString(textendlist);
    AddString(" ", 1); // skl 29.01.2003
  }
  themult = true;
  // AddString(" ",1);  //skl 29.01.2003
  AddString(atype);
  thelevel  = 0;
  theindval = thecurr.Length();
  thecurr.SetInitial(0);
  thefirst = true;
  OpenSub();
}

//=================================================================================================

void StepData_StepWriter::StartComplex()
{
  AddString("( ", 2); // skl 29.01.2003
} // thelevel unchanged

//=================================================================================================

void StepData_StepWriter::EndComplex()
{
  AddString(") ", 2);
} // thelevel unchanged

//  ....                SendField and what goes with it

//=================================================================================================

void StepData_StepWriter::SendField(const StepData_Field&               fild,
                                    const occ::handle<StepData_PDescr>& descr)
{
  bool done = true;
  int  kind = fild.Kind(false); // valeur interne

  if (kind == 16)
  {
    DeclareAndCast(StepData_SelectMember, sm, fild.Transient());
    SendSelect(sm, descr);
    return;
  }
  switch (kind)
  {
      //   here the simple cases; then we cast and see
    case 0:
      SendUndef();
      break;
    case 1:
      Send(fild.Integer());
      break;
    case 2:
      SendBoolean(fild.Boolean());
      break;
    case 3:
      SendLogical(fild.Logical());
      break;
    case 4:
      SendEnum(fild.EnumText());
      break; // enum : descr ?
    case 5:
      Send(fild.Real());
      break;
    case 6:
      Send(fild.String());
      break;
    case 7:
      Send(fild.Entity());
      break;
    case 8:
      done = false;
      break;
    case 9:
      SendDerived();
      break;
    default:
      done = false;
      break;
  }
  if (done)
    return;

  //  What remains: the arrays ...
  int arity = fild.Arity();
  if (arity == 0)
  {
    SendUndef();
    return;
  } // PAS NORMAL
  if (arity == 1)
  {
    OpenSub();
    int i, low = fild.Lower(), up = low + fild.Length() - 1;
    for (i = low; i <= up; i++)
    {
      kind = fild.ItemKind(i);
      done = true;
      switch (kind)
      {
        case 0:
          SendUndef();
          break;
        case 1:
          Send(fild.Integer(i));
          break;
        case 2:
          SendBoolean(fild.Boolean(i));
          break;
        case 3:
          SendLogical(fild.Logical(i));
          break;
        case 4:
          SendEnum(fild.EnumText(i));
          break;
        case 5:
          Send(fild.Real(i));
          break;
        case 6:
          Send(fild.String(i));
          break;
        case 7:
          Send(fild.Entity(i));
          break;
        default:
          SendUndef();
          done = false;
          break; // ANORMAL
      }
    }
    CloseSub();
    return;
  }
  if (arity == 2)
  {
    OpenSub();
    int j, low1 = fild.Lower(1), up1 = low1 + fild.Length(1) - 1;
    for (j = low1; j <= up1; j++)
    {
      int i = 0, low2 = fild.Lower(2), up2 = low2 + fild.Length(2) - 1;
      OpenSub();
      for (i = low2; i <= up2; i++)
      {
        kind = fild.ItemKind(i, j);
        done = true;
        switch (kind)
        {
          case 0:
            SendUndef();
            break;
          case 1:
            Send(fild.Integer(i, j));
            break;
          case 2:
            SendBoolean(fild.Boolean(i, j));
            break;
          case 3:
            SendLogical(fild.Logical(i, j));
            break;
          case 4:
            SendEnum(fild.EnumText(i, j));
            break;
          case 5:
            Send(fild.Real(i, j));
            break;
          case 6:
            Send(fild.String(i, j));
            break;
          case 7:
            Send(fild.Entity(i, j));
            break;
          default:
            SendUndef();
            done = false;
            break; // ANORMAL
        }
      }
      CloseSub();
    }
    CloseSub();
    return;
  }
}

//=================================================================================================

void StepData_StepWriter::SendSelect(const occ::handle<StepData_SelectMember>& sm,
                                     const occ::handle<StepData_PDescr>& /*descr*/)
{
  //    SelectMember case. Process the Select then the value
  //    NB: current processing non-recursive (no SELNAME(SELNAME(..)) )
  bool selname = false;
  if (sm.IsNull())
    return; // ??
  if (sm->HasName())
  {
    selname = true;
    //    SendString (sm->Name());
    //    AddString(textlist);     // SANS AJOUT DE PARAMETRE !!
    OpenTypedSub(sm->Name());
  }
  int kind = sm->Kind();
  switch (kind)
  {
    case 0:
      SendUndef();
      break;
    case 1:
      Send(sm->Integer());
      break;
    case 2:
      SendBoolean(sm->Boolean());
      break;
    case 3:
      SendLogical(sm->Logical());
      break;
    case 4:
      SendEnum(sm->EnumText());
      break; // enum : descr ?
    case 5:
      Send(sm->Real());
      break;
    case 6:
      Send(sm->String());
      break;
    case 8:
      SendArrReal(occ::down_cast<StepData_SelectArrReal>(sm)->ArrReal());
      break;
    default:
      break; // ??
  }
  if (selname)
    CloseSub();
}

//=================================================================================================

void StepData_StepWriter::SendList(const StepData_FieldList&            list,
                                   const occ::handle<StepData_ESDescr>& descr)
{
  // start entity  ?
  int i, nb = list.NbFields();
  for (i = 1; i <= nb; i++)
  {
    occ::handle<StepData_PDescr> pde;
    if (!descr.IsNull())
      pde = descr->Field(i);
    const StepData_Field& fild = list.Field(i);
    SendField(fild, pde);
  }
  // end entity  ?
}

//  ....                Basic Send* methods

//=================================================================================================

void StepData_StepWriter::OpenSub()
{
  AddParam();
  AddString(textlist);
  thefirst = true;
  thelevel++;
}

//=================================================================================================

void StepData_StepWriter::OpenTypedSub(const char* const subtype)
{
  AddParam();
  if (subtype[0] != '\0')
    AddString(subtype, (int)strlen(subtype));
  AddString(textlist);
  thefirst = true;
  thelevel++;
}

//=================================================================================================

void StepData_StepWriter::CloseSub()
{
  AddString(textendlist);
  thefirst = false; // the parameter following a sub-list is therefore not first
  thelevel--;
}

//=================================================================================================

void StepData_StepWriter::AddParam()
{
  if (!thefirst)
    AddString(textparam);
  thefirst = false;
}

//=================================================================================================

void StepData_StepWriter::Send(const int val)
{
  char lval[12];
  AddParam();
  Sprintf(lval, "%d", val);
  AddString(lval, (int)strlen(lval));
}

//=================================================================================================

void StepData_StepWriter::Send(const double val)
{
  //    Floating point value, cleaned of trailing "0000" and "E+00"
  char lval[24] = {};
  int  lng      = thefloatw.Write(val, lval);
  AddParam();
  AddString(lval, lng); // handles specific format: if needed
}

//  Send(String): note, we send a Text ... so between '  '

//=================================================================================================

void StepData_StepWriter::Send(const TCollection_AsciiString& val)
{
  AddParam();
  // Use helper function to clean text while preserving control directives
  TCollection_AsciiString aVal = CleanTextForSend(val);
  int                     aNn  = aVal.Length();

  // Add surrounding quotes
  aVal.Insert(1, '\'');
  aVal.AssignCat('\'');
  aNn += 2;

  //: i2 abv 31 Aug 98: ProSTEP TR9: avoid wrapping text or do it at spaces

  //    Watch out for exceeding 72 characters
  if (thecurr.CanGet(aNn))
    AddString(aVal, 0);
  //: i2
  else
  {
    thefile->Append(thecurr.Moved());
    int anIndst = thelevel * 2;
    if (theindent)
      anIndst += theindval;
    if (anIndst + aNn <= StepLong)
      thecurr.SetInitial(anIndst);
    else
      thecurr.SetInitial(0);
    if (thecurr.CanGet(aNn))
      AddString(aVal, 0);
    else
    {
      while (aNn > 0)
      {
        if (aNn <= StepLong)
        {
          thecurr.Add(aVal); // That's it, we've exhausted everything
          thecurr.FreezeInitial();
          break;
        }
        int aStop = StepLong; // position of last separator
        for (; aStop > 0 && aVal.Value(aStop) != ' '; aStop--)
          ;
        if (!aStop)
        {
          aStop = StepLong;
          for (; aStop > 0 && aVal.Value(aStop) != '\\'; aStop--)
            ;
          if (!aStop)
          {
            aStop = StepLong;
            for (; aStop > 0 && aVal.Value(aStop) != '_'; aStop--)
              ;
            if (!aStop)
              aStop = StepLong;
          }
        }
        TCollection_AsciiString aBval = aVal.Split(aStop);
        thefile->Append(new TCollection_HAsciiString(aVal));
        aVal = aBval;
        aNn -= aStop;
      }
    }
  }
}

//=================================================================================================

void StepData_StepWriter::Send(const occ::handle<Standard_Transient>& val)
{
  char lident[20];
  //  Undefined ?
  if (val.IsNull())
  {
    //   throw Interface_InterfaceMismatch("StepWriter : Sending Null Reference");
    thechecks.CCheck(thenum)->AddFail("Null Reference");
    SendUndef();
    Comment(true);
    SendComment(" NUL REF ");
    Comment(false);
    return;
  }
  int num = themodel->Number(val);
  //  String? (if not listed in the Model)
  if (num == 0)
  {
    if (val->IsKind(STANDARD_TYPE(TCollection_HAsciiString)))
    {
      DeclareAndCast(TCollection_HAsciiString, strval, val);
      Send(TCollection_AsciiString(strval->ToCString()));
      return;
    }
    //  SelectMember? (always, if not listed)
    //  but beware, no description attached
    else if (val->IsKind(STANDARD_TYPE(StepData_SelectMember)))
    {
      DeclareAndCast(StepData_SelectMember, sm, val);
      occ::handle<StepData_PDescr> descr; // null
      SendSelect(sm, descr);
    }
    //  Otherwise, NOT NORMAL!
    else
    {
      thechecks.CCheck(thenum)->AddFail("UnknownReference");
      SendUndef();
      Comment(true);
      SendComment(" UNKNOWN REF ");
      Comment(false);
      //      throw Interface_InterfaceMismatch("StepWriter : Sending Unknown Reference");
    }
  }
  //  Normal case: a good Entity, we send its Ident.
  else
  {
    int idnum = num, idtrue = 0;
    if (thelabmode > 0)
      idtrue = themodel->IdentLabel(val);
    if (thelabmode == 1)
      idnum = idtrue;
    if (idnum == 0)
      idnum = num;
    if (thelabmode < 2 || idnum == idtrue)
      Sprintf(lident, "#%d", idnum);
    else
      Sprintf(lident, "%d:#%d", idnum, idtrue);
    AddParam();
    AddString(lident, (int)strlen(lident));
  }
}

//=================================================================================================

void StepData_StepWriter::SendBoolean(const bool val)
{
  if (val)
    SendString(texttrue);
  else
    SendString(textfalse);
}

//=================================================================================================

void StepData_StepWriter::SendLogical(const StepData_Logical val)
{
  if (val == StepData_LTrue)
    SendString(texttrue);
  else if (val == StepData_LFalse)
    SendString(textfalse);
  else
    SendString(textunknown);
}

//  SendString: note, we give the exact label

//=================================================================================================

void StepData_StepWriter::SendString(const TCollection_AsciiString& val)
{
  AddParam();
  AddString(val);
}

//  SendString: note, we give the exact label

//=================================================================================================

void StepData_StepWriter::SendString(const char* const val)
{
  AddParam();
  AddString(val, (int)strlen(val));
}

//  SendEnum: note, we send an Enum label ... so between .  .

//=================================================================================================

void StepData_StepWriter::SendEnum(const TCollection_AsciiString& val)
{
  if (val.Length() == 1 && val.Value(1) == '$')
  {
    SendUndef();
    return;
  }
  AddParam();
  TCollection_AsciiString aValue = val;
  if (aValue.Value(1) != '.')
    aValue.Prepend('.');
  if (aValue.Value(aValue.Length()) != '.')
    aValue += '.';
  AddString(aValue, 2);
}

//  SendEnum: note, we send an Enum label ... so between .  .

//=================================================================================================

void StepData_StepWriter::SendEnum(const char* const val)
{

  if (val[0] == '$' && val[1] == '\0')
  {
    SendUndef();
    return;
  }
  TCollection_AsciiString aValue(val);
  SendEnum(aValue);
}

//=================================================================================================

void StepData_StepWriter::SendArrReal(const occ::handle<NCollection_HArray1<double>>& anArr)
{
  AddString(textlist);
  if (anArr->Length() > 0)
  {
    // add real
    Send(anArr->Value(1));
    for (int i = 2; i <= anArr->Length(); i++)
    {
      //      AddString(textparam);
      // add real
      Send(anArr->Value(i));
    }
  }
  AddString(textendlist);
}

//=================================================================================================

void StepData_StepWriter::SendUndef()
{
  AddParam();
  AddString(textundef);
}

//=================================================================================================

void StepData_StepWriter::SendDerived()
{
  AddParam();
  AddString(textderived);
}

// EndEntity: if we need to put ; on the line, align it with start of entity ...

//=================================================================================================

void StepData_StepWriter::EndEntity()
{
  // clang-format off
  if (thelevel != 1) throw Interface_InterfaceMismatch("StepWriter : EndEntity");   // parentheses count is wrong ...
  // clang-format on
  AddString(textendent);
  thelevel    = 0; // on garde theindval : sera traite au prochain NewLine
  bool indent = theindent;
  theindent   = false;
  NewLine(false);
  theindent = indent;
  themult   = false;
  // to force indentation if necessary
}

//  current line management (see also NewLine/JoinLine)

//=================================================================================================

void StepData_StepWriter::AddString(const TCollection_AsciiString& astr, const int more)
{
  while (!thecurr.CanGet(astr.Length() + more))
  {
    thefile->Append(thecurr.Moved());
    int indst = thelevel * 2;
    if (theindent)
      indst += theindval;
    thecurr.SetInitial(indst);
  }
  thecurr.Add(astr);
}

//=================================================================================================

void StepData_StepWriter::AddString(const char* const astr, const int lnstr, const int more)
{
  while (!thecurr.CanGet(lnstr + more))
  {
    thefile->Append(thecurr.Moved());
    int indst = thelevel * 2;
    if (theindent)
      indst += theindval;
    thecurr.SetInitial(indst);
  }
  thecurr.Add(astr, lnstr);
}

//   FINAL SENDING

//=================================================================================================

Interface_CheckIterator StepData_StepWriter::CheckList() const
{
  return thechecks;
}

//=================================================================================================

int StepData_StepWriter::NbLines() const
{
  return thefile->Length();
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepData_StepWriter::Line(const int num) const
{
  return thefile->Value(num);
}

//=================================================================================================

bool StepData_StepWriter::Print(Standard_OStream& S)
{
  bool isGood = (S.good());
  int  nb     = thefile->Length();
  for (int i = 1; i <= nb && isGood; i++)
    S << thefile->Value(i)->ToCString() << "\n";

  S << std::flush;
  isGood = (S && S.good());

  return isGood;
}

//=================================================================================================

TCollection_AsciiString StepData_StepWriter::CleanTextForSend(
  const TCollection_AsciiString& theText)
{
  TCollection_AsciiString aResult;
  const int               aNb = theText.Length();

  // Process characters from beginning to end
  for (int anI = 1; anI <= aNb; anI++)
  {
    const char anUncar = theText.Value(anI);

    // Check if we're at the start of a control directive
    bool anIsDirective    = false;
    int  aDirectiveLength = 0;

    if (anUncar == '\\' && anI <= aNb)
    {

      // Check for \X2\ and \X4\ patterns first (need exactly 4 characters: \X2\)
      if (anI + 3 <= aNb && theText.Value(anI + 1) == 'X' && theText.Value(anI + 3) == '\\')
      {
        const char aThirdChar = theText.Value(anI + 2);

        // \X2, \X4, \X0 patterns - special control sequences
        if (aThirdChar == '2' || aThirdChar == '4' || aThirdChar == '0')
        {
          anIsDirective    = true;
          aDirectiveLength = 4; // Basic directive length: \X2\, \X4\, \X0\ (4 chars)

          // For \X2 and \X4, find the terminating \X0 sequence
          if (aThirdChar == '2' || aThirdChar == '4')
          {
            int aJ = anI + 4;
            while (aJ <= aNb - 3)
            {
              if (theText.Value(aJ) == '\\' && theText.Value(aJ + 1) == 'X'
                  && theText.Value(aJ + 2) == '0' && theText.Value(aJ + 3) == '\\')
              {
                aDirectiveLength = (aJ + 4) - anI; // Include the \X0 sequence
                break;
              }
              aJ++;
            }
          }
        }
      }
      // Check for \X{HH}\ pattern (need exactly 5 characters: \X{HH}\)
      else if (anI + 4 <= aNb && theText.Value(anI + 1) == 'X' && theText.Value(anI + 4) == '\\')
      {
        const char aThirdChar  = theText.Value(anI + 2);
        const char aFourthChar = theText.Value(anI + 3);

        // Regular \X{HH}\ pattern - check for two hex characters
        if (std::isxdigit(aThirdChar) && std::isxdigit(aFourthChar))
        {
          anIsDirective    = true;
          aDirectiveLength = 5; // Control directive with two hex chars
        }
      }
      // Check for \S, \N, \T patterns (need exactly 3 characters: \S\)
      else if (anI + 2 <= aNb && theText.Value(anI + 2) == '\\')
      {
        const char aSecondChar = theText.Value(anI + 1);
        if (aSecondChar == 'S' || aSecondChar == 'N' || aSecondChar == 'T')
        {
          anIsDirective    = true;
          aDirectiveLength = 3; // Simple directive pattern
        }
      }
      // Check for \P{char}\ patterns (need exactly 4 characters: \P{char}\)
      else if (anI + 3 <= aNb && theText.Value(anI + 1) == 'P' && theText.Value(anI + 3) == '\\')
      {
        const char aSecondChar = theText.Value(anI + 2);
        if (std::isalpha(aSecondChar))
        {
          anIsDirective    = true;
          aDirectiveLength = 4; // P directive with parameter
        }
      }
    }

    if (anIsDirective)
    {
      // Copy the entire directive as-is
      for (int aJ = 0; aJ < aDirectiveLength; aJ++)
      {
        aResult += theText.Value(anI + aJ);
      }
      anI += aDirectiveLength - 1; // Move past directive (loop will increment by 1)
    }
    else
    {
      // Process non-directive characters
      if (anUncar == '\'')
      {
        aResult += "''"; // Double the quote
      }
      else if (anUncar == '\\')
      {
        aResult += "\\\\"; // Double the backslash
      }
      else if (anUncar == '\n')
      {
        aResult += "\\N\\"; // Convert to directive
      }
      else if (anUncar == '\t')
      {
        aResult += "\\T\\"; // Convert to directive
      }
      else
      {
        aResult += anUncar; // Copy as-is
      }
    }
  }

  return aResult;
}
