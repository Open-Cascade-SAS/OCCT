// Created on: 1999-11-05
// Created by: Denis PASCAL
// Copyright (c) 1999-1999 Matra Datavision
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


#include <DDataStd.hxx>
#include <DDataStd_TreeBrowser.hxx>
#include <DDF.hxx>
#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Drawable3D.hxx>
#include <Draw_Interpretor.hxx>
#include <Standard_GUID.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_ChildNodeIterator.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataXtd_Constraint.hxx>
#include <TDF.hxx>
#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>

#include <stdio.h>
//=======================================================================
//TreeNode : SetNode
//purpose  : SetNode (DOC Entry [ID]) 
//=======================================================================
static Standard_Integer DDataStd_SetNode (Draw_Interpretor& di, 
					  Standard_Integer  n, 
					  const char**            a)
{
  Handle(TDF_Data) DF;
  if (!DDF::GetDF(a[1],DF)) return 1;
  TDF_Label L;
  Handle(TDataStd_TreeNode) TN;
  DDF::AddLabel(DF,a[2],L);
  if (n == 3) {
    TN = TDataStd_TreeNode::Set(L); 
    DDF::ReturnLabel(di,TN->Label());
    return 0;
  } else if (n == 4) {
    if (!Standard_GUID::CheckGUIDFormat(a[3])) {
      di<<"DDataStd_SetNode: The format of GUID is invalid\n";
      return 1;
    }
    const Standard_GUID ID(a[3]);
    TN = TDataStd_TreeNode::Set(L, ID); 
    DDF::ReturnLabel(di,TN->Label());
    return 0;    
  }
  di << "DDataStd_SetNode : Error\n";
  return 1;
}    

//=======================================================================
//TreeNode : AppendNode
//purpose  : AppendNode (DOC FatherEntry childEntry [fatherID]) 
//=======================================================================

static Standard_Integer DDataStd_AppendNode (Draw_Interpretor& di, 
					     Standard_Integer  n, 
					     const char**            a)
{
  if (n >= 4) {
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(a[1],DF))return 1;
    Handle(TDataStd_TreeNode) father, child;

    Standard_GUID ID;
    if (n==4)
      ID=Standard_GUID(TDataStd_TreeNode::GetDefaultTreeID());
    else{
      if (!Standard_GUID::CheckGUIDFormat(a[4])) {
	di<<"DDataStd_AppendNode: The format of GUID is invalid\n";
	return 1;
      }
      const Standard_GUID varID(a[4]);
      ID=varID;
    }

    if (!DDF::Find(DF,a[2],ID,father)) return 1;

    TDF_Label L;
    DDF::AddLabel(DF,a[3],L);
    if ( (L.FindAttribute(ID,child))
       &&(!child->IsRoot()) ){
      di << "DDataStd_AppendNode : Error : childTreeNode don't detach\n";
      return 1;
    }
    child = TDataStd_TreeNode::Set(L,ID);
    if (!father->Append(child)) return 1;
#ifdef OCCT_DEBUG
    di<<"AppendNode: OK\n";
#endif
    return 0;
  }
  di << "DDataStd_AppendNode : Error\n";
  return 1;
}

//=======================================================================
//TreeNode : PrependNode
//purpose  : PrependNode (DOC FatherEntry childEntry [fatherID]) 
//=======================================================================

static Standard_Integer DDataStd_PrependNode (Draw_Interpretor& di, 
					     Standard_Integer  n, 
					     const char**            a)
{
  if (n >= 4) {
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(a[1],DF))return 1;
    Handle(TDataStd_TreeNode) father, child;

    Standard_GUID ID;
    if (n==4)
      ID=Standard_GUID(TDataStd_TreeNode::GetDefaultTreeID());
    else{
      if (!Standard_GUID::CheckGUIDFormat(a[4])) {
	di<<"DDataStd_PrependNode: The format of GUID is invalid\n";
	return 1;
      }
      const Standard_GUID varID(a[4]);
      ID=varID;
    }

    if (!DDF::Find(DF,a[2],ID,father)) return 1;

    TDF_Label L;
    DDF::AddLabel(DF,a[3],L);
    if ( (L.FindAttribute(ID,child))
       &&(!child->IsRoot()) ){
      di << "DDataStd_PrependNode : Error : childTreeNode don't detach\n";
      return 1;
    }
    child = TDataStd_TreeNode::Set(L,ID);
    if (!father->Prepend(child)) return 1;
#ifdef OCCT_DEBUG
    di<<"PrependNode: OK\n";
#endif
    return 0;
  }
  di << "DDataStd_PrependNode : Error\n";
  return 1;
}

//=======================================================================
//TreeNode : RootNode
//purpose  : RootNode (DOC TreeNodeEntry [ID]) 
//=======================================================================

