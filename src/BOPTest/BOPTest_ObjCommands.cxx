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

static Standard_Integer baddobjects   (Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer bclearobjects (Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer baddtools     (Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer bcleartools   (Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer baddcompound  (Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer baddctools    (Draw_Interpretor& , Standard_Integer , const char** );
static Standard_Integer bclear        (Draw_Interpretor&, Standard_Integer, const char**);

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
  theCommands.Add("baddobjects", "Adds objects for Boolean/GF/Split/Cells operations.\n"
                  "\t\tThe command has cumulative effect, thus can be used several times for single operation.\n"
                  "\t\tFor new operation the objects have to be cleared by bclearobjects or bclear commands.\n"
                  "\t\tUsage: baddobjects s1 s2 ...",
                  __FILE__, baddobjects, g);

  theCommands.Add("bclearobjects", "Clears the objects previously added for Boolean/GF/Split/Cells operations.\n"
                  "\t\tUsage: bclearobjects",
                  __FILE__, bclearobjects, g);

  theCommands.Add("baddtools", "Adds tools for Boolean/GF/Split/Cells operations.\n"
                  "\t\tThe command has cumulative effect, thus can be used several times for single operation.\n"
                  "\t\tFor new operation the tools have to be cleared by bcleartools or bclear commands.\n"
                  "\t\tUsage: baddtools s1 s2 ...",
                  __FILE__, baddtools, g);

  theCommands.Add("bcleartools", "Clears the tools previously added for Boolean/GF/Split/Cells operations.\n"
                  "\t\tUsage: bcleartools",
                  __FILE__, bcleartools, g);

  theCommands.Add("baddcompound", "Command for adding multiple objects for Boolean/GF/Split/Cells operations grouped in one object.\n"
                  "\t\tGiven object will be exploded on first level sub-shapes and each of these sub-shapes will act as a separate object.\n"
                  "\t\tThe command has cumulative effect, thus can be used several times for single operation.\n"
                  "\t\tFor new operation the objects have to be cleared by bclearobjects or bclear commands.\n"
                  "\t\tUsage: baddcompound c",
                 __FILE__, baddcompound, g);

  theCommands.Add("baddctools", "Command for adding multiple tools for Boolean/GF/Split/Cells operations grouped in one object.\n"
                  "\t\tGiven object will be exploded on first level sub-shapes and each of these sub-shapes will act as a separate tool.\n"
                  "\t\tThe command has cumulative effect, thus can be used several times for single operation.\n"
                  "\t\tFor new operation the tools have to be cleared by bcleartools or bclear commands.\n"
                  "\t\tUsage: baddctools c",
                  __FILE__, baddctools, g);

  theCommands.Add("bclear" , "Clears both objects and tools previously added for Boolean/GF/Split/Cells operations.\n"
                  "\t\tUsage: bclear",
                  __FILE__, bclear, g);
}

//=======================================================================
//function : baddcompound
//purpose  : 
//=======================================================================
Standard_Integer baddcompound (Draw_Interpretor& di,
                               Standard_Integer n,
                               const char** a)
{
  if (n < 2) {
    di.PrintHelp(a[0]);
    return 1;
  }
  //
  TopoDS_Iterator aIt;
  TopoDS_Shape aS;
  //
  aS=DBRep::Get(a[1]);
  //
  TopTools_ListOfShape& aLS=BOPTest_Objects::Shapes();
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
Standard_Integer baddctools (Draw_Interpretor& di,
                             Standard_Integer n,
                             const char** a)
{
  if (n < 2) {
    di.PrintHelp(a[0]);
    return 1;
  }
  //
  TopoDS_Iterator aIt;
  TopoDS_Shape aS;
  //
  aS=DBRep::Get(a[1]);
  //
  TopTools_ListOfShape& aLT=BOPTest_Objects::Tools();
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
Standard_Integer baddobjects (Draw_Interpretor& di,
                              Standard_Integer n,
                              const char** a)
{
  if (n < 2) {
    di.PrintHelp(a[0]);
    return 1;
  }
  //
  Standard_Integer i;
  TopoDS_Shape aS;
  //
  TopTools_ListOfShape& aLS=BOPTest_Objects::Shapes();
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
Standard_Integer bclearobjects (Draw_Interpretor& di,
                                Standard_Integer n,
                                const char** a)
{
  if (n != 1) {
    di.PrintHelp(a[0]);
    return 1;
  }
  TopTools_ListOfShape& aLS=BOPTest_Objects::Shapes();
  aLS.Clear();
  //
  return 0;
}
//=======================================================================
//function : baddtools
//purpose  : 
//=======================================================================
Standard_Integer baddtools (Draw_Interpretor& di,
                            Standard_Integer n,
                            const char** a)
{
  if (n < 2) {
    di.PrintHelp(a[0]);
    return 1;
  }
  //
  Standard_Integer i;
  TopoDS_Shape aS;
  //
  TopTools_ListOfShape& aLS=BOPTest_Objects::Tools();
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
Standard_Integer bcleartools (Draw_Interpretor& di,
                              Standard_Integer n,
                              const char** a)
{
  if (n != 1) {
    di.PrintHelp(a[0]);
    return 1;
  }
  TopTools_ListOfShape& aLS=BOPTest_Objects::Tools();
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
                        const char** a)
{
  if (n != 1) {
    di.PrintHelp(a[0]);
    return 1;
  }
  //
  BOPTest_Objects::Clear(); 
  return 0;
}
