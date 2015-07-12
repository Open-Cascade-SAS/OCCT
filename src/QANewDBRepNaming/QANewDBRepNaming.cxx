// Created on: 1999-12-08
// Created by: Vladislav ROMASHKO
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


#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <DBRep.hxx>
#include <DDF.hxx>
#include <Draw.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <QANewDBRepNaming.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelMap.hxx>
#include <TNaming.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Name.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Naming.hxx>
#include <TNaming_NamingTool.hxx>
#include <TNaming_Selector.hxx>
#include <TNaming_Tool.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <ViewerTest.hxx>

#include <stdio.h>
//////#include <Draw_PluginMacro.hxx>
//////#include <DBrowser.hxx>
static Standard_Integer QANewDBRepNaming_CheckNaming(Draw_Interpretor&,Standard_Integer,const char ** a);

//static Standard_Integer QANewDBRepNaming_myCheckNaming(Draw_Interpretor&,Standard_Integer,const char ** a);

static Standard_Integer QANewDBRepNaming_CheckSelectShape(Draw_Interpretor&,Standard_Integer,const char ** a);

static Standard_Integer QANewDBRepNaming_CheckSolve(Draw_Interpretor&,Standard_Integer,const char ** a);

#include <QADNaming.hxx>

//=======================================================================
//function : AllCommands
//purpose  : 
//=======================================================================

void  QANewDBRepNaming::AllCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  QANewDBRepNaming::PrimitiveCommands (theCommands);
  QANewDBRepNaming::FeatureCommands (theCommands);

  const char* g = "Check Naming command" ;

  theCommands.Add ("CheckNaming", 
                   "CheckNaming Doc TestLabel Full(1/0) (Label/Viewer) [Label] [DX[DY[DZ]]]",
		   __FILE__, QANewDBRepNaming_CheckNaming, g); 

  theCommands.Add ("CheckSelectShape", 
                   "CheckSelectShape Doc Label SubShapeType",
		   __FILE__, QANewDBRepNaming_CheckSelectShape, g); 

  theCommands.Add ("CheckSolve", 
                   "CheckSolve Doc Label",
		   __FILE__, QANewDBRepNaming_CheckSolve, g);   

  const char* com = "set Draw_NamingData 1";
  theCommands.Eval(com);
}

//=======================================================================
//function : CheckNaming
//purpose  : "CheckNaming Doc TestLabel Full(1/0) ShapeFrom(Viewer/Label) [ShapeLabel]" 
//=======================================================================

static Standard_Integer QANewDBRepNaming_CheckNaming(Draw_Interpretor& di,Standard_Integer nb, const char ** a)
{
  Handle(TDF_Data) DF;
  if(!DDF::GetDF(a[1], DF)) return 1;
  TDF_Label L;
  DDF::AddLabel(DF, a[2], L);

//  cout<<"SELECT A SHAPE ..."<<endl; 
  Handle(TNaming_NamedShape) amyNS;
  TopoDS_Shape aSelectedShape;
  if (!strcmp(a[4],"Label")){
    Handle(TNaming_NamedShape) atiNS=new TNaming_NamedShape;
    TDF_Label L2;
    DDF::AddLabel(DF, a[5], L2);
    if (!L2.FindAttribute(TNaming_NamedShape::GetID(), atiNS)) {
      di<<"Error: Cannot find shape on label."<<"\n";
      return 1;
    }
    amyNS = atiNS;
    aSelectedShape=TNaming_Tool::GetShape(atiNS);
  }
  else
  if (!strcmp(a[4],"Viewer"))
    aSelectedShape = ViewerTest::PickShape(TopAbs_SHAPE);
  else{
    di<<"Error: Incorrect argument #4"<<"\n";
    return 1;
  }

  if (aSelectedShape.IsNull()) {
    di<<"Nothing has been selected ... try again"<<"\n";
    return 1;
  }

  TopTools_IndexedMapOfShape allSubShapes;
  TopExp::MapShapes(aSelectedShape, allSubShapes);

  Standard_Integer i;

  for (i = 1; i <= allSubShapes.Extent(); i++) {
    const TDF_Label& aSubLabel = L.FindChild(i);
    const TopoDS_Shape& aSubShape = allSubShapes(i);
//    cout<<i<<"  ";TopAbs::Print(aSubShape.ShapeType(), cout); cout<<"  ";
    
//     DBRep::Set("Selected", aSubShape);

    TNaming_Selector SL (aSubLabel);
      
//    cout << i << " ";TopAbs::Print(aSelectedShape.ShapeType(), cout); cout<<"  ";
    try {
      OCC_CATCH_SIGNALS
      if(SL.Select(aSubShape, aSelectedShape) == Standard_False) {
	//    if(SL.Select(aSubShape, amyNS->Get()) == Standard_False) 
	di << "Selection error!!!" << "\n";
	return 1;
      }
      Handle(TNaming_NamedShape) aNS;
      if (!aSubLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
	di<<"Selection didn't produced a Named Shape ..."<<"\n";
	return 1;
      }
      const TopoDS_Shape& aResultOfSelection = TNaming_Tool::GetShape(aNS);
      if (!aSubShape.IsSame(aResultOfSelection)) {
	di<<"Failure of selection"<<"\n";
	return 1;
      } 
    } catch (Standard_Failure) {
      di<<"!!! Solving Failed !!!" << "\n";
      continue;
    }
    
  }

// ***

  if (!Draw::Atoi(a[3])) return 0;
  Standard_Real aDX = 100.0;
  Standard_Real aDY = 100.0;
  Standard_Real aDZ = 100.0;
  if(nb > 6) {
    aDX = Draw::Atof(a[6]);
    if(nb > 7) aDY = Draw::Atof(a[7]);
    if(nb > 8) aDZ = Draw::Atof(a[8]);
  }
//  cout<<endl;
//  cout<<"TRANSLATION ... DX = "<<aDX <<" DY = "<< aDY <<" DZ = " << aDZ <<endl;
  gp_Vec aTranslation(aDX, aDY, aDZ);
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(aTranslation);
  TopLoc_Location aLoc(aTrsf);
  const TDF_Label& anOriginLabel = TNaming_Tool::NamedShape(aSelectedShape, L)->Label();
  if (anOriginLabel.IsNull()) di<<"Origin is null"<<"\n";
//  TNaming::Displace(anOriginLabel, aLoc);
//  cout<<"Translate label ";anOriginLabel.EntryDump(cout);cout<<endl;
  TDF_ChildIterator Labelitr(L.Root(), Standard_False);
  for (; Labelitr.More(); Labelitr.Next()) {
//    if (Labelitr.Value() == anOriginLabel) break;
//    cout<<"Translate label ";Labelitr.Value().EntryDump(cout);cout<<endl;
    TNaming::Displace(Labelitr.Value(), aLoc);
  }


  {
    Handle(TNaming_NamedShape) aNS;
    if (!anOriginLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS)) return 1;
    aSelectedShape = TNaming_Tool::GetShape(aNS);
  }    
  TopTools_IndexedMapOfShape allTranslatedSubShapes;
  TopExp::MapShapes(aSelectedShape, allTranslatedSubShapes);

  Standard_Boolean isFailured = Standard_False;

