// Created by: Peter KURNEV
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


#include <BOPTest.hxx>
#include <BOPTest_Objects.hxx>
#include <DBRep.hxx>
#include <Draw_Interpretor.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ListOfShape.hxx>

#include <stdio.h>
//
//
//
static Standard_Integer baddobjects (Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer bclearobjects (Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer baddtools   (Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer bcleartools (Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer baddcompound(Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer baddctools  (Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer bclear   (Draw_Interpretor&, Standard_Integer, const char**);

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
  const char* g = "BOPTest commands";
  // Commands
  theCommands.Add("baddobjects"    , "use baddobjects s1 s2 ..." , __FILE__, baddobjects, g);
  theCommands.Add("bclearobjects"  , "use bclearobjects"         , __FILE__, bclearobjects, g);
  theCommands.Add("baddtools"      , "use baddtools s1 s2 ..."   , __FILE__, baddtools, g);
  theCommands.Add("bcleartools"    , "use bcleartools"           , __FILE__, bcleartools, g);
  theCommands.Add("baddcompound"   , "use baddcompound c"        , __FILE__, baddcompound, g);
  theCommands.Add("baddctools"     , "use baddctools c"          , __FILE__, baddctools, g);
  theCommands.Add("bclear" , "use bclear"        , __FILE__, bclear, g);
}
//=======================================================================
//function : baddcompound
//purpose  : 
//=======================================================================
Standard_Integer baddcompound (Draw_Interpretor& , 
                               Standard_Integer n, 
                               const char** a)
{
  if (n<2) {
    printf(" use baddcompound c\n");
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
//=======================================================================
//function : baddctools
//purpose  : 
//=======================================================================
Standard_Integer baddctools (Draw_Interpretor& , 
                             Standard_Integer n, 
                             const char** a)
{
  if (n<2) {
    printf(" use baddctools c\n");
    return 0;
  }
  //
  TopoDS_Iterator aIt;
  TopoDS_Shape aS;
  //
  aS=DBRep::Get(a[1]);
  //
  BOPCol_ListOfShape& aLT=BOPTest_Objects::Tools();
  aIt.Initialize(aS);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSx=aIt.Value();
    aLT.Append(aSx);
  }
  //
  return 0;
}
//
//=======================================================================
//function :baddobjects
//purpose  : 
//=======================================================================
Standard_Integer baddobjects (Draw_Interpretor& , 
                              Standard_Integer n, 
                              const char** a)
{
  if (n<2) {
    printf(" use baddobjects s1 s2 ...\n");
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
Standard_Integer bclearobjects (Draw_Interpretor& , 
                                Standard_Integer n, 
                                const char** )
{
  if (n!=1) {
    printf(" use bclearobjects\n");
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
Standard_Integer baddtools (Draw_Interpretor& , 
                            Standard_Integer n, 
                            const char** a)
{
  if (n<2) {
    printf(" use baddtools s1 s2 ...\n");
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
Standard_Integer bcleartools (Draw_Interpretor& ,
                              Standard_Integer n, 
                              const char** )
{
  if (n!=1) {
    printf(" use bcleartools\n");
    return 0;
  }
  BOPCol_ListOfShape& aLS=BOPTest_Objects::Tools();
  aLS.Clear();
  //
  return 0;
}
//=======================================================================
//function : bclear
//purpose  : 
//=======================================================================
Standard_Integer bclear(Draw_Interpretor& di, 
                        Standard_Integer n, 
                        const char** ) 
{
  if (n!=1) {
    di << " use bclear\n";
    return 0;
  }
  //
  BOPTest_Objects::Clear(); 
  return 0;
}
