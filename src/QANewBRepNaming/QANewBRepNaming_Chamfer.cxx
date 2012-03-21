// Created on: 1997-09-22
// Created by: VAUTHIER Jean-Claude
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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


#include <QANewBRepNaming_Chamfer.ixx>
#include <QANewBRepNaming_Loader.hxx>

#include <TNaming_Builder.hxx>

#include <Standard_NullObject.hxx>

#include <TDF_Label.hxx>

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
