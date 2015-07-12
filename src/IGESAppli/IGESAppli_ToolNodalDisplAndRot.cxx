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

#include <gp_XYZ.hxx>
#include <IGESAppli_HArray1OfNode.hxx>
#include <IGESAppli_NodalDisplAndRot.hxx>
#include <IGESAppli_Node.hxx>
#include <IGESAppli_ToolNodalDisplAndRot.hxx>
#include <IGESBasic_HArray1OfHArray1OfXYZ.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_HArray1OfGeneralNote.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TColgp_HArray1OfXYZ.hxx>
#include <TColStd_HArray1OfInteger.hxx>

IGESAppli_ToolNodalDisplAndRot::IGESAppli_ToolNodalDisplAndRot ()    {  }


void  IGESAppli_ToolNodalDisplAndRot::ReadOwnParams
  (const Handle(IGESAppli_NodalDisplAndRot)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Integer nbcases = 0;
  Standard_Integer nbnodes = 0;
  Handle(IGESDimen_HArray1OfGeneralNote) tempNotes ;
  Handle(TColStd_HArray1OfInteger) tempNodeIdentifiers ;
  Handle(IGESAppli_HArray1OfNode) tempNodes;
  Handle(IGESBasic_HArray1OfHArray1OfXYZ) tempTransParam ;
  Handle(IGESBasic_HArray1OfHArray1OfXYZ) tempRotParam ;

  Standard_Boolean tempFlag = PR.ReadInteger(PR.Current(),"No. of analysis cases",nbcases);
  if (tempFlag) {
    tempNotes = new IGESDimen_HArray1OfGeneralNote(1,nbcases);
    for (Standard_Integer i=1; i <= nbcases; i++) {
      Handle(IGESDimen_GeneralNote) anentity;
      //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadEntity(IR,PR.Current(),"General Note", STANDARD_TYPE(IGESDimen_GeneralNote), anentity))
	tempNotes->SetValue(i,anentity);
    }
  }
  //szv#4:S4163:12Mar99 moved in if
  if (PR.ReadInteger(PR.Current(),"No. of nodes",nbnodes)) {
    tempNodeIdentifiers = new TColStd_HArray1OfInteger(1,nbnodes);
    tempNodes = new IGESAppli_HArray1OfNode(1,nbnodes);
    tempTransParam = new IGESBasic_HArray1OfHArray1OfXYZ(1, nbnodes);
    tempRotParam = new IGESBasic_HArray1OfHArray1OfXYZ(1, nbnodes);
    for (Standard_Integer j=1; j <=nbnodes; j++) {
      Handle(IGESAppli_Node) aNode = new IGESAppli_Node();
      Standard_Integer nodeno;
      if (PR.ReadInteger( PR.Current(), "Node number", nodeno))
	tempNodeIdentifiers->SetValue(j,nodeno);
      if (PR.ReadEntity(IR,PR.Current(),"Node Directory Entry", STANDARD_TYPE(IGESAppli_Node), aNode))
	tempNodes->SetValue(j,aNode);

      Handle(TColgp_HArray1OfXYZ) tempArray1;
      Handle(TColgp_HArray1OfXYZ) tempArray;
      if (tempFlag) {
	// tempFlag indicates if nbcases has been properly read
	tempArray1 = new TColgp_HArray1OfXYZ(1,nbcases);
	tempArray  = new TColgp_HArray1OfXYZ(1,nbcases);
	for (Standard_Integer k = 1; k <= nbcases; k++) {
	  gp_XYZ atrans,arot;
	  if (PR.ReadXYZ(PR.CurrentList(1,3),"Translation XYZ",atrans))
	    tempArray->SetValue(k,atrans);
	  if (PR.ReadXYZ(PR.CurrentList(1,3),"Rotational XYZ",arot))
	    tempArray1->SetValue(k,arot);
	}
      }
      tempTransParam->SetValue(j,tempArray);
      tempRotParam->SetValue(j,tempArray1);
    }
  }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempNotes,tempNodeIdentifiers,tempNodes,tempRotParam,tempTransParam);
}

