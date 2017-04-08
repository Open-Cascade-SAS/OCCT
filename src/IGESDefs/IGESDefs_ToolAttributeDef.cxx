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
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDefs_AttributeDef.hxx>
#include <IGESDefs_HArray1OfHArray1OfTextDisplayTemplate.hxx>
#include <IGESDefs_ToolAttributeDef.hxx>
#include <IGESGraph_HArray1OfTextDisplayTemplate.hxx>
#include <IGESGraph_TextDisplayTemplate.hxx>
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
#include <TColStd_HArray1OfTransient.hxx>

#include <stdio.h>
IGESDefs_ToolAttributeDef::IGESDefs_ToolAttributeDef ()    {  }


void IGESDefs_ToolAttributeDef::ReadOwnParams
  (const Handle(IGESDefs_AttributeDef)& ent,
  const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 moved down
  Handle(TCollection_HAsciiString) aName;
  Standard_Integer aListType;
  Handle(TColStd_HArray1OfInteger)    attrTypes;
  Handle(TColStd_HArray1OfInteger)    attrValueDataTypes;
  Handle(TColStd_HArray1OfInteger)    attrValueCounts;
  Handle(TColStd_HArray1OfTransient) attrValues;
  Handle(IGESDefs_HArray1OfHArray1OfTextDisplayTemplate) attrValuePointers;
  Standard_Integer nbval;
  Standard_Integer fn = ent->FormNumber();

  if (PR.DefinedElseSkip())
    PR.ReadText(PR.Current(), "Attribute Table Name", aName); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadInteger(PR.Current(), "Attribute List Type", aListType); //szv#4:S4163:12Mar99 `st=` not needed

  Standard_Boolean st = PR.ReadInteger(PR.Current(), "Number of Attributes", nbval);
  if (st && nbval > 0)
    {
      attrTypes          = new TColStd_HArray1OfInteger(1, nbval);
      attrValueDataTypes = new TColStd_HArray1OfInteger(1, nbval);
      attrValueCounts    = new TColStd_HArray1OfInteger(1, nbval);
      if (fn > 0)  attrValues =
	new TColStd_HArray1OfTransient(1, nbval);
      if (fn > 1)  attrValuePointers =
	new IGESDefs_HArray1OfHArray1OfTextDisplayTemplate(1, nbval);
    }
  else  PR.AddFail("Number of Attributes: Not Positive");

  if ( ! attrTypes.IsNull())
    for (Standard_Integer i = 1; i <= nbval; i++)
      {
	Standard_Integer attrType;
	Standard_Integer attrValueDataType;
	Standard_Integer avc;
//  Value according type
	Handle(IGESGraph_HArray1OfTextDisplayTemplate) attrValuePointer;

	//st = PR.ReadInteger(PR.Current(), "Attribute Type", attrType); //szv#4:S4163:12Mar99 moved in if
	if (PR.ReadInteger(PR.Current(), "Attribute Type", attrType))
	  attrTypes->SetValue(i, attrType);

	st = PR.ReadInteger(PR.Current(),"Attribute Data Type",attrValueDataType);
	if (st) attrValueDataTypes->SetValue(i, attrValueDataType);

	if (PR.DefinedElseSkip())
	  st = PR.ReadInteger (PR.Current(),"Attribute Value Count",avc);
	else  avc = 1;

	if (st) {
	  attrValueCounts->SetValue(i, avc);
	  if (fn > 1)  attrValuePointer =
	    new IGESGraph_HArray1OfTextDisplayTemplate(1, avc);
	}

	if (! attrValues.IsNull())
	  if (fn > 0)
	    {
	      Handle(TColStd_HArray1OfInteger)        attrInt;
	      Handle(TColStd_HArray1OfReal)           attrReal;
	      Handle(Interface_HArray1OfHAsciiString) attrStr;
	      Handle(IGESData_HArray1OfIGESEntity)    attrEnt;
	      switch (attrValueDataType) {
	        case 1 : attrInt  = new TColStd_HArray1OfInteger       (1,avc);
		  attrValues->SetValue(i,attrInt);  break;
	        case 2 : attrReal = new TColStd_HArray1OfReal          (1,avc);
		  attrValues->SetValue(i,attrReal); break;
	        case 3 : attrStr  = new Interface_HArray1OfHAsciiString(1,avc);
		  attrValues->SetValue(i,attrStr);  break;
	        case 4 : attrEnt  = new IGESData_HArray1OfIGESEntity   (1,avc);
		  attrValues->SetValue(i,attrEnt);  break;
	        case 6 : attrInt  = new TColStd_HArray1OfInteger       (1,avc);
		  attrValues->SetValue(i,attrInt);  break;
		default : break;
	      }
	      for (Standard_Integer j = 1; j <= avc; j++)
		{
		  switch(attrValueDataType)
		    {
		    case 0:
		      {
			PR.SetCurrentNumber(PR.CurrentNumber() + 1); // passer
////			attrValue->SetValue(j, NULL);    par defaut
			break;
		      }
		    case 1:
		      {
			Standard_Integer temp;
			//st = PR.ReadInteger(PR.Current(), "Attribute Value", temp); //szv#4:S4163:12Mar99 moved in if
			if (PR.ReadInteger(PR.Current(), "Attribute Value", temp))
			  attrInt->SetValue(j, temp);
		      }
		      break;
		    case 2:
		      {
			Standard_Real temp;
			//st = PR.ReadReal(PR.Current(), "Attribute Value", temp); //szv#4:S4163:12Mar99 moved in if
			if (PR.ReadReal(PR.Current(), "Attribute Value", temp))
			  attrReal->SetValue(j, temp);
		      }
		      break;
		    case 3:
		      {
			Handle(TCollection_HAsciiString) temp;
			//st = PR.ReadText(PR.Current(), "Attribute Value", temp); //szv#4:S4163:12Mar99 moved in if
			if (PR.ReadText(PR.Current(), "Attribute Value", temp))
			  attrStr->SetValue(j, temp);
		      }
		      break;
		    case 4:
		      {
			Handle(IGESData_IGESEntity) temp;
			//st = PR.ReadEntity(IR, PR.Current(), "Attribute Value", temp); //szv#4:S4163:12Mar99 moved in if
			if (PR.ReadEntity(IR, PR.Current(), "Attribute Value", temp))
			  attrEnt->SetValue(j, temp);
		      }
		      break;
		    case 5:
			PR.SetCurrentNumber(PR.CurrentNumber() + 1);  // skip
		      break;
		    case 6:
		      {
			Standard_Boolean temp;
			//st = PR.ReadBoolean(PR.Current(), "Attribute Value", temp); //szv#4:S4163:12Mar99 moved in if
			if (PR.ReadBoolean(PR.Current(), "Attribute Value", temp))
			  attrInt->SetValue(j, (temp ? 1 : 0));
		      }
		      break;
		    }
		  if (fn == 2)
		    {
		      Handle(IGESGraph_TextDisplayTemplate) tempText;
		      //st = PR.ReadEntity(IR,PR.Current(),"Attribute Val. Pointer",
					 //STANDARD_TYPE(IGESGraph_TextDisplayTemplate), tempText); //szv#4:S4163:12Mar99 moved in if
		      if (PR.ReadEntity(IR,PR.Current(),"Attribute Val. Pointer",
					STANDARD_TYPE(IGESGraph_TextDisplayTemplate), tempText))
			attrValuePointer->SetValue(j, tempText);
		    }
		}
	      if (fn == 2)
		attrValuePointers->SetValue(i, attrValuePointer);
	    }
      }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init
    (aName, aListType, attrTypes, attrValueDataTypes, attrValueCounts,
     attrValues, attrValuePointers);
}

