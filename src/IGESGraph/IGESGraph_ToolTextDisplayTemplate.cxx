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
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESGraph_TextDisplayTemplate.hxx>
#include <IGESGraph_TextFontDef.hxx>
#include <IGESGraph_ToolTextDisplayTemplate.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESGraph_ToolTextDisplayTemplate::IGESGraph_ToolTextDisplayTemplate ()    {  }


void IGESGraph_ToolTextDisplayTemplate::ReadOwnParams
  (const Handle(IGESGraph_TextDisplayTemplate)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{ 
  //Standard_Boolean              st; //szv#4:S4163:12Mar99 not needed

  Standard_Real                 boxWidth; 
  Standard_Real                 boxHeight; 
  Standard_Integer              fontCode; 
  Handle(IGESGraph_TextFontDef) fontEnt;
  Standard_Real                 slantAngle; 
  Standard_Real                 rotationAngle; 
  Standard_Integer              mirrorFlag; 
  Standard_Integer              rotateFlag; 
  gp_XYZ                        corner;

  // Reading boxWidth(Real)
  PR.ReadReal(PR.Current(), "Character box width", boxWidth); //szv#4:S4163:12Mar99 `st=` not needed

  // Reading boxHeight(Real)
  PR.ReadReal(PR.Current(), "Character box height", boxHeight); //szv#4:S4163:12Mar99 `st=` not needed

  Standard_Integer curnum = PR.CurrentNumber();
  if (PR.DefinedElseSkip())
    {
      // Reading fontCode(Integer, must be positive)
      PR.ReadInteger (PR.Current(), "Font Code", fontCode); //szv#4:S4163:12Mar99 `st=` not needed
      // Reading fontEnt(TextFontDef) ?
      if (fontCode < 0) {
	fontEnt = GetCasted(IGESGraph_TextFontDef,PR.ParamEntity (IR,curnum));
	if (fontEnt.IsNull()) PR.AddFail("Font Entity : incorrect reference");
      }
    }
  else
    fontCode = 1; // Default Value

  if (PR.DefinedElseSkip())
    // Reading slantAngle(Real)
    PR.ReadReal (PR.Current(), "Slant Angle", slantAngle); //szv#4:S4163:12Mar99 `st=` not needed
  else
    slantAngle = M_PI/2.0; // Default Value

  // Reading rotationAngle(Real)
  PR.ReadReal (PR.Current(), "Rotation Angle", rotationAngle); //szv#4:S4163:12Mar99 `st=` not needed

  // Reading mirrorFlag(Integer)
  PR.ReadInteger (PR.Current(), "Mirror Flag", mirrorFlag); //szv#4:S4163:12Mar99 `st=` not needed

  // Reading rotateFlag(Integer)
  PR.ReadInteger (PR.Current(), "Rotate Flag", rotateFlag); //szv#4:S4163:12Mar99 `st=` not needed

  // Reading corner(XYZ)
  PR.ReadXYZ( PR.CurrentList(1, 3), "Lower left coordinates/Increments", corner); //szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init
    (boxWidth, boxHeight, fontCode, fontEnt,
     slantAngle, rotationAngle, mirrorFlag, rotateFlag, corner);
}

void IGESGraph_ToolTextDisplayTemplate::WriteOwnParams
  (const Handle(IGESGraph_TextDisplayTemplate)& ent, IGESData_IGESWriter& IW)  const
{ 
  IW.Send( ent->BoxWidth() );
  IW.Send( ent->BoxHeight() );

  if ( ent->IsFontEntity() )
    IW.Send( ent->FontEntity(), Standard_True );  // negative
  else
    IW.Send( ent->FontCode() );

  IW.Send( ent->SlantAngle() );
  IW.Send( ent->RotationAngle() );
  IW.Send( ent->MirrorFlag() );
  IW.Send( ent->RotateFlag() );
  IW.Send( ent->StartingCorner().X() );
  IW.Send( ent->StartingCorner().Y() );
  IW.Send( ent->StartingCorner().Z() );
}

void  IGESGraph_ToolTextDisplayTemplate::OwnShared
  (const Handle(IGESGraph_TextDisplayTemplate)& ent, Interface_EntityIterator& iter) const
{
  if ( ent->IsFontEntity() )
    iter.GetOneItem( ent->FontEntity() );
}

void IGESGraph_ToolTextDisplayTemplate::OwnCopy
  (const Handle(IGESGraph_TextDisplayTemplate)& another,
   const Handle(IGESGraph_TextDisplayTemplate)& ent, Interface_CopyTool& TC) const
{ 
  Standard_Real                 boxWidth; 
  Standard_Real                 boxHeight; 
  Standard_Integer              fontCode=0; 
  Handle(IGESGraph_TextFontDef) fontEntity;
  Standard_Real                 slantAngle; 
  Standard_Real                 rotationAngle; 
  Standard_Integer              mirrorFlag; 
  Standard_Integer              rotateFlag; 
  gp_XYZ                        corner;

  boxWidth  = another->BoxWidth(); 
  boxHeight = another->BoxHeight(); 

  if ( another->IsFontEntity() )
    fontEntity = 
      Handle(IGESGraph_TextFontDef)::DownCast (TC.Transferred(another->FontEntity()));
  else
    fontCode = another->FontCode();

  slantAngle    = another->SlantAngle(); 
  rotationAngle = another->RotationAngle(); 
  mirrorFlag    = another->MirrorFlag(); 
  rotateFlag    = another->RotateFlag(); 
  corner        = another->StartingCorner().XYZ();

  ent->Init(boxWidth, boxHeight, fontCode, fontEntity,
	    slantAngle, rotationAngle, mirrorFlag, rotateFlag, corner);
}

IGESData_DirChecker IGESGraph_ToolTextDisplayTemplate::DirChecker
  (const Handle(IGESGraph_TextDisplayTemplate)& /*ent*/)  const
{ 
  IGESData_DirChecker DC (312, 0, 1);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefAny);
  DC.SubordinateStatusRequired(0);
  DC.UseFlagRequired(2);
  DC.HierarchyStatusRequired(0);
  return DC;
}

void IGESGraph_ToolTextDisplayTemplate::OwnCheck
  (const Handle(IGESGraph_TextDisplayTemplate)& /*ent*/,
   const Interface_ShareTool& , Handle(Interface_Check)& /*ach*/)  const
{
}

void IGESGraph_ToolTextDisplayTemplate::OwnDump
  (const Handle(IGESGraph_TextDisplayTemplate)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level)  const
{
  Standard_Integer sublevel = (level <= 4) ? 0 : 1;

  S << "IGESGraph_TextDisplayTemplate" << endl;

  S << "Character box width  : "  << ent->BoxWidth() << "  ";
  S << "Character box height : "  << ent->BoxHeight() << endl;
  if ( ent->IsFontEntity() )
    {
      S << "Font Entity : ";
      dumper.Dump(ent->FontEntity(),S, sublevel);
    }
  else
    S << "Font code : " << ent->FontCode();
  S << endl;
  S << "Slant angle    : "  << ent->SlantAngle() << "  ";
  S << "Rotation angle : "  << ent->RotationAngle() << endl;
  S << "Mirror flag    : "  << ent->MirrorFlag() << "  ";
  S << "Rotate flag    : "  << ent->RotateFlag() << endl;
  if ( ent->FormNumber() == 0 )
    S << "Lower Left Corner coordinates : ";
  else
    S << "Increments from coordinates : "; 
  IGESData_DumpXYZL(S,level, ent->StartingCorner(), ent->Location());
  S << endl;
}