static Standard_Integer DDataStd_RootNode (Draw_Interpretor& di, 
					   Standard_Integer  n, 
					   const char**            a)
{
  if (n >= 3) {
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(a[1],DF)) return 1;
    Handle(TDataStd_TreeNode) TN;    

    Standard_GUID ID;
    if (n==3)
      ID=Standard_GUID(TDataStd_TreeNode::GetDefaultTreeID());
    else{
      if (!Standard_GUID::CheckGUIDFormat(a[3])) {
	di<<"DDataStd_RootNode: The format of GUID is invalid\n";
	return 1;
      }
      const Standard_GUID varID(a[3]);
      ID=varID;
    }

    if (!DDF::Find(DF,a[2],ID,TN)) return 1; 
    DDF::ReturnLabel(di,TN->Root()->Label());
    return 0;
  }
  di << "DDataStd_RootNode : Error\n";
  return 1;
}

//=======================================================================
//TreeNode : InsertNodeBefore
//purpose  : InsertNodeBefore (DOC TreeNodeEntry TreeNodeWhichHasToBeBefore [ID]) 
//=======================================================================

static Standard_Integer DDataStd_InsertNodeBefore(Draw_Interpretor& di, 
						  Standard_Integer  n, 
						  const char**            a)
{
  if (n >= 4) {
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(a[1],DF)) return 1;
    Handle(TDataStd_TreeNode) current, TN; 

    Standard_GUID ID;
    if (n==4)
      ID=Standard_GUID(TDataStd_TreeNode::GetDefaultTreeID());
    else{
      if (!Standard_GUID::CheckGUIDFormat(a[4])) {
	di<<"DDataStd_InsertNodeBefore: The format of GUID is invalid\n";
	return 1;
      }
      const Standard_GUID varID(a[4]);
      ID=varID;
    }

    if (!DDF::Find(DF,a[2],ID,current)) return 1;
//     if (!DDF::Find(DF,a[3],ID,TN)) return 1;   

    TDF_Label L;
    if (!DDF::FindLabel(DF,a[3],L)) return 1;
    TN = TDataStd_TreeNode::Set(L,ID);     
    if (!current->InsertBefore(TN)) return 1;
    return 0;
  }
  di << "DDataStd_InsertBefore : Error\n";
  return 1;
}

//=======================================================================
//TreeNode : InsertNodeAfter
//purpose  : InsertNodeAfter (DOC TreeNodeEntry TreeNodeWhichHasToBeAfter [ID]) 
//=======================================================================

static Standard_Integer DDataStd_InsertNodeAfter(Draw_Interpretor& di, 
						 Standard_Integer  n, 
						 const char**            a)
{  
  if (n >= 4) {
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(a[1],DF)) return 1;    
    Handle(TDataStd_TreeNode) current, TN; 

    Standard_GUID ID;
    if (n==4)
      ID=Standard_GUID(TDataStd_TreeNode::GetDefaultTreeID());
    else{
      if (!Standard_GUID::CheckGUIDFormat(a[4])) {
	di<<"DDataStd_InsertNodeAfter: The format of GUID is invalid\n";
	return 1;
      }
      const Standard_GUID varID(a[4]);
      ID=varID;
    }

    if (!DDF::Find(DF,a[2],ID,current)) return 1;
    //if (!DDF::Find(DF,a[3],TDataStd_TreeNode::GetDefaultTreeID(),TN)) return 1;     
    TDF_Label L;
    if (!DDF::FindLabel(DF,a[3],L)) return 1;
    TN = TDataStd_TreeNode::Set(L);   
    if (!current->InsertAfter(TN)) return 1;
    return 0;
  }
  di << "DDataStd_InsertNodeAfter : Error\n";
  return 1;
}

//=======================================================================
//TreeNode : DetachNode
//purpose  : DetachNode (DOC TreeNodeEntry [ID])
//=======================================================================

static Standard_Integer DDataStd_DetachNode (Draw_Interpretor& di,Standard_Integer n, const char** a)
{
  if (n >= 3) {   
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(a[1],DF)) return 1;
    Handle(TDataStd_TreeNode) TN;

    Standard_GUID ID;
    if (n==3)
      ID=Standard_GUID(TDataStd_TreeNode::GetDefaultTreeID());
    else{
      if (!Standard_GUID::CheckGUIDFormat(a[3])) {
	di<<"DDataStd_DetachNode: The format of GUID is invalid\n";
	return 1;
      }
      const Standard_GUID varID(a[3]);
      ID=varID;
    }

    if (!DDF::Find(DF,a[2],ID,TN)) return 1;
    if (!TN->Remove())
      di << "Can't Detach the TreeNode\n";
    return 0; 
  } 
  di << "DDataStd_DetachNode : Error\n";
  return 1;
}

