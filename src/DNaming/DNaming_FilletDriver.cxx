// Created on: 2009-05-06
// Created by: Sergey ZARITCHNY
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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


#include <BRepAlgo.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <DNaming.hxx>
#include <DNaming_FilletDriver.hxx>
#include <ModelDefinitions.hxx>
#include <Precision.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Real.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDF_Label.hxx>
#include <TFunction_Function.hxx>
#include <TFunction_Logbook.hxx>
#include <TNaming.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DNaming_FilletDriver,TFunction_Driver)

//=======================================================================
//function : FilletDriver
//purpose  : Constructor
//=======================================================================
DNaming_FilletDriver::DNaming_FilletDriver()
{}

//=======================================================================
//function : Validate
//purpose  : Validates labels of a function in <log>.
//=======================================================================
void DNaming_FilletDriver::Validate(Handle(TFunction_Logbook)&) const
{}

//=======================================================================
//function : MustExecute
//purpose  : Analyse in <log> if the loaded function must be executed
//=======================================================================
Standard_Boolean DNaming_FilletDriver::MustExecute(const Handle(TFunction_Logbook)&) const
{
  return Standard_True;
}

//=======================================================================
//function : Execute
//purpose  : Execute the function and push in <log> the impacted labels
//=======================================================================
Standard_Integer DNaming_FilletDriver::Execute(Handle(TFunction_Logbook)& theLog) const
{
  Handle(TFunction_Function) aFunction;
  Label().FindAttribute(TFunction_Function::GetID(),aFunction);
  if(aFunction.IsNull()) return -1;

  Handle(TFunction_Function) aPrevFun = DNaming::GetPrevFunction(aFunction);
  if(aPrevFun.IsNull()) return -1;
  const TDF_Label& aLab = RESPOSITION(aPrevFun);
  Handle(TNaming_NamedShape) aContextNS;
  aLab.FindAttribute(TNaming_NamedShape::GetID(), aContextNS);
  if (aContextNS.IsNull() || aContextNS->IsEmpty()) {
#ifdef OCCT_DEBUG
    cout<<"FilletDriver:: Context is empty"<<endl;
#endif
    aFunction->SetFailure(WRONG_ARGUMENT);
    return -1;
  }
  
  const Standard_Real aRadius = DNaming::GetReal(aFunction,FILLET_RADIUS)->Get();  
  const ChFi3d_FilletShape aSurfaceType = 
    (ChFi3d_FilletShape) DNaming::GetInteger(aFunction,FILLET_SURFTYPE)->Get();

  if(aRadius < Precision::Confusion()) {
    aFunction->SetFailure(WRONG_ARGUMENT);
#ifdef OCCT_DEBUG
    cout << "FilletDriver:: Radius < Precision::Confusion" << endl;
#endif
    return -1;
  }


  Handle(TDataStd_UAttribute) aPathObj = DNaming::GetObjectArg(aFunction,FILLET_PATH);
  Handle(TNaming_NamedShape) aPathNS = DNaming::GetObjectValue(aPathObj);
  if (aPathNS.IsNull() || aPathNS->IsEmpty()) {
#ifdef OCCT_DEBUG
    cout<<"FilletDriver:: Path is empty"<<endl;
#endif
    aFunction->SetFailure(WRONG_ARGUMENT);
    return -1;
  }
  
  TopoDS_Shape aPATH = aPathNS->Get();
  TopoDS_Shape aCONTEXT = aContextNS->Get();
  if (aPATH.IsNull() || aCONTEXT.IsNull()) {
#ifdef OCCT_DEBUG
    cout<<"FilletDriver:: Path or Context is null"<<endl;
#endif
    aFunction->SetFailure(WRONG_ARGUMENT);
    return -1;
  }

  TopExp_Explorer expl;
  TopTools_MapOfShape View;
  
  BRepFilletAPI_MakeFillet aMkFillet(aCONTEXT, aSurfaceType);

  if(aPATH.ShapeType() != TopAbs_EDGE && aPATH.ShapeType() != TopAbs_FACE) {
    aFunction->SetFailure(WRONG_ARGUMENT);
    return -1;
  }
    
  if(aPATH.ShapeType() == TopAbs_FACE) {
    for (expl.Init(aPATH, TopAbs_EDGE); expl.More(); expl.Next()){ 
      const TopoDS_Edge& anEdge = TopoDS::Edge(expl.Current());
      if (!View.Add(anEdge)) continue;
      else 
	aMkFillet.Add(aRadius, anEdge); //Edge
    }
  }
  else {
    const TopoDS_Edge& anEdge = TopoDS::Edge(aPATH);
    aMkFillet.Add(aRadius, anEdge); //Edge
  }

  
  aMkFillet.Build();

  if (!aMkFillet.IsDone()) {
    aFunction->SetFailure(ALGO_FAILED);
    return -1;
  }
  TopTools_ListOfShape aLarg;
  aLarg.Append(aCONTEXT);
  if (!BRepAlgo::IsValid(aLarg, aMkFillet.Shape(),Standard_False,Standard_False)) {
    aFunction->SetFailure(RESULT_NOT_VALID);
    return -1;
  }

// Naming
  LoadNamingDS(RESPOSITION(aFunction), aMkFillet, aCONTEXT);

  theLog->SetValid(RESPOSITION(aFunction),Standard_True);  
  aFunction->SetFailure(DONE);
  return 0;
}

