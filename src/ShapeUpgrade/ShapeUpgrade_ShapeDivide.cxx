// Created on: 1999-04-27
// Created by: Pavel DURANDIN
// Copyright (c) 1999-1999 Matra Datavision
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

//    abv 16.06.99 returning ReShape context; processing shared subshapes in compounds
//    sln 29.11.01 Bug21: in method Perform(..) nullify location of compound's faces only if mode myConsiderLocation is on

#include <ShapeUpgrade_ShapeDivide.ixx>
#include <Precision.hxx>
#include <ShapeExtend.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRep_Builder.hxx>
#include <ShapeUpgrade_WireDivide.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>

//=======================================================================
//function : ShapeUpgrade_ShapeDivide
//purpose  : 
//=======================================================================

ShapeUpgrade_ShapeDivide::ShapeUpgrade_ShapeDivide() : myStatus(0)
{
  myPrecision = myMinTol = Precision::Confusion();
  myMaxTol = 1; //Precision::Infinite() ?? pdn
  mySplitFaceTool = new ShapeUpgrade_FaceDivide;
  myContext = new ShapeBuild_ReShape;
  mySegmentMode = Standard_True;
  myEdgeMode = 2;
}

//=======================================================================
//function : ShapeUpgrade_ShapeDivide
//purpose  : 
//=======================================================================

