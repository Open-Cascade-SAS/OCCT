// Created on: 2000-10-31
// Created by: Sergey ZARITCHNY
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


#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <QANewBRepNaming_Fuse.hxx>
#include <QANewBRepNaming_Loader.hxx>
#include <TDF_Label.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

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
#ifdef OCCT_DEBUG
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
