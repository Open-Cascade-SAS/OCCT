// Created on: 1997-10-02
// Created by: Xuan Trang PHAM PHU
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <TopOpeBRepBuild_ShellToSolid.ixx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>
#include <TopOpeBRepBuild_SolidBuilder.hxx>
#include <TopOpeBRepBuild_Builder.hxx>

#include <TopOpeBRepDS_BuildTool.hxx>

#include <TopTools_ListIteratorOfListOfShape.hxx>

//=======================================================================
//function : TopOpeBRepBuild_ShellToSolid
//purpose  : 
//=======================================================================

TopOpeBRepBuild_ShellToSolid::TopOpeBRepBuild_ShellToSolid()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_ShellToSolid::Init()
{
  myLSh.Clear();
}


//=======================================================================
//function : AddShell
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_ShellToSolid::AddShell(const TopoDS_Shell& Sh)
{
  myLSh.Append(Sh);
}

//=======================================================================
//function : MakeSolids
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_ShellToSolid::MakeSolids(const TopoDS_Solid& So,
					      TopTools_ListOfShape& LSo)
{
  LSo.Clear();
  
  TopOpeBRepBuild_ShellFaceSet sfs(So);
  for (TopTools_ListIteratorOfListOfShape it(myLSh);it.More();it.Next())
    sfs.AddShape(it.Value());
  
  Standard_Boolean ForceClass = Standard_True;
  TopOpeBRepBuild_SolidBuilder SB;  
  SB.InitSolidBuilder(sfs,ForceClass);  

  TopOpeBRepDS_BuildTool BT;
  TopOpeBRepBuild_Builder B(BT);
  B.MakeSolids(SB,LSo);

}