ShapeUpgrade_ShapeDivide::ShapeUpgrade_ShapeDivide(const TopoDS_Shape &S) : myStatus(0)
{
  myPrecision = myMinTol = Precision::Confusion();
  myMaxTol = 1; //Precision::Infinite() ?? pdn
  mySplitFaceTool = new ShapeUpgrade_FaceDivide;
  myContext = new ShapeBuild_ReShape;
  mySegmentMode = Standard_True;
  myEdgeMode = 2;
  Init(S);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void ShapeUpgrade_ShapeDivide::Init(const TopoDS_Shape &S)
{
  myShape = S;
}

//=======================================================================
//function : Delete
//purpose  : 
//=======================================================================

void ShapeUpgrade_ShapeDivide::Delete()
{}


//=======================================================================
//function : SetPrecision
//purpose  : 
//=======================================================================

void ShapeUpgrade_ShapeDivide::SetPrecision (const Standard_Real Prec)
{
  myPrecision = Prec;
}

//=======================================================================
//function : SetMaxTolerance
//purpose  : 
//=======================================================================

void ShapeUpgrade_ShapeDivide::SetMaxTolerance (const Standard_Real maxtol)
{
  myMaxTol = maxtol;
}


//=======================================================================
//function : SetMinTolerance
//purpose  : 
//=======================================================================

void ShapeUpgrade_ShapeDivide::SetMinTolerance (const Standard_Real mintol)
{
  myMinTol = mintol;
}


//=======================================================================
//function : SetSurfaceSegmentMode
//purpose  : 
//=======================================================================

void ShapeUpgrade_ShapeDivide::SetSurfaceSegmentMode(const Standard_Boolean Segment)
{
  mySegmentMode = Segment;
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

Standard_Boolean ShapeUpgrade_ShapeDivide::Perform(const Standard_Boolean newContext)
{
  myStatus = ShapeExtend::EncodeStatus ( ShapeExtend_OK );
  if(myShape.IsNull()) {
    myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_FAIL1 );
    return Standard_False;
  }
  
  if ( newContext || myContext.IsNull() ) 
    myContext = new ShapeBuild_ReShape;

  // Process COMPOUNDs separately in order to handle sharing in assemblies
  // NOTE: not optimized: subshape can be processed twice (second time - no modif)
  if ( myShape.ShapeType() == TopAbs_COMPOUND ) {
    Standard_Integer locStatus = myStatus;
    TopoDS_Compound C;
    BRep_Builder B;
    B.MakeCompound ( C );
    TopoDS_Shape savShape = myShape;
    for ( TopoDS_Iterator it(savShape,Standard_False); it.More(); it.Next() ) {
      TopoDS_Shape shape = it.Value();
      TopLoc_Location L = shape.Location();
      if(myContext->ModeConsiderLocation()) {
        TopLoc_Location nullLoc;
        shape.Location ( nullLoc );
      }
      myShape = myContext->Apply ( shape );
      Perform ( Standard_False );
     if(myContext->ModeConsiderLocation()) 
        myResult.Location ( L );
      myResult.Orientation(TopAbs::Compose(myResult.Orientation(),savShape.Orientation()));
      B.Add ( C, myResult );
      locStatus |= myStatus;
    }
    
    myShape = savShape;
    myStatus = locStatus;
    if ( Status ( ShapeExtend_DONE ) ) {
      myResult = myContext->Apply ( C, TopAbs_SHAPE );
      myContext->Replace ( myShape, myResult );
      return Standard_True;
    }
    myResult = myShape;
    return Standard_False;
  }

  // Process FACEs
  Handle(ShapeUpgrade_FaceDivide) SplitFace = GetSplitFaceTool();
  if ( ! SplitFace.IsNull() ) {
    SplitFace->SetPrecision( myPrecision );
    SplitFace->SetMaxTolerance(myMaxTol);
    SplitFace->SetSurfaceSegmentMode(mySegmentMode);
    Handle(ShapeUpgrade_WireDivide) SplitWire = SplitFace->GetWireDivideTool();
    if ( ! SplitWire.IsNull() ) {
      SplitWire->SetMinTolerance(myMinTol);
      SplitWire->SetEdgeMode(myEdgeMode);
    }
    for(TopExp_Explorer exp(myShape,TopAbs_FACE); exp.More(); exp.Next()) {
//smh#8
      TopoDS_Shape tmpF = exp.Current().Oriented ( TopAbs_FORWARD );
      TopoDS_Face F = TopoDS::Face (tmpF); // protection against INTERNAL shapes: cts20105a.rle
      TopoDS_Shape sh = myContext->Apply ( F, TopAbs_SHAPE );
      for (TopExp_Explorer exp2(sh,TopAbs_FACE); exp2.More(); exp2.Next()) {
//szv: try-catch added
	try {
	  OCC_CATCH_SIGNALS
	  for (; exp2.More(); exp2.Next()) {
	    TopoDS_Face face = TopoDS::Face ( exp2.Current() );
	    SplitFace->Init(face);
	    SplitFace->SetContext(myContext);
	    SplitFace->Perform();
	    if(SplitFace->Status(ShapeExtend_FAIL)) {
	      myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_FAIL2 );
	    }
	    if(SplitFace->Status(ShapeExtend_DONE)) {
	      myContext->Replace(face,SplitFace->Result());
	      if(SplitFace->Status(ShapeExtend_DONE1))
		myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE1 );
	      if(SplitFace->Status(ShapeExtend_DONE2))
		myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE2 );
	    }
	  }
	}
	catch (Standard_Failure) {
	  myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_FAIL2 );
#ifdef DEB
	  cout << "\nError: Exception in ShapeUpgrade_FaceDivide::Perform(): ";
	  Standard_Failure::Caught()->Print(cout); cout << endl;
#endif
	}
      }
    }
  }
  
  // Process free WIREs
  Handle(ShapeUpgrade_WireDivide) SplitWire = SplitFace->GetWireDivideTool();
  if ( ! SplitWire.IsNull() ) {
    SplitWire->SetFace (TopoDS_Face());
    SplitWire->SetPrecision( myPrecision );
    SplitWire->SetMaxTolerance(myMaxTol);
    SplitWire->SetMinTolerance(myMinTol);
    SplitWire->SetEdgeMode(myEdgeMode);
    TopExp_Explorer exp;//svv Jan 10 2000 : porting on DEC
    for (exp.Init (myShape, TopAbs_WIRE, TopAbs_FACE); exp.More(); exp.Next()) {
//smh#8
      TopoDS_Shape tmpW = exp.Current().Oriented ( TopAbs_FORWARD );
      TopoDS_Wire W = TopoDS::Wire (tmpW );   // protection against INTERNAL shapes
      TopoDS_Shape sh = myContext->Apply ( W, TopAbs_SHAPE );
      for (TopExp_Explorer exp2 (sh,TopAbs_WIRE); exp2.More(); exp2.Next()) {
	TopoDS_Wire wire = TopoDS::Wire ( exp2.Current() );
	SplitWire->Load (wire);
	SplitWire->SetContext(myContext);
	SplitWire->Perform();
	if(SplitWire->Status(ShapeExtend_FAIL)) {
	  myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_FAIL3 );
	}
	if(SplitWire->Status(ShapeExtend_DONE)) {
	  myContext->Replace(wire,SplitWire->Wire());
	  myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE1 );
	}
      }
    }
  
    // Process free EDGEs
    for (exp.Init (myShape, TopAbs_EDGE, TopAbs_WIRE); exp.More(); exp.Next()) {
//smh#8
      TopoDS_Shape tmpE = exp.Current().Oriented ( TopAbs_FORWARD );
      TopoDS_Edge E = TopoDS::Edge (tmpE );                                       // protection against INTERNAL shapes
      TopoDS_Vertex V1,V2;
      TopExp::Vertices(E,V2,V1);
      if( V1.IsNull() && V2.IsNull() ) continue; // skl 27.10.2004 for OCC5624
      TopoDS_Shape sh = myContext->Apply ( E, TopAbs_SHAPE );
      for (TopExp_Explorer exp2 (sh,TopAbs_EDGE); exp2.More(); exp2.Next()) {
	TopoDS_Edge edge = TopoDS::Edge ( exp2.Current() );
	SplitWire->Load (edge);
	SplitWire->SetContext(myContext);
	SplitWire->Perform();
	if(SplitWire->Status(ShapeExtend_FAIL)) {
	  myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_FAIL3 );
	}
	if(SplitWire->Status(ShapeExtend_DONE)) {
	  myContext->Replace(edge,SplitWire->Wire());
	  myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE1 );
	}
      }
    }
  }
  myResult = myContext->Apply ( myShape, TopAbs_SHAPE );
  return ! myResult.IsSame ( myShape );
}

