// Created on: 1997-09-22
// Created by: VAUTHIER Jean-Claude
// Copyright (c) 1997-1999 Matra Datavision
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


#include <BRepFilletAPI_MakeChamfer.hxx>
#include <QANewBRepNaming_Chamfer.hxx>
#include <QANewBRepNaming_Loader.hxx>
#include <Standard_NullObject.hxx>
#include <TDF_Label.hxx>
#include <TNaming_Builder.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
//function : QANewBRepNaming_Chamfer
//purpose  : 
//=======================================================================
QANewBRepNaming_Chamfer::QANewBRepNaming_Chamfer()
{}

//=======================================================================
//function : QANewBRepNaming_Chamfer
//purpose  : 
//=======================================================================

QANewBRepNaming_Chamfer::QANewBRepNaming_Chamfer(const TDF_Label& ResultLabel):
       QANewBRepNaming_TopNaming(ResultLabel)
{}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void QANewBRepNaming_Chamfer::Init(const TDF_Label& ResultLabel)
{
  if(ResultLabel.IsNull())
    Standard_NullObject::Raise("QANewBRepNaming_Chamfer::Init The Result label is Null ..."); 
  myResultLabel = ResultLabel;
}   

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void  QANewBRepNaming_Chamfer::Load (const TopoDS_Shape&  part,
				BRepFilletAPI_MakeChamfer& mkChamfer) const
{
  TNaming_Builder Ins(ResultLabel());
  Ins.Modify (part, mkChamfer.Shape());

  //New Faces generated from  edges
  TNaming_Builder Builder1(FacesFromEdges());
  QANewBRepNaming_Loader::LoadGeneratedShapes (mkChamfer, part, TopAbs_EDGE, Builder1);

  //Faces of the initial shape modified by MakeChamfer
  TNaming_Builder Builder2(ModifiedFaces());
  QANewBRepNaming_Loader::LoadModifiedShapes (mkChamfer, part, TopAbs_FACE, Builder2, Standard_False);

  //Deleted Faces of the initial shape
  TNaming_Builder Builder3(DeletedFaces());
  QANewBRepNaming_Loader::LoadDeletedShapes(mkChamfer, part, TopAbs_FACE, Builder3);
}

//=======================================================================
//function : FacesFromEdges
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Chamfer::FacesFromEdges() const
{
  return ResultLabel().FindChild(1, Standard_True);
}

//=======================================================================
//function : ModifiedFaces
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Chamfer::ModifiedFaces() const
{
  return ResultLabel().FindChild(2, Standard_True);
}

//=======================================================================
//function : DeletedFaces
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Chamfer::DeletedFaces() const
{
  return ResultLabel().FindChild(3, Standard_True);
}
