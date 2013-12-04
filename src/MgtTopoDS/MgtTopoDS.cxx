// Created on: 1993-03-09
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <MgtTopoDS.ixx>
#include <MgtTopLoc.hxx>

#include <PTopoDS_HShape.hxx>
#include <PTopoDS_TShape.hxx>
#include <PTopoDS_Shape1.hxx>
#include <PTopoDS_TShape1.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS_TShape.hxx>
#include <TopoDS_Iterator.hxx>

#include <PTopoDS_HArray1OfHShape.hxx>
#include <PTopoDS_HArray1OfShape1.hxx>

#include <BRepTools.hxx>

#include <PTColStd_TransientPersistentMap.hxx>
#include <PTColStd_PersistentTransientMap.hxx>

#ifdef chrono
#include <OSD_Timer.hxx>
extern OSD_Timer VertexTimer;
extern OSD_Timer EdgeTimer;
extern OSD_Timer WireTimer;
extern OSD_Timer FaceTimer;
extern OSD_Timer ShellTimer;
extern OSD_Timer SolidTimer;
extern OSD_Timer StoreEdgeTimer;
#define TIMER_START(THETIMER) THETIMER.Start()
#define TIMER_STOP(THETIMER) THETIMER.Stop()
#else
#define TIMER_START(THETIMER)
#define TIMER_STOP(THETIMER)
#endif

//=======================================================================
//function : Translate
//purpose  : Translation Transient Shape -> Persistent Shape
//           Used for upwards compatibility.
//=======================================================================

Handle(PTopoDS_HShape) MgtTopoDS::Translate
(const TopoDS_Shape& aShape,
 const Handle(MgtTopoDS_TranslateTool)& TrTool,
 PTColStd_TransientPersistentMap &aMap)
{
  if (aShape.IsNull()) 
    return new PTopoDS_HShape();

  // Translate the top-level shape
  Handle(PTopoDS_HShape) pHShape = new PTopoDS_HShape();

  Standard_Boolean translated = aMap.IsBound(aShape.TShape());
  if (translated) {
    // get the translated TShape
    
    Handle(Standard_Persistent) aPers = aMap.Find(aShape.TShape());
    Handle(PTopoDS_TShape)& pTS = (Handle(PTopoDS_TShape)&) aPers;

    pHShape->TShape(pTS);
  }
  else {

    // create if not translated and update
    
    switch (aShape.ShapeType()) {
      
    case TopAbs_VERTEX :
      TrTool->MakeVertex(pHShape);
      TrTool->UpdateVertex(aShape,pHShape, aMap);
      break;
      
    case TopAbs_EDGE :
#ifdef chrono
      StoreEdgeTimer.Start();
#endif
      TrTool->MakeEdge(pHShape);
      TrTool->UpdateEdge(aShape,pHShape, aMap);
#ifdef chrono
      StoreEdgeTimer.Stop();
#endif
      break;
      
    case TopAbs_WIRE :
      TrTool->MakeWire(pHShape);
      //TrTool.UpdateWire(aShape,pHShape);
      TrTool->UpdateShape(aShape,pHShape);
      break;
      
    case TopAbs_FACE :
      TrTool->MakeFace(pHShape);
      TrTool->UpdateFace(aShape,pHShape, aMap);
      break;
    
    case TopAbs_SHELL :
      TrTool->MakeShell(pHShape);
      //TrTool.UpdateShell(aShape,pHShape);
      TrTool->UpdateShape(aShape,pHShape);
      break;
    
    case TopAbs_SOLID :
      TrTool->MakeSolid(pHShape);
      //TrTool.UpdateSolid(aShape,pHShape);
      TrTool->UpdateShape(aShape,pHShape);
      break;
      
    case TopAbs_COMPSOLID :
      TrTool->MakeCompSolid(pHShape);
      //TrTool.UpdateCompSolid(aShape,pHShape);
      TrTool->UpdateShape(aShape,pHShape);
      break;
      
  case TopAbs_COMPOUND :
      TrTool->MakeCompound(pHShape);
      //TrTool.UpdateCompound(aShape,pHShape);
      TrTool->UpdateShape(aShape,pHShape);
      break;
    default:
      break;
    }
  
    // bind and copy the sub-elements

    aMap.Bind(aShape.TShape(),pHShape->TShape());
    // copy current Shape
    TopoDS_Shape S = aShape;
    S.Orientation(TopAbs_FORWARD);
    S.Location(TopLoc_Location());
    // count the number of <sub-shape> of the Shape's TShape
    Standard_Integer nbElem = 0;
    TopoDS_Iterator ItCount(S);
    while (ItCount.More()){
      nbElem ++;
      ItCount.Next();
    }

    if (nbElem != 0) {
      Handle(PTopoDS_HArray1OfHShape) myArray = 
	new PTopoDS_HArray1OfHShape(1,nbElem);
      // translate <sub-shapes>
      TopoDS_Iterator ItTrans(S);
      Standard_Integer i = 1;
      while(ItTrans.More()) {
	myArray->SetValue(i, MgtTopoDS::Translate(ItTrans.Value(),
						  TrTool, aMap));
	i++;
	ItTrans.Next();
      }
      pHShape->TShape()->Shapes(myArray);
    }
    
  }
  
  pHShape->Orientation(aShape.Orientation());
  pHShape->Location(MgtTopLoc::Translate(aShape.Location(), aMap));
  return pHShape;
}


