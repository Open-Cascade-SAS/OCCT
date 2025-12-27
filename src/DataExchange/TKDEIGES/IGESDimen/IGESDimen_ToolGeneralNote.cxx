// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_ToolGeneralNote.hxx>
#include <IGESGraph_HArray1OfTextFontDef.hxx>
#include <IGESGraph_TextFontDef.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TColgp_HArray1OfXYZ.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

#include <stdio.h>

IGESDimen_ToolGeneralNote::IGESDimen_ToolGeneralNote() {}

void IGESDimen_ToolGeneralNote::ReadOwnParams(const occ::handle<IGESDimen_GeneralNote>&   ent,
                                              const occ::handle<IGESData_IGESReaderData>& IR,
                                              IGESData_ParamReader&                  PR) const
{
  // bool st; //szv#4:S4163:12Mar99 moved down

  int                        nbval;
  occ::handle<TColStd_HArray1OfInteger>        nbChars;
  occ::handle<TColStd_HArray1OfReal>           boxWidths;
  occ::handle<TColStd_HArray1OfReal>           boxHeights;
  occ::handle<TColStd_HArray1OfInteger>        fontCodes;
  occ::handle<IGESGraph_HArray1OfTextFontDef>  fontEntities;
  occ::handle<TColStd_HArray1OfReal>           slantAngles;
  occ::handle<TColStd_HArray1OfReal>           rotationAngles;
  occ::handle<TColStd_HArray1OfInteger>        mirrorFlags;
  occ::handle<TColStd_HArray1OfInteger>        rotateFlags;
  occ::handle<TColgp_HArray1OfXYZ>             startPoints;
  occ::handle<Interface_HArray1OfHAsciiString> texts;

  bool st = PR.ReadInteger(PR.Current(), "Number of Text Strings", nbval);
  if (st && nbval > 0)
  {
    nbChars        = new TColStd_HArray1OfInteger(1, nbval);
    boxWidths      = new TColStd_HArray1OfReal(1, nbval);
    boxHeights     = new TColStd_HArray1OfReal(1, nbval);
    fontCodes      = new TColStd_HArray1OfInteger(1, nbval);
    fontEntities   = new IGESGraph_HArray1OfTextFontDef(1, nbval);
    slantAngles    = new TColStd_HArray1OfReal(1, nbval);
    rotationAngles = new TColStd_HArray1OfReal(1, nbval);
    mirrorFlags    = new TColStd_HArray1OfInteger(1, nbval);
    rotateFlags    = new TColStd_HArray1OfInteger(1, nbval);
    startPoints    = new TColgp_HArray1OfXYZ(1, nbval);
    texts          = new Interface_HArray1OfHAsciiString(1, nbval);
  }
  else
    PR.AddFail("Number of Text Strings: Not Positive");

  if (!nbChars.IsNull())
  {
    for (int i = 1; i <= nbval; i++)
    {
      int                 nbChar;
      double                    boxWidth;
      double                    boxHeight;
      int                 fontCode;
      occ::handle<IGESGraph_TextFontDef>    fontEntity;
      double                    slantAngle;
      double                    rotationAngle;
      int                 mirrorFlag;
      int                 rotateFlag;
      gp_XYZ                           startPoint;
      occ::handle<TCollection_HAsciiString> text;

      // st = PR.ReadInteger(PR.Current(), "Number of Characters", nbChar); //szv#4:S4163:12Mar99
      // moved in if
      if (PR.ReadInteger(PR.Current(), "Number of Characters", nbChar))
        nbChars->SetValue(i, nbChar);

      // st = PR.ReadReal(PR.Current(), "Box Width", boxWidth); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Box Width", boxWidth))
        boxWidths->SetValue(i, boxWidth);

      // st = PR.ReadReal(PR.Current(), "Box Height", boxHeight); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Box Height", boxHeight))
        boxHeights->SetValue(i, boxHeight);

      int curnum = PR.CurrentNumber();
      if (PR.DefinedElseSkip())
      {
        // Reading fontCode(Integer, must be positive)
        // clang-format off
	      PR.ReadInteger (PR.Current(), "Font Code", fontCode); //szv#4:S4163:12Mar99 `st=` not needed
	      // Reading fontEnt(TextFontDef) ?
        // clang-format on
        if (fontCode < 0)
        {
          fontEntity = GetCasted(IGESGraph_TextFontDef, PR.ParamEntity(IR, curnum));
          if (fontEntity.IsNull())
            PR.AddFail("Font Entity : incorrect reference");
          fontEntities->SetValue(i, fontEntity);
          fontCodes->SetValue(i, -1);
        }
        else
          fontCodes->SetValue(i, fontCode);
      }
      else
      {
        fontCodes->SetValue(i, 1);
      }

      if (PR.DefinedElseSkip())
      {
        // st = PR.ReadReal(PR.Current(), "Slant Angle", slantAngle); //szv#4:S4163:12Mar99 moved in
        // if
        if (PR.ReadReal(PR.Current(), "Slant Angle", slantAngle))
          slantAngles->SetValue(i, slantAngle);
      }
      else
        slantAngles->SetValue(i, M_PI / 2);

      // st = PR.ReadReal(PR.Current(), "Rotation Angle", rotationAngle); //szv#4:S4163:12Mar99
      // moved in if
      if (PR.ReadReal(PR.Current(), "Rotation Angle", rotationAngle))
        rotationAngles->SetValue(i, rotationAngle);

      // st = PR.ReadInteger(PR.Current(), "Mirror Flag", mirrorFlag); //szv#4:S4163:12Mar99 moved
      // in if
      if (PR.ReadInteger(PR.Current(), "Mirror Flag", mirrorFlag))
        mirrorFlags->SetValue(i, mirrorFlag);

      // st = PR.ReadInteger(PR.Current(), "Rotate Flag", rotateFlag); //szv#4:S4163:12Mar99 moved
      // in if
      if (PR.ReadInteger(PR.Current(), "Rotate Flag", rotateFlag))
        rotateFlags->SetValue(i, rotateFlag);

      // st = PR.ReadXYZ(PR.CurrentList(1, 3), "Start Point", startPoint); //szv#4:S4163:12Mar99
      // moved in if
      if (PR.ReadXYZ(PR.CurrentList(1, 3), "Start Point", startPoint))
        startPoints->SetValue(i, startPoint);

      // st = PR.ReadText(PR.Current(), "Text String", text); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadText(PR.Current(), "Text String", text))
        texts->SetValue(i, text);
    }
  }
  // sln 28.09.2001, BUC61004, If the condition is false function ent->Init is not called in order
  // to avoid exception
  if (!(nbChars.IsNull() || boxWidths.IsNull() || boxHeights.IsNull() || fontCodes.IsNull()
        || fontEntities.IsNull() || slantAngles.IsNull() || rotationAngles.IsNull()
        || mirrorFlags.IsNull() || rotateFlags.IsNull() || startPoints.IsNull() || texts.IsNull()))
  {
    DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
    ent->Init(nbChars,
              boxWidths,
              boxHeights,
              fontCodes,
              fontEntities,
              slantAngles,
              rotationAngles,
              mirrorFlags,
              rotateFlags,
              startPoints,
              texts);
  }
}

