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
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDraw_RectArraySubfigure.hxx>
#include <IGESDraw_ToolRectArraySubfigure.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TColStd_HArray1OfInteger.hxx>

IGESDraw_ToolRectArraySubfigure::IGESDraw_ToolRectArraySubfigure ()    {  }


void IGESDraw_ToolRectArraySubfigure::ReadOwnParams
  (const Handle(IGESDraw_RectArraySubfigure)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  gp_XYZ tempLowerLeftCorner;
  Standard_Real tempScaleFactor;
  Handle(IGESData_IGESEntity) tempBaseEntity;
  Handle(TColStd_HArray1OfInteger) tempPositions;
  Standard_Real tempColumnSeparation, tempRowSeparation, tempRotationAngle;
  Standard_Integer tempNbColumns, tempNbRows, tempDoDontFlag, tempListCount;

  PR.ReadEntity(IR, PR.Current(), "Base Entity", tempBaseEntity); //szv#4:S4163:12Mar99 `st=` not needed

  if (PR.DefinedElseSkip())
    PR.ReadReal(PR.Current(), "Scale Factor", tempScaleFactor); //szv#4:S4163:12Mar99 `st=` not needed
  else
    tempScaleFactor = 1.0;      // Setting to default value of 1.0

  //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadXYZ(PR.CurrentList(1, 3), "Lower Left Coordinate Of Array", tempLowerLeftCorner);
  PR.ReadInteger(PR.Current(), "Number Of Columns", tempNbColumns);
  PR.ReadInteger(PR.Current(), "Number Of Rows", tempNbRows);
  PR.ReadReal(PR.Current(), "Horizontal Distance Between Columns", tempColumnSeparation);
  PR.ReadReal(PR.Current(), "Vertical Distance Between Rows", tempRowSeparation);
  PR.ReadReal(PR.Current(), "Rotation Angle", tempRotationAngle);

  //st = PR.ReadInteger(PR.Current(), "DO-DONT List Count", tempListCount); //szv#4:S4163:12Mar99 moved in if
  if (PR.ReadInteger(PR.Current(), "DO-DONT List Count", tempListCount)) {
    // Initialise HArray1 only if there is no error reading its Length
    if (tempListCount > 0)
      tempPositions = new TColStd_HArray1OfInteger (1, tempListCount);
    else if (tempListCount < 0)
      PR.AddFail("DO-DONT List Count : Less than Zero");
  }

  PR.ReadInteger(PR.Current(), "DO-DONT Flag", tempDoDontFlag); //szv#4:S4163:12Mar99 `st=` not needed

  // Read the HArray1 only if its Length was read without any Error
  if (! tempPositions.IsNull()) {
    Standard_Integer I;
    for (I = 1; I <= tempListCount; I++) {
      Standard_Integer tempPos;
      //st = PR.ReadInteger(PR.Current(), "Number Of Position To Process",
			    //tempPos); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadInteger(PR.Current(), "Number Of Position To Process", tempPos))
	tempPositions->SetValue(I, tempPos);
    }
  }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init
    (tempBaseEntity, tempScaleFactor, tempLowerLeftCorner,
     tempNbColumns, tempNbRows, tempColumnSeparation, tempRowSeparation,
     tempRotationAngle, tempDoDontFlag, tempPositions);
}

void IGESDraw_ToolRectArraySubfigure::WriteOwnParams
  (const Handle(IGESDraw_RectArraySubfigure)& ent, IGESData_IGESWriter& IW)  const
{
  IW.Send(ent->BaseEntity());
  IW.Send(ent->ScaleFactor());
  IW.Send(ent->LowerLeftCorner().X());
  IW.Send(ent->LowerLeftCorner().Y());
  IW.Send(ent->LowerLeftCorner().Z());
  IW.Send(ent->NbColumns());
  IW.Send(ent->NbRows());
  IW.Send(ent->ColumnSeparation());
  IW.Send(ent->RowSeparation());
  IW.Send(ent->RotationAngle());
  IW.Send(ent->ListCount());
  IW.SendBoolean(ent->DoDontFlag());
  Standard_Integer I;
  Standard_Integer up  = ent->ListCount();
  for (I = 1; I <= up; I++)
    IW.Send(ent->ListPosition(I));
}