void  IGESAppli_ToolNodalDisplAndRot::WriteOwnParams
  (const Handle(IGESAppli_NodalDisplAndRot)& ent, IGESData_IGESWriter& IW) const
{
  Standard_Integer nbcases = ent->NbCases();
  Standard_Integer nbnodes = ent->NbNodes();

  IW.Send(nbcases);
  for (Standard_Integer i = 1; i <= nbcases; i ++)
    IW.Send(ent->Note(i));
  IW.Send(nbnodes);
  for (Standard_Integer j = 1; j <= nbnodes; j ++)
    {
      IW.Send(ent->NodeIdentifier(j));
      IW.Send(ent->Node(j));
      for (Standard_Integer k = 1; k <= nbcases; k ++)
	{
          IW.Send(ent->TranslationParameter(j,k).X());
          IW.Send(ent->TranslationParameter(j,k).Y());
          IW.Send(ent->TranslationParameter(j,k).Z());
          IW.Send(ent->RotationalParameter(j,k).X());
          IW.Send(ent->RotationalParameter(j,k).Y());
          IW.Send(ent->RotationalParameter(j,k).Z());
	}
    }
}

void  IGESAppli_ToolNodalDisplAndRot::OwnShared
  (const Handle(IGESAppli_NodalDisplAndRot)& ent, Interface_EntityIterator& iter) const
{
  Standard_Integer nbcases = ent->NbCases();
  Standard_Integer nbnodes = ent->NbNodes();

  for (Standard_Integer i = 1; i <= nbcases; i ++)
    iter.GetOneItem(ent->Note(i));
  for (Standard_Integer j = 1; j <= nbnodes; j ++)
    iter.GetOneItem(ent->Node(j));
}

void  IGESAppli_ToolNodalDisplAndRot::OwnCopy
  (const Handle(IGESAppli_NodalDisplAndRot)& another,
   const Handle(IGESAppli_NodalDisplAndRot)& ent, Interface_CopyTool& TC) const
{
  Standard_Integer nbcases = another->NbCases();
  Standard_Integer nbnodes = another->NbNodes();
  Handle(IGESDimen_HArray1OfGeneralNote) aNotes =
    new IGESDimen_HArray1OfGeneralNote(1,nbcases);
  Handle(TColStd_HArray1OfInteger) aNodeIdentifiers =
    new TColStd_HArray1OfInteger(1,nbnodes);
  Handle(IGESAppli_HArray1OfNode) aNodes =
    new IGESAppli_HArray1OfNode(1,nbnodes);
  Handle(IGESBasic_HArray1OfHArray1OfXYZ) aTransParam =
    new IGESBasic_HArray1OfHArray1OfXYZ(1, nbnodes);
  Handle(IGESBasic_HArray1OfHArray1OfXYZ) aRotParam =
    new IGESBasic_HArray1OfHArray1OfXYZ(1, nbnodes);

  for (Standard_Integer i=1 ;i <=nbnodes; i++)
    {
      aNodeIdentifiers->SetValue(i,(another->NodeIdentifier(i)));
      DeclareAndCast(IGESAppli_Node,anitem,TC.Transferred(another->Node(i)));
      aNodes->SetValue(i,anitem);
    }
  for (Standard_Integer j=1 ;j <=nbcases; j++)
    {
      DeclareAndCast
	(IGESDimen_GeneralNote,anitem,TC.Transferred(another->Note(j)));
      aNotes->SetValue(j,anitem);
    }
  for (Standard_Integer n=1 ;n <=nbnodes; n++)
    {
      Handle(TColgp_HArray1OfXYZ) tempArray1 = new
	TColgp_HArray1OfXYZ(1,nbcases);
      Handle(TColgp_HArray1OfXYZ) tempArray2 = new
	TColgp_HArray1OfXYZ(1,nbcases);
      for (Standard_Integer k=1;k<= nbcases;k++)
	{
          tempArray1->SetValue(k,another->TranslationParameter(n,k));
          tempArray2->SetValue(k,another->RotationalParameter(n,k));
	}
      aTransParam->SetValue(n,tempArray1);
      aRotParam->SetValue(n,tempArray2);
    }

  ent->Init(aNotes,aNodeIdentifiers,aNodes,aRotParam,aTransParam);
}