void IGESDefs_ToolAttributeDef::WriteOwnParams
  (const Handle(IGESDefs_AttributeDef)& ent, IGESData_IGESWriter& IW) const
{
  if (ent->HasTableName()) IW.Send(ent->TableName());
  else                     IW.SendVoid();
  IW.Send(ent->ListType());
  Standard_Integer upper = ent->NbAttributes();
  IW.Send(upper);

  for (Standard_Integer i = 1; i <= upper; i++)
    {
      Standard_Integer check = ent->AttributeValueDataType(i);
      Standard_Integer count = ent->AttributeValueCount(i);
      IW.Send(ent->AttributeType(i));
      IW.Send(check);
      IW.Send(count);
      if (ent->FormNumber() > 0)
	{
          for (Standard_Integer j = 1; j <= count; j++)
	    {
	      switch (check) {
	        case 0 : IW.SendVoid();  break;
		case 1 : IW.Send(ent->AttributeAsInteger(i,j));  break;
		case 2 : IW.Send(ent->AttributeAsReal(i,j));     break;
		case 3 : IW.Send(ent->AttributeAsString(i,j));   break;
		case 4 : IW.Send(ent->AttributeAsEntity(i,j));   break;
	        case 5 : IW.SendVoid();  break;
		case 6 : IW.SendBoolean(ent->AttributeAsLogical(i,j)); break;
		default : break;
	      }
              if ( ent->FormNumber() == 2)
		IW.Send(ent->AttributeTextDisplay(i,j));
	    }
	}
    }
}

