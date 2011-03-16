//--------------------------------------------------------------------
//
//  File Name : IGESDefs_AssociativityDef.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESDefs_ToolAssociativityDef.ixx>
#include <IGESData_ParamCursor.hxx>
#include <IGESBasic_HArray1OfHArray1OfInteger.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <IGESData_Dump.hxx>
#include <Interface_Macros.hxx>


IGESDefs_ToolAssociativityDef::IGESDefs_ToolAssociativityDef ()    {  }


void  IGESDefs_ToolAssociativityDef::ReadOwnParams
  (const Handle(IGESDefs_AssociativityDef)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{ 
  //Standard_Boolean st; //szv#4:S4163:12Mar99 moved down
  Handle(TColStd_HArray1OfInteger) requirements;
  Handle(TColStd_HArray1OfInteger) orders;
  Handle(TColStd_HArray1OfInteger) numItems;
  Handle(IGESBasic_HArray1OfHArray1OfInteger) items;
  Standard_Integer nbval;

  Standard_Boolean st = PR.ReadInteger(PR.Current(), "No. of Class definitions", nbval);
  if (st && nbval > 0)
    {
      requirements = new TColStd_HArray1OfInteger(1, nbval);
      orders = new TColStd_HArray1OfInteger(1, nbval);
      numItems = new TColStd_HArray1OfInteger(1, nbval);
      items = new IGESBasic_HArray1OfHArray1OfInteger(1, nbval);
    }
  else  PR.AddFail("No. of Class definitions: Not Positive");

  if ( ! requirements.IsNull())
    for (Standard_Integer i = 1; i <= nbval; i++)
      {
	Standard_Integer requirement;
	Standard_Integer order;
	Standard_Integer numItem;
	Handle(TColStd_HArray1OfInteger) item;

	//st = PR.ReadInteger(PR.Current(), "Back Pointer Requirement", requirement); //szv#4:S4163:12Mar99 moved in if
	if (PR.ReadInteger(PR.Current(), "Back Pointer Requirement", requirement))
	  requirements->SetValue(i, requirement);
	
	//st = PR.ReadInteger(PR.Current(), "Ordered/Unordered Class", order); //szv#4:S4163:12Mar99 moved in if
	if (PR.ReadInteger(PR.Current(), "Ordered/Unordered Class", order))
	  orders->SetValue(i, order);
	
	//st = PR.ReadInteger(PR.Current(),"No. of items per entry", numItem); //szv#4:S4163:12Mar99 moved in if
	if (PR.ReadInteger(PR.Current(),"No. of items per entry", numItem)) {
	  Standard_Integer temp;
	  numItems->SetValue(i, numItem);
	  item = new TColStd_HArray1OfInteger(1, numItem);
	  for (Standard_Integer j = 1; j <= numItem; j++)
	    {
	      //Standard_Integer temp; //szv#4:S4163:12Mar99 moved out of for
	      //st = PR.ReadInteger(PR.Current(), "Item", temp); //szv#4:S4163:12Mar99 moved in if
	      if (PR.ReadInteger(PR.Current(), "Item", temp))
		item->SetValue(j, temp);
	    }
	  items->SetValue(i, item);
	}
      }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(requirements, orders, numItems, items);
}

void  IGESDefs_ToolAssociativityDef::WriteOwnParams
  (const Handle(IGESDefs_AssociativityDef)& ent, IGESData_IGESWriter& IW) const 
{ 
  Standard_Integer upper = ent->NbClassDefs();
  IW.Send(upper); 
  for (Standard_Integer i = 1; i <= upper; i++) {
    IW.Send(ent->BackPointerReq(i));
    IW.Send(ent->ClassOrder(i));
    IW.Send(ent->NbItemsPerClass(i));
    Standard_Integer items = ent->NbItemsPerClass(i);
    for (Standard_Integer j = 1; j <= items; j++)
      IW.Send(ent->Item(i,j));
  }
}

void  IGESDefs_ToolAssociativityDef::OwnShared
  (const Handle(IGESDefs_AssociativityDef)& /* ent */, Interface_EntityIterator& /* iter */) const
{
}

void  IGESDefs_ToolAssociativityDef::OwnCopy
  (const Handle(IGESDefs_AssociativityDef)& another,
   const Handle(IGESDefs_AssociativityDef)& ent, Interface_CopyTool& /* TC */) const
{ 

  Handle(TColStd_HArray1OfInteger) requirements;
  Handle(TColStd_HArray1OfInteger) orders;
  Handle(TColStd_HArray1OfInteger) numItems;
  Handle(IGESBasic_HArray1OfHArray1OfInteger) items;

  Standard_Integer nbval = another->NbClassDefs();

  requirements = new TColStd_HArray1OfInteger(1, nbval);
  orders = new TColStd_HArray1OfInteger(1, nbval);
  numItems = new TColStd_HArray1OfInteger(1, nbval);
  items = new IGESBasic_HArray1OfHArray1OfInteger(1, nbval);

  for (Standard_Integer i = 1; i <= nbval; i++)
    {
      Standard_Integer requirement = another->BackPointerReq(i);
      requirements->SetValue(i, requirement);
      Standard_Integer order = another->ClassOrder(i);
      orders->SetValue(i, order);
      Standard_Integer numItem = another->NbItemsPerClass(i);
      numItems->SetValue(i, numItem);
      Handle(TColStd_HArray1OfInteger) item;
      item = new TColStd_HArray1OfInteger(1, numItem);

      for (Standard_Integer j = 1; j <= numItem; j++)
	{
          Standard_Integer temp = another->Item(i, j);
          item->SetValue(j, temp);
	}
      items->SetValue(i, item);
    }
  ent->Init(requirements, orders, numItems, items);
  ent->SetFormNumber (another->FormNumber());
}

IGESData_DirChecker  IGESDefs_ToolAssociativityDef::DirChecker
  (const Handle(IGESDefs_AssociativityDef)& /* ent */ ) const 
{ 
  IGESData_DirChecker DC (302, 5001, 9999);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusRequired(0);
  DC.UseFlagRequired(2);
  DC.HierarchyStatusIgnored();
  return DC;
}

void  IGESDefs_ToolAssociativityDef::OwnCheck
  (const Handle(IGESDefs_AssociativityDef)& /* ent */,
   const Interface_ShareTool& , Handle(Interface_Check)& /* ach */) const 
{
}

void  IGESDefs_ToolAssociativityDef::OwnDump
  (const Handle(IGESDefs_AssociativityDef)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const 
{ 
  S << "IGESDefs_AssociativityDef" << endl;
  S << "Number of Class Definitions : " << ent->NbClassDefs() << endl;
  S << "Back Pointer Requirement  : " << endl; 
  S << "Ordered / Unordered Class : " << endl;
  S << "Number Of Items per Entry : " << endl;
  S << "Items : " << endl; 
  IGESData_DumpVals(S,-level,1, ent->NbClassDefs(),ent->BackPointerReq);
  S << endl;
  if (level > 4)
    {
// Warning : Item is a JAGGED Array
      Standard_Integer upper = ent->NbClassDefs();
      for (Standard_Integer i = 1; i <= upper; i ++) {
	S << "[" << i << "]: " << endl;
	S << "Back Pointer Requirement : "  << ent->BackPointerReq(i) << "  ";
	if (ent->IsBackPointerReq(i)) S << "(Yes)  ";
	else                          S << "(No)   ";
	S << " Ordered/Unordered Class : "  << ent->ClassOrder(i) << " ";
	if (ent->IsOrdered(i)) S << "(Yes)" <<endl;
	else                   S << "(No)"  <<endl;
	S << "Number Of Items per Entry : " << ent->NbItemsPerClass(i);
	if (level < 6) {
	  S << " [ask level > 5 for more]" << endl;
	  continue;
	}
	S << endl << " [";
	for (Standard_Integer j = 1; j <= ent->NbItemsPerClass(i); j ++)
	  S << "  " << ent->Item(i,j);
	S << "]" << endl;
      }
    }
  S << endl;
}

