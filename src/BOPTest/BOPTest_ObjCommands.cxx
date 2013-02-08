// Created by: Peter KURNEV
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


#include <BOPTest.ixx>
#include <stdio.h>

#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
//
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
//
#include <TopTools_ListOfShape.hxx>
//
#include <BOPTest_Objects.hxx>

static Standard_Integer baddobjects   (Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer bclearobjects (Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer baddtools   (Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer bcleartools (Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer baddcompound(Draw_Interpretor& , Standard_Integer , const char** );
//
//=======================================================================
//function :ObjCommands
//purpose  : 
//=======================================================================
  void BOPTest::ObjCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  // Chapter's name
  const char* g = "BOP commands";
  // Commands
  theCommands.Add("baddobjects"    , "baddobjects s1 s2 ..."    , __FILE__, baddobjects, g);
  theCommands.Add("bclearobjects"  , "bclearobjects"            , __FILE__, bclearobjects, g);
  theCommands.Add("baddtools"      , "baddtools s1 s2 ..."      , __FILE__, baddtools, g);
  theCommands.Add("bcleartools"    , "bcleartools"              , __FILE__, bcleartools, g);
  theCommands.Add("baddcompound"   , "baddcompound c"           , __FILE__, baddcompound, g);
}
//=======================================================================
//function : baddcompound
//purpose  : 
//=======================================================================
Standard_Integer baddcompound (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n<2) {
    printf(" Use baddcompound c\n");
    return 0;
  }
  //
  TopoDS_Iterator aIt;
  TopoDS_Shape aS;
  //
  aS=DBRep::Get(a[1]);
  //
  BOPCol_ListOfShape& aLS=BOPTest_Objects::Shapes();
  aIt.Initialize(aS);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSx=aIt.Value();
    aLS.Append(aSx);
  }
  //
  return 0;
}
//
//=======================================================================
//function :baddobjects
//purpose  : 
//=======================================================================
Standard_Integer baddobjects (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n<2) {
    printf(" Use baddobjects s1 s2 ...\n");
    return 0;
  }
  //
  Standard_Integer i;
  TopoDS_Shape aS;
  //
  BOPCol_ListOfShape& aLS=BOPTest_Objects::Shapes();
  for (i = 1; i < n; ++i) {
    aS=DBRep::Get(a[i]);
    aLS.Append(aS);
  }
  //
  return 0;
}
//=======================================================================
//function : bclearobjects
//purpose  : 
//=======================================================================
Standard_Integer bclearobjects (Draw_Interpretor& , Standard_Integer n, const char** )
{
  if (n!=1) {
    printf(" Use bclearobjects\n");
    return 0;
  }
  BOPCol_ListOfShape& aLS=BOPTest_Objects::Shapes();
  aLS.Clear();
  //
  return 0;
}
//=======================================================================
//function : baddtools
//purpose  : 
//=======================================================================
Standard_Integer baddtools (Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n<2) {
    printf(" Use baddtools s1 s2 ...\n");
    return 0;
  }
  //
  Standard_Integer i;
  TopoDS_Shape aS;
  //
  BOPCol_ListOfShape& aLS=BOPTest_Objects::Tools();
  for (i = 1; i < n; ++i) {
    aS=DBRep::Get(a[i]);
    aLS.Append(aS);
  }
  //
  return 0;
}
//=======================================================================
//function : bcleartools
//purpose  : 
//=======================================================================
Standard_Integer bcleartools (Draw_Interpretor& , Standard_Integer n, const char** )
{
  if (n!=1) {
    printf(" Use bcleartools\n");
    return 0;
  }
  BOPCol_ListOfShape& aLS=BOPTest_Objects::Tools();
  aLS.Clear();
  //
  return 0;
}