//=======================================================================
//function : Translate
//purpose  : Translation Persistent HShape -> Transient Shape
//           Used for upwards compatibility.
//=======================================================================

void MgtTopoDS::Translate
(const Handle(PTopoDS_HShape)&             aHShape, 
 const Handle(MgtTopoDS_TranslateTool)&    TrTool,
 PTColStd_PersistentTransientMap&          aMap,
 TopoDS_Shape&                             theShape)
{
  if (aHShape->TShape().IsNull()) return;

  // Translate the top-level shape
#ifndef DEB // Linux porting - to avoid warnings
  if ( aMap.IsBound(aHShape->TShape()) ) {
#else
  Standard_Boolean translated = aMap.IsBound(aHShape->TShape());
  if (translated) {
#endif
    // get the translated TShape
    Handle(TopoDS_TShape) TS = 
      *((Handle(TopoDS_TShape)*) &aMap.Find(aHShape->TShape()));
    theShape.TShape(TS);
  }

  else {
    // create if not translated and update
    switch (aHShape->TShape()->ShapeType()) {
      
    case TopAbs_VERTEX :
      TIMER_START(VertexTimer);
      TrTool->MakeVertex(theShape);
      TrTool->UpdateVertex(aHShape,theShape, aMap);
      TIMER_STOP(VertexTimer);
      break;
      
    case TopAbs_EDGE :
      TIMER_START(EdgeTimer);
      TrTool->MakeEdge(theShape);
      TrTool->UpdateEdge(aHShape,theShape, aMap);
      TIMER_STOP(EdgeTimer);
      break;
      
    case TopAbs_WIRE :
      TIMER_START(WireTimer);
      TrTool->MakeWire(theShape);
      //TrTool.UpdateWire(aHShape,theShape);
      TrTool->UpdateShape(aHShape,theShape);
      TIMER_STOP(WireTimer);
      break;
      
    case TopAbs_FACE :
      TIMER_START(FaceTimer);
      TrTool->MakeFace(theShape);
      TrTool->UpdateFace(aHShape,theShape, aMap);
      TIMER_STOP(FaceTimer);
      break;
      
    case TopAbs_SHELL :
      TIMER_START(ShellTimer);
      TrTool->MakeShell(theShape);
      //TrTool.UpdateShell(aHShape,theShape);
      TrTool->UpdateShape(aHShape,theShape);
      TIMER_STOP(ShellTimer);
      break;
      
    case TopAbs_SOLID :
      TIMER_START(SolidTimer);
      TrTool->MakeSolid(theShape);
      //TrTool.UpdateSolid(aHShape,theShape);
      TrTool->UpdateShape(aHShape,theShape);
      TIMER_STOP(SolidTimer);
      break;
      
    case TopAbs_COMPSOLID :
      TrTool->MakeCompSolid(theShape);
      //TrTool.UpdateCompSolid(aHShape,theShape);
      TrTool->UpdateShape(aHShape,theShape);
      break;
      
    case TopAbs_COMPOUND :
      TrTool->MakeCompound(theShape);
      //TrTool.UpdateCompound(aHShape,theShape);
      TrTool->UpdateShape(aHShape,theShape);
      break;
    default:
      break;
    }
    
    // bind and copy the sub-elements

    Standard_Boolean wasFree = theShape.Free();
    theShape.Free(Standard_True);
    aMap.Bind(aHShape->TShape(), theShape.TShape());
    // count the number of <sub-shape> of the Shape's TShape
    // Is there any sub-shape 
    if (!aHShape->TShape()->Shapes().IsNull()) {
#ifndef DEB // Linux porting - to avoid warnings
      for (Standard_Integer i = 1 ; i<= aHShape->TShape()->Shapes()->Length(); i++) {
#else
      Standard_Integer nbElem = aHShape->TShape()->Shapes()->Length();
      for (Standard_Integer i = 1 ; i<=nbElem ; i++) {
#endif
	TopoDS_Shape subShape;
	MgtTopoDS::Translate(aHShape->TShape()->Shapes(i),
			     TrTool, aMap, subShape);
	TrTool->Add(theShape,subShape);
      }
    }
    theShape.Free(wasFree);
  }

  theShape.Orientation(aHShape->Orientation());
  theShape.Location(MgtTopLoc::Translate(aHShape->Location(), aMap));
}


//=======================================================================
//function : Translate1
//purpose  : Translation Transient Shape1 -> Persistent Shape
//=======================================================================

void MgtTopoDS::Translate1
(const TopoDS_Shape& aShape,
 const Handle(MgtTopoDS_TranslateTool1)& TrTool,
 PTColStd_TransientPersistentMap &aMap,
 PTopoDS_Shape1& aPShape)
{
  if (aShape.IsNull()) return;

  if (aMap.IsBound(aShape.TShape())) {
    // get the translated TShape
    Handle(PTopoDS_TShape1) pTS = 
      *((Handle(PTopoDS_TShape1)*) &aMap.Find(aShape.TShape()));
    aPShape.TShape(pTS);
  }
  else {

    // create if not translated and update
    
    switch (aShape.ShapeType()) {
      
    case TopAbs_VERTEX :
      TrTool->MakeVertex(aPShape);
      TrTool->UpdateVertex(aShape,aPShape,aMap);
      break;
      
    case TopAbs_EDGE :
      TIMER_START(StoreEdgeTimer);
      TrTool->MakeEdge(aPShape);
      TrTool->UpdateEdge(aShape,aPShape,aMap);
      TIMER_STOP(StoreEdgeTimer);
      break;
      
    case TopAbs_WIRE :
      TrTool->MakeWire(aPShape);
      TrTool->UpdateShape(aShape,aPShape);
      break;
      
    case TopAbs_FACE :
      TrTool->MakeFace(aPShape);
      TrTool->UpdateFace(aShape,aPShape,aMap);
      break;
    
    case TopAbs_SHELL :
      TrTool->MakeShell(aPShape);
      TrTool->UpdateShape(aShape,aPShape);
      break;
    
    case TopAbs_SOLID :
      TrTool->MakeSolid(aPShape);
      TrTool->UpdateShape(aShape,aPShape);
      break;
      
    case TopAbs_COMPSOLID :
      TrTool->MakeCompSolid(aPShape);
      TrTool->UpdateShape(aShape,aPShape);
      break;
      
  case TopAbs_COMPOUND :
      TrTool->MakeCompound(aPShape);
      TrTool->UpdateShape(aShape,aPShape);
      break;
    default:
      break;
    }
  
    // bind and copy the sub-elements
    aMap.Bind(aShape.TShape(),aPShape.TShape());
    // copy current Shape
    TopoDS_Shape S = aShape;
    S.Orientation(TopAbs_FORWARD);
    S.Location(TopLoc_Location());
    // count the number of <sub-shape> of the Shape's TShape
    Standard_Integer nbElem = 0;
    TopoDS_Iterator itr(S);
    for (;itr.More();++nbElem,itr.Next());

    if (nbElem) {
      Handle(PTopoDS_HArray1OfShape1) myArray = 
	new PTopoDS_HArray1OfShape1(1,nbElem);
      // translate <sub-shapes>
      
      PTopoDS_Shape1 pSh;
      nbElem = 1;
      for (itr.Initialize(S);itr.More();++nbElem,itr.Next()) {
	MgtTopoDS::Translate1(itr.Value(),TrTool,aMap,pSh);
	myArray->SetValue(nbElem,pSh);
	pSh.Nullify();
      }
      aPShape.TShape()->Shapes(myArray);
    }
    
  }
  
  aPShape.Orientation(aShape.Orientation());
  aPShape.Location(MgtTopLoc::Translate(aShape.Location(), aMap));
}


//=======================================================================
//function : Translate1
//purpose  : Translation Persistent Shape -> Transient Shape
//=======================================================================

void MgtTopoDS::Translate1
(const PTopoDS_Shape1& aPShape, 
 const Handle(MgtTopoDS_TranslateTool1)& TrTool,
 PTColStd_PersistentTransientMap& aMap,
 TopoDS_Shape& aShape)
{
  if (aPShape.TShape().IsNull()) return;
  
  // Translate the top-level shape
  if (aMap.IsBound(aPShape.TShape())) {
    // get the translated TShape
    Handle(TopoDS_TShape) TS = 
      *((Handle(TopoDS_TShape)*) &aMap.Find(aPShape.TShape()));
    aShape.TShape(TS);
  }
  else {

    // create if not translated and update
    
    switch (aPShape.TShape()->ShapeType()) {
      
    case TopAbs_VERTEX :
      TIMER_START(VertexTimer);
      TrTool->MakeVertex(aShape);
      TrTool->UpdateVertex(aPShape,aShape, aMap);
      TIMER_STOP(VertexTimer);
      break;
      
    case TopAbs_EDGE :
      TIMER_START(EdgeTimer);
      TrTool->MakeEdge(aShape);
      TrTool->UpdateEdge(aPShape,aShape, aMap);
      TIMER_STOP(EdgeTimer);
      break;
      
    case TopAbs_WIRE :
      TIMER_START(WireTimer);
      TrTool->MakeWire(aShape);
      //TrTool.UpdateWire(aPShape,aShape);
      TrTool->UpdateShape(aPShape,aShape);
      TIMER_STOP(WireTimer);
      break;
      
    case TopAbs_FACE :
      TIMER_START(FaceTimer);
      TrTool->MakeFace(aShape);
      TrTool->UpdateFace(aPShape,aShape, aMap);
      TIMER_STOP(FaceTimer);
      break;
      
    case TopAbs_SHELL :
      TIMER_START(ShellTimer);
      TrTool->MakeShell(aShape);
      //TrTool.UpdateShell(aPShape,aShape);
      TrTool->UpdateShape(aPShape,aShape);
      TIMER_STOP(ShellTimer);
      break;
      
    case TopAbs_SOLID :
      TIMER_START(SolidTimer);
      TrTool->MakeSolid(aShape);
      //TrTool.UpdateSolid(aPShape,aShape);
      TrTool->UpdateShape(aPShape,aShape);
      TIMER_STOP(SolidTimer);
      break;
      
    case TopAbs_COMPSOLID :
      TrTool->MakeCompSolid(aShape);
      //TrTool.UpdateCompSolid(aPShape,aShape);
      TrTool->UpdateShape(aPShape,aShape);
      break;
      
    case TopAbs_COMPOUND :
      TrTool->MakeCompound(aShape);
      //TrTool.UpdateCompound(aPShape,aShape);
      TrTool->UpdateShape(aPShape,aShape);
      break;
    default:
      break;
    }
    
    // bind and copy the sub-elements

    Standard_Boolean wasFree = aShape.Free();
    aShape.Free(Standard_True);
    aMap.Bind(aPShape.TShape(), aShape.TShape());
    // count the number of <sub-shape> of the Shape's TShape
    // Is there any sub-shape 
    if (!aPShape.TShape()->Shapes().IsNull()) {
      Standard_Integer nbElem = aPShape.TShape()->Shapes()->Length();
      for (Standard_Integer i = 1 ; i<=nbElem ; i++) {
	TopoDS_Shape subShape;
	MgtTopoDS::Translate1(aPShape.TShape()->Shapes(i),
			      TrTool, aMap,
			      subShape);
	TrTool->Add(aShape,subShape);
      }
    }
    aShape.Free(wasFree);
  }

  aShape.Orientation(aPShape.Orientation());
  aShape.Location(MgtTopLoc::Translate(aPShape.Location(), aMap));
}