//  cout<<"SOLVING ..."<<endl;
  TDF_LabelMap scope;
  TDF_ChildIterator itr(L.Root(), Standard_True);
  i = 1;
  TCollection_AsciiString aNotSolved;
  for (itr.Initialize(L, Standard_False); itr.More(); itr.Next(), i++) {
    Handle(TNaming_NamedShape) aNS;
    if (!itr.Value().FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
//      cout<<"Wrong selection ..."<<endl;
      return 1;
    }
//    cout<<i<<"  ";TopAbs::Print(TypeOfNaming, cout); cout<<"  ";
//     DBRep::Set("Solved", aNS->Get());
    TNaming_Selector SLSolving(itr.Value());
    if (!SLSolving.Solve(scope)) {
      isFailured = Standard_True;
      aNotSolved += " ";
      aNotSolved += i;
      continue;
    }
    const Handle(TNaming_NamedShape)& aResultOfSolving = SLSolving.NamedShape();
    if (aResultOfSolving.IsNull()) {
//       cout<<"Failure of Solving: it didn't produced a shape!"<<endl;
//       return 1;
      isFailured = Standard_True;
      aNotSolved += " ";
      aNotSolved += i;
      continue;
    }      
    TopoDS_Shape aRes;
/*    if (TypeOfNaming == TopAbs_WIRE || TypeOfNaming == TopAbs_SHELL) {
      TopoDS_Iterator itr(aResultOfSolving->Get());
      TopoDS_Shape aSubShape;
      if (itr.More()) aSubShape = itr.Value();
      TDF_LabelMap one,two;
      TNaming_Tool::FindShape(one, two, aResultOfSolving, aRes);
      if (aRes.IsNull()) {
//  	cout<<"Failure of TNaming_Tool::FindShape()!"<<endl;
// 	return 1;
	Sprintf(aNotSolved,"%s %d",aNotSolved,i);
	isFailured = Standard_True;
//	break;
	continue;
      }
    } else */
    aRes = aResultOfSolving->Get();
    if (!aRes.IsSame(allTranslatedSubShapes(i))) {
//       cout<<"Failure of solving of "<<i<<"-th sub label"<<endl;
//       TNaming::DumpShape(aRes);
//       TNaming::DumpShape(allTranslatedSubShapes(i));
//       return 1;
      gp_XYZ aCoord1(0,0,0),aCoord2(0,0,0);
      TopTools_MapOfShape aVertices;
      if (aRes.ShapeType()==TopAbs_VERTEX) aCoord1=BRep_Tool::Pnt(TopoDS::Vertex(aRes)).Coord(); else {
	TopExp_Explorer anExp(aRes,TopAbs_VERTEX);
	for(;anExp.More();anExp.Next()) {
	  // mpv: that's only stub for the seam edges bug
	  if (aVertices.Contains(anExp.Current())) continue;
	  aVertices.Add(anExp.Current());
	  
	  aCoord1.Add(BRep_Tool::Pnt(TopoDS::Vertex(anExp.Current())).Coord());
// 	  gp_XYZ a = BRep_Tool::Pnt(TopoDS::Vertex(anExp.Current())).Coord();
// 	  cout<<"a1 ("<<a.X()<<" "<<a.Y()<<" "<<a.Z()<<")"<<endl;
	}
      }
      if (allTranslatedSubShapes(i).ShapeType()==TopAbs_VERTEX)
	aCoord2=BRep_Tool::Pnt(TopoDS::Vertex(allTranslatedSubShapes(i))).Coord();
      else {
	aVertices.Clear();
	TopExp_Explorer anExp(allTranslatedSubShapes(i),TopAbs_VERTEX);
	for(;anExp.More();anExp.Next()) {
	  // mpv: that's only stub for the seam edges bug
	  if (aVertices.Contains(anExp.Current())) continue;
	  aVertices.Add(anExp.Current());

	  aCoord2.Add(BRep_Tool::Pnt(TopoDS::Vertex(anExp.Current())).Coord());
// 	  gp_XYZ a = BRep_Tool::Pnt(TopoDS::Vertex(anExp.Current())).Coord();
// 	  cout<<"a1 ("<<a.X()<<" "<<a.Y()<<" "<<a.Z()<<")"<<endl;
	}
      }
      if (!aCoord1.IsEqual(aCoord2,Precision::Confusion()) || aRes.ShapeType() != allTranslatedSubShapes(i).ShapeType()) {
	    aNotSolved += " ";
        aNotSolved += i;
	    isFailured = Standard_True;
	    continue;
      }
    }    
//    cout<<endl;
  }

  if (isFailured) {
    di<<aNotSolved;
//mpv: let's no catch returns 1 in tclsh    return 1;
  }
