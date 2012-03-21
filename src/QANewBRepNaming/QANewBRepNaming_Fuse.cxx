// Created on: 2000-10-31
// Created by: Sergey ZARITCHNY
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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

#include <QANewBRepNaming_Fuse.ixx>
#include <TNaming_Builder.hxx>
#include <QANewBRepNaming_Loader.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopoDS_Iterator.hxx>
#include <TNaming_Tool.hxx>

//=======================================================================
//function : QANewBRepNaming_Fuse
//purpose  : 
//=======================================================================

QANewBRepNaming_Fuse::QANewBRepNaming_Fuse() {}

//=======================================================================
//function : QANewBRepNaming_Fuse
//purpose  : 
//=======================================================================

QANewBRepNaming_Fuse::QANewBRepNaming_Fuse(const TDF_Label& ResultLabel)
     :QANewBRepNaming_BooleanOperationFeat(ResultLabel) {}


//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void QANewBRepNaming_Fuse::Load(BRepAlgoAPI_BooleanOperation& MS) const {
  const TopoDS_Shape& ResSh = MS.Shape();
  const TopoDS_Shape& ObjSh = MS.Shape1();
  const TopoDS_Shape& ToolSh = MS.Shape2();

  if (ResSh.IsNull()) {
#ifdef MDTV_DEB
    cout<<"QANewBRepNaming_Fuse::Load(): The result of the boolean operation is null"<<endl;
#endif
    return;
  }

  // Naming of the result:
  LoadResult(MS);

  // Naming of modified faces: 
  TNaming_Builder ModBuilder(ModifiedFaces());    
  QANewBRepNaming_Loader::LoadModifiedShapes (MS, ObjSh,  TopAbs_FACE, ModBuilder, Standard_True);  
  QANewBRepNaming_Loader::LoadModifiedShapes (MS, ToolSh, TopAbs_FACE, ModBuilder, Standard_True);    

  // Naming of deleted faces:
  if(MS.HasDeleted()){
    TNaming_Builder DelBuilder(DeletedFaces());
    QANewBRepNaming_Loader::LoadDeletedShapes  (MS, ObjSh,  TopAbs_FACE, DelBuilder);
    QANewBRepNaming_Loader::LoadDeletedShapes  (MS, ToolSh, TopAbs_FACE, DelBuilder);
  }

  // Naming of the content of the result:
  LoadContent(MS);
}
