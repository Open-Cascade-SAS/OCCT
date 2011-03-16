//--------------------------------------------------------------------
//
//  File Name : IGESDimen_CenterLine.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESDimen_ToolCenterLine.ixx>
#include <IGESData_ParamCursor.hxx>
#include <gp_XY.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_HArray1OfXY.hxx>
#include <IGESData_LineFontEntity.hxx>
#include <IGESData_Dump.hxx>
#include <Interface_Macros.hxx>


IGESDimen_ToolCenterLine::IGESDimen_ToolCenterLine ()    {  }


void  IGESDimen_ToolCenterLine::ReadOwnParams
  (const Handle(IGESDimen_CenterLine)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{ 
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  Standard_Integer datatype;
  Standard_Real zDisplacement; 
  Standard_Integer nbval;
  Handle(TColgp_HArray1OfXY) dataPoints;

  PR.ReadInteger(PR.Current(), "Interpretation Flag", datatype); //szv#4:S4163:12Mar99 `st=` not needed

  Standard_Boolean st = PR.ReadInteger(PR.Current(), "Number of data points", nbval);
  if (st && nbval > 0)  dataPoints = new TColgp_HArray1OfXY(1, nbval);
  else  PR.AddFail("Number of data points: Not Positive");

  PR.ReadReal(PR.Current(), "Common Z Displacement", zDisplacement); //szv#4:S4163:12Mar99 `st=` not needed

  if (! dataPoints.IsNull())
    for (Standard_Integer i = 1; i <= nbval; i++)
      {
	gp_XY tempXY;
	//st = PR.ReadXY(PR.CurrentList(1, 2), "Data Points", tempXY); //szv#4:S4163:12Mar99 moved in if
	if (PR.ReadXY(PR.CurrentList(1, 2), "Data Points", tempXY))
	  dataPoints->SetValue(i, tempXY);
      }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(datatype, zDisplacement, dataPoints);
}

void  IGESDimen_ToolCenterLine::WriteOwnParams
  (const Handle(IGESDimen_CenterLine)& ent, IGESData_IGESWriter& IW) const 
{ 
  Standard_Integer upper = ent->NbPoints();
  IW.Send(ent->Datatype());
  IW.Send(upper);
  IW.Send(ent->ZDisplacement());
  for (Standard_Integer i = 1; i <= upper; i++)
    {
      IW.Send((ent->Point(i)).X());
      IW.Send((ent->Point(i)).Y());
    }
}

void  IGESDimen_ToolCenterLine::OwnShared
  (const Handle(IGESDimen_CenterLine)& /* ent */, Interface_EntityIterator& /* iter */) const
{
}

void  IGESDimen_ToolCenterLine::OwnCopy
  (const Handle(IGESDimen_CenterLine)& another,
   const Handle(IGESDimen_CenterLine)& ent, Interface_CopyTool& /* TC */) const
{
  Standard_Integer datatype = another->Datatype();
  Standard_Integer nbval = another->NbPoints();
  Standard_Real zDisplacement = another->ZDisplacement();

  Handle(TColgp_HArray1OfXY) dataPoints = new TColgp_HArray1OfXY(1, nbval);

  for (Standard_Integer i = 1; i <= nbval; i++)
    {
      gp_Pnt tempPnt = (another->Point(i));
      gp_XY tempPnt2d(tempPnt.X(), tempPnt.Y());
      dataPoints->SetValue(i, tempPnt2d);
    }
  ent->Init(datatype, zDisplacement, dataPoints);
  ent->SetCrossHair (another->IsCrossHair());
}

Standard_Boolean  IGESDimen_ToolCenterLine::OwnCorrect
  (const Handle(IGESDimen_CenterLine)& ent) const
{
  Standard_Boolean res = (ent->RankLineFont() != 1);
  if (res) {
    Handle(IGESData_LineFontEntity) nulfont;
    ent->InitLineFont(nulfont,1);
  }
  if (ent->Datatype() == 1) return res;
//  Forcer DataType = 1 -> reconstruire
  Standard_Integer nb = ent->NbPoints();
  if (nb == 0) return res;  // rien pu faire (est-ce possible ?)
  Handle(TColgp_HArray1OfXY) pts = new TColgp_HArray1OfXY(1,nb);
  for (Standard_Integer i = 1; i <= nb; i ++)
    pts->SetValue(i, gp_XY(ent->Point(i).X(),ent->Point(i).Y()) );
  ent->Init (1,ent->ZDisplacement(),pts);
  return Standard_True;
}

IGESData_DirChecker  IGESDimen_ToolCenterLine::DirChecker
  (const Handle(IGESDimen_CenterLine)& /* ent */) const 
{ 
  IGESData_DirChecker DC (106, 20, 21);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefValue);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.UseFlagRequired(1);
  DC.HierarchyStatusIgnored();
  return DC;
}

void  IGESDimen_ToolCenterLine::OwnCheck
  (const Handle(IGESDimen_CenterLine)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const 
{
  if (ent->RankLineFont() != 1)
    ach->AddFail("Line Font Pattern != 1");
  if (ent->Datatype() != 1)
    ach->AddFail("Interpretation Flag != 1");
  if (ent->NbPoints()%2 != 0)
    ach->AddFail("Number of data points is not even");
}

void  IGESDimen_ToolCenterLine::OwnDump
  (const Handle(IGESDimen_CenterLine)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const 
{ 
  S << "IGESDimen_CenterLine" << endl;
  if (ent->IsCrossHair()) S << "Cross Hair" << endl;
  else                    S << "Through Circle Centers" << endl;
  S << "Data Type : "             << ent->Datatype() << "  ";
  S << "Number of Data Points : " << ent->NbPoints()  << "  ";
  S << "Common Z displacement : " << ent->ZDisplacement() << "  ";
  S << "Data Points : " << endl;
  IGESData_DumpListXYLZ(S ,level,1, ent->NbPoints(),ent->Point,
			ent->Location(), ent->ZDisplacement());
  S << endl;
}