//  cout<<"Good!"<<endl;
  return 0;  
}
  
//=======================================================================
//function : CheckSelectShape
//purpose  : "CheckSelectShape Doc Label SubShapeType" 
//=======================================================================

static Standard_Integer QANewDBRepNaming_CheckSelectShape(Draw_Interpretor& di, Standard_Integer /*nb*/, const char ** a)
{
  Handle(TDF_Data) DF;
  if(!DDF::GetDF(a[1], DF)) return 1;
  TDF_Label L;
  DDF::AddLabel(DF, a[2], L);

  di<<"SELECT A SUB-SHAPE ..."<<"\n"; 
  TopoDS_Shape aSelectedShape = ViewerTest::PickShape((TopAbs_ShapeEnum)Draw::Atoi(a[3]));
  if (aSelectedShape.IsNull()) {
    di<<"QANewDBRepNaming_CheckSelectShape(): Nothing is selected ..."<<"\n";
    return 1;
  }
  // Find the context:
  TopoDS_Shape aContext;
  TDF_ChildIterator itr(L.Root());
  Standard_Boolean contextFound = Standard_False;
  for (; itr.More(); itr.Next()) {
    Handle(TNaming_NamedShape) NS;
    if (itr.Value().FindAttribute(TNaming_NamedShape::GetID(), NS)) {
      aContext = NS->Get();
      TopExp_Explorer expl(aContext, (TopAbs_ShapeEnum)Draw::Atoi(a[3]));
      for (; expl.More(); expl.Next()) {
	if (expl.Current().IsSame(aSelectedShape)) {
	  di<<"QANewDBRepNaming_CheckSelectShape(): The context is found at ";

	  //NS->Label().EntryDump(cout);
          Standard_SStream aSStream;
	  NS->Label().EntryDump(aSStream);
          di << aSStream;
          di<<"\n";
	  contextFound = Standard_True;
	  break;
	}
      }
      if (contextFound) break;
    }
    aContext = TopoDS_Shape();
  }
  // Selection:
  TNaming_Selector aSelector(L);
  if (aContext.IsNull()) {
    aSelector.Select(aSelectedShape);
  } else {
    aSelector.Select(aSelectedShape, aContext);
  }
  return 0;
}

//=======================================================================
//function : CheckSelectShape
//purpose  : "CheckSelectShape Doc Label SubShapeType" 
//=======================================================================

static Standard_Integer QANewDBRepNaming_CheckSolve(Draw_Interpretor&,Standard_Integer /*nb*/, const char ** a)
{
  Handle(TDF_Data) DF;
  if(!DDF::GetDF(a[1], DF)) return 1;
  TDF_Label L;
  DDF::AddLabel(DF, a[2], L);
  Handle(TNaming_NamedShape) aNS;
  L.FindAttribute(TNaming_NamedShape::GetID(), aNS);

  return 0;
}
