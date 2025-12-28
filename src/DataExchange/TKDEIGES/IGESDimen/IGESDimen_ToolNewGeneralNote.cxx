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

#include <gp_XYZ.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDimen_NewGeneralNote.hxx>
#include <IGESDimen_ToolNewGeneralNote.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <gp_XYZ.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

#include <stdio.h>

IGESDimen_ToolNewGeneralNote::IGESDimen_ToolNewGeneralNote() {}

void IGESDimen_ToolNewGeneralNote::ReadOwnParams(const occ::handle<IGESDimen_NewGeneralNote>& ent,
                                                 const occ::handle<IGESData_IGESReaderData>&  IR,
                                                 IGESData_ParamReader&                   PR) const
{
  // bool st; //szv#4:S4163:12Mar99 moved down

  int                        nbval;
  double                           width;
  double                           height;
  int                        justifyCode;
  gp_XYZ                                  areaLoc;
  double                           areaRotationAngle;
  gp_XYZ                                  baseLinePos;
  double                           normalInterlineSpace;
  occ::handle<NCollection_HArray1<int>>        charDisplays;
  occ::handle<NCollection_HArray1<double>>           charWidths;
  occ::handle<NCollection_HArray1<double>>           charHeights;
  occ::handle<NCollection_HArray1<double>>           interCharSpaces;
  occ::handle<NCollection_HArray1<double>>           interlineSpaces;
  occ::handle<NCollection_HArray1<int>>        fontStyles;
  occ::handle<NCollection_HArray1<double>>           charAngles;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> controlCodeStrings;
  occ::handle<NCollection_HArray1<int>>        nbChars;
  occ::handle<NCollection_HArray1<double>>           boxWidths;
  occ::handle<NCollection_HArray1<double>>           boxHeights;
  occ::handle<NCollection_HArray1<int>>        charSetCodes;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>    charSetEntities;
  occ::handle<NCollection_HArray1<double>>           slantAngles;
  occ::handle<NCollection_HArray1<double>>           rotationAngles;
  occ::handle<NCollection_HArray1<int>>        mirrorFlags;
  occ::handle<NCollection_HArray1<int>>        rotateFlags;
  occ::handle<NCollection_HArray1<gp_XYZ>>             startPoints;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> texts;

  // szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadReal(PR.Current(), "Text Width", width);
  PR.ReadReal(PR.Current(), "Text Height", height);
  PR.ReadInteger(PR.Current(), "Justification Code", justifyCode);
  PR.ReadXYZ(PR.CurrentList(1, 3), "Area Location Point", areaLoc);
  PR.ReadReal(PR.Current(), "Area Rotation Angle", areaRotationAngle);
  PR.ReadXYZ(PR.CurrentList(1, 3), "Base Line Position", baseLinePos);
  PR.ReadReal(PR.Current(), "NormalInterline Spacing", normalInterlineSpace);

  bool st = PR.ReadInteger(PR.Current(), "Number of Text Strings", nbval);
  if (st && nbval > 0)
  {
    charDisplays       = new NCollection_HArray1<int>(1, nbval);
    charWidths         = new NCollection_HArray1<double>(1, nbval);
    charHeights        = new NCollection_HArray1<double>(1, nbval);
    interCharSpaces    = new NCollection_HArray1<double>(1, nbval);
    interlineSpaces    = new NCollection_HArray1<double>(1, nbval);
    fontStyles         = new NCollection_HArray1<int>(1, nbval);
    charAngles         = new NCollection_HArray1<double>(1, nbval);
    controlCodeStrings = new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, nbval);
    nbChars            = new NCollection_HArray1<int>(1, nbval);
    boxWidths          = new NCollection_HArray1<double>(1, nbval);
    boxHeights         = new NCollection_HArray1<double>(1, nbval);
    charSetCodes       = new NCollection_HArray1<int>(1, nbval);
    charSetEntities    = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nbval);
    slantAngles        = new NCollection_HArray1<double>(1, nbval);
    rotationAngles     = new NCollection_HArray1<double>(1, nbval);
    mirrorFlags        = new NCollection_HArray1<int>(1, nbval);
    rotateFlags        = new NCollection_HArray1<int>(1, nbval);
    startPoints        = new NCollection_HArray1<gp_XYZ>(1, nbval);
    texts              = new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, nbval);
  }
  else
    PR.AddFail("Number of Text Strings: Not Positive");

  if (!charDisplays.IsNull())
    for (int i = 1; i <= nbval; i++)
    {
      int                 charDisplay;
      double                    charWidth;
      double                    charHeight;
      double                    interCharSpace;
      double                    interlineSpace;
      int                 fontStyle;
      double                    charAngle;
      occ::handle<TCollection_HAsciiString> controlCodeString;
      int                 nbChar;
      double                    boxWidth;
      double                    boxHeight;
      int                 charSetCode;
      occ::handle<IGESData_IGESEntity>      charSetEntity;
      double                    slantAngle;
      double                    rotationAngle;
      int                 mirrorFlag;
      int                 rotateFlag;
      gp_XYZ                           startPoint;
      occ::handle<TCollection_HAsciiString> text;

      // st = PR.ReadInteger(PR.Current(), "Character Display",charDisplay); //szv#4:S4163:12Mar99
      // moved in if
      if (PR.ReadInteger(PR.Current(), "Character Display", charDisplay))
        charDisplays->SetValue(i, charDisplay);

      // st = PR.ReadReal(PR.Current(), "Character Width", charWidth); //szv#4:S4163:12Mar99 moved
      // in if
      if (PR.ReadReal(PR.Current(), "Character Width", charWidth))
        charWidths->SetValue(i, charWidth);

      // st = PR.ReadReal(PR.Current(), "Character Height", charHeight); //szv#4:S4163:12Mar99 moved
      // in if
      if (PR.ReadReal(PR.Current(), "Character Height", charHeight))
        charHeights->SetValue(i, charHeight);

      // st = PR.ReadReal(PR.Current(), "Inter-character space", interCharSpace);
      // //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Inter-character space", interCharSpace))
        interCharSpaces->SetValue(i, interCharSpace);

      // st = PR.ReadReal(PR.Current(), "Interline space", interlineSpace); //szv#4:S4163:12Mar99
      // moved in if
      if (PR.ReadReal(PR.Current(), "Interline space", interlineSpace))
        interlineSpaces->SetValue(i, interlineSpace);

      // st = PR.ReadInteger(PR.Current(), "Font Style", fontStyle); //szv#4:S4163:12Mar99 moved in
      // if
      if (PR.ReadInteger(PR.Current(), "Font Style", fontStyle))
        fontStyles->SetValue(i, fontStyle);

      // st = PR.ReadReal(PR.Current(), "Character Angle", charAngle); //szv#4:S4163:12Mar99 moved
      // in if
      if (PR.ReadReal(PR.Current(), "Character Angle", charAngle))
        charAngles->SetValue(i, charAngle);

      // st = PR.ReadText(PR.Current(),"Control Code String", controlCodeString);
      // //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadText(PR.Current(), "Control Code String", controlCodeString))
        controlCodeStrings->SetValue(i, controlCodeString);

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
	    PR.ReadInteger(PR.Current(), "Character Set Interpretation Code",charSetCode); //szv#4:S4163:12Mar99 `st=` not needed
	    // Reading charSetEnt
        // clang-format on
        if (charSetCode < 0)
        {
          charSetEntity = PR.ParamEntity(IR, curnum);
          if (charSetEntity.IsNull())
            PR.AddFail("Char Set Interpretation Entity : incorrect reference");
          charSetEntities->SetValue(i, charSetEntity);
          charSetCodes->SetValue(i, -1);
        }
        else
        {
          charSetCodes->SetValue(i, charSetCode);
        }
      }

      else
        charSetCodes->SetValue(i, 1);

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

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(width,
            height,
            justifyCode,
            areaLoc,
            areaRotationAngle,
            baseLinePos,
            normalInterlineSpace,
            charDisplays,
            charWidths,
            charHeights,
            interCharSpaces,
            interlineSpaces,
            fontStyles,
            charAngles,
            controlCodeStrings,
            nbChars,
            boxWidths,
            boxHeights,
            charSetCodes,
            charSetEntities,
            slantAngles,
            rotationAngles,
            mirrorFlags,
            rotateFlags,
            startPoints,
            texts);
}

