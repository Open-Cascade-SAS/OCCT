//--------------------------------------------------------------------
//
//  File Name : IGESAppli_PinNumber.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESAppli_ToolPinNumber.ixx>
#include <IGESData_ParamCursor.hxx>
#include <TCollection_HAsciiString.hxx>
#include <IGESData_LevelListEntity.hxx>
#include <Interface_Macros.hxx>
#include <IGESData_Dump.hxx>


IGESAppli_ToolPinNumber::IGESAppli_ToolPinNumber ()    {  }


void  IGESAppli_ToolPinNumber::ReadOwnParams
  (const Handle(IGESAppli_PinNumber)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{
  Standard_Integer tempNbPropertyValues;
  Handle(TCollection_HAsciiString) tempPinNumber;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadInteger(PR.Current(),"Number of property values",tempNbPropertyValues);
  PR.ReadText(PR.Current(),"PinNumber",tempPinNumber);

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempNbPropertyValues,tempPinNumber);
}

void  IGESAppli_ToolPinNumber::WriteOwnParams
  (const Handle(IGESAppli_PinNumber)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->NbPropertyValues());
  IW.Send(ent->PinNumberVal());
}

void  IGESAppli_ToolPinNumber::OwnShared
  (const Handle(IGESAppli_PinNumber)& /* ent */, Interface_EntityIterator& /* iter */) const
{
}

void  IGESAppli_ToolPinNumber::OwnCopy
  (const Handle(IGESAppli_PinNumber)& another,
   const Handle(IGESAppli_PinNumber)& ent, Interface_CopyTool& /* TC */) const
{
  Standard_Integer aNbPropertyValues;
  Handle(TCollection_HAsciiString) aPinNumber =
    new TCollection_HAsciiString(another->PinNumberVal());
  aNbPropertyValues = another->NbPropertyValues();
  ent->Init(aNbPropertyValues,aPinNumber);
}

Standard_Boolean  IGESAppli_ToolPinNumber::OwnCorrect
  (const Handle(IGESAppli_PinNumber)& ent) const
{
  Standard_Boolean res = (ent->SubordinateStatus() != 0);
  if (res) {
    Handle(IGESData_LevelListEntity) nulevel;
    ent->InitLevel(nulevel,0);
  }
  return res;    // RAZ level selon subordibate
}

IGESData_DirChecker  IGESAppli_ToolPinNumber::DirChecker
  (const Handle(IGESAppli_PinNumber)& /* ent */ ) const
{
  IGESData_DirChecker DC(406,8);  //Form no = 8 & Type = 406
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void  IGESAppli_ToolPinNumber::OwnCheck
  (const Handle(IGESAppli_PinNumber)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  if (ent->SubordinateStatus() != 0)
    if (ent->DefLevel() != IGESData_DefOne &&
	ent->DefLevel() != IGESData_DefSeveral)
      ach->AddFail("Level type: Incorrect");
  if (ent->NbPropertyValues() != 1)
    ach->AddFail("Number of Property Values != 1");
  //UNFINISHED
  //Level to be ignored if the property is subordinate -- queried
}

void  IGESAppli_ToolPinNumber::OwnDump
  (const Handle(IGESAppli_PinNumber)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer /* level */) const
{
  S << "IGESAppli_PinNumber" << endl;
  S << "Number of Property Values : " << ent->NbPropertyValues() << endl;
  S << "PinNumber : ";
  IGESData_DumpString(S,ent->PinNumberVal());
  S << endl;
}
