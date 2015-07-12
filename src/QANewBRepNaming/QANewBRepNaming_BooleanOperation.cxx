// Created on: 1999-11-11
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


#include <BRepAlgo_BooleanOperation.hxx>
#include <QANewBRepNaming_BooleanOperation.hxx>
#include <QANewBRepNaming_Loader.hxx>
#include <Standard_NullObject.hxx>
#include <TDF_Label.hxx>
#include <TNaming_Builder.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

//=======================================================================
//function : QANewBRepNaming_BooleanOperation
//purpose  : 
//=======================================================================
QANewBRepNaming_BooleanOperation::QANewBRepNaming_BooleanOperation()
{}

//=======================================================================
//function : QANewBRepNaming_BooleanOperation
//purpose  : 
//=======================================================================

QANewBRepNaming_BooleanOperation::QANewBRepNaming_BooleanOperation(const TDF_Label& Label)
     : QANewBRepNaming_TopNaming(Label)
{}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void QANewBRepNaming_BooleanOperation::Init(const TDF_Label& Label)
{
  if(Label.IsNull())
    Standard_NullObject::Raise("QANewBRepNaming_BooleanOperation::Init The Result label is Null ..."); 
  myResultLabel = Label;
}

//=======================================================================
//function : FirstModified
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_BooleanOperation::FirstModified() const
{
  return ResultLabel().FindChild(1, Standard_True); 
}

//=======================================================================
//function : FirstDeleted
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_BooleanOperation::FirstDeleted() const
{
  return ResultLabel().FindChild(2, Standard_True);
}

//=======================================================================
//function : SecondModified
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_BooleanOperation::SecondModified() const
{
  return ResultLabel().FindChild(3, Standard_True); 
}

//=======================================================================
//function : SecondDeleted
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_BooleanOperation::SecondDeleted() const
{
  return ResultLabel().FindChild(4, Standard_True);
}

//=======================================================================
//function : Intersections
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_BooleanOperation::Intersections() const
{
  return ResultLabel().FindChild(5, Standard_True);
}

//=======================================================================
//function : Load (BooleanOperation)
//purpose  : for a new part
//=======================================================================
//modified by NIZNHY-PKV Wed Jun 19 09:07:51 2002 f
void  QANewBRepNaming_BooleanOperation::Load (BRepAlgo_BooleanOperation& MS) const
//void  QANewBRepNaming_BooleanOperation::Load (BRepAlgoAPI_BooleanOperation& MS) const
//modified by NIZNHY-PKV Wed Jun 19 09:07:55 2002 t
{
  const TopoDS_Shape& S1 = MS.Shape1();
  const TopoDS_Shape& S2 = MS.Shape2();

  TNaming_Builder Builder (ResultLabel());
  Builder.Modify (S1, MS.Shape());
  Builder.Modify (S2, MS.Shape());
  
  TopAbs_ShapeEnum SST1 = TopAbs_FACE; 
  TopAbs_ShapeEnum SST2 = TopAbs_FACE; 
  
  if (S1.ShapeType() == TopAbs_FACE || S1.ShapeType() == TopAbs_WIRE )  
    SST1 = TopAbs_EDGE;
  
  if (S2.ShapeType() == TopAbs_FACE || S2.ShapeType() == TopAbs_WIRE )  
    SST2 = TopAbs_EDGE;
  
  TNaming_Builder Builder1(FirstModified());
  QANewBRepNaming_Loader::LoadModifiedShapes(MS, S1, SST1, Builder1);
  TNaming_Builder Builder2(FirstDeleted());
  QANewBRepNaming_Loader::LoadDeletedShapes(MS, S1, SST1, Builder2);
  
  TNaming_Builder Builder3(SecondModified());
  QANewBRepNaming_Loader::LoadModifiedShapes(MS, S2, SST2, Builder3);
  TNaming_Builder Builder4(SecondDeleted());
  QANewBRepNaming_Loader::LoadDeletedShapes(MS, S2, SST2, Builder4);
  
  // load the intersection edges
  TNaming_Builder Builder5(Intersections());
  Handle(TopOpeBRepBuild_HBuilder) build = MS.Builder();  
  TopTools_ListIteratorOfListOfShape its = build->Section();
  for (; its.More(); its.Next()) {
    Builder5.Select(its.Value(),its.Value());
  }  
}
