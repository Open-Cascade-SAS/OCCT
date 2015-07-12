// Created on: 1999-12-23
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
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepLib.hxx>
#include <BRepTools.hxx>
#include <DBRep.hxx>
#include <DDF.hxx>
#include <DDocStd.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <GCE2d_MakeLine.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <QANewBRepNaming_Common.hxx>
#include <QANewBRepNaming_Cut.hxx>
#include <QANewBRepNaming_Fuse.hxx>
#include <QANewBRepNaming_Gluing.hxx>
#include <QANewBRepNaming_Intersection.hxx>
#include <QANewBRepNaming_Limitation.hxx>
#include <QANewDBRepNaming.hxx>
#include <QANewModTopOpe_Glue.hxx>
#include <QANewModTopOpe_Intersection.hxx>
#include <QANewModTopOpe_Limitation.hxx>
#include <Standard_Assert.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDF_LabelMap.hxx>
#include <TDocStd_Document.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

//==============================================================
//function : QANewDBRepNaming_NameBooleanOperationFeat
//purpose  : NameBooleanOperationFeat Doc Label Op = 1(Fuse), 2(Cut), 3(Common), 4(Inserction) 5(Limitation) DrawShape1 DrawShape2 [Forward = 0 || Reversed = 1 || BothSides = 2] 
//==============================================================
static Standard_Integer QANewDBRepNaming_NameBooleanOperationFeat (Draw_Interpretor& di,
							      Standard_Integer nb, 
							      const char ** arg)
{
  if (nb != 6 && nb != 7) {
    di <<  "QANewDBRepNaming_NameBooleanOperationFeat : Error"  << "\n";
    return 1;
  }

  Handle(TDocStd_Document) D;
  if (!DDocStd::GetDocument(arg[1],D)) return 1;   
  TDF_Label L;
  if (!DDF::AddLabel(D->GetData(), arg[2], L)) return 1;  
  
  const Standard_Integer Op = (Standard_Integer)Draw::Atoi(arg[3]);
  const TopoDS_Shape& S1 = DBRep::Get(arg[4]);
  const TopoDS_Shape& S2 = DBRep::Get(arg[5]);

  Standard_Integer Orientation = 0;
  if (nb == 7) Orientation = Draw::Atoi(arg[6]);

  switch (Op) {
  case 1: {
    di <<  "BRepAlgoAPI_Fuse"  << "\n";
    BRepAlgoAPI_Fuse mkFuse(S1, S2);

    Standard_ASSERT_RAISE(mkFuse.IsDone(), "Fuse failed");
    QANewBRepNaming_Fuse nameBool(L);
    nameBool.Load(mkFuse);
    break;
  }
  case 2: {
    di <<  "BRepAlgoAPI_Cut"  << "\n";
    BRepAlgoAPI_Cut mkCut(S1, S2);

    Standard_ASSERT_RAISE(mkCut.IsDone(), "Cut failed");
    QANewBRepNaming_Cut nameBool(L);
    nameBool.Load(mkCut);
    break; 
  }
  case 3: {
    di <<  "BRepAlgoAPI_Common"  << "\n";
    BRepAlgoAPI_Common mkCommon(S1, S2);

    Standard_ASSERT_RAISE(mkCommon.IsDone(), "Common failed");
    QANewBRepNaming_Common nameBool(L);
    nameBool.Load(mkCommon);
    break;
  }
  case 4: {
    QANewModTopOpe_Intersection mkSection(S1, S2);
    Standard_ASSERT_RAISE(mkSection.IsDone(), "Section failed");
    QANewBRepNaming_Intersection nameBool(L);
    nameBool.Load(mkSection);
    break;
  }    
  case 5: {
    QANewModTopOpe_Limitation mkLimit(S1, S2);
    const QANewModTopOpe_ModeOfLimitation aMode = (QANewModTopOpe_ModeOfLimitation) Orientation;
    mkLimit.SetMode(aMode);
    mkLimit.Cut();
//     if (Orientation = 0) mkLimit.CutForward();
//     else if (Orientation = 1) mkLimit.CutReversed();
//     else if (Orientation = 2) mkLimit.CutBothSides();
    Standard_ASSERT_RAISE(mkLimit.IsDone(),"Limit failed");
    QANewBRepNaming_Limitation nameBool(L);
    nameBool.Load(mkLimit);
    break;
  }  
  default:
    di<<"QANewDBRepNaming_NameBooleanOperation The type of operation must be 1 - fuse, 2 - cut or 3 - common"<<"\n";
  }

  return 0;
}    

//==============================================================
//function : QANewDBRepNaming_NameFuse
//purpose  : NameFuse Doc Label DrawShape1 DrawShape2
//==============================================================

