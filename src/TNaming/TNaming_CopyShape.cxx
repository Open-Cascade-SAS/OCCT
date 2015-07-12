// Created on: 2000-02-14
// Created by: Denis PASCAL
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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


#include <TNaming_CopyShape.hxx>
#include <TNaming_TranslateTool.hxx>
#include <TopAbs.hxx>
#include <TopLoc_Datum3D.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

//=======================================================================
//function : CopyTool
//purpose  : Tool to copy a set of shape(s), using the aMap
//=======================================================================
void TNaming_CopyShape::CopyTool( const TopoDS_Shape& aShape,
				 TColStd_IndexedDataMapOfTransientTransient&  aMap,
				 TopoDS_Shape&       aResult)
{

  Handle(TNaming_TranslateTool) TrTool = new TNaming_TranslateTool();
  TNaming_CopyShape::Translate (aShape, aMap, aResult, TrTool) ;
}

//=======================================================================
//function : Translate
//purpose  : TNaming
//=======================================================================

void TNaming_CopyShape::Translate( const TopoDS_Shape& aShape,		         
				  TColStd_IndexedDataMapOfTransientTransient&  aMap,
				  TopoDS_Shape& aResult,
				  const Handle(TNaming_TranslateTool)& TrTool)
{
  aResult.Nullify();

  if (aShape.IsNull()) return;

  if (aMap.Contains(aShape.TShape())) {
    // get the translated TShape
    Handle(TopoDS_TShape) TS =
       *((Handle(TopoDS_TShape)*) &aMap.FindFromKey(aShape.TShape()));
    aResult.TShape(TS);
  }
  else {

    // create if not translated and update
    
    switch (aShape.ShapeType()) {
      
    case TopAbs_VERTEX :
      TrTool->MakeVertex(aResult);
      TrTool->UpdateVertex(aShape,aResult,aMap);
      break;
      
    case TopAbs_EDGE :
      TrTool->MakeEdge(aResult);
      TrTool->UpdateEdge(aShape,aResult,aMap);
      break;
      
    case TopAbs_WIRE :
      TrTool->MakeWire(aResult);
      TrTool->UpdateShape(aShape,aResult);
      break;
      
    case TopAbs_FACE :
      TrTool->MakeFace(aResult);
      TrTool->UpdateFace(aShape,aResult,aMap);
      break;
    
    case TopAbs_SHELL :
      TrTool->MakeShell(aResult);
      TrTool->UpdateShape(aShape,aResult);
      break;
    
    case TopAbs_SOLID :
      TrTool->MakeSolid(aResult);
      TrTool->UpdateShape(aShape,aResult);
      break;
      
    case TopAbs_COMPSOLID :
      TrTool->MakeCompSolid(aResult);
      TrTool->UpdateShape(aShape,aResult);
      break;
      
  case TopAbs_COMPOUND :
      TrTool->MakeCompound(aResult);
      TrTool->UpdateShape(aShape,aResult);
      break;

    default:
      break;
    }
  
    // bind and copy the sub-elements
    aMap.Add(aShape.TShape(),aResult.TShape()); //TShapes
    TopoDS_Shape S = aShape;
    S.Orientation(TopAbs_FORWARD);
    S.Location(TopLoc_Location()); //Identity
    // copy current Shape
    TopoDS_Iterator itr(S, Standard_False);
    Standard_Boolean wasFree = aResult.Free();
    aResult.Free(Standard_True);
    // translate <sub-shapes>
    for (;itr.More();itr.Next()) {
      TopoDS_Shape subShape;
      TNaming_CopyShape::Translate(itr.Value(), aMap, subShape, TrTool);
      TrTool->Add(aResult, subShape);// add subshapes
    }

    aResult.Free(wasFree);
  }
  
  aResult.Orientation(aShape.Orientation());
  aResult.Location(TNaming_CopyShape::Translate(aShape.Location(), aMap));
  TrTool->UpdateShape(aShape,aResult);
// #ifdef OCCT_DEBUG
//     if(fShar) {
//       cout << "=== Shareable shape ===" << endl;
//       cout << "aShape Type = " <<(aShape.TShape())->DynamicType() << endl;
//       if(aShape.Orientation() == aResult.Orientation())
// 	cout<<"\tSource and result shapes have the same Orientation"<< endl;
//       if((aShape.Location().IsEqual(aResult.Location())))
// 	cout <<"\tSource and result shapes have the same Locations" << endl;
//       if((aShape.IsSame(aResult)))
// 	 cout <<"\tShapes arew the same (i.e. the same TShape and the same Locations)" << endl;
//     }	    
// #endif
}

//=======================================================================
// static TranslateDatum3D
//=======================================================================
static Handle(TopLoc_Datum3D) TranslateDatum3D(const Handle(TopLoc_Datum3D)& D,
					       TColStd_IndexedDataMapOfTransientTransient&  aMap)
{
  Handle(TopLoc_Datum3D) TD;
  if(aMap.Contains(D))
       TD = Handle(TopLoc_Datum3D)::DownCast(aMap.FindFromKey(D));
  else {
    TD = new TopLoc_Datum3D(D->Transformation());
    aMap.Add(D, TD);
  }
  return TD;
}
//=======================================================================
//function : Translates
//purpose  : Topological Location
//=======================================================================

TopLoc_Location TNaming_CopyShape::Translate(const TopLoc_Location& L,
					     TColStd_IndexedDataMapOfTransientTransient&  aMap)
{
  TopLoc_Location result;

 if (!L.IsIdentity()) {
   result = Translate(L.NextLocation(),aMap) * 
     TopLoc_Location(TranslateDatum3D(L.FirstDatum(),aMap)).Powered(L.FirstPower());

 }
  return result;
}