//=======================================================================
//function : Result
//purpose  : 
//=======================================================================

TopoDS_Shape ShapeUpgrade_ShapeDivide::Result() const
{
  return myResult;
}

//=======================================================================
//function : GetContext
//purpose  : 
//=======================================================================

Handle(ShapeBuild_ReShape) ShapeUpgrade_ShapeDivide::GetContext() const
{
  //if ( myContext.IsNull() ) myContext = new ShapeBuild_ReShape;
  return myContext;
}

//=======================================================================
//function : SetContext
//purpose  : 
//=======================================================================

void ShapeUpgrade_ShapeDivide::SetContext (const Handle(ShapeBuild_ReShape) &context)
{
  myContext = context;
}

//=======================================================================
//function : SetSplitFaceTool
//purpose  : 
//=======================================================================

void ShapeUpgrade_ShapeDivide::SetSplitFaceTool(const Handle(ShapeUpgrade_FaceDivide)& splitFaceTool)
{
  mySplitFaceTool = splitFaceTool;
}

//=======================================================================
//function : GetSplitFaceTool
//purpose  : 
//=======================================================================

Handle(ShapeUpgrade_FaceDivide) ShapeUpgrade_ShapeDivide::GetSplitFaceTool () const
{
  return mySplitFaceTool;
}

//=======================================================================
//function : Status
//purpose  : 
//=======================================================================

Standard_Boolean ShapeUpgrade_ShapeDivide::Status (const ShapeExtend_Status status) const
{
  return ShapeExtend::DecodeStatus ( myStatus, status );
}

//=======================================================================
//function : SetEdgeMode
//purpose  : 
//=======================================================================

 void ShapeUpgrade_ShapeDivide::SetEdgeMode(const Standard_Integer aEdgeMode) 
{
  myEdgeMode = aEdgeMode;
}
