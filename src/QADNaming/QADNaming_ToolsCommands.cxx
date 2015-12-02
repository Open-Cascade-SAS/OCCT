// Created on: 1999-06-24
// Created by: Sergey ZARITCHNY
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

#include <Draw.hxx>
#include <DBRep.hxx>
#include <Draw_Interpretor.hxx>
#include <QADNaming.hxx>
#include <BRepTools.hxx>
#include <TopoDS_Face.hxx>
#include <TopLoc_Location.hxx>
#include <BRep_Builder.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <BRepTools.hxx>
#include <TCollection_AsciiString.hxx>
#include <TNaming_Translator.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <DNaming_DataMapOfShapeOfName.hxx>
#include <DNaming_DataMapIteratorOfDataMapOfShapeOfName.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopAbs.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>



//=======================================================================
//function : QADNaming_CheckHasSame 
//purpose  : CheckIsSame  Shape1 Shape2 
//           - for test ShapeCopy mechanism
//=======================================================================

static Standard_Integer QADNaming_CheckHasSame (Draw_Interpretor& di,
					      Standard_Integer nb, 
					      const char** arg)
{
  if(nb < 4) return 1;
  TopoDS_Shape S1 = DBRep::Get(arg[1]);
  if ( S1.IsNull() ) {
    BRep_Builder aBuilder;
    BRepTools::Read( S1, arg[1], aBuilder);
    }
  
  TopoDS_Shape S2 = DBRep::Get(arg[2]);
  if ( S2.IsNull() ) {
    BRep_Builder aBuilder;
    BRepTools::Read( S2, arg[2], aBuilder);
    }
  char M[8];
  strcpy(M, arg[3]);
  strtok(M, " \t");
  TopAbs_ShapeEnum mod = TopAbs_FACE;
  if(M[0] == 'F' || M[0] == 'f')
    mod = TopAbs_FACE;
  else if(M[0] == 'E' || M[0] == 'e')
    mod = TopAbs_EDGE;
  else if(M[0] == 'V' || M[0] == 'v')
    mod = TopAbs_VERTEX;
  else 
    return 1;

  TopExp_Explorer Exp1, Exp2;

  TopTools_MapOfShape M1, M2;
  for(Exp1.Init(S1, mod);Exp1.More();Exp1.Next()) {
    M1.Add(Exp1.Current());
  }
  for(Exp2.Init(S2, mod);Exp2.More();Exp2.Next()) {
    M2.Add(Exp2.Current());
  }

  TopTools_MapIteratorOfMapOfShape itr1(M1);
  TopTools_MapIteratorOfMapOfShape itr2;
  for(;itr1.More();itr1.Next()) {
    const TopoDS_Shape& s1 = itr1.Key();
    
    for(itr2.Initialize(M2);itr2.More();itr2.Next()) {
      const TopoDS_Shape& s2 = itr2.Key();
      if(s1.IsSame(s2))
	di << "Shapes " << arg[1]<< " and "<< arg[2]<< " have SAME subshapes\n";
    }
  }

  return 0;
}           
//=======================================================================
//function : QADNaming_TCopyShape
//purpose  : CopyShape  Shape1 [Shape2 ...] 
//           - for test ShapeCopy mechanism
//=======================================================================

static Standard_Integer QADNaming_TCopyShape (Draw_Interpretor& di,
					      Standard_Integer nb, 
					      const char** arg)
{
  TNaming_Translator TR;
  if(nb < 2) return (1);

  DNaming_DataMapOfShapeOfName aDMapOfShapeOfName;
  for(Standard_Integer i= 1;i < nb; i++) {
    TopoDS_Shape S = DBRep::Get(arg[i]);
    TCollection_AsciiString name(arg[i]);
    name.AssignCat("_c");
    if ( S.IsNull() ) {
      BRep_Builder aBuilder;
      BRepTools::Read( S, arg[i], aBuilder);
    }
    
// Add to Map                
    if(S.IsNull()) return(1);
    else {
      aDMapOfShapeOfName.Bind(S, name);
      TR.Add(S);
    }
  } // for ...

// PERFORM 
  TR.Perform();

  if(TR.IsDone()){
//    cout << "QADNaming_CopyShape:: Copy is Done " << endl;

    DNaming_DataMapIteratorOfDataMapOfShapeOfName itrn(aDMapOfShapeOfName);
    for(;itrn.More();itrn.Next()) {
      TCollection_AsciiString name = itrn.Value();
      const TopoDS_Shape& Result = TR.Copied(itrn.Key());
      DBRep::Set(name.ToCString(), Result);
      di.AppendElement(name.ToCString());
    }
    return 0;
  }
  di << "QADNaming_CopyShape : Error\n";
  return 1;
}

//=======================================================================
//function : ToolsCommands
//purpose  : 
//=======================================================================

void QADNaming::ToolsCommands (Draw_Interpretor& theCommands)
{  

  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  const char* g = "Naming data commands" ;
  
  theCommands.Add ("CopyShape", 
                   "CopyShape (Shape1 [Shape2] ...)",
		   __FILE__, QADNaming_TCopyShape, g); 

  theCommands.Add ("CheckSame", 
                   "CheckSame (Shape1 Shape2 ExploMode{F|E|V]",
		   __FILE__, QADNaming_CheckHasSame, g);   
}
