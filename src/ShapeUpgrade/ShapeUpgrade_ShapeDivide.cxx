// Created on: 1999-04-27
// Created by: Pavel DURANDIN
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

//    abv 16.06.99 returning ReShape context; processing shared subshapes in compounds
//    sln 29.11.01 Bug21: in method Perform(..) nullify location of compound's faces only if mode myConsiderLocation is on

#include <BRep_Builder.hxx>
#include <Message_Msg.hxx>
#include <Precision.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeExtend.hxx>
#include <ShapeExtend_BasicMsgRegistrator.hxx>
#include <ShapeUpgrade_FaceDivide.hxx>
#include <ShapeUpgrade_ShapeDivide.hxx>
#include <ShapeUpgrade_WireDivide.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

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
  //myMsgReg = new ShapeExtend_BasicMsgRegistrator;
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
//function : ~ShapeUpgrade_ShapeDivide
//purpose  : 
//=======================================================================

ShapeUpgrade_ShapeDivide::~ShapeUpgrade_ShapeDivide()
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
    Message_Msg doneMsg = GetFaceMsg();

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
              SendMsg( face, doneMsg, Message_Info );
	      if(SplitFace->Status(ShapeExtend_DONE1))
		myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE1 );
	      if(SplitFace->Status(ShapeExtend_DONE2))
		myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE2 );
	    }
	  }
	}
	catch (Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
	  cout << "\nError: Exception in ShapeUpgrade_FaceDivide::Perform(): ";
	  anException.Print(cout); cout << endl;
#endif
	  (void)anException;
	  myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_FAIL2 );
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
    Message_Msg doneMsg = GetWireMsg();

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
          SendMsg( wire, doneMsg, Message_Info );
	  myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE1 );
	}
      }
    }
  
    // Process free EDGEs
    Message_Msg edgeDoneMsg = GetEdgeMsg();
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
          SendMsg( edge, edgeDoneMsg, Message_Info );
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

//=======================================================================
//function : SetMsgRegistrator
//purpose  : 
//=======================================================================

void ShapeUpgrade_ShapeDivide::SetMsgRegistrator(const Handle(ShapeExtend_BasicMsgRegistrator)& msgreg) 
{
  myMsgReg = msgreg;
}

//=======================================================================
//function : MsgRegistrator
//purpose  : 
//=======================================================================

Handle(ShapeExtend_BasicMsgRegistrator) ShapeUpgrade_ShapeDivide::MsgRegistrator() const
{
  return myMsgReg;
}

//=======================================================================
//function : SendMsg
//purpose  :
//=======================================================================

void ShapeUpgrade_ShapeDivide::SendMsg(const TopoDS_Shape& shape,
                                       const Message_Msg& message,
                                       const Message_Gravity gravity) const
{
  if ( !myMsgReg.IsNull() )
    myMsgReg->Send (shape, message, gravity);
}

Message_Msg ShapeUpgrade_ShapeDivide::GetFaceMsg() const
{
  return "ShapeDivide.FaceDivide.MSG0";
}
Message_Msg ShapeUpgrade_ShapeDivide::GetWireMsg() const
{
  return "ShapeDivide.WireDivide.MSG0";
}
Message_Msg ShapeUpgrade_ShapeDivide::GetEdgeMsg() const
{
  return "ShapeDivide.EdgeDivide.MSG0";
}
