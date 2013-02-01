// Created on: 2002-03-18
// Created by: QA Admin
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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

#include <QABugs.hxx>

#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <AIS_InteractiveContext.hxx>
#include <ViewerTest.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>

#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <GC_MakeTrimmedCone.hxx>

static Standard_Integer BUC60857 (Draw_Interpretor& di, Standard_Integer /*argc*/,const char ** argv)
{
  gp_Ax2  Cone_Ax;
  double R1=8, R2=16, H1=20, H2=40, angle;
  gp_Pnt P0(0,0,0), P1(0,0,20), P2(0,0,45);
  angle = 2*M_PI;

  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) {
    di << "Use vinit command before " << argv[0] << "\n";
    return 1;
  }

  Handle(Geom_RectangularTrimmedSurface) S = GC_MakeTrimmedCone (P1, P2, R1, R2).Value();
  TopoDS_Shape myshape = BRepBuilderAPI_MakeFace(S, Precision::Confusion()).Shape();
  DBRep::Set("BUC60857_BLUE",myshape);
  Handle(AIS_Shape) ais1 = new AIS_Shape(myshape);
  aContext->Display(ais1);
  aContext->SetColor(ais1, Quantity_NOC_BLUE1);

  Handle(Geom_RectangularTrimmedSurface) S2 = GC_MakeTrimmedCone (P1, P2, R1, 0).Value();
  TopoDS_Shape myshape2 = BRepBuilderAPI_MakeFace(S2, Precision::Confusion()).Shape();
  DBRep::Set("BUC60857_RED",myshape2);
  Handle(AIS_Shape) ais2 = new AIS_Shape(myshape2);
  aContext->Display(ais2);
  aContext->SetColor(ais2, Quantity_NOC_RED);

  Handle(Geom_RectangularTrimmedSurface) S3 = GC_MakeTrimmedCone (P1, P2, R2, R1).Value();
  TopoDS_Shape myshape3 = BRepBuilderAPI_MakeFace(S3, Precision::Confusion()).Shape();
  DBRep::Set("BUC60857_GREEN",myshape3);
  Handle(AIS_Shape) ais3 = new AIS_Shape(myshape3);
  aContext->Display(ais3);
  aContext->SetColor(ais3, Quantity_NOC_GREEN);

  return 0;
}

#include <ViewerTest_DoubleMapOfInteractiveAndName.hxx>
#include <ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName.hxx>
#include <SelectMgr_Selection.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <SelectBasics_SensitiveEntity.hxx>
#if ! defined(WNT)
extern ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS();
#else
Standard_EXPORT ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS();
#endif

static Standard_Integer OCC137 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv) 
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) {
    di << argv[0] << "ERROR : use 'vinit' command before " << "\n";
    return 1;
  }
  if ( argc < 2 || argc > 3) {
    di << "ERROR : Usage : " << argv[0] << " highlight_mode [shape]" << "\n";
    return 1;
  }
  
  ViewerTest_DoubleMapOfInteractiveAndName aMap ;
  if(argc != 3) {
    aMap.Assign(GetMapOfAIS());
  } else {
    ViewerTest_DoubleMapOfInteractiveAndName& aMap1 = GetMapOfAIS();
    TCollection_AsciiString aName(argv[2]);
    Handle(AIS_InteractiveObject) AISObj;
    if(!aMap1.IsBound2(aName)) {
      di << "Use 'vdisplay' before" << "\n";
      return 1;
    } else {
      AISObj = Handle(AIS_InteractiveObject)::DownCast(aMap1.Find2(aName));
      if(AISObj.IsNull()){
        di << argv[2] << " : No interactive object" << "\n";
        return 1;
      }
      aMap.Bind(AISObj,aName);
    }
  }
  ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it(GetMapOfAIS());
  while ( it.More() ) {
    Handle(AIS_InteractiveObject) AISObj = Handle(AIS_InteractiveObject)::DownCast(it.Key1());
    AISObj->SetHilightMode(Draw::Atoi(argv[1]));
    if(AISObj->HasSelection(4)) {
      //Handle(SelectMgr_Selection)& aSelection = AISObj->Selection(4);
      const Handle(SelectMgr_Selection)& aSelection = AISObj->Selection(4);
      if(!aSelection.IsNull()) {
        for(aSelection->Init();aSelection->More();aSelection->Next()) {
          Handle(StdSelect_BRepOwner) aO = Handle(StdSelect_BRepOwner)::DownCast(aSelection->Sensitive()->OwnerId());
          aO->SetHilightMode(Draw::Atoi(argv[1]));
        }
      }
    }
    it.Next();
  }
  
  return 0;
}

static Standard_Integer OCC137_z (Draw_Interpretor& di, Standard_Integer argc, const char ** argv) 
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) {
    di << argv[0] << "ERROR : use 'vinit' command before " << "\n";
    return 1;
  }
  if ( argc != 1 && argc != 2) {
    di << "ERROR : Usage : " << argv[0] << " [ZDetection_mode]" << "\n";
    return 1;
  }
  aContext->SetZDetection(((argc == 1 || (argc == 2 && Draw::Atoi(argv[1]) == 1)) ? Standard_True : Standard_False));
  return 0;
}

void QABugs::Commands_9(Draw_Interpretor& theCommands) {
  const char *group = "QABugs";

  theCommands.Add ("BUC60857", "BUC60857", __FILE__, BUC60857, group);
  theCommands.Add("OCC137","OCC137 mode [shape]",__FILE__,OCC137,group);
  theCommands.Add("OCC137_z","OCC137_z [ZDetection_mode]",__FILE__,OCC137_z,group);

  return;
}
