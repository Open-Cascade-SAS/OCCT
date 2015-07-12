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

#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESGraph_ToolUniformRectGrid.hxx>
#include <IGESGraph_UniformRectGrid.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESGraph_ToolUniformRectGrid::IGESGraph_ToolUniformRectGrid ()    {  }


void IGESGraph_ToolUniformRectGrid::ReadOwnParams
  (const Handle(IGESGraph_UniformRectGrid)& ent,
   const Handle(IGESData_IGESReaderData)& /*IR*/, IGESData_ParamReader& PR) const
{ 
  Standard_Integer nbPropertyValues;
  Standard_Integer finite;
  Standard_Integer line;
  Standard_Integer weighted;
  gp_XY            gridPoint;
  gp_XY            gridSpacing;
  Standard_Integer nbPointsX = 0;
  Standard_Integer nbPointsY = 0;

  // Reading nbPropertyValues(Integer)
  PR.ReadInteger(PR.Current(), "No. of property values", nbPropertyValues);
  if (nbPropertyValues != 9)
    PR.AddFail("No. of Property values : Value is not 9");

  // Reading finite(Integer)
  PR.ReadInteger(PR.Current(), "Finite/infinite grid flag", finite);

  // Reading line(Integer)
  PR.ReadInteger(PR.Current(), "Line/point grid flag", line);

  // Reading weighted(Integer)
  PR.ReadInteger(PR.Current(), "Weighted/unweighted grid flag", weighted);

  // Reading gridPoint(XY)
  PR.ReadXY(PR.CurrentList(1, 2), "Grid point coordinates", gridPoint);

  // Reading gridSpacing(XY)
  PR.ReadXY(PR.CurrentList(1, 2), "Grid Spacing coordinates", gridSpacing);

  // Reading nbPointsX(Integer)  ?? even if not IsFinite ?
  if (finite != 0 || PR.DefinedElseSkip())
    PR.ReadInteger
      (PR.Current(), "No. of points/lines in X direction", nbPointsX);

  // Reading nbPointsY(Integer)
  if (finite != 0 || PR.DefinedElseSkip())
    PR.ReadInteger
      (PR.Current(), "No. of points/lines in Y direction", nbPointsY);

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init
    (nbPropertyValues, finite, line, weighted,
     gridPoint, gridSpacing, nbPointsX, nbPointsY);
}

void IGESGraph_ToolUniformRectGrid::WriteOwnParams
  (const Handle(IGESGraph_UniformRectGrid)& ent, IGESData_IGESWriter& IW)  const
{
  IW.Send( ent->NbPropertyValues() );
  IW.SendBoolean( ent->IsFinite() );
  IW.SendBoolean( ent->IsLine() );
  IW.SendBoolean( !ent->IsWeighted() );
  IW.Send( ent->GridPoint().X() );
  IW.Send( ent->GridPoint().Y() );
  IW.Send( ent->GridSpacing().X() );
  IW.Send( ent->GridSpacing().Y() );
  IW.Send( ent->NbPointsX() );  // ?? even if not IsFinite ??
  IW.Send( ent->NbPointsY() );
}

void  IGESGraph_ToolUniformRectGrid::OwnShared
  (const Handle(IGESGraph_UniformRectGrid)& /*ent*/, Interface_EntityIterator& /*iter*/) const
{
}

void IGESGraph_ToolUniformRectGrid::OwnCopy
  (const Handle(IGESGraph_UniformRectGrid)& another,
   const Handle(IGESGraph_UniformRectGrid)& ent, Interface_CopyTool& /*TC*/) const
{
  ent->Init
    (9, (another->IsFinite() ? 1 : 0), (another->IsLine() ? 1 : 0),
     (another->IsWeighted() ? 0 : 1),
     another->GridPoint().XY(),another->GridSpacing().XY(),
     another->NbPointsX(), another->NbPointsY());
}

Standard_Boolean  IGESGraph_ToolUniformRectGrid::OwnCorrect
  (const Handle(IGESGraph_UniformRectGrid)& ent) const
{
  Standard_Boolean res = (ent->NbPropertyValues() != 9);
  if (res) ent->Init
    (9, (ent->IsFinite() ? 1 : 0), (ent->IsLine() ? 1 : 0),
     (ent->IsWeighted() ? 0 : 1),ent->GridPoint().XY(),ent->GridSpacing().XY(),
     ent->NbPointsX(), ent->NbPointsY());    // nbpropertyvalues=9
  return res;
}

IGESData_DirChecker IGESGraph_ToolUniformRectGrid::DirChecker
  (const Handle(IGESGraph_UniformRectGrid)& /*ent*/)  const
{ 
  IGESData_DirChecker DC (406, 22);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESGraph_ToolUniformRectGrid::OwnCheck
  (const Handle(IGESGraph_UniformRectGrid)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach)  const
{
  if ( ent->IsFinite() != 0 && ent->IsFinite() != 1 )
    ach->AddFail("Finite/infinite grid flag : Value != 0/1");
  if ( ent->IsLine() != 0 && ent->IsLine() != 1 )
    ach->AddFail("Line/point grid flag : Value != 0/1");
  if ( ent->IsWeighted() != 0 && ent->IsWeighted() != 1 )
    ach->AddFail("Weighted/unweighted grid flag : Value != 0/1");
  if (ent->NbPropertyValues() != 9)
    ach->AddFail("No. of Property values : Value != 9");
}

void IGESGraph_ToolUniformRectGrid::OwnDump
  (const Handle(IGESGraph_UniformRectGrid)& ent, const IGESData_IGESDumper& /*dumper*/,
   const Handle(Message_Messenger)& S, const Standard_Integer /*level*/)  const
{
  S << "IGESGraph_UniformRectGrid" << endl;

  S << "No. of property values : " << ent->NbPropertyValues() << endl;
  S << "Grid         : " << ( ent->IsFinite() ? "Finite" : "Infinite");
  S << "  -  Composed of " << ( ent->IsLine() ? "Lines" : "Points");
  S << "  -  " << ( ent->IsWeighted() ? "Weighted" : "Unweighted") << endl;
  S << "Grid Point   : ";
  IGESData_DumpXY(S, ent->GridPoint());
  S << "  Grid Spacing : ";
  IGESData_DumpXY(S, ent->GridSpacing());  S << endl;
  if (ent->IsFinite())
    S << "No. of points/lines in direction :  X : " << ent->NbPointsX()
      << "  -  Y : " << ent->NbPointsY() << endl;
}