//=======================================================================
//function : TreeBrowse
//purpose  : 
//  arg 1  : DF name
//  arg 2  : Entry
// [arg 3] : Browser name
//=======================================================================

static Standard_Integer DDataStd_TreeBrowse (Draw_Interpretor& di, 
						 Standard_Integer  n, 
						 const char**            a)
{
  if (n<3) return 1;
  
  Handle(TDF_Data) DF;
  if (!DDF::GetDF (a[1], DF)) return 1;

  TDF_Label lab;
  TDF_Tool::Label(DF,a[2],lab);

  Handle(DDataStd_TreeBrowser) NewTreeNode = new DDataStd_TreeBrowser (lab);
  char *name = new char[50];
  if (n == 4) Sprintf(name,"treebrowser_%s",a[3]);
  else        Sprintf(name,"treebrowser_%s",a[1]);

  Draw::Set(name, NewTreeNode);
  TCollection_AsciiString inst1("treebrowser ");
  inst1.AssignCat(name);
  di.Eval(inst1.ToCString());
  return 0;
}


//=======================================================================
//function : OpenNode
//purpose  : 
//  arg 1  : Browser name
// [arg 2] : Label entry
//=======================================================================

static Standard_Integer DDataStd_OpenNode (Draw_Interpretor& di, 
					   Standard_Integer  n, 
					   const char**            a)
{
  if (n < 2) return 1;
  
  Handle(DDataStd_TreeBrowser) browser =
    Handle(DDataStd_TreeBrowser)::DownCast (Draw::Get(a[1], Standard_True));

  TDF_Label lab;
  if (n == 3) TDF_Tool::Label(browser->Label().Data(),a[2],lab);

  if (n == 2 || lab.IsNull()) {
    TCollection_AsciiString list = browser->OpenRoot();
    di<<list.ToCString();
  }
  else {
    TCollection_AsciiString list = browser->OpenNode(lab);
    di<<list.ToCString();
  }
  return 0;
}

//=======================================================================
//TreeNode : ChildNodeIterate
//purpose  : ChildNodeIterate Doc TreeNode AllLevels [ID]
//=======================================================================

static Standard_Integer DDataStd_ChildNodeIterate (Draw_Interpretor& di, 
						   Standard_Integer  n, 
						   const char**            a)
{
  if (n >= 4) {
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(a[1],DF)) return 1;
    const Standard_Boolean AllLevels = (Draw::Atoi(a[3]) != 0);
    Handle(TDataStd_TreeNode) TN, Value;

    Standard_GUID ID;
    if (n==4)
      ID=Standard_GUID(TDataStd_TreeNode::GetDefaultTreeID());
    else{
      if (!Standard_GUID::CheckGUIDFormat(a[4])) {
	di<<"DDataStd_ChildNodeIterate: The format of GUID is invalid\n";
	return 1;
      }
      const Standard_GUID varID(a[4]);
      ID=varID;
    }

    if (!DDF::Find(DF, a[2], ID, TN)) return 1; 
    //di<<"TDataStd_ChildNodeIterator: \n";
    TDataStd_ChildNodeIterator itr(TN, AllLevels);
    for (; itr.More(); itr.Next()) {
      Value = itr.Value();
      TCollection_AsciiString entry;
      TDF_Tool::Entry(Value->Label(), entry);
      di<<entry<<"\n";
      //di<<entry;
      //if(itr.More()==Standard_True)
	//di<<" ";
    }
    di<<"\n";
    return 0;
  }
  di << "DDataStd_ChildNodeIterate : Error\n";
  return 1;
}  

static TDataStd_ChildNodeIterator cni;

//=======================================================================
//TreeNode : InitChildNodeIterator
//purpose  : InitChildNodeIterator Doc TreeNode AllLevels [ID]
//=======================================================================

static Standard_Integer DDataStd_InitChildNodeIterator (Draw_Interpretor& di, 
							Standard_Integer  n, 
							const char**            a)
{
  if (n >= 4) {
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(a[1],DF)) return 1;
    Handle(TDataStd_TreeNode) TN, Value;

    Standard_GUID ID;
    if (n==4)
      ID=Standard_GUID(TDataStd_TreeNode::GetDefaultTreeID());
    else{
      if (!Standard_GUID::CheckGUIDFormat(a[4])) {
	di<<"DDataStd_ChildNodeIterator: The format of GUID is invalid\n";
	return 1;
      }
      const Standard_GUID varID(a[4]);
      ID=varID;
    }

    if (!DDF::Find(DF, a[2], ID, TN)) return 1; 
    const Standard_Boolean AllLevels = (Draw::Atoi(a[3]) != 0);
    cni.Initialize(TN, AllLevels);
    return 0;
  }
  di << "DDataStd_InitChildNodeIterator : Error\n";
  return 1;
}  

//=======================================================================
//TreeNode : ChildNodeMore
//purpose  : ChildNodeMore
//=======================================================================