void  IGESDefs_ToolAttributeDef::OwnShared
  (const Handle(IGESDefs_AttributeDef)& ent, Interface_EntityIterator& iter) const
{
  Standard_Integer upper = ent->NbAttributes();
  for (Standard_Integer i = 1; i <= upper; i++)
    {
      Standard_Integer check = ent->AttributeValueDataType(i);
      Standard_Integer count = ent->AttributeValueCount(i);
      if (ent->FormNumber() > 0)
	{
          for (Standard_Integer j = 1; j <= count; j++)
	    {
              if ( check == 4 ) iter.GetOneItem(ent->AttributeAsEntity(i,j));
	      if ( ent->FormNumber() == 2)
		iter.GetOneItem(ent->AttributeTextDisplay(i,j));
	    }
	}
    }
}


void IGESDefs_ToolAttributeDef::OwnCopy
  (const Handle(IGESDefs_AttributeDef)& another,
   const Handle(IGESDefs_AttributeDef)& ent, Interface_CopyTool& TC) const
{
  Handle(TCollection_HAsciiString) aName;
  if (!another->TableName().IsNull()) aName =
    new TCollection_HAsciiString(another->TableName());
  Standard_Integer aListType = another->ListType();

  Handle(TColStd_HArray1OfInteger) attrTypes;
  Handle(TColStd_HArray1OfInteger) attrValueDataTypes;
  Handle(TColStd_HArray1OfInteger) attrValueCounts;
  Handle(TColStd_HArray1OfTransient) attrValues;
  Handle(IGESDefs_HArray1OfHArray1OfTextDisplayTemplate)  attrValuePointers;
  Standard_Integer nbval = another->NbAttributes();

  attrTypes = new TColStd_HArray1OfInteger(1, nbval);
  attrValueDataTypes = new TColStd_HArray1OfInteger(1, nbval);
  attrValueCounts = new TColStd_HArray1OfInteger(1, nbval);
  if (another->HasValues()) attrValues =
    new TColStd_HArray1OfTransient(1, nbval);
  if (another->HasTextDisplay()) attrValuePointers =
    new IGESDefs_HArray1OfHArray1OfTextDisplayTemplate(1, nbval);

  for (Standard_Integer i = 1; i <= nbval; i++)
    {
      Standard_Integer attrType = another->AttributeType(i);
      attrTypes->SetValue(i, attrType);
      Standard_Integer attrValueDataType = another->AttributeValueDataType(i);
      attrValueDataTypes->SetValue(i, attrValueDataType);
      Standard_Integer avc               = another->AttributeValueCount(i);
      attrValueCounts->SetValue(i, avc);
      Handle(IGESGraph_HArray1OfTextDisplayTemplate) attrValuePointer;

      if (another->HasTextDisplay()) attrValuePointer =
	new IGESGraph_HArray1OfTextDisplayTemplate(1, avc);

      if (another->HasValues())
	{
	  Handle(TColStd_HArray1OfInteger)        attrInt;
	  Handle(TColStd_HArray1OfReal)           attrReal;
	  Handle(Interface_HArray1OfHAsciiString) attrStr;
	  Handle(IGESData_HArray1OfIGESEntity)    attrEnt;
	  switch (attrValueDataType) {
	    case 1 : attrInt  = new TColStd_HArray1OfInteger       (1,avc);
	      attrValues->SetValue(i,attrInt);  break;
	    case 2 : attrReal = new TColStd_HArray1OfReal          (1,avc);
	      attrValues->SetValue(i,attrReal); break;
	    case 3 : attrStr  = new Interface_HArray1OfHAsciiString(1,avc);
	      attrValues->SetValue(i,attrStr);  break;
	    case 4 : attrEnt  = new IGESData_HArray1OfIGESEntity   (1,avc);
	      attrValues->SetValue(i,attrEnt);  break;
	    case 6 : attrInt  = new TColStd_HArray1OfInteger       (1,avc);
	      attrValues->SetValue(i,attrInt);  break;
	    default : break;
	  }
          for (Standard_Integer j = 1; j <= avc; j ++)
	    {
              switch(attrValueDataType)
		{
		case 0:
		  break;
		case 1: attrInt->SetValue(j,another->AttributeAsInteger(i,j));
		  break;
		case 2: attrReal->SetValue(j,another->AttributeAsReal(i,j));
		  break;
		case 3: attrStr->SetValue(j,new TCollection_HAsciiString
					   (another->AttributeAsString(i,j)));
		  break;
		case 4: {
		  DeclareAndCast(IGESData_IGESEntity,Ent,TC.Transferred
				 (another->AttributeAsEntity(i,j)));
		  attrEnt->SetValue(j,Ent);
		}
		  break;
		case 5:
		  break;
		case 6: attrInt->SetValue
		  (j,(another->AttributeAsLogical(i,j) ? 1 : 0));
		  break;
		default : break;
		}
              if (another->HasTextDisplay())
		{
                  DeclareAndCast(IGESGraph_TextDisplayTemplate, temptext,
				 TC.Transferred(another->AttributeTextDisplay(i,j)));
                  attrValuePointer->SetValue (j, temptext);
		}
	    }
          if (another->HasTextDisplay())
	    attrValuePointers->SetValue(i, attrValuePointer);
	}
    }
  ent->Init
    (aName, aListType, attrTypes, attrValueDataTypes, attrValueCounts,
     attrValues, attrValuePointers);
}

