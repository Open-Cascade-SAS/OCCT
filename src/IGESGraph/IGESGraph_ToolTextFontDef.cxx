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

#include <gp_GTrsf.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <IGESBasic_HArray1OfHArray1OfInteger.hxx>
#include <IGESBasic_HArray1OfHArray1OfXY.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESGraph_TextFontDef.hxx>
#include <IGESGraph_ToolTextFontDef.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TColgp_HArray1OfXY.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>

IGESGraph_ToolTextFontDef::IGESGraph_ToolTextFontDef ()    {  }


void IGESGraph_ToolTextFontDef::ReadOwnParams
  (const Handle(IGESGraph_TextFontDef)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{ 
  //Standard_Boolean                            st; //szv#4:S4163:12Mar99 moved down
  Standard_Integer                            nbval;

  Standard_Integer                            fontCode;
  Handle(TCollection_HAsciiString)            fontName;
  Standard_Integer                            supersededFont;
  Handle(IGESGraph_TextFontDef)               supersededEntity;
  Standard_Integer                            scale;
  Handle(TColStd_HArray1OfInteger)            aSCIICodes;
  Handle(TColStd_HArray1OfInteger)            nextCharX, nextCharY;
  Handle(TColStd_HArray1OfInteger)            penMotions;
  Handle(IGESBasic_HArray1OfHArray1OfInteger) penFlags;
  Handle(IGESBasic_HArray1OfHArray1OfInteger) movePenX, movePenY;

  Standard_Integer                            tempCode, tempNextX,tempNextY;
  Standard_Integer                            tempMotion;
  Standard_Integer                            tempFlag, tempMoveX,tempMoveY;

  // Reading fontCode(Integer)
  PR.ReadInteger (PR.Current(), "Font Code", fontCode); //szv#4:S4163:12Mar99 `st=` not needed

  // Reading fontName(String)
  PR.ReadText (PR.Current(), "Font Name", fontName); //szv#4:S4163:12Mar99 `st=` not needed

  if ( PR.IsParamEntity(PR.CurrentNumber()) )
    {
      supersededFont = -1;

      // Reading supersededEntity(TextFontDef)
      PR.ReadEntity (IR, PR.Current(), "Text Definition Entity",
		     STANDARD_TYPE(IGESGraph_TextFontDef), supersededEntity); //szv#4:S4163:12Mar99 `st=` not needed
    }
  else
    // Reading supersededFont(Integer)
    PR.ReadInteger(PR.Current(), "No. of superseded font", supersededFont); //szv#4:S4163:12Mar99 `st=` not needed

  // Reading scale(Integer)
  PR.ReadInteger(PR.Current(), "Grid units eqvt to one text height", scale); //szv#4:S4163:12Mar99 `st=` not needed

  // Reading nbval(Integer)
  Standard_Boolean st = PR.ReadInteger(PR.Current(), "No. of characters in this defn", nbval);
  if (st && nbval > 0)
    {
      aSCIICodes = new TColStd_HArray1OfInteger(1, nbval);
      nextCharX  = new TColStd_HArray1OfInteger(1, nbval);
      nextCharY  = new TColStd_HArray1OfInteger(1, nbval);
      penMotions = new TColStd_HArray1OfInteger(1, nbval);
      penFlags   = new IGESBasic_HArray1OfHArray1OfInteger(1, nbval);
      movePenX   = new IGESBasic_HArray1OfHArray1OfInteger(1, nbval);
      movePenY   = new IGESBasic_HArray1OfHArray1OfInteger(1, nbval);
      
      for ( Standard_Integer i = 1; i <= nbval; i++ )
	{
          // Reading aSCIICodes(HArray1OfInteger)
          if (PR.ReadInteger(PR.Current(), "array aSCIICodes", tempCode)) //szv#4:S4163:12Mar99 `st=` not needed
	    aSCIICodes->SetValue(i, tempCode);
	  
          // Reading nextChars(HArray1OfInteger*2)
          if (PR.ReadInteger(PR.Current(), "array nextChar X", tempNextX)) //szv#4:S4163:12Mar99 `st=` not needed
	    nextCharX->SetValue(i, tempNextX);
          if (PR.ReadInteger(PR.Current(), "array nextChar Y", tempNextY)) //szv#4:S4163:12Mar99 `st=` not needed
	    nextCharY->SetValue(i, tempNextY);
	  
          // Reading penMotions(HArray1OfInteger)
          if (PR.ReadInteger(PR.Current(), "array penMotions", tempMotion)) { //szv#4:S4163:12Mar99 `st=` not needed
	    penMotions->SetValue(i, tempMotion);
	    if (tempMotion > 0) {
	      Handle(TColStd_HArray1OfInteger) intarray, xarray, yarray;

	      intarray = new TColStd_HArray1OfInteger(1, tempMotion);
	      xarray   = new TColStd_HArray1OfInteger(1, tempMotion);
	      yarray   = new TColStd_HArray1OfInteger(1, tempMotion);
	      
	      for ( Standard_Integer j = 1; j <= tempMotion; j++ )  {
		if (PR.DefinedElseSkip()) {
		  // Reading penFlags(HArray1OfHArray1OfInteger)
		  if (PR.ReadInteger(PR.Current(), "array penFlags", tempFlag)) //szv#4:S4163:12Mar99 `st=` not needed
		    intarray->SetValue(j, tempFlag);
		}
		else  intarray->SetValue(j, 0); // Default Value
		  
		// Reading movePenTo(HArray1OfHArray1OfInteger*2)
		if (PR.ReadInteger(PR.Current(), "array movePenTo X", tempMoveX)) //szv#4:S4163:12Mar99 `st=` not needed
		  xarray->SetValue(j, tempMoveX);
		if (PR.ReadInteger(PR.Current(), "array movePenTo Y", tempMoveY)) //szv#4:S4163:12Mar99 `st=` not needed
		  yarray->SetValue(j, tempMoveY);
	      }
	      penFlags->SetValue(i, intarray);
	      movePenX->SetValue(i, xarray);
	      movePenY->SetValue(i, yarray);

	    }
	    else  PR.AddFail("Count of Pen motions : Not Positive");
	  }
	}
    }
  else  PR.AddFail ("Count of characters in this defn : Not Positive");

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init
    (fontCode, fontName, supersededFont, supersededEntity,
     scale, aSCIICodes, nextCharX, nextCharY,
     penMotions, penFlags, movePenX, movePenY);
}

void IGESGraph_ToolTextFontDef::WriteOwnParams
  (const Handle(IGESGraph_TextFontDef)& ent, IGESData_IGESWriter& IW)  const
{
  Standard_Integer IX,IY;
  IW.Send( ent->FontCode() );
  IW.Send( ent->FontName() );

  if ( ent->IsSupersededFontEntity() ) 
    IW.Send( ent->SupersededFontEntity(), Standard_True );  // negative
  else
    IW.Send( ent->SupersededFontCode() );

  IW.Send( ent->Scale() );

  Standard_Integer Up  = ent->NbCharacters();
  IW.Send( Up );
  for ( Standard_Integer i = 1; i <= Up; i++)
    {
      IW.Send( ent->ASCIICode(i) );
      ent->NextCharOrigin (i,IX,IY);
      IW.Send( IX );
      IW.Send( IY );
      IW.Send( ent->NbPenMotions(i) );
      for ( Standard_Integer j = 1; j <= ent->NbPenMotions(i); j ++)
	{
          IW.SendBoolean( ent->IsPenUp(i,j) );
	  ent->NextPenPosition (i,j, IX,IY);
          IW.Send( IX );
          IW.Send( IY );
	}
    }
} 
 
void  IGESGraph_ToolTextFontDef::OwnShared
  (const Handle(IGESGraph_TextFontDef)& ent, Interface_EntityIterator& iter) const
{
  if ( ent->IsSupersededFontEntity() ) 
    iter.GetOneItem( ent->SupersededFontEntity() );
}

void IGESGraph_ToolTextFontDef::OwnCopy
  (const Handle(IGESGraph_TextFontDef)& another,
   const Handle(IGESGraph_TextFontDef)& ent, Interface_CopyTool& TC) const
{
  Standard_Integer                            nbval;
  Standard_Integer                            fontCode;
  Handle(TCollection_HAsciiString)            fontName;
  Standard_Integer                            supersededFont=0;
  Handle(IGESGraph_TextFontDef)               supersededEntity;
  Standard_Integer                            scale;
  Handle(TColStd_HArray1OfInteger)            aSCIICodes, nextCharX,nextCharY;
  Handle(TColStd_HArray1OfInteger)            penMotions;
  Handle(IGESBasic_HArray1OfHArray1OfInteger) penFlags,movePenX,movePenY;

  Standard_Integer                            tempMotion;
  Handle(TColStd_HArray1OfInteger)            intarray,xarray,yarray;
 
  nbval       = another->NbCharacters();
  aSCIICodes  = new TColStd_HArray1OfInteger(1, nbval);
  nextCharX   = new TColStd_HArray1OfInteger(1, nbval);
  nextCharY   = new TColStd_HArray1OfInteger(1, nbval);
  penMotions  = new TColStd_HArray1OfInteger(1, nbval);
  penFlags    = new IGESBasic_HArray1OfHArray1OfInteger(1, nbval);
  movePenX    = new IGESBasic_HArray1OfHArray1OfInteger(1, nbval);
  movePenY    = new IGESBasic_HArray1OfHArray1OfInteger(1, nbval);

  fontCode = another->FontCode();
  fontName = new TCollection_HAsciiString(another->FontName());

  if ( another->IsSupersededFontEntity() )
    supersededEntity = 
      Handle(IGESGraph_TextFontDef)::DownCast (TC.Transferred(another->SupersededFontEntity()));
  else
    supersededFont = another->SupersededFontCode();

  scale = another->Scale();

  Standard_Integer j, IX,IY;

  for (Standard_Integer i = 1; i <= nbval; i++)
    {
      aSCIICodes->SetValue( i, another->ASCIICode(i) );
      ent->NextCharOrigin (i,IX,IY);
      nextCharX->SetValue ( i, IX);
      nextCharY->SetValue ( i, IY);

      tempMotion = another->NbPenMotions(i);
      penMotions->SetValue( i, tempMotion );

      intarray = new TColStd_HArray1OfInteger(1, tempMotion);
      xarray   = new TColStd_HArray1OfInteger(1, tempMotion);
      yarray   = new TColStd_HArray1OfInteger(1, tempMotion);

      for (j = 1; j <= tempMotion; j++)
	{
          if ( another->IsPenUp(i, j) ) intarray->SetValue(j, 1);
          else                          intarray->SetValue(j, 0);

	  another->NextPenPosition(i, j, IX,IY);
          xarray->SetValue(j, IX);
          yarray->SetValue(j, IY);
	}
      penFlags->SetValue(i, intarray);
      movePenX->SetValue(i, xarray);
      movePenY->SetValue(i, yarray);
    }

  ent->Init(fontCode, fontName, supersededFont, supersededEntity,
	    scale, aSCIICodes, nextCharX, nextCharY, penMotions,
	    penFlags, movePenX, movePenY);
}

IGESData_DirChecker IGESGraph_ToolTextFontDef::DirChecker
  (const Handle(IGESGraph_TextFontDef)& /*ent*/)  const
{ 
  IGESData_DirChecker DC (310, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusRequired(0);
  DC.UseFlagRequired(2);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESGraph_ToolTextFontDef::OwnCheck
  (const Handle(IGESGraph_TextFontDef)& /*ent*/,
   const Interface_ShareTool& , Handle(Interface_Check)& /*ach*/)  const
{
}

void IGESGraph_ToolTextFontDef::OwnDump
  (const Handle(IGESGraph_TextFontDef)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level)  const
{
  Standard_Integer sublevel = (level <= 4) ? 0 : 1;
  Standard_Integer nbchars  = ent->NbCharacters();

  S << "IGESGraph_TextFontDef" << endl;

  S << "Font Code : " << ent->FontCode() << endl;
  S << "Font Name : ";
  IGESData_DumpString(S,ent->FontName());
  S << endl;
  if ( ent->IsSupersededFontEntity() ) {
    S << "Text Definition Entity : ";
    dumper.Dump(ent->SupersededFontEntity(),S, sublevel);
  }
  else  S << "Superseding Font Number : " << ent->SupersededFontCode();
  S << endl; 
  S << "No. of Grid Units eqvt to 1 Text Height : " << ent->Scale() << endl;
  S << "ASCII Codes                              : " << endl
    << "Grid Locations of next character origins : " << endl
    << "Pen Motions                              : " << endl
    << "Pen Positions                            : " << endl
    << "Grid Locations the pen moves to          : ";
  S << "Count = "      << nbchars << endl;
  IGESData_DumpVals(S,-level,1,nbchars,ent->ASCIICode);
  S << endl;
  if (level > 4 )
    {
      Handle(TColgp_HArray1OfXY) arrXY;
      Standard_Integer I, J, nbmotions;
      for (I = 1; I <= nbchars; I++)
	{
	  Standard_Integer IX,IY;
	  S << "[" << I << "]: ";
	  S << "ASCII Code : " << ent->ASCIICode(I) << endl;
	  S << "Grid Location of next character's origin : ";
	  ent->NextCharOrigin(I,IX,IY);
	  S << "X=" << IX << " Y=" << IY;
	  nbmotions = ent->NbPenMotions(I);
	  S << "  No. of Pen Motions : " << nbmotions;
	  if (level <= 5) S << " [ ask level > 5 for Details ]" << endl;
	  else {
	    S << endl;
	    for (J = 1; J <= nbmotions; J++)
	      {
		S << "Pen up(1) / down(0) flag : " << (Standard_Integer)ent->IsPenUp(I,J);
		S << " Next Pen Position : ";
		ent->NextPenPosition(I,J, IX,IY);
		S << " X="<<IX<<" Y="<<IY;
		S << endl;
	      }
	  }
	}
    }
  S << endl;
}
