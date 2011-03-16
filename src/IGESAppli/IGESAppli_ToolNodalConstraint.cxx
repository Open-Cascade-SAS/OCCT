//--------------------------------------------------------------------
//
//  File Name : IGESAppli_NodalConstraint.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESAppli_ToolNodalConstraint.ixx>
#include <IGESData_ParamCursor.hxx>
#include <IGESDefs_HArray1OfTabularData.hxx>
#include <IGESAppli_Node.hxx>
#include <IGESDefs_TabularData.hxx>
#include <IGESData_Dump.hxx>
#include <Interface_Macros.hxx>


IGESAppli_ToolNodalConstraint::IGESAppli_ToolNodalConstraint ()    {  }


void  IGESAppli_ToolNodalConstraint::ReadOwnParams
  (const Handle(IGESAppli_NodalConstraint)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR)  const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Integer num, i;
  Standard_Integer tempType;
  Handle(IGESAppli_Node) tempNode;
  Handle(IGESDefs_HArray1OfTabularData) tempTabularDataProps;
  if (!PR.ReadInteger(PR.Current(), "Number of cases", num)) num = 0;
  if (num > 0)  tempTabularDataProps = new IGESDefs_HArray1OfTabularData(1, num);
  else  PR.AddFail("Number of cases: Not Positive");
  //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadInteger(PR.Current(), "Type of Constraint", tempType);
  PR.ReadEntity(IR, PR.Current(), "Node", STANDARD_TYPE(IGESAppli_Node), tempNode);

  if (!tempTabularDataProps.IsNull())
    for ( i = 1; i <= num; i++ )
      {
	Handle(IGESDefs_TabularData) tempEntity;
	//szv#4:S4163:12Mar99 moved in if
	if (PR.ReadEntity(IR, PR.Current(), "Tabular Data Property",
			  STANDARD_TYPE(IGESDefs_TabularData), tempEntity))
	  tempTabularDataProps->SetValue(i, tempEntity);
      }
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init (tempType, tempNode, tempTabularDataProps);
}

void  IGESAppli_ToolNodalConstraint::WriteOwnParams
  (const Handle(IGESAppli_NodalConstraint)& ent, IGESData_IGESWriter& IW) const
{
  Standard_Integer i, num;
  IW.Send(ent->NbCases());
  IW.Send(ent->Type());
  IW.Send(ent->NodeEntity());
  for ( num = ent->NbCases(), i = 1; i <= num; i++ )
    IW.Send(ent->TabularData(i));
}

void  IGESAppli_ToolNodalConstraint::OwnShared
  (const Handle(IGESAppli_NodalConstraint)& ent, Interface_EntityIterator& iter) const
{
  Standard_Integer i, num;
  iter.GetOneItem(ent->NodeEntity());
  for ( num = ent->NbCases(), i = 1; i <= num; i++ )
    iter.GetOneItem(ent->TabularData(i));
}

void  IGESAppli_ToolNodalConstraint::OwnCopy
  (const Handle(IGESAppli_NodalConstraint)& another,
   const Handle(IGESAppli_NodalConstraint)& ent, Interface_CopyTool& TC) const
{
  Standard_Integer num = another->NbCases();
  Standard_Integer tempType = another->Type();
  DeclareAndCast(IGESAppli_Node, tempNode,
		 TC.Transferred(another->NodeEntity()));
  Handle(IGESDefs_HArray1OfTabularData) tempTabularDataProps =
    new IGESDefs_HArray1OfTabularData(1, num);
  for ( Standard_Integer i = 1; i <= num; i++ )
    {
      DeclareAndCast(IGESDefs_TabularData, new_item,
		     TC.Transferred(another->TabularData(i)));
      tempTabularDataProps->SetValue(i, new_item);
    }
  ent->Init(tempType, tempNode, tempTabularDataProps);
}

IGESData_DirChecker  IGESAppli_ToolNodalConstraint::DirChecker
  (const Handle(IGESAppli_NodalConstraint)& /* ent */ ) const
{
  IGESData_DirChecker DC(418, 0);
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.HierarchyStatusIgnored();
  return DC;
}

void  IGESAppli_ToolNodalConstraint::OwnCheck
  (const Handle(IGESAppli_NodalConstraint)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  if ((ent->Type() != 1) && (ent->Type() != 2))
    ach->AddFail("Type of Constraint != 1,2");
}

void  IGESAppli_ToolNodalConstraint::OwnDump
  (const Handle(IGESAppli_NodalConstraint)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  Standard_Integer sublevel = (level > 4) ? 1 : 0;
  S << "IGESAppli_NodalConstraint" << endl;
  S << "Type of Constraint : " << ent->Type() << endl;
  S << "Node : ";
  dumper.Dump(ent->NodeEntity(),S, sublevel);
  S << endl;
  S << "Tabular Data Properties : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbCases(),ent->TabularData);
  S << endl;
}