//=======================================================================
//function : LoadAndName
//purpose  : 
//=======================================================================
void DNaming_FilletDriver::LoadNamingDS (const TDF_Label& theResultLabel, 
					 BRepFilletAPI_MakeFillet& theMkFillet,
					 const TopoDS_Shape& theContext) const 
{
  TNaming_Builder aBuilder (theResultLabel);
  TopoDS_Shape aResult = theMkFillet.Shape();

  if (aResult.ShapeType() == TopAbs_COMPOUND) {
    Standard_Integer nbSubResults = 0;
    TopoDS_Iterator itr(aResult);
    for (; itr.More(); itr.Next()) nbSubResults++;
    if (nbSubResults == 1) {
      itr.Initialize(aResult);
      if (itr.More()) aResult = itr.Value();
    }
  }
  if (aResult.IsNull()) aBuilder.Generated(aResult);
  else 
    aBuilder.Modify(theContext, aResult);

  TopTools_DataMapOfShapeShape SubShapes;
  for (TopExp_Explorer Exp(aResult, TopAbs_FACE); Exp.More(); Exp.Next()) {
    SubShapes.Bind(Exp.Current(),Exp.Current());
  }

  //New faces generated from edges
  TNaming_Builder anEdgeBuilder(theResultLabel.FindChild(1,Standard_True));
  DNaming::LoadAndOrientGeneratedShapes(theMkFillet, theContext, TopAbs_EDGE, anEdgeBuilder, SubShapes);

  //Faces of the initial shape modified by theMkFillet
  TNaming_Builder aFacesBuilder(theResultLabel.FindChild(2,Standard_True));
  DNaming::LoadAndOrientModifiedShapes(theMkFillet, theContext, TopAbs_FACE, aFacesBuilder, SubShapes);


  //New faces generated from vertices (if exist)
  TNaming_Builder aVFacesBuilder(theResultLabel.FindChild(3,Standard_True));
  DNaming::LoadAndOrientGeneratedShapes(theMkFillet, theContext, TopAbs_VERTEX, aVFacesBuilder, SubShapes);  

  //Deleted faces of the initial shape
  TNaming_Builder aDelBuilder(theResultLabel.FindChild(4,Standard_True));
  DNaming::LoadDeletedShapes(theMkFillet, theContext, TopAbs_FACE, aDelBuilder);

}
