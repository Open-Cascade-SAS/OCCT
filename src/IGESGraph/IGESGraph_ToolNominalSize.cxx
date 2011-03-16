//--------------------------------------------------------------------
//
//  File Name : IGESGraph_NominalSize.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGraph_ToolNominalSize.ixx>
#include <IGESData_ParamCursor.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <IGESData_Dump.hxx>


IGESGraph_ToolNominalSize::IGESGraph_ToolNominalSize ()    {  }


void IGESGraph_ToolNominalSize::ReadOwnParams
  (const Handle(IGESGraph_NominalSize)& ent,
   const Handle(IGESData_IGESReaderData)& /*IR*/, IGESData_ParamReader& PR) const
{ 
  //Standard_Boolean          st; //szv#4:S4163:12Mar99 not needed

  Standard_Integer          nbPropertyValues;
  Standard_Real             nominalSizeValue; 
  Handle(TCollection_HAsciiString) nominalSizeName; 
  Handle(TCollection_HAsciiString) standardName; 

  // Reading nbPropertyValues(Integer)
  PR.ReadInteger(PR.Current(), "No. of property values", nbPropertyValues); //szv#4:S4163:12Mar99 `st=` not needed
  if ( (nbPropertyValues != 2) && (nbPropertyValues != 3) )
    PR.AddFail("No. of Property values : Value is not 2/3");

  // Reading nominalSizeValue(Real)
  PR.ReadReal (PR.Current(), "Nominal size value", nominalSizeValue); //szv#4:S4163:12Mar99 `st=` not needed

  // Reading nominalSizeName(String)
  PR.ReadText (PR.Current(), "Nominal size name", nominalSizeName); //szv#4:S4163:12Mar99 `st=` not needed

  if ( PR.NbParams() >= PR.CurrentNumber() )
    {
      Standard_Integer num = PR.CurrentNumber(); 
      if ( PR.ParamType(num) == Interface_ParamText )
	// Reading standardName(String)
	PR.ReadText (PR.Current(), "Name of relevant engg. standard", 
		     standardName); //szv#4:S4163:12Mar99 `st=` not needed
    }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init
    (nbPropertyValues, nominalSizeValue, nominalSizeName,  standardName);
}

void IGESGraph_ToolNominalSize::WriteOwnParams
  (const Handle(IGESGraph_NominalSize)& ent, IGESData_IGESWriter& IW)  const
{ 
  IW.Send( ent->NbPropertyValues() );
  IW.Send( ent->NominalSizeValue() );
  IW.Send( ent->NominalSizeName() );

  if (ent->HasStandardName() )
    IW.Send( ent->StandardName() );  // optionnal
}

void  IGESGraph_ToolNominalSize::OwnShared
  (const Handle(IGESGraph_NominalSize)& /*ent*/, Interface_EntityIterator& /*iter*/) const
{
}

void IGESGraph_ToolNominalSize::OwnCopy
  (const Handle(IGESGraph_NominalSize)& another,
   const Handle(IGESGraph_NominalSize)& ent, Interface_CopyTool& /*TC*/) const
{ 
  Standard_Integer          nbPropertyValues; 
  Standard_Real             nominalSizeValue; 
  Handle(TCollection_HAsciiString) nominalSizeName; 
  Handle(TCollection_HAsciiString) standardName;

  nbPropertyValues = another->NbPropertyValues();
  nominalSizeValue = another->NominalSizeValue();
  nominalSizeName  = new TCollection_HAsciiString(another->NominalSizeName());
  if (another->HasStandardName()) standardName     =
    new TCollection_HAsciiString(another->StandardName());

  ent->Init(nbPropertyValues, nominalSizeValue, nominalSizeName, standardName);
}

Standard_Boolean  IGESGraph_ToolNominalSize::OwnCorrect
  (const Handle(IGESGraph_NominalSize)& ent) const
{
  Standard_Integer nbp = 2;
  if (ent->HasStandardName()) nbp = 3;
  Standard_Boolean res = ( ent->NbPropertyValues() != nbp);
  if (res) ent->Init
    (nbp,ent->NominalSizeValue(),ent->NominalSizeName(),ent->StandardName());
  return res;    // nbpropertyvalues=2/3 selon standard
}

IGESData_DirChecker IGESGraph_ToolNominalSize::DirChecker
  (const Handle(IGESGraph_NominalSize)& /*ent*/)  const
{ 
  IGESData_DirChecker DC (406, 13);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESGraph_ToolNominalSize::OwnCheck
  (const Handle(IGESGraph_NominalSize)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach)  const
{
  Standard_Integer nbp = 2;
  if (ent->HasStandardName()) nbp = 3;
  if ( ent->NbPropertyValues() != nbp)  ach->AddFail
    ("No. of Property values : Value != 2/3 according Standard Name Status");
}

void IGESGraph_ToolNominalSize::OwnDump
  (const Handle(IGESGraph_NominalSize)& ent, const IGESData_IGESDumper& /*dumper*/,
   const Handle(Message_Messenger)& S, const Standard_Integer /*level*/)  const
{
  S << "IGESGraph_NominalSize" << endl;

  S << "No. of property values : " << ent->NbPropertyValues() << endl;
  S << "Nominal size value : "     << ent->NominalSizeValue() << endl;
  S << "Nominal size name  : ";
  IGESData_DumpString(S,ent->NominalSizeName());
  S << endl;
  S << "Name of relevant engineering standard : ";
  IGESData_DumpString(S,ent->StandardName());
  S << endl;
}