IGESData_DirChecker  IGESAppli_ToolNodalDisplAndRot::DirChecker
  (const Handle(IGESAppli_NodalDisplAndRot)& /* ent */ ) const
{
  IGESData_DirChecker DC(138,0);  //Form no = 0 & Type = 138
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void  IGESAppli_ToolNodalDisplAndRot::OwnCheck
  (const Handle(IGESAppli_NodalDisplAndRot)& /* ent */,
   const Interface_ShareTool& , Handle(Interface_Check)& /* ach */) const
{
}

void  IGESAppli_ToolNodalDisplAndRot::OwnDump
  (const Handle(IGESAppli_NodalDisplAndRot)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESAppli_NodalDisplAndRot" << endl;

  Standard_Integer nbcases = ent->NbCases();
  Standard_Integer nbnodes = ent->NbNodes();

  S << "No. of analysis cases : " << nbcases << endl;
  S << "General Notes : ";
  IGESData_DumpEntities(S,dumper ,level,1, nbcases,ent->Note);
  S << endl;

//  gp_GTrsf loca;  // true location n.u.
  switch (level)
    {
    case 4:
      S << "Nodes : ";
      IGESData_DumpEntities(S,dumper,-level,1, nbnodes,ent->Node);
      S << "NodeIdentifiers : ";
      IGESData_DumpVals(S,-level,1, nbnodes,ent->NodeIdentifier);
      S << "Translation Parameters : ";
//  ?? Possibly JAGGED ??
//      IGESData_DumpListXYZL(S,-level,1,nbcases,ent->TranslationParameter,loca);
      S << "Rotational Parameters : ";
//      IGESData_DumpListXYZL(S,-level,1,nbcases,ent->RotationalParameter,loca);
      S << "  TO BE DONE" << endl;
      break;
    case 5:
    case 6: {
      for (Standard_Integer i = 1; i <= nbcases; i ++)
	{
	  S << "[" << i << "]: ";
	  dumper.Dump (ent->Note(i),S, 1);
	  S << endl;
	}
      S << "Nodes : " << endl;
      S << "Node Identifiers : "  << endl;
      S << "Translation Parameters : " << endl;
      S << "Rotational Parameters : Count = " << nbcases;
//     IGESData_DumpListXYZL(S,-level,1,nbcases,ent->RotationalParameter,loca);
      S << endl;
      for (Standard_Integer j = 1; j <= nbnodes; j ++)
	{
	  S << "[" << j << "]:  -  NodeIdentifier : "
	    << ent->NodeIdentifier(j) << "  -  Node :"<<endl; ;
	  S << "Node : ";
	  dumper.Dump (ent->Node(j),S, 1);
	  S << "  -  Parameters : " << endl;
	  for (Standard_Integer k = 1; k <= nbcases; k ++)
	    {
	      S << " [" << k << "]: "  << endl;
	      S << "Translational Parameter : X="
		<< ent->TranslationParameter(j,k).X() << ", Y="
		<< ent->TranslationParameter(j,k).Y() << ", Z="
		<< ent->TranslationParameter(j,k).Z() << endl;
	      S << "Rotational Parameter : X="
		<< ent->RotationalParameter(j,k).X() << ", Y="
		<< ent->RotationalParameter(j,k).Y() << ", Z="
		<< ent->RotationalParameter(j,k).Z();
	      if (k == nbcases) S << "] ";
	      S << endl;
	    }
	}
    }
      break;
    default: break;
    }
  S << endl;
}
