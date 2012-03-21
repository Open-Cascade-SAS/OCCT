// Created on: 1999-06-19
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
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


#include <SWDRAW_ShapeProcessAPI.ixx>

#include <SWDRAW.hxx>
#include <DBRep.hxx>
#include <TopoDS_Shape.hxx>
#include <ShapeProcessAPI_ApplySequence.hxx>
#include <ShapeProcess_ShapeContext.hxx>
#include <stdio.h>

//=======================================================================
//function : ApplySequence
//purpose  : 
//=======================================================================

static Standard_Integer ApplySequence(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) { di<<"Specify result, shape, resource name and prefix (optional)"<<"\n"; return 1 /* Error */; }
  Standard_CString arg1 = argv[1];
  Standard_CString arg2 = argv[2];
  Standard_CString arg3 = argv[3];
  Standard_CString arg4 = "";
  if (argc > 4) arg4 = argv[4];
  TopoDS_Shape Shape = DBRep::Get(arg2);
  if (Shape.IsNull()) { di<<"Shape unknown : "<<arg2<<"\n"; return 1 /* Error */; }
  
  ShapeProcessAPI_ApplySequence seq (arg3, arg4);
  TopoDS_Shape result = seq.PrepareShape(Shape, Standard_True, TopAbs_FACE);//fill history map for faces and above
  seq.PrintPreparationResult();
  DBRep::Set (arg1,result);
  return 0;
}
  
//=======================================================================
//function : InitCommands
//purpose  : 
//=======================================================================

void SWDRAW_ShapeProcessAPI::InitCommands(Draw_Interpretor& theCommands) 
{
  static Standard_Boolean initactor = Standard_False;
  if (!initactor) {

    initactor = Standard_True;

    Standard_CString g = SWDRAW::GroupName(); // "Tests of DivideTool";

    theCommands.Add ("DT_ApplySeq","DT_ApplySeq result shape rscfilename [prefix]",
                     __FILE__,ApplySequence,g);
  }
}  
