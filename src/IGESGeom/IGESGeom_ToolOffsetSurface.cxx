//--------------------------------------------------------------------
//
//  File Name : IGESGeom_OffsetSurface.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGeom_ToolOffsetSurface.ixx>
#include <IGESData_ParamCursor.hxx>
#include <gp_XYZ.hxx>
#include <gp_Vec.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_Dump.hxx>
#include <Interface_Macros.hxx>

// MGE 31/07/98
#include <Message_Msg.hxx>
#include <IGESData_Status.hxx>

IGESGeom_ToolOffsetSurface::IGESGeom_ToolOffsetSurface ()    {  }


void IGESGeom_ToolOffsetSurface::ReadOwnParams
  (const Handle(IGESGeom_OffsetSurface)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  // MGE 31/07/98
  // Building of messages
  //========================================
  Message_Msg Msg162("XSTEP_162");
  //========================================

  gp_XYZ anIndicator;
  Standard_Real aDistance;
  Handle(IGESData_IGESEntity) aSurface;
  IGESData_Status aStatus;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  // Reading the offset indicator
  PR.ReadXYZ(PR.CurrentList(1, 3), Msg162, anIndicator); //szv#4:S4163:12Mar99 `st=` not needed
  // Reading the offset distance
  if (!PR.ReadReal(PR.Current(), aDistance)){
    Message_Msg Msg163("XSTEP_163");
    PR.SendFail(Msg163);
  } //szv#4:S4163:12Mar99 `st=` not needed
  // Reading the surface entity to be offset
  if (!PR.ReadEntity(IR, PR.Current(), aStatus, aSurface)){
    Message_Msg Msg164("XSTEP_164");
    switch(aStatus) {
    case IGESData_ReferenceError: {  
      Message_Msg Msg216 ("IGES_216");
      Msg164.Arg(Msg216.Value());
      PR.SendFail(Msg164);
      break; }
    case IGESData_EntityError: {
      Message_Msg Msg217 ("IGES_217");
      Msg164.Arg(Msg217.Value());
      PR.SendFail(Msg164);
      break; }
    default:{
    }
    }
  }//szv#4:S4163:12Mar99 `st=` not needed


/*
  // Reading the offset indicator
  st = PR.ReadXYZ(PR.CurrentList(1, 3), "Offset Indicator", anIndicator);
  // Reading the offset distance
  st = PR.ReadReal(PR.Current(), "The Offset Distance ", aDistance);
  // Reading the surface entity to be offset
  st = PR.ReadEntity(IR, PR.Current(), "Surface entity to be offset", aSurface);
*/

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(anIndicator, aDistance, aSurface);
}

void IGESGeom_ToolOffsetSurface::WriteOwnParams
  (const Handle(IGESGeom_OffsetSurface)& ent, IGESData_IGESWriter& IW)  const
{
  IW.Send( ent->OffsetIndicator().X() );
  IW.Send( ent->OffsetIndicator().Y() );
  IW.Send( ent->OffsetIndicator().Z() );
  IW.Send( ent->Distance() );
  IW.Send( ent->Surface() );
}

void  IGESGeom_ToolOffsetSurface::OwnShared
  (const Handle(IGESGeom_OffsetSurface)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem( ent->Surface() );
}

void IGESGeom_ToolOffsetSurface::OwnCopy
  (const Handle(IGESGeom_OffsetSurface)& another,
   const Handle(IGESGeom_OffsetSurface)& ent, Interface_CopyTool& TC) const
{
  DeclareAndCast(IGESData_IGESEntity, aSurface, TC.Transferred(another->Surface()) );

  gp_XYZ anIndicator = (another->OffsetIndicator()).XYZ();
  Standard_Real aDistance = another->Distance();

  ent->Init(anIndicator, aDistance, aSurface);
}


IGESData_DirChecker IGESGeom_ToolOffsetSurface::DirChecker
  (const Handle(IGESGeom_OffsetSurface)& /* ent */ )   const
{
  IGESData_DirChecker DC(140, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
//  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESGeom_ToolOffsetSurface::OwnCheck
  (const Handle(IGESGeom_OffsetSurface)& /* ent */,
   const Interface_ShareTool& , Handle(Interface_Check)& /* ach */)  const
{
}

void IGESGeom_ToolOffsetSurface::OwnDump
  (const Handle(IGESGeom_OffsetSurface)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level)  const
{
  Standard_Integer sublevel = (level <= 4) ? 0 : 1;

  S << "IGESGeom_OffsetSurface" << endl;

  S << "Offset Indicator     : ";
  IGESData_DumpXYZL(S,level, ent->OffsetIndicator(), ent->VectorLocation());
  S << endl;
  S << "Offset Distance      : " << ent->Distance() << "  ";
  S << "Surface to be offset : ";
  dumper.Dump(ent->Surface(),S, sublevel);
  S << endl;
}