IGESData_DirChecker  IGESDefs_ToolAttributeDef::DirChecker
  (const Handle(IGESDefs_AttributeDef)& /* ent */ ) const
{
  IGESData_DirChecker DC (322, 0, 2);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusRequired(0);
  DC.UseFlagRequired(2);
  DC.GraphicsIgnored(1);
  return DC;
}

void IGESDefs_ToolAttributeDef::OwnCheck
  (const Handle(IGESDefs_AttributeDef)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  Standard_Integer nb = ent->NbAttributes();
  Standard_Integer fn = ent->FormNumber();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    char mess[80];
    if (ent->AttributeType(i) < 0 || ent->AttributeType(i) > 9999) {
      sprintf(mess,"Attribute Type n0.%d not in <0 - 9999>", ent->AttributeType(i));
      ach->AddFail(mess);
    }
    Standard_Integer aty = ent->AttributeValueDataType(i);
    if (aty < 0 || aty > 6) {
      sprintf(mess,"Attribute Value Data Type n0.%d not in <0 - 6>", aty);
      ach->AddFail(mess);
    }
    if (ent->AttributeValueCount(i) <= 0) continue;
    Handle(Standard_Transient) list = ent->AttributeList(i);
    if (fn > 0 && ent.IsNull()) {
      if (aty == 0 || aty == 5) continue;
      sprintf(mess,"Form Number > 0 and Attribute Value List n0.%d undefined", aty);
      ach->AddFail(mess);
      continue;
    }
    else if (fn == 0) continue;
    mess[0] = '\0';
    switch (aty)
    {
      case 1:
        if (!list->IsKind(STANDARD_TYPE(TColStd_HArray1OfInteger)))
        {
          sprintf(mess,"Attribute List n0.%d (Integers) badly defined", aty);
        }
        break;
      case 2:
        if (!list->IsKind(STANDARD_TYPE(TColStd_HArray1OfReal)))
        {
          sprintf(mess,"Attribute List n0.%d (Reals) badly defined", aty);
        }
        break;
      case 3:
        if (!list->IsKind(STANDARD_TYPE(Interface_HArray1OfHAsciiString)))
        {
          sprintf(mess,"Attribute List n0.%d (Strings) badly defined", aty);
        }
        break;
      case 4:
        if (!list->IsKind(STANDARD_TYPE(IGESData_HArray1OfIGESEntity)))
        {
          sprintf(mess,"Attribute List n0.%d (IGES Pointers) badly defined", aty);
        }
        break;
      case 6:
        if (!list->IsKind(STANDARD_TYPE(TColStd_HArray1OfInteger)))
        {
          sprintf(mess,"Attribute List n0.%d (Logicals i.e. Integers) badly defined", aty);
        }
        break;
      default:
        break;
    }
    if (mess[0] != '\0') ach->AddFail(mess);
  }
}


