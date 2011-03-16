//--------------------------------------------------------------------
//
//  File Name : IGESGeom_TabulatedCylinder.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGeom_ToolTabulatedCylinder.ixx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_IGESEntity.hxx>
#include <gp_XYZ.hxx>
#include <gp_Pnt.hxx>
#include <IGESData_Dump.hxx>
#include <Interface_Macros.hxx>

// MGE 31/07/98
#include <Message_Msg.hxx>
#include <IGESData_Status.hxx>


IGESGeom_ToolTabulatedCylinder::IGESGeom_ToolTabulatedCylinder ()    {  }


void IGESGeom_ToolTabulatedCylinder::ReadOwnParams
  (const Handle(IGESGeom_TabulatedCylinder)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  // MGE 31/07/98
  // Building of messages
  //========================================
  Message_Msg Msg157("XSTEP_157");
  //========================================

  Handle(IGESData_IGESEntity) aDirectrix;
  gp_XYZ anEnd;
  IGESData_Status aStatus;

  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  // Reading the directrix curve entity
  if (!PR.ReadEntity(IR, PR.Current(), aStatus, aDirectrix)){ //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg156("XSTEP_156");
    switch(aStatus) {
    case IGESData_ReferenceError: {  
      Message_Msg Msg216 ("IGES_216");
      Msg156.Arg(Msg216.Value());
      PR.SendFail(Msg156);
      break; }
    case IGESData_EntityError: {
      Message_Msg Msg217 ("IGES_217");
      Msg156.Arg(Msg217.Value());
      PR.SendFail(Msg156);
      break; }
    default:{
    }
    }
  } 
  //st = PR.ReadEntity(IR, PR.Current(), "Directrix", aDirectrix);

  // Reading the terminate point of the generatrix
  PR.ReadXYZ(PR.CurrentList(1, 3), Msg157, anEnd); //szv#4:S4163:12Mar99 `st=` not needed
  //st = PR.ReadXYZ(PR.CurrentList(1, 3), "Terminate Point", anEnd);

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(aDirectrix, anEnd);
}

void IGESGeom_ToolTabulatedCylinder::WriteOwnParams
  (const Handle(IGESGeom_TabulatedCylinder)& ent, IGESData_IGESWriter& IW)  const
{
  IW.Send( ent->Directrix() );
  IW.Send( ent->EndPoint().X() );
  IW.Send( ent->EndPoint().Y() );
  IW.Send( ent->EndPoint().Z() );
}

void  IGESGeom_ToolTabulatedCylinder::OwnShared
  (const Handle(IGESGeom_TabulatedCylinder)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem( ent->Directrix() );
}

void IGESGeom_ToolTabulatedCylinder::OwnCopy
  (const Handle(IGESGeom_TabulatedCylinder)& another,
   const Handle(IGESGeom_TabulatedCylinder)& ent, Interface_CopyTool& TC) const
{
  DeclareAndCast(IGESData_IGESEntity, aDirectrix,
		 TC.Transferred(another->Directrix()));

  gp_XYZ anEnd = another->EndPoint().XYZ();

  ent->Init(aDirectrix, anEnd);
}


IGESData_DirChecker IGESGeom_ToolTabulatedCylinder::DirChecker
  (const Handle(IGESGeom_TabulatedCylinder)& /* ent */ )  const
{
  IGESData_DirChecker DC(122, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
//  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESGeom_ToolTabulatedCylinder::OwnCheck
  (const Handle(IGESGeom_TabulatedCylinder)& /* ent */,
   const Interface_ShareTool& , Handle(Interface_Check)& /* ach */)  const
{  }

void IGESGeom_ToolTabulatedCylinder::OwnDump
  (const Handle(IGESGeom_TabulatedCylinder)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level)  const
{
  Standard_Integer sublevel = (level <= 4) ? 0 : 1;

  S << "IGESGeom_TabulatedCylinder" << endl;

  S << "Directrix       : ";
  dumper.Dump(ent->Directrix(),S, sublevel);
  S << endl;
  S << "Terminate Point : ";
  IGESData_DumpXYZL(S,level, ent->EndPoint(), ent->Location());
  S << endl;
}
