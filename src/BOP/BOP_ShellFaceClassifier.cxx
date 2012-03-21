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

#include <BOP_ShellFaceClassifier.ixx>

#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>

// ==================================================================
// function: BOP_ShellFaceClassifier::BOP_ShellFaceClassifier
// purpose:
// ==================================================================
BOP_ShellFaceClassifier::BOP_ShellFaceClassifier
    (const BOP_BlockBuilder& theBlockBuilder)
:
  BOP_CompositeClassifier(theBlockBuilder)
{
}

// ===============================================================================================
// function: Clear
// purpose: 
// ===============================================================================================
  void BOP_ShellFaceClassifier::Clear() 
{
  mySolidClassifier.Clear();
  myFaceShellMap.Clear();
}

// ===============================================================================================
// function: CompareShapes
// purpose: 
// ===============================================================================================
  TopAbs_State BOP_ShellFaceClassifier::CompareShapes(const TopoDS_Shape& theShape1,
						      const TopoDS_Shape& theShape2) 
{
  ResetShape(theShape1);
  myShell = TopoDS::Shell(theShape2);
  mySolidClassifier.LoadShell(myShell);
  TopAbs_State aState = State();
  return aState;
}

// ===============================================================================================
// function: CompareElementToShape
// purpose: 
// ===============================================================================================
  TopAbs_State BOP_ShellFaceClassifier::CompareElementToShape(const TopoDS_Shape& theElement,
							      const TopoDS_Shape& theShape) 
{
  ResetElement(theElement);
  myShell = TopoDS::Shell(theShape);
  mySolidClassifier.LoadShell(myShell);
  TopAbs_State aState = State();
  return aState;
}

// ===============================================================================================
// function: ResetShape
// purpose: 
// ===============================================================================================
  void BOP_ShellFaceClassifier::ResetShape(const TopoDS_Shape& theShape) 
{
  TopExp_Explorer anExp(theShape, TopAbs_FACE);
  const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());
  ResetElement(aFace);
}

// ===============================================================================================
// function: ResetElement
// purpose: 
// ===============================================================================================
  void BOP_ShellFaceClassifier::ResetElement(const TopoDS_Shape& theElement) 
{
  const TopAbs_ShapeEnum aShapeType= theElement.ShapeType();

  // initialize myPoint with first vertex of face <E>
  myFirstCompare = Standard_True;
  TopExp_Explorer anExp(theElement, TopAbs_VERTEX);

  if(anExp.More()) {
    const TopoDS_Vertex& aVertex = TopoDS::Vertex(anExp.Current());
    myPoint = BRep_Tool::Pnt(aVertex);
  }
  else {

    if(aShapeType == TopAbs_FACE) {
      BRepAdaptor_Surface BAS(TopoDS::Face(theElement));
      myPoint = BAS.Value((BAS.FirstUParameter()+BAS.LastUParameter()) * 0.5,
			  (BAS.FirstVParameter()+BAS.LastVParameter()) * 0.5);
    }
    else {
      myPoint.SetCoord(0., 0., 0.);
    }
  }
}

// ===============================================================================================
// function: CompareElement
// purpose: 
// ===============================================================================================
  void BOP_ShellFaceClassifier::CompareElement(const TopoDS_Shape& theElement) 
{
  
  if(myFirstCompare) {
    Standard_Boolean found = myFaceShellMap.IsBound(theElement);
    
    if(!found) {
      myBuilder.MakeShell(myShell);
      myBuilder.Add(myShell, theElement);
      myFaceShellMap.Bind(theElement, myShell);
    }
    else {
      TopoDS_Shape sbid = myFaceShellMap.Find(theElement);
      myShell = TopoDS::Shell(sbid);
    }
    myFirstCompare = Standard_False;
  }
  else {
    myBuilder.Add(myShell, theElement);
  }
}

// ==================================================================
// function: State
// purpose: 
// ==================================================================
  TopAbs_State BOP_ShellFaceClassifier::State() 
{
  TopAbs_State aState = TopAbs_UNKNOWN;
  Standard_Real aTolerance = Precision::Confusion();
  mySolidClassifier.Classify(myShell, myPoint, aTolerance);
  aState = mySolidClassifier.State();
  return aState;
}