void IGESDefs_ToolAttributeDef::OwnDump
  (const Handle(IGESDefs_AttributeDef)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  Standard_Integer sublevel = (level > 4) ? 1 : 0;

  S << "IGESDefs_AttributeDef" << endl;
  S << "Attribute Table Name: ";
  IGESData_DumpString(S,ent->TableName());
  S << endl;
  S << "Attribute List Type  : " << ent->ListType() << endl;
  S << "Number of Attributes : " << ent->NbAttributes() << endl;
  S << "Attribute Types : " << endl;
  S << "Attribute Value Data Types : " << endl;
  S << "Attribute Value Counts : " << endl;
  if (ent->HasValues())       S << "Attribute Values : " << endl;
  if (ent->HasTextDisplay())  S << "Attribute Value Entities : " << endl;
  IGESData_DumpVals(S,-level,1, ent->NbAttributes(),ent->AttributeType);
  S << endl;
  if (level > 4)
    {
      Standard_Integer upper = ent->NbAttributes();
      for (Standard_Integer i = 1; i <= upper ; i ++)
	{
	  Standard_Integer avc = ent->AttributeValueCount(i);
	  Standard_Integer typ = ent->AttributeValueDataType(i);
          S << "[" << i << "]:  ";
          S << "Attribute Type : " << ent->AttributeType(i) << "  ";
          S << "Value Data Type : " << typ;
	  switch (typ) {
	    case 0 : S << "  (Void)";     break;
	    case 1 : S << " : Integer ";  break;
	    case 2 : S << " : Real    ";  break;
	    case 3 : S << " : String  ";  break;
	    case 4 : S << " : Entity  ";  break;
	    case 5 : S << " (Not Used)";  break;
	    case 6 : S << " : Logical ";  break;
	    default :   break;
	  }
          S << "   Count : " << avc << endl;
          if (ent->HasValues())
	    {
	      if (level <= 5) {
		S << " [ content (Values) : ask level > 5 ]" << endl;
		continue;
	      }
              for (Standard_Integer j = 1; j <= avc; j ++)
		{
		  S << "[" << j << "]: ";
                  switch(ent->AttributeValueDataType(i))
		    {
		    case 0:  S << "(Void) ";
                      break;
		    case 1:  S << ent->AttributeAsInteger(i,j);
		      break;
		    case 2:  S << ent->AttributeAsReal(i,j);
		      break;
		    case 3:  IGESData_DumpString(S,ent->AttributeAsString(i,j));
		      break;
		    case 4: dumper.Dump(ent->AttributeAsEntity(i,j),S,level-5);
		      break;
		    case 5:  S << "(Not Used)";
                      break;
		    case 6:
		      S << ( ent->AttributeAsLogical(i,j) ? "True" : "False");
		      break;
		    default : break;
		    }
                  if (ent->HasTextDisplay())
		    {
                      S << "  Attribute Value Pointer : ";
                      dumper.Dump (ent->AttributeTextDisplay(i,j),S, sublevel);
		    }
		  S << endl;
		}
	    }
	}
    }
  S << endl;
}