static Standard_Integer QANewDBRepNaming_NameFuse (Draw_Interpretor& di,
					      Standard_Integer nb, 
					      const char ** arg)
{
  if (nb != 5) {
    di <<  "QANewDBRepNaming_NameFuse : Error"  << "\n";
    return 1;
  }

  Handle(TDocStd_Document) D;
  if (!DDocStd::GetDocument(arg[1],D)) return 1;   
  TDF_Label L;
  if (!DDF::AddLabel(D->GetData(), arg[2], L)) return 1;  
  const TopoDS_Shape& S1 = DBRep::Get(arg[3]);
  const TopoDS_Shape& S2 = DBRep::Get(arg[4]);
  QANewBRepNaming_Fuse nameBool(L);

  di <<  "BRepAlgoAPI_Fuse"  << "\n";
  BRepAlgoAPI_Fuse mkFuse(S1, S2);

  Standard_ASSERT_RAISE(mkFuse.IsDone(), "Fuse failed");
  nameBool.Load(mkFuse);
  return 0;
}    

//==============================================================
//function : QANewDBRepNaming_NameCut
//purpose  : NameCut Doc Label DrawShape1 DrawShape2
//==============================================================

static Standard_Integer QANewDBRepNaming_NameCut (Draw_Interpretor& di,
					     Standard_Integer nb, 
					     const char ** arg)
{
  if (nb != 5) {
    di <<  "QANewDBRepNaming_NameCut : Error"  << "\n";
    return 1;
  }

  Handle(TDocStd_Document) D;
  if (!DDocStd::GetDocument(arg[1],D)) return 1;   
  TDF_Label L;
  if (!DDF::AddLabel(D->GetData(), arg[2], L)) return 1;  
  const TopoDS_Shape& S1 = DBRep::Get(arg[3]);
  const TopoDS_Shape& S2 = DBRep::Get(arg[4]);
// DBRep::Set("aShape", S1);
  QANewBRepNaming_Cut nameBool(L);
// DBRep::Set("S1", S1);
// DBRep::Set("S2", S2);

  di <<  "BRepAlgoAPI_Cut"  << "\n";
  BRepAlgoAPI_Cut mkCut(S1, S2);

// DBRep::Set("CutRes", mkCut.Shape());
// DBRep::Set("Shape1", mkCut.Shape1());
// DBRep::Set("Shape2", mkCut.Shape2());
// BRepTools::Write(mkCut.Shape1(), "/dn04/OS/SAMTECH/env/S1.brep");
  Standard_ASSERT_RAISE(mkCut.IsDone(), "Cut failed");
  nameBool.Load(mkCut);
// BRepTools::Write(mkCut.Shape1(), "/dn04/OS/SAMTECH/env/S2.brep");
  return 0;
}    

//==============================================================
//function : QANewDBRepNaming_NameCommon
//purpose  : NameCommon Doc Label DrawShape1 DrawShape2
//==============================================================

static Standard_Integer QANewDBRepNaming_NameCommon (Draw_Interpretor& di,
						Standard_Integer nb, 
						const char ** arg)
{
  if (nb != 5) {
    di <<  "QANewDBRepNaming_NameCommon : Error"  << "\n";
    return 1;
  }

  Handle(TDocStd_Document) D;
  if (!DDocStd::GetDocument(arg[1],D)) return 1;   
  TDF_Label L;
  if (!DDF::AddLabel(D->GetData(), arg[2], L)) return 1;  
  const TopoDS_Shape& S1 = DBRep::Get(arg[3]);
  const TopoDS_Shape& S2 = DBRep::Get(arg[4]);
  QANewBRepNaming_Common nameBool(L);

  di <<  "BRepAlgoAPI_Common"  << "\n";
  BRepAlgoAPI_Common mkCommon(S1, S2);

  Standard_ASSERT_RAISE(mkCommon.IsDone(), "Common failed");
  nameBool.Load(mkCommon);
  return 0;
}    

//==============================================================
//function : QANewDBRepNaming_NameIntersection
//purpose  : NameIntersection Doc Label DrawShape1 DrawShape2
//==============================================================

static Standard_Integer QANewDBRepNaming_NameIntersection (Draw_Interpretor& di,
						      Standard_Integer nb, 
						      const char ** arg)
{
  if (nb != 5) {
    di <<  "QANewDBRepNaming_NameIntersection : Error"  << "\n";
    return 1;
  }

  Handle(TDocStd_Document) D;
  if (!DDocStd::GetDocument(arg[1],D)) return 1;   
  TDF_Label L;
  if (!DDF::AddLabel(D->GetData(), arg[2], L)) return 1;  
  const TopoDS_Shape& S1 = DBRep::Get(arg[3]);
  const TopoDS_Shape& S2 = DBRep::Get(arg[4]);
  QANewBRepNaming_Intersection nameBool(L);
  QANewModTopOpe_Intersection mkIntersection(S1, S2);
  Standard_ASSERT_RAISE(mkIntersection.IsDone(), "Section failed");
  nameBool.Load(mkIntersection);
  return 0;
}    

//==============================================================
//function : QANewDBRepNaming_NameLimit
//purpose  : NameLimit Doc Label DrawShape1 DrawShape2 [Orientation (Forward - 0, Reversed - 1 or BothSise - 2)]
//==============================================================