void IGESDimen_ToolGeneralNote::WriteOwnParams(const occ::handle<IGESDimen_GeneralNote>& ent,
                                               IGESData_IGESWriter&                 IW) const
{
  int nbval = ent->NbStrings();
  IW.Send(nbval);

  for (int i = 1; i <= nbval; i++)
  {
    IW.Send(ent->NbCharacters(i));
    IW.Send(ent->BoxWidth(i));
    IW.Send(ent->BoxHeight(i));
    if (ent->IsFontEntity(i))
      IW.Send(ent->FontEntity(i), true); // negative
    else
      IW.Send(ent->FontCode(i));
    IW.Send(ent->SlantAngle(i));
    IW.Send(ent->RotationAngle(i));
    IW.Send(ent->MirrorFlag(i));
    IW.Send(ent->RotateFlag(i));
    IW.Send((ent->StartPoint(i)).X());
    IW.Send((ent->StartPoint(i)).Y());
    IW.Send((ent->StartPoint(i)).Z());
    IW.Send(ent->Text(i));
  }
}

void IGESDimen_ToolGeneralNote::OwnShared(const occ::handle<IGESDimen_GeneralNote>& ent,
                                          Interface_EntityIterator&            iter) const
{
  int nbval = ent->NbStrings();
  for (int i = 1; i <= nbval; i++)
  {
    if (ent->IsFontEntity(i))
      iter.GetOneItem(ent->FontEntity(i));
  }
}

