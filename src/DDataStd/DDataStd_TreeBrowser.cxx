// File:	DDesign_TreeNodeBrowser.cxx
//      	--------------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Nov 27 1997	Creation



#include <DDataStd_TreeBrowser.ixx>
#include <DDataStd.hxx>
#include <TDF_Tool.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDF.hxx>

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Communication convention with tcl:
// tcl waits for a string of characters, being an information list.
// In this list, each item is separated from another by a separator: '\'.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define TDF_BrowserSeparator1 '\\'
#define TDF_BrowserSeparator2 ' '
#define TDF_BrowserSeparator3 '#'
#define TDF_BrowserSeparator4 ','


//=======================================================================
//function : DDesign_TreeNodeBrowser
//purpose  : 
//=======================================================================

DDataStd_TreeBrowser::DDataStd_TreeBrowser(const TDF_Label& aLabel)
: myRoot(aLabel)
{}


//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void DDataStd_TreeBrowser::DrawOn(Draw_Display& dis) const
{ cout<<"DDataStd_TreeBrowser"<<endl; }


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D) DDataStd_TreeBrowser::Copy() const
{ return new DDataStd_TreeBrowser(myRoot); }


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void DDataStd_TreeBrowser::Dump(Standard_OStream& S) const
{
  S<<"DDataStd_TreeBrowser on a label: "<<endl;
  S<<myRoot;
}


//=======================================================================
//function : Whatis
//purpose  : 
//=======================================================================

void DDataStd_TreeBrowser::Whatis(Draw_Interpretor& I) const
{ I<<"function browser"; }


//=======================================================================
//function : Label
//purpose  : 
//=======================================================================

void DDataStd_TreeBrowser::Label(const TDF_Label& aLabel)
{ myRoot = aLabel; }


//=======================================================================
//function : Label
//purpose  : 
//=======================================================================

TDF_Label DDataStd_TreeBrowser::Label() const
{ return myRoot; }


//=======================================================================
//function : OpenRoot
//purpose  : 
//=======================================================================

TCollection_AsciiString DDataStd_TreeBrowser::OpenRoot() const
{
  TCollection_AsciiString list;
  Handle(TDataStd_TreeNode) TN;
  if (myRoot.FindAttribute (TDataStd_TreeNode::GetDefaultTreeID(),TN)) {
    OpenNode(TN,list);
  }
  return list;
}


//=======================================================================
//function : OpenNode
//purpose  : 
// the items are separated by "\\".
//=======================================================================

TCollection_AsciiString DDataStd_TreeBrowser::OpenNode(const TDF_Label& aLabel) const
{
  TCollection_AsciiString list;
  Handle(TDataStd_TreeNode) nodeToOpen;
  if (aLabel.FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), nodeToOpen)) {
    Standard_Boolean split = Standard_False;
    Handle(TDataStd_TreeNode) current = nodeToOpen->First();
    while (!current.IsNull()) {
      if (split) list.AssignCat(TDF_BrowserSeparator1);
      OpenNode(current,list);
      split = Standard_True;
      current = current->Next();
    }
  }
  return list;
}


//=======================================================================
//function : OpenNode
//purpose  : 
// An item is composed as follows:
// "LabelEntry "Name" DynamicType Executable|Forgotten Failed|Success First|Null [ LabelFather]"
// First/Null : has/has not child
//=======================================================================

void DDataStd_TreeBrowser::OpenNode (const Handle(TDataStd_TreeNode)& aTreeNode,
				     TCollection_AsciiString& aList) const
{
  // Label entry. -0
  TCollection_AsciiString tmp;
  TDF_Tool::Entry(aTreeNode->Label(),tmp);
  aList.AssignCat(tmp);
  // Name         -1
  aList.AssignCat(TDF_BrowserSeparator2);
  Handle(TDataStd_Name) name;
  aList.AssignCat("\"");
  if (aTreeNode->Label().FindAttribute(TDataStd_Name::GetID(),name)) {
    TCollection_AsciiString tmpStr(name->Get(),'?');
    tmpStr.ChangeAll(' ','_');
    aList.AssignCat(tmpStr);
  }
  aList.AssignCat("\"");
  // Dynamic type.      -2
  aList.AssignCat(TDF_BrowserSeparator2);
  TCollection_ExtendedString ext;
  if (TDF::ProgIDFromGUID(aTreeNode->ID(), ext))
    aList.AssignCat(TCollection_AsciiString(ext,'?'));
  else aList.AssignCat(aTreeNode->DynamicType()->Name());
   // Executable or Forgotten?  -3
  //  aList.AssignCat(TDF_BrowserSeparator2);
  //   if (aTreeNode->IsExecutable()) aList.AssignCat("Executable");
  //   else aList.AssignCat("Forgotten");
  // Failed or Success?        -4
  //  aList.AssignCat(TDF_BrowserSeparator2);
  //   if (aTreeNode->Failed()) aList.AssignCat("Failed");
  //   else aList.AssignCat("Success");
  //Children?             -3
  aList.AssignCat(TDF_BrowserSeparator2);
  if (aTreeNode->First().IsNull()) aList.AssignCat("Null");
  else aList.AssignCat("First");
  // Father?                -4
  aList.AssignCat(TDF_BrowserSeparator2);
  if (!aTreeNode->HasFather()) aList.AssignCat("Null");
  else {
    TDF_Tool::Entry(aTreeNode->Father()->Label(),tmp);
    aList.AssignCat(tmp);
  }
  // First?                -5
  aList.AssignCat(TDF_BrowserSeparator2);
  if (!aTreeNode->HasFirst()) aList.AssignCat("Null");
  else {
    TDF_Tool::Entry(aTreeNode->First()->Label(),tmp);
    aList.AssignCat(tmp);
  }
  // Next?                -6
  aList.AssignCat(TDF_BrowserSeparator2);
  if (!aTreeNode->HasNext()) aList.AssignCat("Null");
  else {
    TDF_Tool::Entry(aTreeNode->Next()->Label(),tmp);
    aList.AssignCat(tmp);
  }
  // Previous?                -7
  aList.AssignCat(TDF_BrowserSeparator2);
  if (!aTreeNode->HasPrevious()) aList.AssignCat("Null");
  else {
    TDF_Tool::Entry(aTreeNode->Previous()->Label(),tmp);
    aList.AssignCat(tmp);
  }
}