static Standard_Integer QANewDBRepNaming_NameLimit (Draw_Interpretor& di,
					       Standard_Integer nb, 
					       const char ** arg)
{
  if (nb != 5 && nb != 6) {
    di <<  "QANewDBRepNaming_NameLimit : Error"  << "\n";
    return 1;
  }

  Handle(TDocStd_Document) D;
  if (!DDocStd::GetDocument(arg[1],D)) return 1;   
  TDF_Label L;
  if (!DDF::AddLabel(D->GetData(), arg[2], L)) return 1;  
  const TopoDS_Shape& S1 = DBRep::Get(arg[3]);
  const TopoDS_Shape& S2 = DBRep::Get(arg[4]);
  Standard_Integer Orientation = 0;
  if (nb == 6) {
    Orientation = Draw::Atoi(arg[5]);
  }
  QANewBRepNaming_Limitation nameBool(L);
  const QANewModTopOpe_ModeOfLimitation aMode = (QANewModTopOpe_ModeOfLimitation) Orientation;
  QANewModTopOpe_Limitation mkLimit(S1, S2, aMode);
  mkLimit.Cut();
 //  if (Orientation == 0) mkLimit.CutForward();
//   else if (Orientation == 1) mkLimit.CutReversed();
//   else if (Orientation == 2) mkLimit.CutBothSides();
  Standard_ASSERT_RAISE(mkLimit.IsDone(), "Limit failed");
  nameBool.Load(mkLimit);
  return 0;
}    

//==============================================================
//function : QANewDBRepNaming_NameGlue
//purpose  : NameGlue Doc Label ObjectLabel ToolLabel [auxiliary valid labels]
//==============================================================

static Standard_Integer QANewDBRepNaming_NameGlue (Draw_Interpretor& di,
					      Standard_Integer nb, 
					      const char ** arg)
{
  if (nb < 5) {
    di <<  "QANewDBRepNaming_NameGlue : Error"  << "\n";
    return 1;
  }

  Handle(TDocStd_Document) D;
  if (!DDocStd::GetDocument(arg[1],D)) return 1;   
  TDF_Label L,OL,TL;
  if (!DDF::AddLabel(D->GetData(), arg[2], L)) return 1;  
  if (!DDF::AddLabel(D->GetData(), arg[3], OL)) return 1;  
  if (!DDF::AddLabel(D->GetData(), arg[4], TL)) return 1;  
  TDF_LabelMap aLog;
  for(Standard_Integer i=5;i<nb;i++) {
    TDF_Label aLabel;
    if (!DDF::AddLabel(D->GetData(), arg[i], aLabel)) return 1;
    aLog.Add(aLabel);
  }
  Handle(TNaming_NamedShape) anObjectNS,aToolNS;
  if (!OL.FindAttribute(TNaming_NamedShape::GetID(),anObjectNS)) return 1;
  if (!TL.FindAttribute(TNaming_NamedShape::GetID(),aToolNS)) return 1;

  QANewModTopOpe_Glue aMKGlue(TNaming_Tool::CurrentShape(anObjectNS),TNaming_Tool::CurrentShape(aToolNS));
  if (!aMKGlue.IsDone()) return 1;
  aMKGlue.Build();
  if (!aMKGlue.IsDone()) return 1;

  QANewBRepNaming_Gluing aGlue(L);
  aGlue.SetContext(TNaming_Tool::CurrentShape(anObjectNS),TNaming_Tool::CurrentShape(aToolNS));
  aGlue.SetLog(aLog);
  aGlue.Load(aMKGlue);
  return 0;
}    

//=======================================================================
//function : FeatureCommands
//purpose  : 
//=======================================================================

void QANewDBRepNaming::FeatureCommands (Draw_Interpretor& theCommands)
{  
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  const char* g = "Naming algorithm commands for features" ;


  theCommands.Add ("NameBooleanOperationFeat", 
		   "NameBooleanOperationFeat Doc Label Operation(1, 2, 3, 4 or 5) DrawShape1 DrawShape2 [Forward = 0 || Reversed = 1 || BothSides = 2] ",
		   __FILE__, QANewDBRepNaming_NameBooleanOperationFeat, g);

  theCommands.Add ("NameFuse", 
		   "NameFuse Doc Label DrawShape1 DrawShape2",
		   __FILE__, QANewDBRepNaming_NameFuse, g);

  theCommands.Add ("NameCut", 
		   "NameCut Doc Label DrawShape1 DrawShape2",
		   __FILE__, QANewDBRepNaming_NameCut, g);

  theCommands.Add ("NameCommon", 
		   "NameCommon Doc Label DrawShape1 DrawShape2",
		   __FILE__, QANewDBRepNaming_NameCommon, g);

  theCommands.Add ("NameIntersection", 
		   "NameIntersection Doc Label DrawShape1 DrawShape2",
		   __FILE__, QANewDBRepNaming_NameIntersection, g);

  theCommands.Add ("NameLimit", 
		   "NameLimit Doc Label DrawShape1 DrawShape2 [Orientation (Forward - 0, Reversed - 1 or BothSise - 2)]",
		   __FILE__, QANewDBRepNaming_NameLimit, g);

  theCommands.Add ("NameGlue", 
		   "NameGlue Doc Label ObjectLabel ToolLabel [auxiliary valid labels]",
		   __FILE__, QANewDBRepNaming_NameGlue, g);

}
