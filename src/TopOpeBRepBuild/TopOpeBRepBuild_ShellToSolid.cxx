// Created on: 1997-10-02
// Created by: Xuan Trang PHAM PHU
// Copyright (c) 1997-1999 Matra Datavision
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


#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopOpeBRepBuild_Builder.hxx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>
#include <TopOpeBRepBuild_ShellToSolid.hxx>
#include <TopOpeBRepBuild_SolidBuilder.hxx>
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