void IGESDimen_ToolNewGeneralNote::WriteOwnParams(const occ::handle<IGESDimen_NewGeneralNote>& ent,
                                                  IGESData_IGESWriter&                    IW) const
{
  IW.Send(ent->TextWidth());
  IW.Send(ent->TextHeight());
  IW.Send(ent->JustifyCode());
  IW.Send(ent->AreaLocation().X());
  IW.Send(ent->AreaLocation().Y());
  IW.Send(ent->AreaLocation().Z());
  IW.Send(ent->AreaRotationAngle());
  IW.Send(ent->BaseLinePosition().X());
  IW.Send(ent->BaseLinePosition().Y());
  IW.Send(ent->BaseLinePosition().Z());
  IW.Send(ent->NormalInterlineSpace());
  int nbval = ent->NbStrings();
  IW.Send(nbval);

  for (int i = 1; i <= nbval; i++)
  {
    IW.Send(ent->CharacterDisplay(i));
    IW.Send(ent->CharacterWidth(i));
    IW.Send(ent->CharacterHeight(i));
    IW.Send(ent->InterCharacterSpace(i));
    IW.Send(ent->InterlineSpace(i));
    IW.Send(ent->FontStyle(i));
    IW.Send(ent->CharacterAngle(i));
    IW.Send(ent->ControlCodeString(i));
    IW.Send(ent->NbCharacters(i));
    IW.Send(ent->BoxWidth(i));
    IW.Send(ent->BoxHeight(i));
    if (ent->IsCharSetEntity(i))
      IW.Send(ent->CharSetEntity(i), true); // negative
    else
      IW.Send(ent->CharSetCode(i));
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

void IGESDimen_ToolNewGeneralNote::OwnShared(const occ::handle<IGESDimen_NewGeneralNote>& ent,
                                             Interface_EntityIterator&               iter) const
{
  int nbval = ent->NbStrings();
  for (int i = 1; i <= nbval; i++)
  {

    if (ent->IsCharSetEntity(i))
      iter.GetOneItem(ent->CharSetEntity(i));
  }
}

void IGESDimen_ToolNewGeneralNote::OwnCopy(const occ::handle<IGESDimen_NewGeneralNote>& another,
                                           const occ::handle<IGESDimen_NewGeneralNote>& ent,
                                           Interface_CopyTool&                     TC) const
{
  double    width                = another->TextWidth();
  double    height               = another->TextHeight();
  int justifyCode          = another->JustifyCode();
  gp_XYZ           areaLoc              = (another->AreaLocation()).XYZ();
  double    areaRotationAngle    = another->AreaRotationAngle();
  gp_XYZ           baseLinePos          = (another->BaseLinePosition()).XYZ();
  double    normalInterlineSpace = another->NormalInterlineSpace();
  int nbval                = another->NbStrings();

  occ::handle<NCollection_HArray1<int>>        charDisplays;
  occ::handle<NCollection_HArray1<double>>           charWidths;
  occ::handle<NCollection_HArray1<double>>           charHeights;
  occ::handle<NCollection_HArray1<double>>           interCharSpaces;
  occ::handle<NCollection_HArray1<double>>           interlineSpaces;
  occ::handle<NCollection_HArray1<int>>        fontStyles;
  occ::handle<NCollection_HArray1<double>>           charAngles;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> controlCodeStrings;
  occ::handle<NCollection_HArray1<int>>        nbChars;
  occ::handle<NCollection_HArray1<double>>           boxWidths;
  occ::handle<NCollection_HArray1<double>>           boxHeights;
  occ::handle<NCollection_HArray1<int>>        charSetCodes;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>    charSetEntities;
  occ::handle<NCollection_HArray1<double>>           slantAngles;
  occ::handle<NCollection_HArray1<double>>           rotationAngles;
  occ::handle<NCollection_HArray1<int>>        mirrorFlags;
  occ::handle<NCollection_HArray1<int>>        rotateFlags;
  occ::handle<NCollection_HArray1<gp_XYZ>>             startPoints;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> texts;

  charDisplays       = new NCollection_HArray1<int>(1, nbval);
  charWidths         = new NCollection_HArray1<double>(1, nbval);
  charHeights        = new NCollection_HArray1<double>(1, nbval);
  interCharSpaces    = new NCollection_HArray1<double>(1, nbval);
  interlineSpaces    = new NCollection_HArray1<double>(1, nbval);
  fontStyles         = new NCollection_HArray1<int>(1, nbval);
  charAngles         = new NCollection_HArray1<double>(1, nbval);
  controlCodeStrings = new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, nbval);
  nbChars            = new NCollection_HArray1<int>(1, nbval);
  boxWidths          = new NCollection_HArray1<double>(1, nbval);
  boxHeights         = new NCollection_HArray1<double>(1, nbval);
  charSetCodes       = new NCollection_HArray1<int>(1, nbval);
  charSetEntities    = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nbval);
  slantAngles        = new NCollection_HArray1<double>(1, nbval);
  rotationAngles     = new NCollection_HArray1<double>(1, nbval);
  mirrorFlags        = new NCollection_HArray1<int>(1, nbval);
  rotateFlags        = new NCollection_HArray1<int>(1, nbval);
  startPoints        = new NCollection_HArray1<gp_XYZ>(1, nbval);
  texts              = new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, nbval);

  for (int i = 1; i <= nbval; i++)
  {
    charDisplays->SetValue(i, another->CharacterDisplay(i));
    charWidths->SetValue(i, another->CharacterWidth(i));
    charHeights->SetValue(i, another->CharacterHeight(i));
    interCharSpaces->SetValue(i, another->InterCharacterSpace(i));
    interlineSpaces->SetValue(i, another->InterlineSpace(i));
    fontStyles->SetValue(i, another->FontStyle(i));
    charAngles->SetValue(i, another->CharacterAngle(i));
    controlCodeStrings->SetValue(i, new TCollection_HAsciiString(another->ControlCodeString(i)));
    nbChars->SetValue(i, another->NbCharacters(i));
    boxWidths->SetValue(i, another->BoxWidth(i));
    boxHeights->SetValue(i, another->BoxHeight(i));

    if (another->IsCharSetEntity(i))
    {
      DeclareAndCast(IGESData_IGESEntity, charSetEntity, TC.Transferred(another->CharSetEntity(i)));
      charSetEntities->SetValue(i, charSetEntity);
    }
    else
      charSetCodes->SetValue(i, another->CharSetCode(i));

    slantAngles->SetValue(i, another->SlantAngle(i));
    rotationAngles->SetValue(i, another->RotationAngle(i));
    mirrorFlags->SetValue(i, another->MirrorFlag(i));
    rotateFlags->SetValue(i, another->RotateFlag(i));
    startPoints->SetValue(i, another->StartPoint(i).XYZ());
    texts->SetValue(i, new TCollection_HAsciiString(another->Text(i)));
  }

  ent->Init(width,
            height,
            justifyCode,
            areaLoc,
            areaRotationAngle,
            baseLinePos,
            normalInterlineSpace,
            charDisplays,
            charWidths,
            charHeights,
            interCharSpaces,
            interlineSpaces,
            fontStyles,
            charAngles,
            controlCodeStrings,
            nbChars,
            boxWidths,
            boxHeights,
            charSetCodes,
            charSetEntities,
            slantAngles,
            rotationAngles,
            mirrorFlags,
            rotateFlags,
            startPoints,
            texts);
}

