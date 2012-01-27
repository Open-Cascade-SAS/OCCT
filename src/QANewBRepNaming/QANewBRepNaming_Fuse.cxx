// File:	QANewBRepNaming_Fuse.cdl
// Created:	Tue Oct 31 14:38:18 2000
// Author:	Sergey ZARITCHNY
//		<szy@opencascade.com>
// Copyright:	Open CASCADE 2003

// Lastly modified by :
// +---------------------------------------------------------------------------+
// !       szy ! Adopted                                 ! 9-06-2003! 3.0-00-%L%!
// +---------------------------------------------------------------------------+

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

// @@SDM: begin

// File history synopsis (creation,modification,correction)
// +---------------------------------------------------------------------------+
// ! Developer !              Comments                   !   Date   ! Version  !
// +-----------!-----------------------------------------!----------!----------+
// !       vro ! Creation                                !31-10-2000!3.0-00-3!
// !       vro ! Redesign                                !13-12-2000! 3.0-00-3!
// !       vro ! Result may be null                      !19-03-2001! 3.0-00-3!
// !       szy ! Modified Load                           ! 8-05-2003! 3.0-00-%L%!
// !       szy ! Redesigned Load                         !21-05-2003! 3.0-00-%L%!
// !       szy ! Adopted                                 ! 9-06-2003! 3.0-00-%L%!
// +---------------------------------------------------------------------------+
// Lastly modified by : szy                                    Date :  9-06-2003

// @@SDM: end
