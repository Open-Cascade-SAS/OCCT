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

#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDefs_AttributeDef.hxx>
#include <IGESDefs_AttributeTable.hxx>
#include <IGESDefs_ToolAttributeTable.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfTransient.hxx>

IGESDefs_ToolAttributeTable::IGESDefs_ToolAttributeTable ()    {  }


void  IGESDefs_ToolAttributeTable::ReadOwnParams
  (const Handle(IGESDefs_AttributeTable)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{ 
  Standard_Integer nr = 1;
  Standard_Integer j;
  Standard_Boolean st = Standard_True;

  Handle(IGESDefs_AttributeDef) ab = ent->Definition();  // formerly loaded
  Handle(TColStd_HArray2OfTransient) list2;
  if (ab.IsNull()) {
    PR.AddFail("No Attribute Definition as Structure");
    return;
  }
  Standard_Integer na = ab->NbAttributes();

  if (ent->FormNumber() == 1)
    st = PR.ReadInteger(PR.Current(),"No. of rows",nr);
  if (st) list2 = new TColStd_HArray2OfTransient(1,na,1,nr);

//  AttributeDef repeated once (Form 0) or <nr> times (Form 1)
  for (Standard_Integer k = 1; k <= nr; k ++)
    {
      for (Standard_Integer i = 1; i <= na; i ++)
	{
	  Standard_Integer avc   = ab->AttributeValueCount(i);
	  Standard_Integer atype = ab->AttributeValueDataType(i);
	  switch (atype)
	    {
            case 0 : 
	      for (j = 1; j <= avc; j ++)
		PR.SetCurrentNumber(PR.CurrentNumber() + 1);  // skip
	      break;
            case 1 : {
	      Handle(TColStd_HArray1OfInteger) attrInt = new TColStd_HArray1OfInteger(1,avc);
	      list2->SetValue(i,k,attrInt);
	      Standard_Integer item;
	      for (j = 1; j <= avc; j ++)  {
		//st = PR.ReadInteger(PR.Current(),"Value",item); //szv#4:S4163:12Mar99 moved in if
		if (PR.ReadInteger(PR.Current(),"Value",item))
		  attrInt->SetValue(j,item);
	      }
	    }
	      break;
            case 2 : {
	      Handle(TColStd_HArray1OfReal) attrReal = new TColStd_HArray1OfReal(1,avc);
	      list2->SetValue(i,k,attrReal);
	      Standard_Real item;
	      for (j = 1; j <= avc; j ++) {
		//st = PR.ReadReal(PR.Current(),"Value",item); //szv#4:S4163:12Mar99 moved in if
		if (PR.ReadReal(PR.Current(),"Value",item))
		  attrReal->SetValue(j,item);
	      }
	    }
	      break;
	    case 3 : {
	      Handle(Interface_HArray1OfHAsciiString) attrStr =	new Interface_HArray1OfHAsciiString(1,avc);
	      list2->SetValue(i,k,attrStr);
	      Handle(TCollection_HAsciiString) item;
	      for (j = 1; j <= avc; j ++) {
		//st = PR.ReadText(PR.Current(),"Value",item); //szv#4:S4163:12Mar99 moved in if
		if (PR.ReadText(PR.Current(),"Value",item))
		  attrStr->SetValue(j,item);
	      }
	    }
	      break;
	    case 4 : {
	      Handle(IGESData_HArray1OfIGESEntity) attrEnt = new IGESData_HArray1OfIGESEntity(1,avc);
	      list2->SetValue(i,k,attrEnt);
	      Handle(IGESData_IGESEntity) item;
	      for (j = 1; j <= avc; j ++) {
		//st = PR.ReadEntity(IR,PR.Current(),"Value",item); //szv#4:S4163:12Mar99 moved in if
		if (PR.ReadEntity(IR,PR.Current(),"Value",item))
		  attrEnt->SetValue(j,item);
	      }
	    }
	      break;
	    case 5 :
	      for (j = 1; j <= avc; j ++)
		  PR.SetCurrentNumber(PR.CurrentNumber() + 1);  // skip
	      break;
	    case 6 : {    // Here item takes value 0 or 1
	      Handle(TColStd_HArray1OfInteger) attrInt  = new TColStd_HArray1OfInteger(1,avc);
	      list2->SetValue(i,k,attrInt);
	      Standard_Integer item;
	      for (j = 1; j <= avc; j ++) {
		  //st = PR.ReadInteger(PR.Current(),"Value",item); //szv#4:S4163:12Mar99 moved in if
		  if (PR.ReadInteger(PR.Current(),"Value",item))
		    attrInt->SetValue(j,item);
		}
	    }
	      break;
              default : break;
	    }
	}
    }
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(list2);
}

void  IGESDefs_ToolAttributeTable::WriteOwnParams
  (const Handle(IGESDefs_AttributeTable)& ent, IGESData_IGESWriter& IW) const 
{
  Handle(IGESDefs_AttributeDef) ab = ent->Definition();

  Standard_Integer nr = ent->NbRows();
  Standard_Integer na = ent->NbAttributes();
  if (ent->FormNumber() == 1) IW.Send(nr);
  for (Standard_Integer k = 1; k <= nr; k ++)
    {
      for (Standard_Integer i = 1; i <= na; i ++)
	{
          Standard_Integer count = ab->AttributeValueCount(i);
	  for (Standard_Integer j = 1;j <= count; j++)
	    {
	      switch(ab->AttributeValueDataType(i))
		{
		case 0 : IW.SendVoid();  break;
		case 1 : IW.Send(ent->AttributeAsInteger(i,k,j));  break;
		case 2 : IW.Send(ent->AttributeAsReal   (i,k,j));  break;
		case 3 : IW.Send(ent->AttributeAsString (i,k,j));  break;
		case 4 : IW.Send(ent->AttributeAsEntity (i,k,j));  break;
		case 5 : IW.SendVoid();  break;
		case 6 : IW.SendBoolean(ent->AttributeAsLogical(i,k,j)); break;
		default : break;
		}
	    }
	}
    }
}

void  IGESDefs_ToolAttributeTable::OwnShared
  (const Handle(IGESDefs_AttributeTable)& ent, Interface_EntityIterator& iter) const
{
  Handle(IGESDefs_AttributeDef) ab = ent->Definition();
  Standard_Integer na = ent->NbAttributes();
  Standard_Integer nr = ent->NbRows();
  for (Standard_Integer k = 1; k <= nr; k ++)
    {
      for (Standard_Integer i = 1; i <= na; i ++)
	{
	  if (ab->AttributeValueDataType(i) != 4) continue;
          Standard_Integer avc = ab->AttributeValueCount(i);
	  for (Standard_Integer j = 1; j <= avc; j ++)
	    iter.GetOneItem(ent->AttributeAsEntity(i,k,j));
	}
    }
}

void  IGESDefs_ToolAttributeTable::OwnCopy
  (const Handle(IGESDefs_AttributeTable)& another,
   const Handle(IGESDefs_AttributeTable)& ent, Interface_CopyTool& TC) const
{ 
  Standard_Integer j = 1;
  Handle(IGESDefs_AttributeDef) ab = another->Definition();
  Standard_Integer na = another->NbAttributes();
  Standard_Integer nr = another->NbRows();
  Handle(TColStd_HArray2OfTransient) list2 =
    new TColStd_HArray2OfTransient(1,na,1,nr);
  for (Standard_Integer k = 1; k <= nr; k ++)
    {
      for (Standard_Integer i = 1; i <= na; i ++)
	{
	  Standard_Integer avc   = ab->AttributeValueCount(i);
	  Standard_Integer atype = ab->AttributeValueDataType(i);
	  switch (atype)
	    {
            case 0 : ////    list2->SetValue(i,k,NULL);    par defaut
	      break;
            case 1 : {
	      DeclareAndCast(TColStd_HArray1OfInteger,otherInt,
			     another->AttributeList(i,k));
	      Handle(TColStd_HArray1OfInteger) attrInt  =
		new TColStd_HArray1OfInteger (1,avc);
	      list2->SetValue(i,k,attrInt);
	      for (j = 1; j <= avc; j++)
		attrInt->SetValue(j,otherInt->Value(j));
	    }
	      break;
            case 2 : {
	      DeclareAndCast(TColStd_HArray1OfReal,otherReal,
			     another->AttributeList(i,k));
	      Handle(TColStd_HArray1OfReal) attrReal  =
		new TColStd_HArray1OfReal (1,avc);
	      list2->SetValue(i,k,attrReal);
	      for (j = 1; j <= avc; j++)
		attrReal->SetValue(j,otherReal->Value(j));
	    }
	      break;
	    case 3 : {
	      DeclareAndCast(Interface_HArray1OfHAsciiString,otherStr,
			     another->AttributeList(i,k));
	      Handle(Interface_HArray1OfHAsciiString) attrStr  =
		new Interface_HArray1OfHAsciiString (1,avc);
	      list2->SetValue(i,k,attrStr);
	      for (j = 1; j <= avc; j++)
		attrStr->SetValue
		  (j,new TCollection_HAsciiString(otherStr->Value(j)));
	    }
	      break;
	    case 4 : {
	      DeclareAndCast(IGESData_HArray1OfIGESEntity,otherEnt,
			     another->AttributeList(i,k));
	      Handle(IGESData_HArray1OfIGESEntity) attrEnt  =
		new IGESData_HArray1OfIGESEntity (1,avc);
	      list2->SetValue(i,k,attrEnt);
	      for (j = 1; j <= avc; j++)
		attrEnt->SetValue(j,GetCasted(IGESData_IGESEntity,
					      TC.Transferred(otherEnt->Value(j))));
	    }
	      break;
	    case 5 : /////	      list2->SetValue(i,k,NULL);    par defaut
	      break;
	    case 6 :{    // Here item takes value 0 or 1
	      DeclareAndCast(TColStd_HArray1OfInteger,otherInt,
			     another->AttributeList(i,k));
	      Handle(TColStd_HArray1OfInteger) attrInt  =
		new TColStd_HArray1OfInteger (1,avc);
	      list2->SetValue(i,k,attrInt);
	      for (j = 1; j <= avc; j++)
		attrInt->SetValue(j,otherInt->Value(j));
	    }
	      break;
              default : break;
	    }
	}
    }
  ent->Init(list2);
}

IGESData_DirChecker  IGESDefs_ToolAttributeTable::DirChecker
  (const Handle(IGESDefs_AttributeTable)& /* ent */ ) const 
{ 
  IGESData_DirChecker DC(422,0,1);
  DC.Structure(IGESData_DefReference);
  DC.GraphicsIgnored();
  DC.BlankStatusIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void  IGESDefs_ToolAttributeTable::OwnCheck
  (const Handle(IGESDefs_AttributeTable)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const 
{
  if (ent->Definition().IsNull()) {
    if (ent->HasStructure()) ach->AddFail
      ("Structure in Directory Entry is not an Attribute Definition Table");
    else ach->AddFail("No Attribute Definition defined");
  }
  if (ent->FormNumber() == 0 && ent->NbRows() != 1)
    ach->AddFail("Form 0 with several Rows");
  if (ent->NbAttributes() != ent->Definition()->NbAttributes())
    ach->AddFail("Mismatch between Definition (Structure) and Content");
}

void  IGESDefs_ToolAttributeTable::OwnDump
  (const Handle(IGESDefs_AttributeTable)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{ 
  S << "IGESDefs_AttributeTable" << endl;

  Handle(IGESDefs_AttributeDef) ab = ent->Definition();

  Standard_Integer na = ent->NbAttributes();
  Standard_Integer nr = ent->NbRows();
  if (ent->FormNumber() == 1)
    S << "Number of Rows (i.e. complete sets of Attributes) : " << nr << endl;
  else S << "One set of Attributes" << endl;
  S << "Number of defined Attributes : " << na << endl;
  if (level <= 4) S <<
    " [ structure : see Structure in Directory Entry; content : level > 4 ]" <<endl;
  else
    for (Standard_Integer k = 1; k <= nr; k ++)
      {
	for (Standard_Integer i = 1; i <= na; i ++)
	  {
	    Standard_Integer avc = ab->AttributeValueCount(i);
	    S << "[At.no."<<i<<" Row:"<<k<<"]";
	    switch (ab->AttributeValueDataType(i)) {
	      case 0 : S << "  (Void) ";   break;
	      case 1 : S << "  Integer";  break;
	      case 2 : S << "  Real   ";  break;
	      case 3 : S << "  String ";  break;
	      case 4 : S << "  Entity ";  break;
	      case 5 : S << " (Not used)"; break;
	      case 6 : S << "  Logical";  break;
	      default : break;
	    }
	    S << " :";
	    for (Standard_Integer j = 1;j <= avc; j++) {
	      S << "  ";
	      switch(ab->AttributeValueDataType(i)) {
		case 1 : S << ent->AttributeAsInteger(i,k,j);  break;
		case 2 : S << ent->AttributeAsReal   (i,k,j);  break;
		case 3 : IGESData_DumpString(S,ent->AttributeAsString (i,k,j));
		  break;
		case 4 : dumper.Dump(ent->AttributeAsEntity (i,k,j),S,level-5);
		  break;
		case 6 : S << (ent->AttributeAsLogical(i,k,j) ? "True" : "False");
		  break;
		default : break;
	      }
	    }
	    S << endl;
	  }
      }
  S << endl;
}