static Standard_Integer DDataStd_ChildNodeMore (Draw_Interpretor& di, 
						Standard_Integer  /*n*/, 
						const char**            /*a*/)
{
  if (cni.More()) {
    //cout<<"TRUE"<<endl;
    di<<"TRUE\n";
  }
  else {
    //cout<<"FALSE"<<endl;
    di<<"FALSE\n";  
  }
  return 0;
}  

//=======================================================================
//TreeNode : ChildNodeNext 
//purpose  : ChildNodeNext 
//=======================================================================

static Standard_Integer DDataStd_ChildNodeNext (Draw_Interpretor& /*di*/, 
						Standard_Integer  /*n*/, 
						const char**            /*a*/)
{
  cni.Next();
  return 0;
}  

//=======================================================================
//TreeNode : ChildNodeNextBrother 
//purpose  : ChildNodeNext Brother 
//=======================================================================

static Standard_Integer DDataStd_ChildNodeNextBrother (Draw_Interpretor& /*di*/, 
						       Standard_Integer  /*n*/, 
						       const char**            /*a*/)
{
  cni.NextBrother();
  return 0;
}  

//=======================================================================
//TreeNode : ChildNodeValue
//purpose  : ChildNodeValue
//=======================================================================

static Standard_Integer DDataStd_ChildNodeValue (Draw_Interpretor& di, 
						 Standard_Integer  /*n*/, 
						 const char**            /*a*/)
{
  TCollection_AsciiString entry;
  TDF_Tool::Entry(cni.Value()->Label(), entry);
  //cout<<entry<<endl;
  di  <<entry<<"\n";
  return 0;
}  

//=======================================================================
//TreeNode : TreeCommands
//purpose  : 
//=======================================================================

void DDataStd::TreeCommands (Draw_Interpretor& theCommands)
{ 
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;  

  const char* g = "DData : Standard Attribute Commands";

  // SET

  theCommands.Add ("SetNode", 
                   "SetNode (DOC Entry [GUID])",
		   __FILE__, DDataStd_SetNode, g);  

  // EDIT

  theCommands.Add ("AppendNode", 
                   "AppendNode (DOC FatherEntry childEntry [fatherGUID])",
		   __FILE__, DDataStd_AppendNode, g);  

  theCommands.Add ("PrependNode", 
                   "PrependNode (DOC FatherEntry childEntry [fatherGUID])",
		   __FILE__, DDataStd_PrependNode, g);  

  theCommands.Add ("InsertNodeBefore", 
                   "InsertNodeBefore (DOC TreeNodeEntry TreeNodeWhichHasToBeBefore [GUID])",
		   __FILE__, DDataStd_InsertNodeBefore, g);  

  theCommands.Add ("InsertNodeAfter", 
                   "InsertNodeAfter (DOC TreeNodeEntry TreeNodeWhichHasToBeAfter [GUID])",
		   __FILE__, DDataStd_InsertNodeAfter, g);  

  theCommands.Add ("DetachNode", 
                   "DetachNode (DOC TreeNodeEntry [GUID])",
		   __FILE__, DDataStd_DetachNode, g);  

  theCommands.Add ("RootNode", 
                   "RootNode (DOC TreeNodeEntry [GUID])",
		   __FILE__, DDataStd_RootNode, g);


  // TREE BROWSER
  
  theCommands.Add ("TreeBrowse",
		   "TreeBrowse dfname entry [browsername]",
		   __FILE__, DDataStd_TreeBrowse, g);

  theCommands.Add ("OpenNode",
		   "PRIVATE COMMAND FOR TREE BROWSER!\nReturns the list of sub-TreeNodes : OpenTreeNode browsername [entry]",
		   __FILE__, DDataStd_OpenNode, g);    



  // TREE ITERATOR

  theCommands.Add ("ChildNodeIterate",
		   "ChildNodeIterate Doc TreeNode AllLevels [GUID]",
		   __FILE__, DDataStd_ChildNodeIterate, g);

  theCommands.Add ("InitChildNodeIterator",
		   "InitChildNodeIterator Doc TreeNode AllLevels [GUID]",
		   __FILE__, DDataStd_InitChildNodeIterator, g);

  theCommands.Add ("ChildNodeMore",
		   "ChildNodeMore",
		   __FILE__, DDataStd_ChildNodeMore, g);

  theCommands.Add ("ChildNodeNext",
		   "ChildNodeNext",
		   __FILE__, DDataStd_ChildNodeNext, g);

  theCommands.Add ("ChildNodeNextBrother",
		   "ChildNodeNextBrother",
		   __FILE__, DDataStd_ChildNodeNextBrother, g);

  theCommands.Add ("ChildNodeValue",
		   "ChildNodeValue",
		   __FILE__, DDataStd_ChildNodeValue, g);

}
