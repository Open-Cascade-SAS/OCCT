//--------------------------------------------------------------------
//
//  File Name : IGESGeom_SurfaceOfRevolution.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGeom_ToolSurfaceOfRevolution.ixx>
#include <IGESData_ParamCursor.hxx>
#include <IGESGeom_Line.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Interface_Macros.hxx>


// MGE 31/07/98
#include <Message_Msg.hxx>
#include <IGESData_Status.hxx>

IGESGeom_ToolSurfaceOfRevolution::IGESGeom_ToolSurfaceOfRevolution ()    {  }


void IGESGeom_ToolSurfaceOfRevolution::ReadOwnParams
  (const Handle(IGESGeom_SurfaceOfRevolution)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  // MGE 31/07/98

  Handle(IGESGeom_Line) anAxis;
  Handle(IGESData_IGESEntity) aGeneratrix;
  Standard_Real aStartAngle, anEndAngle;
  IGESData_Status aStatus;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  if (!PR.ReadEntity(IR, PR.Current(), aStatus, STANDARD_TYPE(IGESGeom_Line), anAxis)){ //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg152("XSTEP_152");
    switch(aStatus) {
    case IGESData_ReferenceError: {  
      Message_Msg Msg216 ("IGES_216");
      Msg152.Arg(Msg216.Value());
      PR.SendFail(Msg152);
      break; }
    case IGESData_EntityError: {
      Message_Msg Msg217 ("IGES_217");
      Msg152.Arg(Msg217.Value());
      PR.SendFail(Msg152);
      break; }
    case IGESData_TypeError: {
      Message_Msg Msg218 ("IGES_218");
      Msg152.Arg(Msg218.Value());
      PR.SendFail(Msg152);
      break; }
    default:{
    }
    }
  }
  if (!PR.ReadEntity(IR, PR.Current(), aStatus, aGeneratrix)){ //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg153("XSTEP_153");
    switch(aStatus) {
    case IGESData_ReferenceError: {  
      Message_Msg Msg216 ("IGES_216");
      Msg153.Arg(Msg216.Value());
      PR.SendFail(Msg153);
      break; }
    case IGESData_EntityError: {
      Message_Msg Msg217 ("IGES_217");
      Msg153.Arg(Msg217.Value());
      PR.SendFail(Msg153);
      break; }
    default:{
    }
    }
  }
  if (!PR.ReadReal(PR.Current(),  aStartAngle)){ //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg154("XSTEP_154");
    PR.SendFail(Msg154);
  }
  if (!PR.ReadReal(PR.Current(), anEndAngle)){ //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg155("XSTEP_155");
    PR.SendFail(Msg155);
  }
/*
  st = PR.ReadEntity(IR, PR.Current(), "Axis", STANDARD_TYPE(IGESGeom_Line), anAxis);
  st = PR.ReadEntity(IR, PR.Current(), "Generatrix", aGeneratrix);
  st = PR.ReadReal(PR.Current(), "StartAngle", aStartAngle);
  st = PR.ReadReal(PR.Current(), "EndAngle", anEndAngle);
*/
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(anAxis, aGeneratrix, aStartAngle, anEndAngle);
}

void IGESGeom_ToolSurfaceOfRevolution::WriteOwnParams
  (const Handle(IGESGeom_SurfaceOfRevolution)& ent, IGESData_IGESWriter& IW)  const
{
  IW.Send(ent->AxisOfRevolution());
  IW.Send(ent->Generatrix());
  IW.Send(ent->StartAngle());
  IW.Send(ent->EndAngle());
}

void  IGESGeom_ToolSurfaceOfRevolution::OwnShared
  (const Handle(IGESGeom_SurfaceOfRevolution)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->AxisOfRevolution());
  iter.GetOneItem(ent->Generatrix());
}

void IGESGeom_ToolSurfaceOfRevolution::OwnCopy
  (const Handle(IGESGeom_SurfaceOfRevolution)& another,
   const Handle(IGESGeom_SurfaceOfRevolution)& ent, Interface_CopyTool& TC) const
{
  DeclareAndCast(IGESGeom_Line, anAxis,
		 TC.Transferred(another->AxisOfRevolution()));
  DeclareAndCast(IGESData_IGESEntity, aGeneratrix,
		 TC.Transferred(another->Generatrix()));
  Standard_Real aStartAngle = another->StartAngle();
  Standard_Real anEndAngle = another->EndAngle();

  ent->Init(anAxis, aGeneratrix, aStartAngle, anEndAngle);
}


IGESData_DirChecker IGESGeom_ToolSurfaceOfRevolution::DirChecker
  (const Handle(IGESGeom_SurfaceOfRevolution)& /*ent*/ )   const
{
  IGESData_DirChecker DC(120, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
//  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);

  DC.HierarchyStatusIgnored ();
  return DC;
}

void IGESGeom_ToolSurfaceOfRevolution::OwnCheck
  (const Handle(IGESGeom_SurfaceOfRevolution)& /*ent*/,
   const Interface_ShareTool& , Handle(Interface_Check)& /*ach*/)  const
{
//  Standard_Real diffang = ent->EndAngle() - ent->StartAngle();
//  if (diffang <= 0.0 || diffang > 2.0 * PI)
//    ach.AddFail("0 < TA - SA <=  2Pi is not satisfied");
}

void IGESGeom_ToolSurfaceOfRevolution::OwnDump
  (const Handle(IGESGeom_SurfaceOfRevolution)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level)  const
{

  S << "IGESGeom_SurfaceOfRevolution" << endl << endl;
  Standard_Integer sublevel = (level <= 4) ? 0 : 1;

  S << "Axis Of Revolution : ";
  dumper.Dump(ent->AxisOfRevolution(),S, sublevel);
  S << endl;
  S << "Generatrix         : ";
  dumper.Dump(ent->Generatrix(),S, sublevel);
  S << endl;
  S << "Start Angle        : " << ent->StartAngle() << "  ";
  S << "End Angle   : " << ent->EndAngle() << endl;
}