IGESData_DirChecker IGESDimen_ToolNewGeneralNote::DirChecker(
  const occ::handle<IGESDimen_NewGeneralNote>& /* ent */) const
{
  IGESData_DirChecker DC(213, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefValue);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.UseFlagRequired(1);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESDimen_ToolNewGeneralNote::OwnCheck(const occ::handle<IGESDimen_NewGeneralNote>& ent,
                                            const Interface_ShareTool&,
                                            occ::handle<Interface_Check>& ach) const
{

  int jcode = ent->JustifyCode();
  if ((jcode < 0) || (jcode > 3))
    ach->AddFail("Justify Code != 0, 1, 2, 3");
  int upper = ent->NbStrings();
  for (int i = 1; i <= upper; i++)
  {
    if (ent->NbCharacters(i) != ent->Text(i)->Length())
    {
      char mess[80];
      Sprintf(mess, "%d : Number of Characters != Length of Text String", i);
      ach->AddFail(mess);
    }

    int charcode = ent->CharSetCode(i);
    if ((charcode >= 0) && (charcode != 1) && ((charcode < 1001) || (charcode > 1003)))
    {
      char mess[80];
      Sprintf(mess, "%d : Character Set Code != 1, 1001, 1002, 1003", i);
      ach->AddFail(mess);
    }

    int chardisp = ent->CharacterDisplay(i);
    if ((chardisp < 0) || (chardisp > 1))
    {
      char mess[80];
      Sprintf(mess, "%d : Character Display != 0, 1", i);
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

void IGESDimen_ToolNewGeneralNote::OwnDump(const occ::handle<IGESDimen_NewGeneralNote>& ent,
                                           const IGESData_IGESDumper&              dumper,
                                           Standard_OStream&                       S,
                                           const int                  level) const
{
  int sublevel = (level > 4) ? 1 : 0;
  int nbval    = ent->NbStrings();

  S << "IGESDimen_NewGeneralNote\n"
    << "Text Area  : Width : " << ent->TextWidth() << "  "
    << "Height : " << ent->TextHeight() << "  "
    << "Justification Code  : " << ent->JustifyCode() << "\n"
    << "Text Area Location Point : ";
  IGESData_DumpXYZL(S, level, ent->AreaLocation(), ent->Location());
  S << "Rotation Angle of Text : " << ent->AreaRotationAngle() << "\n"
    << "Base Line Position : ";
  IGESData_DumpXYZL(S, level, ent->BaseLinePosition(), ent->Location());
  S << "Normal Interline Spacing : " << ent->NormalInterlineSpace() << "\n"
    << "Number of Text Strings : " << nbval << "\n";

  S << "Character Display :\n"
    << "Character Width :\n"
    << "Character Height :\n"
    << "Inter Character Spacing :\n"
    << "Interline Spacing :\n"
    << "Font Styles :\n"
    << "Character Angle :\n"
    << "Control Code String :\n"
    << "Number of Characters :\n"
    << "Box Widths :\n"
    << "Box Heights :\n"
    << "Character Set Codes :\n"
    << "Character Set Entities :\n"
    << "Slant Angles :\n"
    << "Rotation Angles :\n"
    << "Mirror Flags :\n"
    << "Rotate Flags :\n"
    << "Start Points :\n"
    << "Texts : ";
  IGESData_DumpVals(S, -level, 1, nbval, ent->NbCharacters);
  S << "\n";
  if (level > 4)
  {
    S << "Details of each String\n";
    for (int i = 1; i <= nbval; i++)
    {
      S << "[" << i << "]:\n"
        << "Character Display : " << ent->CharacterDisplay(i) << "  "
        << "Character Width   : " << ent->CharacterWidth(i) << "  "
        << "Character Height  : " << ent->CharacterHeight(i) << "\n"
        << "Inter Character Spacing : " << ent->InterCharacterSpace(i) << "  "
        << "Interline Spacing : " << ent->InterlineSpace(i) << "\n"
        << "Font Styles       : " << ent->FontStyle(i) << "\n"
        << "Character Angle   : " << ent->CharacterAngle(i) << "\n"
        << "Control Code String : ";
      IGESData_DumpString(S, ent->ControlCodeString(i));
      S << "\n"
        << "Number of Characters : " << ent->NbCharacters(i) << "  "
        << "Box Width  : " << ent->BoxWidth(i) << "  "
        << "Box Height : " << ent->BoxHeight(i) << "\n";
      if (ent->IsCharSetEntity(i))
      {
        S << "Character Set Entity : ";
        dumper.Dump(ent->CharSetEntity(i), S, sublevel);
        S << "\n";
      }
      else
        S << "Character Set Code : " << ent->CharSetCode(i) << "\n"

          << "Slant Angle : " << ent->SlantAngle(i) << "  "
          << "Rotation Angle : " << ent->RotationAngle(i) << "  "
          << "Mirror Flag : " << ent->MirrorFlag(i) << "  "
          << "Rotate Flag : " << ent->RotateFlag(i) << "\n"
          << "Start Point : ";
      IGESData_DumpXYZL(S, level, ent->StartPoint(i), ent->Location());
      S << "Text : ";
      IGESData_DumpString(S, ent->Text(i));
      S << "\n";
    }
  }
  else
    S << std::endl;
}