void IGESDimen_ToolGeneralNote::OwnCopy(const occ::handle<IGESDimen_GeneralNote>& another,
                                        const occ::handle<IGESDimen_GeneralNote>& ent,
                                        Interface_CopyTool&                  TC) const
{
  int nbval = another->NbStrings();

  occ::handle<TColStd_HArray1OfInteger>        nbChars;
  occ::handle<TColStd_HArray1OfReal>           boxWidths;
  occ::handle<TColStd_HArray1OfReal>           boxHeights;
  occ::handle<TColStd_HArray1OfInteger>        fontCodes;
  occ::handle<IGESGraph_HArray1OfTextFontDef>  fontEntities;
  occ::handle<TColStd_HArray1OfReal>           slantAngles;
  occ::handle<TColStd_HArray1OfReal>           rotationAngles;
  occ::handle<TColStd_HArray1OfInteger>        mirrorFlags;
  occ::handle<TColStd_HArray1OfInteger>        rotateFlags;
  occ::handle<TColgp_HArray1OfXYZ>             startPoints;
  occ::handle<Interface_HArray1OfHAsciiString> texts;

  nbChars        = new TColStd_HArray1OfInteger(1, nbval);
  boxWidths      = new TColStd_HArray1OfReal(1, nbval);
  boxHeights     = new TColStd_HArray1OfReal(1, nbval);
  fontCodes      = new TColStd_HArray1OfInteger(1, nbval);
  fontEntities   = new IGESGraph_HArray1OfTextFontDef(1, nbval);
  slantAngles    = new TColStd_HArray1OfReal(1, nbval);
  rotationAngles = new TColStd_HArray1OfReal(1, nbval);
  mirrorFlags    = new TColStd_HArray1OfInteger(1, nbval);
  rotateFlags    = new TColStd_HArray1OfInteger(1, nbval);
  startPoints    = new TColgp_HArray1OfXYZ(1, nbval);
  texts          = new Interface_HArray1OfHAsciiString(1, nbval);

  for (int i = 1; i <= nbval; i++)
  {
    int nbChar = another->NbCharacters(i);
    nbChars->SetValue(i, nbChar);
    double boxWidth = another->BoxWidth(i);
    boxWidths->SetValue(i, boxWidth);
    double boxHeight = another->BoxHeight(i);
    boxHeights->SetValue(i, boxHeight);

    if (another->IsFontEntity(i))
    {
      DeclareAndCast(IGESGraph_TextFontDef, fontEntity, TC.Transferred(another->FontEntity(i)));
      fontEntities->SetValue(i, fontEntity);
      fontCodes->SetValue(i, -1);
    }
    else
    {
      int fontCode = another->FontCode(i);
      fontCodes->SetValue(i, fontCode);
      ////          fontEntities->SetValue(i, NULL);    by default
    }

    double slantAngle = another->SlantAngle(i);
    slantAngles->SetValue(i, slantAngle);
    double rotationAngle = another->RotationAngle(i);
    rotationAngles->SetValue(i, rotationAngle);
    int mirrorFlag = another->MirrorFlag(i);
    mirrorFlags->SetValue(i, mirrorFlag);
    int rotateFlag = another->RotateFlag(i);
    rotateFlags->SetValue(i, rotateFlag);
    gp_XYZ startPoint = (another->StartPoint(i)).XYZ();
    startPoints->SetValue(i, startPoint);
    occ::handle<TCollection_HAsciiString> text = new TCollection_HAsciiString(another->Text(i));
    texts->SetValue(i, text);
  }

  ent->Init(nbChars,
            boxWidths,
            boxHeights,
            fontCodes,
            fontEntities,
            slantAngles,
            rotationAngles,
            mirrorFlags,
            rotateFlags,
            startPoints,
            texts);
  ent->SetFormNumber(another->FormNumber());
}

