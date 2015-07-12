// Created on: 1993-06-17
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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


#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_BlockBuilder.hxx>
#include <TopOpeBRepBuild_ShellFaceClassifier.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepBuild_GettraceCHK();
#endif

//=======================================================================
//function : TopOpeBRepBuild_ShellFaceClassifier
//purpose  : 
//=======================================================================

TopOpeBRepBuild_ShellFaceClassifier::TopOpeBRepBuild_ShellFaceClassifier
(const TopOpeBRepBuild_BlockBuilder& BB) :
TopOpeBRepBuild_CompositeClassifier(BB)
{
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_ShellFaceClassifier::Clear()
{
  mySolidClassifier.Clear();
  myFaceShellMap.Clear();
}

//=======================================================================
//function : CompareShapes
//purpose  : 
//=======================================================================

TopAbs_State  TopOpeBRepBuild_ShellFaceClassifier::CompareShapes
(const TopoDS_Shape& B1, const TopoDS_Shape& B2)
{
#ifdef OCCT_DEBUG
//  const TopAbs_ShapeEnum t1 = B1.ShapeType();
//  const TopAbs_ShapeEnum t2 = B2.ShapeType();
#endif

  ResetShape(B1);
  myShell = TopoDS::Shell(B2); 
  mySolidClassifier.LoadShell(myShell);
//  myShell.Free(Standard_True);
  TopAbs_State state = State();
  return state;
}


//=======================================================================
//function : CompareElementToShape
//purpose  : 
//=======================================================================

TopAbs_State  TopOpeBRepBuild_ShellFaceClassifier::CompareElementToShape
(const TopoDS_Shape& F, const TopoDS_Shape& SHE)
{
#ifdef OCCT_DEBUG
//  const TopAbs_ShapeEnum t1 = F.ShapeType();
//  const TopAbs_ShapeEnum t2 = SHE.ShapeType();
#endif

  ResetElement(F);
  myShell = TopoDS::Shell(SHE);
  mySolidClassifier.LoadShell(myShell);
//  myShell.Free(Standard_True);  
  TopAbs_State state = State();
  return state;
}


//=======================================================================
//function : ResetShape
//purpose  : 
//=======================================================================

void  TopOpeBRepBuild_ShellFaceClassifier::ResetShape(const TopoDS_Shape& SHE)
{
#ifdef OCCT_DEBUG
//  const TopAbs_ShapeEnum t1 = SHE.ShapeType();
#endif

  TopExp_Explorer ex(SHE,TopAbs_FACE);
  const TopoDS_Face& F = TopoDS::Face(ex.Current());
  ResetElement(F);
}


//=======================================================================
//function : ResetElement
//purpose  : 
//=======================================================================

void  TopOpeBRepBuild_ShellFaceClassifier::ResetElement(const TopoDS_Shape& F)
{
  const TopAbs_ShapeEnum t = F.ShapeType();


  // initialize myPoint3d with first vertex of face <E>
  myFirstCompare = Standard_True;

  TopExp_Explorer ex(F,TopAbs_VERTEX);
  if ( ex.More() ) {
    const TopoDS_Vertex& V = TopoDS::Vertex(ex.Current());
    myPoint3d = BRep_Tool::Pnt(V);
  }
  else {
    if(t == TopAbs_FACE) {
      BRepAdaptor_Surface BAS(TopoDS::Face(F));
      myPoint3d = BAS.Value(.5*(BAS.FirstUParameter()+BAS.LastUParameter()),
			    .5*(BAS.FirstVParameter()+BAS.LastVParameter()));
    }
    else {
      myPoint3d.SetCoord(0.,0.,0.);
    }
  }

}

//=======================================================================
//function : CompareElement
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_ShellFaceClassifier::CompareElement(const TopoDS_Shape& F)
{
#ifdef OCCT_DEBUG
//  const TopAbs_ShapeEnum t = F.ShapeType();
#endif
  Standard_Boolean bRet = Standard_True;
  //
  if (myFirstCompare) {
    Standard_Boolean found = myFaceShellMap.IsBound(F);
    if ( !found ) {
      // la face F est la premiere d'un bloc de faces 
      // la face F d'un bloc de faces (F est supposee n'appartenir qu'a 
      // un seul shell) n'a pas encore ete rencontree <=> le bloc de faces
      // dont F fait partie n'a pas encore ete shellise
      // on cree un shell et on l'attache a F
      myBuilder.MakeShell(myShell);
      myBuilder.Add(myShell,F);
      myFaceShellMap.Bind(F,myShell);
    }
    else {
      // la face F est la premiere d'un bloc de faces 
      // on recupere le shell correspondant au bloc de faces (shellise)
      // dont F fait partie.
      TopoDS_Shape sbid = myFaceShellMap.Find(F);
      myShell = TopoDS::Shell(sbid);
      bRet = !bRet;
    }
    myFirstCompare = Standard_False;
  }
  else {
    // F n'est pas la premiere face d'un bloc.
    // myShell est necessairement defini et represente le bloc de faces
    // dont F fait partie
    myBuilder.Add(myShell,F);
  }
  return bRet;
}

#ifdef OCCT_DEBUG
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <TCollection_AsciiString.hxx>
static Standard_Integer STATIC_ishell = 0;
#endif

//=======================================================================
//function : State
//purpose  : 
//=======================================================================

TopAbs_State TopOpeBRepBuild_ShellFaceClassifier::State()
{
  TopAbs_State state;
  Standard_Real tol3d = Precision::Confusion();

#ifdef OCCT_DEBUG
  if (TopOpeBRepBuild_GettraceCHK()) {
    STATIC_ishell++;
    TCollection_AsciiString home("/home/wb/mdl/gti/prod/TTOPOPE/src/test/data/");
    TCollection_AsciiString sname("shell_"); sname = home + sname + STATIC_ishell;
    TCollection_AsciiString vname("vertex_"); vname = home + vname + STATIC_ishell;
    BRep_Builder B;TopoDS_Vertex V;B.MakeVertex(V,myPoint3d,tol3d);
    cout<<"TopOpeBRepBuild_ShellFaceClassifier : write shell "<<sname;
    cout<<" vertex "<<vname<<endl;
    BRepTools::Write(myShell,sname.ToCString());
    BRepTools::Write(V,vname.ToCString());
  }
#endif
  mySolidClassifier.Classify(myShell,myPoint3d,tol3d);
  state = mySolidClassifier.State();
  return state;
}
