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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepLib.hxx>
#include <BRepTools.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_ElementarySurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomLib.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <Message_Msg.hxx>
#include <Poly_Polygon3D.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeAnalysis_DataMapOfShapeListOfReal.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeFix_Edge.hxx>
#include <ShapeFix_Face.hxx>
#include <ShapeFix_FixSmallFace.hxx>
#include <ShapeFix_Shape.hxx>
#include <ShapeFix_Wire.hxx>
#include <Standard_Type.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColgp_SequenceOfXYZ.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_Array1OfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeFix_FixSmallFace,ShapeFix_Root)

//#include <GeomLProp_SLProps.hxx>
//#include <TColStd_Array2OfReal.hxx>
//#include <TColStd_Array1OfReal.hxx>
ShapeFix_FixSmallFace::ShapeFix_FixSmallFace()
{
  myStatus = ShapeExtend::EncodeStatus ( ShapeExtend_OK );
  SetPrecision(Precision::Confusion());

}

 void ShapeFix_FixSmallFace::Init(const TopoDS_Shape& S) 
{
  myShape = S;
  if ( Context().IsNull() ) 
    SetContext ( new ShapeBuild_ReShape );
  myResult = myShape;
  Context()->Apply(myShape);
}


 void ShapeFix_FixSmallFace::Perform() 
{
  FixSpotFace();
  FixStripFace();
}

 TopoDS_Shape ShapeFix_FixSmallFace::FixSpotFace() 
{
 
  // gp_Pnt spot;
  // Standard_Real spotol;
  Standard_Boolean done = Standard_False;
  TopAbs_ShapeEnum st = myShape.ShapeType();
  if (st == TopAbs_COMPOUND || st == TopAbs_COMPSOLID || st == TopAbs_SOLID || st == TopAbs_SHELL ||  st == TopAbs_FACE) { 
    for (TopExp_Explorer itf (myShape,TopAbs_FACE); itf.More(); itf.Next()) {
//smh#8
      TopoDS_Shape tmpFace = Context()->Apply(itf.Current());
      TopoDS_Face F = TopoDS::Face (tmpFace);
      if(F.IsNull()) continue;
      if (myAnalyzer.CheckSpotFace (F,Precision())) 
	{
	  ReplaceVerticesInCaseOfSpot(F,Precision());
	  RemoveFacesInCaseOfSpot(F);
	  myStatus = ShapeExtend::EncodeStatus ( ShapeExtend_DONE1 );
	  done = Standard_True;
	}
    }
    myShape = Context()->Apply(myShape);
    Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire;
    if (done)
      {
	if (myShape.IsNull()) return myShape;
	/*ShapeFix_Wire sfw;
	sfw.SetContext(Context());
	sfw.SetPrecision(Precision::Confusion());
	if (myShape.IsNull()) return myShape;
	for (TopExp_Explorer itfw (myShape,TopAbs_FACE); itfw.More(); itfw.Next()) {
	  for (TopExp_Explorer itw (myShape,TopAbs_WIRE); itw.More(); itw.Next()) {
	    TopoDS_Wire w = TopoDS::Wire(itw.Current());
	    sfw.Init(w, TopoDS::Face(itfw.Current()), Precision::Confusion());
	    sfw->FixNotchedEdgesMode() = 0;
	    if(sfw.Perform())
		Context()->Replace(w, sfw.Wire());
	  }			 
	}*/
	myShape = FixShape();
      }
  
  //myShape = Context()->Apply(myShape);
  myResult = myShape;
  }
  return myShape;
}

 Standard_Boolean ShapeFix_FixSmallFace::ReplaceVerticesInCaseOfSpot(TopoDS_Face& F,const Standard_Real /*tol*/) const
{

  TColgp_SequenceOfXYZ thePositions;
  gp_XYZ thePosition;
  BRep_Builder theBuilder;
  Standard_Real theMaxDev;
  Standard_Real theMaxTol = 0.0;
  thePositions.Clear();
  gp_Pnt thePoint;
//smh#8
  TopoDS_Shape tmpFace = Context()->Apply(F);
  F = TopoDS::Face(tmpFace);
  // gka Mar2000 Protection against faces without wires
  // but they occur due to bugs in the algorithm itself, it needs to be fixed
  Standard_Boolean isWir = Standard_False;
  for(TopoDS_Iterator itw(F,Standard_False) ; itw.More();itw.Next()) {
    if(itw.Value().ShapeType() != TopAbs_WIRE)
      continue;
    TopoDS_Wire w1 = TopoDS::Wire(itw.Value());
    if (!w1.IsNull()) {isWir = Standard_True; break;}
  }
  if(!isWir) return Standard_True;
  //Accumulating positions and maximal vertex tolerance
  for (TopExp_Explorer iter_vertex(F,TopAbs_VERTEX); iter_vertex.More(); iter_vertex.Next()) {
    TopoDS_Vertex V = TopoDS::Vertex (iter_vertex.Current());
    thePoint = BRep_Tool::Pnt(V);
    if (theMaxTol <= (BRep_Tool::Tolerance(V))) theMaxTol = BRep_Tool::Tolerance(V);
    thePositions.Append(thePoint.XYZ());
  } 
  //Calculate common vertex position
  thePosition = gp_XYZ(0.,0.,0.);
  Standard_Integer theNbPos = thePositions.Length();
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (  i = 1; i <= theNbPos; i++ ) thePosition += thePositions.Value(i);
  if ( theNbPos > 1 ) thePosition /= theNbPos;
   
  // Calculate maximal deviation
  theMaxDev = 0.;
  for (  i = 1; i <= theNbPos; i++ ) {
    Standard_Real theDeviation = (thePosition-thePositions.Value(i)).Modulus();
    if ( theDeviation > theMaxDev ) theMaxDev = theDeviation;
  }
  theMaxDev *= 1.00001; 
  
  //Cretate new vertex with mean point
  TopoDS_Vertex theSharedVertex;
  theBuilder.MakeVertex(theSharedVertex);
  theBuilder.UpdateVertex( theSharedVertex, gp_Pnt(thePosition), theMaxDev+theMaxTol/2 );
  //Use external tolerance
//   if (tol!=-1.0) theBuilder.UpdateVertex( theSharedVertex, tol);
  //Replacing all vertices in the face by new one
  TopoDS_Vertex theNewVertex;
  for ( TopExp_Explorer iter_vert(F,TopAbs_VERTEX); iter_vert.More(); iter_vert.Next()) {
    TopoDS_Vertex V = TopoDS::Vertex (iter_vert.Current());
    if (V.Orientation()==TopAbs_FORWARD)  
//smh#8
      {
	TopoDS_Shape tmpVertexFwd = theSharedVertex.Oriented(TopAbs_FORWARD);
	theNewVertex = TopoDS::Vertex(tmpVertexFwd);
      }
    else 
//smh#8
      {
	TopoDS_Shape tmpVertexRev = theSharedVertex.Oriented(TopAbs_REVERSED);
	theNewVertex = TopoDS::Vertex(tmpVertexRev);
      }
    Context()->Replace(V, theNewVertex);
  }
  return Standard_True;
}

 Standard_Boolean ShapeFix_FixSmallFace::RemoveFacesInCaseOfSpot(const TopoDS_Face& F) const
{
  for ( TopExp_Explorer iter_vert(F,TopAbs_EDGE); iter_vert.More(); iter_vert.Next()) {
    TopoDS_Edge Ed = TopoDS::Edge (iter_vert.Current());
    Context()->Remove(Ed);
  }
  Context()->Remove(F);
  SendWarning( F, Message_Msg( "FixAdvFace.FixSpotFace.MSG0" ));
  return Standard_True;


}

 TopoDS_Shape ShapeFix_FixSmallFace::FixStripFace(const Standard_Boolean wasdone) 
{
  if(myShape.IsNull()) return myShape;
  TopAbs_ShapeEnum st = myShape.ShapeType();
  // BRep_Builder theBuilder;
  Standard_Boolean done = wasdone;
  if (st == TopAbs_COMPOUND || st == TopAbs_COMPSOLID ||st == TopAbs_SOLID || st == TopAbs_SHELL || st == TopAbs_FACE) {
    for (TopExp_Explorer itf (myShape,TopAbs_FACE); itf.More(); itf.Next()) {
      TopoDS_Face F = TopoDS::Face (itf.Current());
//smh#8
      TopoDS_Shape tmpFace = Context()->Apply(F);
      F= TopoDS::Face(tmpFace);
      if(F.IsNull()) continue;
      // Standard_Real dmax = 1;
      TopoDS_Edge E1,E2;
      if (myAnalyzer.CheckStripFace (F, E1,E2,Precision()))  
	{
	  if(ReplaceInCaseOfStrip(F,E1,E2, Precision()))
	    RemoveFacesInCaseOfStrip(F);
	  myStatus = ShapeExtend::EncodeStatus ( ShapeExtend_DONE2 );
	  done = Standard_True;
	}
    }
    myShape = Context()->Apply(myShape);
    //Particular case of empty shell
    if (!myShape.IsNull())
      {
	for (TopExp_Explorer exp_s (myShape,TopAbs_SHELL); exp_s.More(); exp_s.Next()) {
	  TopoDS_Shell Sh = TopoDS::Shell (exp_s.Current());
	  TopExp_Explorer ex_sh(Sh,TopAbs_FACE);
	  if (!ex_sh.More())  { Context()->Remove(Sh);
// 				cout << "Empty shell was removed" << endl; 
			      }
	}
	myShape = Context()->Apply(myShape);
    //Fixing of missing pcurves on new edges, if thay were inserted
	if (done)
	  {
	    if (myShape.IsNull()) return myShape;
	    TopoDS_Shape theResult;
	    myShape = FixShape();
	    //myShape = Context()->Apply(myShape);
	    myResult = myShape;
	  }
	
      }
  }
  return myShape; 

}

 Standard_Boolean ShapeFix_FixSmallFace::ReplaceInCaseOfStrip(TopoDS_Face& F,TopoDS_Edge& E1,TopoDS_Edge& E2, const Standard_Real tol) const
{
  if(E1.IsNull() || E2.IsNull()) return Standard_False;
  TopoDS_Edge theSharedEdge;
  TopoDS_Face F1,F2;
//smh#8
  TopoDS_Shape tmpFace = Context()->Apply(F);
  F= TopoDS::Face(tmpFace);
  for(TopExp_Explorer expf(myShape,TopAbs_FACE); expf.More(); expf.Next()) {
//smh#8
    TopoDS_Shape tmpShape = Context()->Apply(expf.Current());
    TopoDS_Face tempF = TopoDS::Face (tmpShape);
    if(tempF.IsNull() || tempF.IsSame(F)) continue;
    for(TopExp_Explorer expe(tempF,TopAbs_EDGE); expe.More(); expe.Next()) {
      TopoDS_Edge tempE = TopoDS::Edge (expe.Current());
      if(tempE.IsSame(E1)) F1 = tempF;
      if(tempE.IsSame(E2)) F2 = tempF; 
      if(!F1.IsNull()) break; // && !F2.IsNull()) break;
    }
  }
  
  //Compute shared edge for this face
  if(F1.IsNull() && F2.IsNull()) return Standard_True;
  TopoDS_Edge E1tmp = E1;
  TopoDS_Edge E2tmp = E2;
  if(F1.IsNull()) {
    E1tmp = E2;
    E2tmp = E1;
    F1 = F2;
  }
  theSharedEdge = ComputeSharedEdgeForStripFace(F, E1tmp, E2tmp, F1, tol);
  //Replace two long edges by new one
  if (theSharedEdge.IsNull()) return Standard_False;
  if (E1.Orientation()==TopAbs_REVERSED) {
      Context()->Replace(E1tmp, theSharedEdge.Oriented(TopAbs_REVERSED));
      if(F.Orientation() == F1.Orientation())
        Context()->Replace(E2tmp, theSharedEdge);
      else 
       Context()->Replace(E2tmp, theSharedEdge.Oriented(TopAbs_REVERSED));
    }
  else 
    {
      Context()->Replace(E1tmp, theSharedEdge);
     if(F.Orientation() == F1.Orientation())
      Context()->Replace(E2tmp, theSharedEdge.Oriented(TopAbs_REVERSED));
     else
      Context()->Replace(E2tmp, theSharedEdge);
    }
    
  //Remove short edges
  for (TopExp_Explorer exp_e (F,TopAbs_EDGE); exp_e.More(); exp_e.Next()) {
    TopoDS_Edge shortedge = TopoDS::Edge (exp_e.Current());
    if (!shortedge.IsSame(E1tmp) && !shortedge.IsSame(E2tmp)) Context()->Remove(shortedge); 
  }
  
  return Standard_True;
}

 Standard_Boolean ShapeFix_FixSmallFace::RemoveFacesInCaseOfStrip(const TopoDS_Face& F) const
{
  Context()->Remove(F);
  SendWarning( F, Message_Msg( "FixAdvFace.FixStripFace.MSG0" ));
  return Standard_True;
}

 TopoDS_Edge ShapeFix_FixSmallFace::ComputeSharedEdgeForStripFace(const TopoDS_Face& /*F*/,const TopoDS_Edge& E1,const TopoDS_Edge& E2,const TopoDS_Face& F1,const Standard_Real tol) const
{

  BRep_Builder theBuilder;
  //Compute deviation between two vertices and create new vertices
  TopoDS_Edge theNewEdge;
  TopoDS_Vertex V1,V2, V3, V4;
  TopExp::Vertices (E1,V1,V2);
  TopExp::Vertices (E2,V3,V4);
  gp_Pnt p1, p2;
  Standard_Real dev;
  p1 = BRep_Tool::Pnt(V1);
  p2 =  BRep_Tool::Pnt(V3);
  dev = p1.Distance(p2);
  TopoDS_Vertex theFirstVer;
  TopoDS_Vertex theSecondVer;
  theBuilder.MakeVertex(theFirstVer);
  theBuilder.MakeVertex(theSecondVer);
  gp_XYZ thePosition;
  TopoDS_Shape temp;
  
  if ((dev<=BRep_Tool::Tolerance (V1)) || (dev<=BRep_Tool::Tolerance (V3)) || (dev<=tol)) {
      if (V1.IsSame(V3))  
// #ifdef AIX  CKY : applies to all platforms
	theFirstVer = V1; 
//	theFirstVer = TopoDS::Vertex(V1); 

      else {
	dev = (dev/2)*1.0001;
	thePosition = (p1.XYZ()+p2.XYZ())/2;  
	theBuilder.UpdateVertex(theFirstVer, gp_Pnt(thePosition), dev );
	//if(Context()->Status(V1, temp) != 0) theFirstVer = TopoDS::Vertex (temp); //If this vertex already recorded in map
	//else                                                                       //take recorded vertex 
	//  if(theRepVert->Status(V3, temp) != 0) theFirstVer = TopoDS::Vertex (temp);
	if (V1.Orientation()==TopAbs_FORWARD) Context()->Replace(V1, theFirstVer.Oriented(TopAbs_FORWARD));
	else Context()->Replace(V1, theFirstVer.Oriented(TopAbs_REVERSED));
	if (V3.Orientation()==TopAbs_FORWARD) Context()->Replace(V3, theFirstVer.Oriented(TopAbs_FORWARD));
	else Context()->Replace(V3, theFirstVer.Oriented(TopAbs_REVERSED));
	
     }
      if(V1.IsSame(V2) || V3.IsSame(V4))
	theSecondVer = theFirstVer; 
      else {
	if (!V2.IsSame(V4))   {
	  // #ifdef AIX  CKY : applies to all platforms
	  
	  p1 = BRep_Tool::Pnt(V2);
	  p2 =  BRep_Tool::Pnt(V4);
	  dev = p1.Distance(p2);
	  thePosition = (p1.XYZ()+p2.XYZ())/2;  
	  theBuilder.UpdateVertex(theSecondVer, gp_Pnt(thePosition), dev );
	  //if(theRepVert->Status(V2, temp) != 0) theSecondVer = TopoDS::Vertex (temp); //If this vertex already recorded in map
	  //else 
	  //  if(theRepVert->Status(V4, temp) != 0) theSecondVer = TopoDS::Vertex (temp);
	}
	else theSecondVer = V2;
	
      }
      if (!V2.IsSame(theSecondVer))   {
	if (V2.Orientation()==TopAbs_FORWARD) Context()->Replace(V2, theSecondVer.Oriented(TopAbs_FORWARD));
	else Context()->Replace(V2, theSecondVer.Oriented(TopAbs_REVERSED));
	if (V4.Orientation()==TopAbs_FORWARD) Context()->Replace(V4, theSecondVer.Oriented(TopAbs_FORWARD));
	else Context()->Replace(V4, theSecondVer.Oriented(TopAbs_REVERSED));
      }
    }
  else   {
      p2 =  BRep_Tool::Pnt(V4);  
      dev = p1.Distance(p2);
      if ((dev<=BRep_Tool::Tolerance (V1)) || (dev<=BRep_Tool::Tolerance (V4)) || (dev<=tol)) {
	  if (V1.IsSame(V4))  
// #ifdef AIX  CKY : applies to all platforms
	    theFirstVer = V1;
//	    theFirstVer = TopoDS::Vertex(V1);

	  else {
	    dev = (dev/2)*1.0001;
	    thePosition = (p1.XYZ()+p2.XYZ())/2;  
	    theBuilder.UpdateVertex(theFirstVer, gp_Pnt(thePosition), dev );
	//    if(theRepVert->Status(V1, temp) != 0) theFirstVer = TopoDS::Vertex (temp); //If this vertex already recorded in map
	  //  else 
	  //    if(theRepVert->Status(V4, temp) != 0) theFirstVer = TopoDS::Vertex (temp);
	    if (V1.Orientation()==TopAbs_FORWARD) Context()->Replace(V1, theFirstVer.Oriented(TopAbs_FORWARD));
	    else Context()->Replace(V1, theFirstVer.Oriented(TopAbs_REVERSED));
	    if (V4.Orientation()==TopAbs_FORWARD) Context()->Replace(V4, theFirstVer.Oriented(TopAbs_FORWARD));
	    else Context()->Replace(V4, theFirstVer.Oriented(TopAbs_REVERSED));
	  }
	  if(V1.IsSame(V2) || V3.IsSame(V4))
	    theSecondVer = theFirstVer; 
	  else {
	    
	    if (!V2.IsSame(V3)) {
	      p1 = BRep_Tool::Pnt(V2);
	      p2 =  BRep_Tool::Pnt(V3);
	      dev = p1.Distance(p2);
	      thePosition = (p1.XYZ()+p2.XYZ())/2;  
	      theBuilder.UpdateVertex(theSecondVer, gp_Pnt(thePosition), dev );
	    }
	    else theSecondVer = V2;
	  }
	      //  if(theRepVert->Status(V2, temp) != 0) theSecondVer = TopoDS::Vertex (temp); //If this vertex already recorded in map
	      // else 
	      //  if(theRepVert->Status(V3, temp) != 0) theSecondVer = TopoDS::Vertex (temp);
	  if (!V2.IsSame(theSecondVer)) {
	      if (V2.Orientation()==TopAbs_FORWARD) Context()->Replace(V2, theSecondVer.Oriented(TopAbs_FORWARD));
	    else Context()->Replace(V2, theSecondVer.Oriented(TopAbs_REVERSED));
	    if (V3.Orientation()==TopAbs_FORWARD) Context()->Replace(V3, theSecondVer.Oriented(TopAbs_FORWARD));
	    else Context()->Replace(V3, theSecondVer.Oriented(TopAbs_REVERSED));
	  }
	  
	} 
      else {
#ifdef OCCT_DEBUG
	cout << "The face is not strip face"  << endl;
#endif
	return theNewEdge;
      }
    }
  if (theFirstVer.IsNull() || theSecondVer.IsNull()) return theNewEdge;
  //Cretate new edge 
  theBuilder.MakeEdge(theNewEdge);
  Standard_Real f, l, fp1, lp1/*, fp2, lp2*/;
  TopLoc_Location loc;
  Handle(Geom_Curve) the3dcurve;
  the3dcurve = BRep_Tool::Curve(E1, f, l);
  Handle(Geom2d_Curve) the2dcurve1, the2dcurve2, thenew1, thenew2;
  if (!F1.IsNull())   
    {
      the2dcurve1 = BRep_Tool::CurveOnSurface(E1, F1, fp1, lp1);
      if(!the2dcurve1.IsNull() && fp1!=f && lp1!=l) GeomLib::SameRange(Precision::Confusion(), the2dcurve1, fp1, lp1, f, l, thenew1);  
    }
      
 /* if (!F2.IsNull())   
    {
      the2dcurve2 = BRep_Tool::CurveOnSurface(E2, F2, fp2, lp2);
      if(!the2dcurve2.IsNull()) GeomLib::SameRange(Precision::Confusion(), the2dcurve2, fp2, lp2, f, l, thenew2); 
    }*/
  
  Standard_Real maxdev; 
  if ((BRep_Tool::Tolerance(theFirstVer))<=(BRep_Tool::Tolerance(theSecondVer))) 
    maxdev = (BRep_Tool::Tolerance(theSecondVer));
  else  maxdev = (BRep_Tool::Tolerance(theFirstVer));
  theBuilder.UpdateVertex(theFirstVer, maxdev);
  theBuilder.UpdateVertex(theSecondVer, maxdev);
  //Standard_Boolean IsFree = Standard_True;
  theBuilder.SameParameter(theNewEdge, Standard_False);
  the3dcurve = BRep_Tool::Curve(E1, f, l);
  theBuilder.UpdateEdge(theNewEdge, the3dcurve, maxdev);
  theBuilder.Range(theNewEdge, f, l);
  if (!F1.IsNull() && !thenew1.IsNull())  
    {
      theBuilder.UpdateEdge(theNewEdge, thenew1, F1, maxdev); 
      //IsFree = Standard_False;
    }
  /*if (!F2.IsNull() && !thenew2.IsNull())
    {
      theBuilder.UpdateEdge(theNewEdge, thenew2, F2, maxdev);
      IsFree = Standard_False;
    }*/
  theBuilder.Add(theNewEdge, theFirstVer.Oriented(TopAbs_FORWARD));
  theBuilder.Add(theNewEdge, theSecondVer.Oriented(TopAbs_REVERSED));
  //Call fixsameparameter for computing distance between 3d and pcurves, if edge is not free
//   if (!IsFree)
//     {
//       ShapeFix_Edge sfe;
//       if (!F1.IsNull() && !thenew1.IsNull()) sfe.FixReversed2d(theNewEdge, F1);
//       if (!F2.IsNull() && !thenew2.IsNull()) sfe.FixReversed2d(theNewEdge, F2);
//       sfe.FixSameParameter(theNewEdge, maxdev);
//     }
  return theNewEdge;


}

 TopoDS_Shape ShapeFix_FixSmallFace::FixSplitFace(const TopoDS_Shape& /*S*/) 
{
  if (myShape.IsNull()) return myShape;
  TopAbs_ShapeEnum st = myShape.ShapeType();
  Standard_Boolean done = Standard_False;
  TopoDS_Compound theSplittedFaces;
  BRep_Builder theBuilder;
  if (st == TopAbs_COMPOUND || st == TopAbs_COMPSOLID ||
      st == TopAbs_SOLID || st ==  TopAbs_SHELL || st ==  TopAbs_FACE) {
    for (TopExp_Explorer itf (myShape,TopAbs_FACE); itf.More(); itf.Next()) {
      TopoDS_Face F = TopoDS::Face (itf.Current());
      TopoDS_Compound CompSplittedFaces;
      theBuilder.MakeCompound(CompSplittedFaces);
      if(SplitOneFace(F, CompSplittedFaces)) {
        done = Standard_True;
        Context()->Replace(F, CompSplittedFaces);
      }
    }
  }
  if(done) myStatus = ShapeExtend::EncodeStatus ( ShapeExtend_DONE3 );
  myShape = Context()->Apply(myShape);
  myResult = myShape;
  return myShape;
}

 Standard_Boolean ShapeFix_FixSmallFace::SplitOneFace(TopoDS_Face& F,TopoDS_Compound& theSplittedFaces) 
{
  TopTools_DataMapOfShapeListOfShape MapEdges; 
  ShapeAnalysis_DataMapOfShapeListOfReal MapParam;
  TopoDS_Compound theAllVert;
  BRep_Builder theBuilder;
  theBuilder.MakeCompound(theAllVert);
//smh#8
  TopoDS_Shape tmpShape = Context()->Apply(F);
  F = TopoDS::Face(tmpShape);
  if (myAnalyzer.CheckSplittingVertices(F,MapEdges,MapParam,theAllVert) != 0) 
    {
      TopoDS_Wire tempwire;
      //Take information about splitting vertices
      if (theAllVert.IsNull()) return Standard_False;
      //Standard_Integer i;
      TopoDS_Vertex V;
      TopExp_Explorer itc(theAllVert,TopAbs_VERTEX); V = TopoDS::Vertex (itc.Current());
      if (V.IsNull()) return Standard_False;
      gp_Pnt proj;
      gp_Pnt vp = BRep_Tool::Pnt(V);
      TopoDS_Vertex theNewVertex;
      TopoDS_Edge E;
      TopoDS_Edge theFirstEdge, theSecondEdge;
      
      {
	//If one vertex presents do splitting by two faces
	ShapeAnalysis_Curve SAC;
	for (TopExp_Explorer ite(F,TopAbs_EDGE); ite.More(); ite.Next()) {
	  E = TopoDS::Edge (ite.Current());
	  TopoDS_Vertex V1,V2;
	  TopExp::Vertices (E,V1,V2);
	  Standard_Real cf,cl;
	  Handle(Geom_Curve) C3D = BRep_Tool::Curve (E,cf,cl);
	  if (C3D.IsNull()) continue;
	  if (V.IsSame(V1) || V.IsSame(V2)) continue;
	  Standard_Real vt = BRep_Tool::Tolerance (V);
	  Standard_Real param;
	  Standard_Real dist = SAC.Project (C3D,vp,vt*10.,proj,param,cf,cl);
	  if (dist==0) continue; //Projection on same curve but on other edge ?
	  if ( dist <= vt ) 
	    {
	      theBuilder.MakeVertex(theNewVertex);
	      theBuilder.UpdateVertex(theNewVertex, proj, Precision::Confusion());
	      theBuilder.MakeEdge(theFirstEdge);
	      theBuilder.MakeEdge(theSecondEdge);
	      Standard_Real f, l;
	      Handle(Geom_Curve) the3dcurve = BRep_Tool::Curve(E, f, l);
	      theBuilder.UpdateEdge(theFirstEdge, the3dcurve,Precision::Confusion());
	      theBuilder.UpdateEdge(theSecondEdge, the3dcurve,Precision::Confusion());
	      if (V1.Orientation()==TopAbs_FORWARD) 
		{
		  theBuilder.Add(theFirstEdge, V1);
		  theBuilder.Add(theFirstEdge,theNewVertex.Oriented(TopAbs_REVERSED));
		  theBuilder.Add(theSecondEdge,theNewVertex.Oriented(TopAbs_FORWARD));
		  theBuilder.Add(theSecondEdge, V2);
		}
	      else {
		theBuilder.Add(theFirstEdge,V2);
		theBuilder.Add(theFirstEdge,theNewVertex.Oriented(TopAbs_REVERSED));
		theBuilder.Add(theSecondEdge,theNewVertex.Oriented(TopAbs_FORWARD));
		theBuilder.Add(theSecondEdge, V1);
	      }
	      theBuilder.Range(theFirstEdge, cf, param);
	      theBuilder.Range(theSecondEdge, param, cl);
	      //Replace old edge by two new edges
	      TopoDS_Wire twoedges;
	      theBuilder.MakeWire(twoedges);
	      if (E.Orientation() == TopAbs_FORWARD)
		{
		  theBuilder.Add(twoedges, theFirstEdge.Oriented(TopAbs_FORWARD));
		  theBuilder.Add(twoedges, theSecondEdge.Oriented(TopAbs_FORWARD));
		}
	      else 
		{
		  theBuilder.Add(twoedges, theFirstEdge.Oriented(TopAbs_REVERSED));
		  theBuilder.Add(twoedges, theSecondEdge.Oriented(TopAbs_REVERSED));
		}
	      Context()->Replace(E, twoedges);
	      break;
	    }
	}
	if (theNewVertex.IsNull()) return Standard_False;
	//Create split edge
	TopoDS_Edge theSplitEdge;
	gp_Lin lin(vp, gp_Dir(gp_Vec( vp, proj)));
	Standard_Real firstparam = ElCLib::Parameter(lin, vp);
	Standard_Real lastparam = ElCLib::Parameter(lin, proj);
	Handle(Geom_Line) L = new Geom_Line( vp, gp_Vec( vp, proj));
	Handle(Geom_Curve) the3dc = L;
	theBuilder.MakeEdge(theSplitEdge, the3dc, Precision::Confusion());
	theBuilder.Add(theSplitEdge, V.Oriented(TopAbs_FORWARD));
	theBuilder.Add(theSplitEdge, theNewVertex.Oriented(TopAbs_REVERSED));  
	theBuilder.Range(theSplitEdge, firstparam, lastparam);
	//Add pcurve in new edge
	Handle(ShapeFix_Edge) sfe = new ShapeFix_Edge;
	sfe->FixAddPCurve(theSplitEdge, F, Standard_False);
	//Reorder the wire 
	TopoDS_Wire wireonface;
	//Inher loop is not support yet !!!
	TopExp_Explorer itw(F,TopAbs_WIRE);
	wireonface = TopoDS::Wire (itw.Current());
	itw.Next();
	if (itw.More()) return Standard_False; //if face contains more than one wire  
	Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire;
	sfw->Init(wireonface, F, Precision::Confusion());
	sfw->FixReorder();
	wireonface = sfw->Wire();
	
	//Create two new wires
	TopoDS_Wire w1, w2;
	theBuilder.MakeWire(w1);
	theBuilder.MakeWire(w2);
	theBuilder.MakeWire(tempwire);
	for (TopExp_Explorer itnew(wireonface, TopAbs_EDGE ); itnew.More(); itnew.Next()) 
	  {
	    TopoDS_Edge ce = TopoDS::Edge (itnew.Current());
	    if (ce.IsSame(E)) 
	      { 
		theBuilder.Remove(wireonface, ce);
		theBuilder.Add(wireonface, theFirstEdge.Oriented(TopAbs_FORWARD)); 
		theBuilder.Add(wireonface, theSecondEdge.Oriented(TopAbs_FORWARD)); 
	      }
	  }
	sfw->Init(wireonface, F, Precision::Confusion());
	sfw->FixReorder();
	wireonface = sfw->Wire();
	  
	for (TopExp_Explorer itere(wireonface, TopAbs_EDGE); itere.More(); itere.Next()) 
	  {
	    TopoDS_Edge ce = TopoDS::Edge (itere.Current());
	    TopoDS_Vertex thecontrol;
	    if (ce.Orientation () == TopAbs_FORWARD) thecontrol = TopExp::LastVertex(ce);  
	    else thecontrol = TopExp::FirstVertex(ce);
	    theBuilder.Add(w1, ce);
	    if (thecontrol.IsSame(V))
	      {
		theBuilder.Add(w1, theSplitEdge.Oriented(TopAbs_FORWARD));
		TopoDS_Wire wtemp = w1;
		w1 = w2;
		w2 = wtemp;
	      }
	    if (thecontrol.IsSame(theNewVertex))
	      {
		theBuilder.Add(w1, theSplitEdge.Oriented(TopAbs_REVERSED));
		TopoDS_Wire wtemp = w1;
		w1 = w2;
		w2 = wtemp;
	      }
	  }
	if ( w1.IsNull()|| w2.IsNull() ) return Standard_False;
	//Create two new faces and replace old one
	TopoDS_Face F1;
	TopoDS_Face F2;
	theBuilder.MakeFace(F1, BRep_Tool::Surface(F), Precision::Confusion());
	theBuilder.MakeFace(F2, BRep_Tool::Surface(F), Precision::Confusion());      
	theBuilder.Add(F1, w1);
	theBuilder.Add(F2, w2);
	TopoDS_Compound tf;
	theBuilder.MakeCompound(tf);
	theBuilder.Add(tf,F1);
	theBuilder.Add(tf, F2);
 //Call recursive spliteoneface() for each face     
	if(!SplitOneFace(F1, theSplittedFaces)) theBuilder.Add(theSplittedFaces, F1);
	if(!SplitOneFace(F2, theSplittedFaces)) theBuilder.Add(theSplittedFaces, F2);
      }
      return Standard_True ;
    }
  return Standard_False ;
}

 TopoDS_Face ShapeFix_FixSmallFace::FixFace(const TopoDS_Face& F) 
{
//smh#8
  TopoDS_Shape emptyCopied = F.EmptyCopied();
  TopoDS_Face theFixedFace = TopoDS::Face(emptyCopied);
 // BRep_Builder theBuilder;
  
 // Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire;
 // sfw->SetContext(Context());
 // for (TopExp_Explorer exp_w (F,TopAbs_WIRE); exp_w.More(); exp_w.Next()) {
 //   TopoDS_Wire theCurWire = TopoDS::Wire (exp_w.Current());
    
 //   sfw->Init(theCurWire,  F, Precision::Confusion());
 //   if(sfw->NbEdges() == 0) continue;
 //   sfw->FixNotchedEdgesMode() = 0;
 //   sfw->Perform();
 //   theCurWire = sfw->Wire();
 //   theBuilder.Add(theFixedFace, theCurWire);
 // }
  Handle(ShapeFix_Face) sff = new ShapeFix_Face;
  sff->SetContext(Context());
  sff->Init(F);
  sff->Perform();
  //sff->Init(theFixedFace);
  //sff->FixOrientation();
  theFixedFace = sff->Face();
  return theFixedFace; 
}

 TopoDS_Shape ShapeFix_FixSmallFace::FixShape() 
{
  TopoDS_Shape FixSh;
  if(myShape.IsNull()) return FixSh;
  /*ShapeFix_Shape sfs;
  sfs.SetContext(Context());

  sfs.SetPrecision(Precision::Confusion());
  sfs.Init(myShape);
  sfs.Perform();
  FixSh = sfs.Shape();*/
  for(TopExp_Explorer expf(myShape,TopAbs_FACE) ; expf.More(); expf.Next()) {
   TopoDS_Face F = TopoDS::Face(expf.Current());
//smh#8
   TopoDS_Shape tmpFace = Context()->Apply(F);
   F= TopoDS::Face(tmpFace);
   TopoDS_Face newF = FixFace(F);
   Context()->Replace(F,newF);
  }
  FixSh = Context()->Apply(myShape);  
  return FixSh;

}
 TopoDS_Shape ShapeFix_FixSmallFace::Shape() 
{
  return myShape;
}

Standard_Boolean ShapeFix_FixSmallFace::FixPinFace (TopoDS_Face& /*F*/)
{
  return Standard_True;
}