IGESData_DirChecker IGESDimen_ToolGeneralNote::DirChecker(
  const occ::handle<IGESDimen_GeneralNote>& /* ent */) const
{
  IGESData_DirChecker DC(212, 0, 105);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefValue);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.UseFlagRequired(1);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESDimen_ToolGeneralNote::OwnCheck(const occ::handle<IGESDimen_GeneralNote>& ent,
                                         const Interface_ShareTool&,
                                         occ::handle<Interface_Check>& ach) const
{
  if (((ent->FormNumber() < 0) || (ent->FormNumber() > 8))
      && ((ent->FormNumber() < 100) || (ent->FormNumber() > 102)) && (ent->FormNumber() != 105))
    ach->AddFail("Form Number: Not Valid");

  int upper = ent->NbStrings();
  for (int i = 1; i <= upper; i++)
  {
    if (ent->NbCharacters(i) != ent->Text(i)->Length())
    {
      char mess[80];
      Sprintf(mess, "%d : Number of Characters != Length of Text String", i);
      ach->AddFail(mess);
    }
    int mflag = ent->MirrorFlag(i);
    if ((mflag < 0) || (mflag > 2))
    {
      char mess[80];
      Sprintf(mess, "%d : Mirror flag != 0, 1, 2", i);
      ach->AddFail(mess);
    }
    int rflag = ent->RotateFlag(i);
    if ((rflag < 0) || (rflag > 1))
    {
      char mess[80];
      Sprintf(mess, "%d : Rotate flag != 0, 1", i);
      ach->AddFail(mess);
    }
  }
}

void IGESDimen_ToolGeneralNote::OwnDump(const occ::handle<IGESDimen_GeneralNote>& ent,
                                        const IGESData_IGESDumper&           dumper,
                                        Standard_OStream&                    S,
                                        const int               level) const
{
  int sublevel = (level > 4) ? 1 : 0;
  int upper    = ent->NbStrings();

  S << "IGESDimen_GeneralNote\n"
    << "Number of Text Strings : " << upper << "\n"
    << "Number of Characters :\n"
    << "Box Widths :\n"
    << "Box Heights :\n"
    << "Font Codes :\n"
    << "Font Entities :\n"
    << "Slant Angles :\n"
    << "Rotation Angles :\n"
    << "Mirror Flags :\n"
    << "Rotate Flags :\n"
    << "Start Points :\n"
    << "Texts : ";
  IGESData_DumpVals(S, -level, 1, ent->NbStrings(), ent->NbCharacters);
  S << "\n";
  if (level > 4)
  {
    S << "Details of each String\n";
    for (int i = 1; i <= upper; i++)
    {
      S << "[" << i << "]:\n"
        << "Number of Characters : " << ent->NbCharacters(i) << "  "
        << "Box Width  : " << ent->BoxWidth(i) << "  "
        << "Box Height : " << ent->BoxHeight(i) << "\n";
      if (ent->IsFontEntity(i))
      {
        S << "Font Entity : ";
        dumper.Dump(ent->FontEntity(i), S, sublevel);
        S << "\n";
      }
      else
        S << "Font Code : " << ent->FontCode(i) << "\n"
          << "Slant Angle : " << ent->SlantAngle(i) << "  "
          << "Rotation Angle : " << ent->RotationAngle(i) << "  "
          << "Mirror Flag : " << ent->MirrorFlag(i) << "  "
          << "Rotate Flag : " << ent->RotateFlag(i) << "\n"
          << "Start Point : ";
      IGESData_DumpXYZL(S, level, ent->StartPoint(i), ent->Location());
      S << "\nText : ";
      IGESData_DumpString(S, ent->Text(i));
      S << "\n";
    }
  }
  S << std::endl;
}
