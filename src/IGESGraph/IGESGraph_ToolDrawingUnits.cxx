//--------------------------------------------------------------------
//
//  File Name : IGESGraph_DrawingUnits.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGraph_ToolDrawingUnits.ixx>
#include <IGESData_ParamCursor.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <IGESData_Dump.hxx>


IGESGraph_ToolDrawingUnits::IGESGraph_ToolDrawingUnits ()    {  }


void IGESGraph_ToolDrawingUnits::ReadOwnParams
  (const Handle(IGESGraph_DrawingUnits)& ent,
   const Handle(IGESData_IGESReaderData)& /*IR*/, IGESData_ParamReader& PR) const
{ 
  //Standard_Boolean          st; //szv#4:S4163:12Mar99 not needed

  Standard_Integer          nbPropertyValues;
  Standard_Integer          flag;
  Handle(TCollection_HAsciiString) unit;

  // Reading nbPropertyValues(Integer)
  PR.ReadInteger(PR.Current(), "No. of property values", nbPropertyValues); //szv#4:S4163:12Mar99 `st=` not needed
  if (nbPropertyValues != 2)
    PR.AddFail("No. of Property values : Value is not 2");

  // Reading flag(Integer)
  PR.ReadInteger(PR.Current(), "Units Flag", flag); //szv#4:S4163:12Mar99 `st=` not needed

  // Reading unit(String)
  PR.ReadText(PR.Current(), "Units Name", unit); //szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(nbPropertyValues, flag, unit);
}

void IGESGraph_ToolDrawingUnits::WriteOwnParams
  (const Handle(IGESGraph_DrawingUnits)& ent, IGESData_IGESWriter& IW)  const
{ 
  IW.Send( ent->NbPropertyValues() );
  IW.Send( ent->Flag() );
  IW.Send( ent->Unit() );
}

void  IGESGraph_ToolDrawingUnits::OwnShared
  (const Handle(IGESGraph_DrawingUnits)& /*ent*/, Interface_EntityIterator& /*iter*/) const
{
}

void IGESGraph_ToolDrawingUnits::OwnCopy
  (const Handle(IGESGraph_DrawingUnits)& another,
   const Handle(IGESGraph_DrawingUnits)& ent, Interface_CopyTool& /*TC*/) const
{ 
  Standard_Integer          NbPropertyValues; 
  Standard_Integer          Flag; 
  Handle(TCollection_HAsciiString) Unit; 
 
  NbPropertyValues = another->NbPropertyValues();
  Flag = another->Flag();
  Unit = new TCollection_HAsciiString(another->Unit());

  ent->Init(NbPropertyValues, Flag, Unit);
}

Standard_Boolean  IGESGraph_ToolDrawingUnits::OwnCorrect
  (const Handle(IGESGraph_DrawingUnits)& ent) const
{
  Standard_Boolean res = (ent->NbPropertyValues() != 2);
//   ya aussi les noms : Flag a priorite sur Unit
  Standard_Integer unf = ent->Flag();
  Handle(TCollection_HAsciiString) name;
  Standard_CString unm = "";
  if (!ent->Unit().IsNull()) unm  = ent->Unit()->ToCString();
  switch (unf) {
    case  1 : if (strcmp(unm,"IN") && strcmp(unm,"INCH"))
      name = new TCollection_HAsciiString ("IN");  break;
    case  2 : if (strcmp(unm,"MM"))
      name = new TCollection_HAsciiString ("MM");  break;
    case  3 : break;  // nom libre
    case  4 : if (strcmp(unm,"FT"))
      name = new TCollection_HAsciiString ("FT");  break;
    case  5 : if (strcmp(unm,"MI"))
      name = new TCollection_HAsciiString ("MI");  break;
    case  6 : if (strcmp(unm,"M"))
      name = new TCollection_HAsciiString ("M");   break;
    case  7 : if (strcmp(unm,"KM"))
      name = new TCollection_HAsciiString ("KM");  break;
    case  8 : if (strcmp(unm,"MIL"))
      name = new TCollection_HAsciiString ("MIL"); break;
    case  9 : if (strcmp(unm,"UM"))
      name = new TCollection_HAsciiString ("UM");  break;
    case 10 : if (strcmp(unm,"CM"))
      name = new TCollection_HAsciiString ("CM");  break;
    case 11 : if (strcmp(unm,"UIN"))
      name = new TCollection_HAsciiString ("UIN"); break;
    default : break;    // on ne peut rien faire ... ?
  }

  res |= (!name.IsNull());
  if (name.IsNull()) name = ent->Unit();
  if (res) ent->Init(2,unf,name);    // nbpropertyvalues=2 + Unit Flag//Name
  return res;
}

IGESData_DirChecker IGESGraph_ToolDrawingUnits::DirChecker
  (const Handle(IGESGraph_DrawingUnits)& /*ent*/)  const
{ 
  IGESData_DirChecker DC (406, 17);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESGraph_ToolDrawingUnits::OwnCheck
  (const Handle(IGESGraph_DrawingUnits)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach)  const
{
  if (ent->NbPropertyValues() != 2)
    ach->AddFail("No. of Property values : Value != 2");
//    Check Flag//Unit Name
  Standard_Integer unf = ent->Flag();
  if (ent->Unit().IsNull()) {
    if (unf == 3) ach->AddFail
      ("Unit Flag = 3 (user def.) and Unit Name undefined");
  } else {
    Standard_CString unm  = ent->Unit()->ToCString();
    Standard_Boolean unok = Standard_True;
    switch (unf) {
      case  1 : unok = (!strcmp(unm,"IN") || !strcmp(unm,"INCH"));  break;
      case  2 : unok = !strcmp(unm,"MM");   break;
      case  3 : unok = Standard_True;       break;  // nom libre
      case  4 : unok = !strcmp(unm,"FT");   break;
      case  5 : unok = !strcmp(unm,"MI");   break;
      case  6 : unok = !strcmp(unm,"M");    break;
      case  7 : unok = !strcmp(unm,"KM");   break;
      case  8 : unok = !strcmp(unm,"MIL");  break;
      case  9 : unok = !strcmp(unm,"UM");   break;
      case 10 : unok = !strcmp(unm,"CM");   break;
      case 11 : unok = !strcmp(unm,"UIN");  break;
      default : ach->AddFail("Unit Flag not in range 1 - 11");
	break;
    }
    if (!unok) ach->AddFail("Unit Flag & Name not accorded");
  }
}

void IGESGraph_ToolDrawingUnits::OwnDump
  (const Handle(IGESGraph_DrawingUnits)& ent, const IGESData_IGESDumper& /*dumper*/,
   const Handle(Message_Messenger)& S, const Standard_Integer /*level*/)  const
{
  S << "IGESGraph_DrawingUnits" << endl;

  S << "No. of property values : " << ent->NbPropertyValues() << endl;
  S << "  Units Flag : " << ent->Flag();
  S << "  Units Name : ";
  IGESData_DumpString(S,ent->Unit());
  S << "  computed Value (in meters) : " << ent->UnitValue();
  S << endl;
}
