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
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_ToolGeneralNote.hxx>
#include <IGESGraph_HArray1OfTextFontDef.hxx>
#include <IGESGraph_TextFontDef.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TColgp_HArray1OfXYZ.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

#include <stdio.h>
IGESDimen_ToolGeneralNote::IGESDimen_ToolGeneralNote ()    {  }


void  IGESDimen_ToolGeneralNote::ReadOwnParams
  (const Handle(IGESDimen_GeneralNote)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{ 
  //Standard_Boolean st; //szv#4:S4163:12Mar99 moved down

  Standard_Integer nbval;
  Handle(TColStd_HArray1OfInteger) nbChars;
  Handle(TColStd_HArray1OfReal) boxWidths; 
  Handle(TColStd_HArray1OfReal) boxHeights;
  Handle(TColStd_HArray1OfInteger) fontCodes; 
  Handle(IGESGraph_HArray1OfTextFontDef) fontEntities;
  Handle(TColStd_HArray1OfReal) slantAngles; 
  Handle(TColStd_HArray1OfReal) rotationAngles;
  Handle(TColStd_HArray1OfInteger) mirrorFlags; 
  Handle(TColStd_HArray1OfInteger) rotateFlags;
  Handle(TColgp_HArray1OfXYZ) startPoints; 
  Handle(Interface_HArray1OfHAsciiString) texts; 

  Standard_Boolean st = PR.ReadInteger(PR.Current(), "Number of Text Strings", nbval);
  if (st && nbval > 0) {
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
  else  PR.AddFail("Number of Text Strings: Not Positive");


  if (! nbChars.IsNull())
    {
      for (Standard_Integer i = 1; i <= nbval; i++)
	{
          Standard_Integer nbChar;
          Standard_Real boxWidth;
          Standard_Real boxHeight;
          Standard_Integer fontCode;
          Handle(IGESGraph_TextFontDef) fontEntity;
          Standard_Real slantAngle;
          Standard_Real rotationAngle;
          Standard_Integer mirrorFlag;
          Standard_Integer rotateFlag;
          gp_XYZ startPoint;
          Handle(TCollection_HAsciiString) text;

          //st = PR.ReadInteger(PR.Current(), "Number of Characters", nbChar); //szv#4:S4163:12Mar99 moved in if
	  if (PR.ReadInteger(PR.Current(), "Number of Characters", nbChar))
	    nbChars->SetValue(i, nbChar);

          //st = PR.ReadReal(PR.Current(), "Box Width", boxWidth); //szv#4:S4163:12Mar99 moved in if
	  if (PR.ReadReal(PR.Current(), "Box Width", boxWidth))
	    boxWidths->SetValue(i, boxWidth);

          //st = PR.ReadReal(PR.Current(), "Box Height", boxHeight); //szv#4:S4163:12Mar99 moved in if
	  if (PR.ReadReal(PR.Current(), "Box Height", boxHeight))
	    boxHeights->SetValue(i, boxHeight);

	  Standard_Integer curnum = PR.CurrentNumber();
	  if (PR.DefinedElseSkip())
	    {
	      // Reading fontCode(Integer, must be positive)
	      PR.ReadInteger (PR.Current(), "Font Code", fontCode); //szv#4:S4163:12Mar99 `st=` not needed
	      // Reading fontEnt(TextFontDef) ?
	      if (fontCode < 0) {
		fontEntity = GetCasted(IGESGraph_TextFontDef,PR.ParamEntity (IR,curnum));
		if (fontEntity.IsNull()) PR.AddFail ("Font Entity : incorrect reference");
		fontEntities->SetValue(i, fontEntity);
		fontCodes->SetValue(i, -1);
	      }
	      else fontCodes->SetValue(i, fontCode);
	    }
          else
	    {
              fontCodes->SetValue(i, 1);
	    }

          if (PR.DefinedElseSkip())
	    {
              //st = PR.ReadReal(PR.Current(), "Slant Angle", slantAngle); //szv#4:S4163:12Mar99 moved in if
	      if (PR.ReadReal(PR.Current(), "Slant Angle", slantAngle))
		slantAngles->SetValue(i, slantAngle);
	    }
          else
              slantAngles->SetValue(i, M_PI/2);

          //st = PR.ReadReal(PR.Current(), "Rotation Angle", rotationAngle); //szv#4:S4163:12Mar99 moved in if
	  if (PR.ReadReal(PR.Current(), "Rotation Angle", rotationAngle))
	    rotationAngles->SetValue(i, rotationAngle);

          //st = PR.ReadInteger(PR.Current(), "Mirror Flag", mirrorFlag); //szv#4:S4163:12Mar99 moved in if
	  if (PR.ReadInteger(PR.Current(), "Mirror Flag", mirrorFlag))
	    mirrorFlags->SetValue(i, mirrorFlag);

          //st = PR.ReadInteger(PR.Current(), "Rotate Flag", rotateFlag); //szv#4:S4163:12Mar99 moved in if
	  if (PR.ReadInteger(PR.Current(), "Rotate Flag", rotateFlag))
	    rotateFlags->SetValue(i, rotateFlag);

          //st = PR.ReadXYZ(PR.CurrentList(1, 3), "Start Point", startPoint); //szv#4:S4163:12Mar99 moved in if
	  if (PR.ReadXYZ(PR.CurrentList(1, 3), "Start Point", startPoint))
	    startPoints->SetValue(i, startPoint);

          //st = PR.ReadText(PR.Current(), "Text String", text); //szv#4:S4163:12Mar99 moved in if
	  if (PR.ReadText(PR.Current(), "Text String", text))
	    texts->SetValue(i, text);
	}
    }
  //sln 28.09.2001, BUC61004, If the condition is false function ent->Init is not called in order to avoid exception
  if(!(nbChars.IsNull() || boxWidths.IsNull() || boxHeights.IsNull() || fontCodes.IsNull() || 
       fontEntities.IsNull() ||  slantAngles.IsNull() || rotationAngles.IsNull() || 
       mirrorFlags.IsNull() || rotateFlags.IsNull() || startPoints.IsNull() || texts.IsNull()))
    {
      DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
      ent->Init
        (nbChars, boxWidths, boxHeights, fontCodes, fontEntities,
         slantAngles, rotationAngles, mirrorFlags, rotateFlags,
         startPoints, texts);
    }
}

void  IGESDimen_ToolGeneralNote::WriteOwnParams
  (const Handle(IGESDimen_GeneralNote)& ent, IGESData_IGESWriter& IW) const 
{ 
  Standard_Integer nbval = ent->NbStrings();
  IW.Send(nbval);

  for (Standard_Integer i = 1; i <= nbval; i++)
    {
      IW.Send(ent->NbCharacters(i));
      IW.Send(ent->BoxWidth(i));
      IW.Send(ent->BoxHeight(i));
      if (ent->IsFontEntity(i))
	IW.Send(ent->FontEntity(i),Standard_True);  // negative
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

void  IGESDimen_ToolGeneralNote::OwnShared
  (const Handle(IGESDimen_GeneralNote)& ent, Interface_EntityIterator& iter) const
{
  Standard_Integer nbval = ent->NbStrings();
  for (Standard_Integer i = 1; i <= nbval; i++) {
    if (ent->IsFontEntity(i))
      iter.GetOneItem(ent->FontEntity(i));
  }
}

void  IGESDimen_ToolGeneralNote::OwnCopy
  (const Handle(IGESDimen_GeneralNote)& another,
   const Handle(IGESDimen_GeneralNote)& ent, Interface_CopyTool& TC) const
{
  Standard_Integer nbval = another->NbStrings();

  Handle(TColStd_HArray1OfInteger) nbChars;
  Handle(TColStd_HArray1OfReal) boxWidths; 
  Handle(TColStd_HArray1OfReal) boxHeights;
  Handle(TColStd_HArray1OfInteger) fontCodes; 
  Handle(IGESGraph_HArray1OfTextFontDef) fontEntities;
  Handle(TColStd_HArray1OfReal) slantAngles; 
  Handle(TColStd_HArray1OfReal) rotationAngles;
  Handle(TColStd_HArray1OfInteger) mirrorFlags; 
  Handle(TColStd_HArray1OfInteger) rotateFlags;
  Handle(TColgp_HArray1OfXYZ) startPoints; 
  Handle(Interface_HArray1OfHAsciiString) texts; 

  nbChars        = new TColStd_HArray1OfInteger(1, nbval);
  boxWidths      = new TColStd_HArray1OfReal   (1, nbval);
  boxHeights     = new TColStd_HArray1OfReal   (1, nbval);
  fontCodes      = new TColStd_HArray1OfInteger(1, nbval);
  fontEntities   = new IGESGraph_HArray1OfTextFontDef(1, nbval);
  slantAngles    = new TColStd_HArray1OfReal   (1, nbval);
  rotationAngles = new TColStd_HArray1OfReal   (1, nbval);
  mirrorFlags    = new TColStd_HArray1OfInteger(1, nbval);
  rotateFlags    = new TColStd_HArray1OfInteger(1, nbval);
  startPoints    = new TColgp_HArray1OfXYZ     (1, nbval);
  texts          = new Interface_HArray1OfHAsciiString(1, nbval);

  for (Standard_Integer i = 1; i <= nbval; i++)
    {
      Standard_Integer nbChar = another->NbCharacters(i);
      nbChars->SetValue(i, nbChar);
      Standard_Real boxWidth = another->BoxWidth(i);
      boxWidths->SetValue(i, boxWidth);
      Standard_Real boxHeight = another->BoxHeight(i);
      boxHeights->SetValue(i, boxHeight);

      if (another->IsFontEntity(i))
	{
          DeclareAndCast(IGESGraph_TextFontDef, fontEntity, 
			 TC.Transferred(another->FontEntity(i)));
          fontEntities->SetValue(i, fontEntity);
          fontCodes->SetValue(i, -1);
	}
      else
	{
          Standard_Integer fontCode = another->FontCode(i);
          fontCodes->SetValue(i, fontCode);
////          fontEntities->SetValue(i, NULL);    par defaut
	}

      Standard_Real slantAngle = another->SlantAngle(i);
      slantAngles->SetValue(i, slantAngle);
      Standard_Real rotationAngle = another->RotationAngle(i);
      rotationAngles->SetValue(i, rotationAngle);
      Standard_Integer mirrorFlag = another->MirrorFlag(i);
      mirrorFlags->SetValue(i, mirrorFlag);
      Standard_Integer rotateFlag = another->RotateFlag(i);
      rotateFlags->SetValue(i, rotateFlag);
      gp_XYZ startPoint = (another->StartPoint(i)).XYZ();
      startPoints->SetValue(i, startPoint);
      Handle(TCollection_HAsciiString) text =
	new TCollection_HAsciiString(another->Text(i));
      texts->SetValue(i, text);
    }

  ent->Init(nbChars, boxWidths, boxHeights, fontCodes, fontEntities,
	    slantAngles, rotationAngles, mirrorFlags, rotateFlags,
	    startPoints, texts);
  ent->SetFormNumber(another->FormNumber());
}

IGESData_DirChecker  IGESDimen_ToolGeneralNote::DirChecker
  (const Handle(IGESDimen_GeneralNote)& /* ent */ ) const 
{ 
  IGESData_DirChecker DC (212, 0, 105);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefValue);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.UseFlagRequired(1);
  DC.HierarchyStatusIgnored();
  return DC;
}

void  IGESDimen_ToolGeneralNote::OwnCheck
  (const Handle(IGESDimen_GeneralNote)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const 
{
  if (((ent->FormNumber() <   0) || (ent->FormNumber() >   8)) &&
      ((ent->FormNumber() < 100) || (ent->FormNumber() > 102)) &&
      (ent->FormNumber() != 105))
    ach->AddFail("Form Number: Not Valid");

  Standard_Integer upper = ent->NbStrings();
  for (Standard_Integer i = 1; i <= upper; i++)
    {
      if (ent->NbCharacters(i) != ent->Text(i)->Length())
	{
	  char mess[80];
	  sprintf(mess,"%d : Number of Characters != Length of Text String",i);
          ach->AddFail(mess);
	}
      Standard_Integer mflag = ent->MirrorFlag(i);
      if ((mflag < 0) || (mflag > 2))
	{
	  char mess[80];
	  sprintf(mess, "%d : Mirror flag != 0, 1, 2",i);
          ach->AddFail(mess);
	}
      Standard_Integer rflag = ent->RotateFlag(i);
      if ((rflag < 0) || (rflag > 1))
	{
	  char mess[80];
	  sprintf(mess, "%d : Rotate flag != 0, 1",i);
          ach->AddFail(mess);
	}
    }
}

void  IGESDimen_ToolGeneralNote::OwnDump
  (const Handle(IGESDimen_GeneralNote)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const 
{ 
  Standard_Integer sublevel = (level > 4) ? 1 : 0;
  Standard_Integer upper = ent->NbStrings();

  S << "IGESDimen_GeneralNote" << endl;
  S << "Number of Text Strings : " << upper << endl;
  S << "Number of Characters : " << endl;
  S << "Box Widths : " << endl;
  S << "Box Heights : " << endl;
  S << "Font Codes : " << endl;
  S << "Font Entities : " << endl;
  S << "Slant Angles : " << endl;
  S << "Rotation Angles : " << endl;
  S << "Mirror Flags : " << endl;
  S << "Rotate Flags : " << endl;
  S << "Start Points : " << endl;
  S << "Texts : ";
  IGESData_DumpVals(S,-level,1, ent->NbStrings(),ent->NbCharacters);
  S << endl;
  if (level > 4)
    {
      S << "Details of each String" << endl;
      for ( Standard_Integer i = 1; i <= upper; i++)
	{
          S << "[" << i << "]: " << endl;
          S << "Number of Characters : " << ent->NbCharacters(i) << "  ";
          S << "Box Width  : " << ent->BoxWidth(i)  << "  ";
          S << "Box Height : " << ent->BoxHeight(i) << endl;
          if (ent->IsFontEntity(i))
	    {
              S << "Font Entity : ";
              dumper.Dump (ent->FontEntity(i),S, sublevel);
              S << endl;
	    }
          else
	    S << "Font Code : " << ent->FontCode(i) << endl;
          S << "Slant Angle : " << ent->SlantAngle(i) << "  ";
          S << "Rotation Angle : " << ent->RotationAngle(i) << "  ";
          S << "Mirror Flag : " << ent->MirrorFlag(i) << "  ";
          S << "Rotate Flag : " << ent->RotateFlag(i) << endl;
          S << "Start Point : ";
          IGESData_DumpXYZL(S,level, ent->StartPoint(i), ent->Location());
          S << "\nText : ";
          IGESData_DumpString(S,ent->Text(i));
          S << endl;
	}
    }
  S << endl;
}