void  IGESDraw_ToolRectArraySubfigure::OwnShared
  (const Handle(IGESDraw_RectArraySubfigure)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->BaseEntity());
}

void IGESDraw_ToolRectArraySubfigure::OwnCopy
  (const Handle(IGESDraw_RectArraySubfigure)& another,
   const Handle(IGESDraw_RectArraySubfigure)& ent, Interface_CopyTool& TC) const
{
  DeclareAndCast(IGESData_IGESEntity, tempBaseEntity,
                 TC.Transferred(another->BaseEntity()));
  Standard_Real tempScaleFactor = another->ScaleFactor();
  gp_XYZ tempLowerLeftCorner = (another->LowerLeftCorner()).XYZ();
  Standard_Integer tempNbColumns = another->NbColumns();
  Standard_Integer tempNbRows = another->NbRows();
  Standard_Real tempColumnSeparation = another->ColumnSeparation();
  Standard_Real tempRowSeparation = another->RowSeparation();
  Standard_Real tempRotationAngle = another->RotationAngle();
  Standard_Integer tempListCount = another->ListCount();
  Standard_Integer tempDoDontFlag = (another->DoDontFlag() ? 1 : 0);
  Handle(TColStd_HArray1OfInteger) tempPositions;
  if (tempListCount != 0) {
    tempPositions = new TColStd_HArray1OfInteger (1,tempListCount);
    Standard_Integer I;
    for (I = 1; I <= tempListCount; I++)
      tempPositions->SetValue(I, another->ListPosition(I));
  }

  ent->Init(tempBaseEntity, tempScaleFactor, tempLowerLeftCorner,
	    tempNbColumns, tempNbRows, tempColumnSeparation, tempRowSeparation,
	    tempRotationAngle, tempDoDontFlag, tempPositions);
}

IGESData_DirChecker IGESDraw_ToolRectArraySubfigure::DirChecker
  (const Handle(IGESDraw_RectArraySubfigure)& /*ent*/)  const
{
  IGESData_DirChecker DC(412, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.GraphicsIgnored(1);

  return DC;
}

void IGESDraw_ToolRectArraySubfigure::OwnCheck
  (const Handle(IGESDraw_RectArraySubfigure)& /*ent*/,
   const Interface_ShareTool& , Handle(Interface_Check)& /*ach*/)  const
{
}

void IGESDraw_ToolRectArraySubfigure::OwnDump
  (const Handle(IGESDraw_RectArraySubfigure)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level)  const
{
  Standard_Integer tempSubLevel = (level <= 4) ? 0 : 1;

  S << "IGESDraw_RectArraySubfigure" << endl;

  S << "Base Entity : ";
  dumper.Dump(ent->BaseEntity(),S, tempSubLevel);
  S << endl;
  S << "Scale Factor : " << ent->ScaleFactor() << "  ";
  S << "Lower Left Corner Of Array : ";
  IGESData_DumpXYZL(S,level, ent->LowerLeftCorner(),gp_GTrsf()); // no location
  S << "Number Of Columns : " << ent->NbColumns()   << "  ";
  S << "Number Of Rows    : " << ent->NbRows()      << endl;
  S << "Horizontal Distance Between Columns : " << ent->ColumnSeparation()<<endl;
  S << "Vertical Distance Between Rows      : " << ent->RowSeparation() <<endl;
  S << "Rotation Angle (in radians)         : " << ent->RotationAngle() <<endl;
  S << "Do-Dont Flag : ";
  if (ent->DoDontFlag()) S << "(1)Dont  ";
  else                   S << "(0)Do  ";
  S << "Do-Dont List : ";
  IGESData_DumpVals(S ,level,1, ent->ListCount(),ent->ListPosition);
  S << endl;
}
