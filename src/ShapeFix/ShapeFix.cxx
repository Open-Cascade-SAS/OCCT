// Created on: 2000-01-21
// Created by: data exchange team
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

#include <ShapeFix.hxx>
//:k2 abv 16.12.98: eliminating code duplication
//pdn     18.12.98: checking deviation for SP edges
//:   abv 22.02.99: method FillFace() removed since PRO13123 is fixed
//szv#4 S4163
//szv#9:S4244:19Aug99: Added method FixWireGaps
//szv#10:S4244:23Aug99: Added method FixFaceGaps

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>

#include <Precision.hxx>

#include <Standard_Failure.hxx>

#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <Geom_Surface.hxx>

//:i2
#include <gp_Pnt.hxx>
#include <Geom_Plane.hxx>
#include <ShapeFix_Edge.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <ShapeAnalysis_Surface.hxx>

#include <ShapeFix_Edge.hxx>
#include <ShapeFix_Shape.hxx>
#include <ShapeFix_Wire.hxx>
#include <ShapeFix_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <TopTools_MapOfShape.hxx>
#include <BRepLib.hxx>

#include <ShapeAnalysis_Edge.hxx>
#include <ShapeBuild_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopExp.hxx>

#include <Message_ProgressSentry.hxx>
#include <Message_Msg.hxx>
#include <ShapeExtend_BasicMsgRegistrator.hxx>

//=======================================================================
//function : SameParameter
//purpose  : 
//=======================================================================

Standard_Boolean ShapeFix::SameParameter(const TopoDS_Shape& shape,
                                         const Standard_Boolean enforce,
                                         const Standard_Real preci,
                                         const Handle(Message_ProgressIndicator)& theProgress,
                                         const Handle(ShapeExtend_BasicMsgRegistrator)& theMsgReg)
{
  // Calculate number of edges
  Standard_Integer aNbEdges = 0;
  for ( TopExp_Explorer anEdgeExp(shape, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next() )
    ++aNbEdges;

  // Calculate number of faces
  Standard_Integer aNbFaces = 0;
  for ( TopExp_Explorer anEdgeExp(shape, TopAbs_FACE); anEdgeExp.More(); anEdgeExp.Next() )
    ++aNbFaces;

  TopTools_IndexedDataMapOfShapeListOfShape aMapEF;
  TopExp::MapShapesAndAncestors(shape, TopAbs_EDGE, TopAbs_FACE, aMapEF);

  BRep_Builder B;
  //Standard_Integer nbexcp = 0; 
  Standard_Integer nbfail = 0,  numedge = 0; 
  Standard_Boolean status = Standard_True;
  Standard_Real tol = preci;
  Standard_Boolean iatol = (tol > 0);
  Handle(ShapeFix_Edge) sfe = new ShapeFix_Edge;
  TopExp_Explorer ex(shape,TopAbs_EDGE);
  Message_Msg doneMsg("FixEdge.SameParameter.MSG0");

  // Start progress scope (no need to check if progress exists -- it is safe)
  Message_ProgressSentry aPSentryForSameParam(theProgress, "Fixing same parameter problem", 0, 2, 1);

  {
    // Start progress scope (no need to check if progress exists -- it is safe)
    Message_ProgressSentry aPSentry(theProgress, "Fixing edge", 0, aNbEdges, 1);

    while ( ex.More() )
    {
      TopoDS_Edge E;
      while ( ex.More() && aPSentry.More() )
      {
        numedge ++;
        int ierr = 0;
        TopLoc_Location loc;
        E = TopoDS::Edge (ex.Current());
        ex.Next();
  	
        if (!iatol)
          tol = BRep_Tool::Tolerance (E);
        if (enforce)
        {
          B.SameRange     (E,Standard_False);
          B.SameParameter (E,Standard_False);
        }

        TopTools_ListOfShape aListOfFaces;
        aMapEF.FindFromKey(E, aListOfFaces);
        if (aListOfFaces.Extent() != 0)
        {
          TopTools_ListOfShape::Iterator aListOfFacesIt(aListOfFaces);
          for ( ; aListOfFacesIt.More() ; aListOfFacesIt.Next())
          {
            TopoDS_Face aF = TopoDS::Face( aListOfFacesIt.Value() );
            sfe->FixSameParameter (E, aF);
          }
        }
        else
        {
          sfe->FixSameParameter (E); // K2-SEP97
        }

        if (!BRep_Tool::SameParameter (E)) { ierr = 1; nbfail ++; }
        
        if (ierr)
        {
          status = Standard_False;
          B.SameRange (E,Standard_False);
          B.SameParameter (E,Standard_False);
        }
        else if ( !theMsgReg.IsNull() && !sfe->Status( ShapeExtend_OK ) )
        {
          theMsgReg->Send( E, doneMsg, Message_Warning );
        }

        // Complete step in current progress scope
        aPSentry.Next();     
      } // -- end while

      // Halt algorithm in case of user's abort
      if ( !aPSentry.More() )
        return Standard_False;
    }

  }
  // Switch to "Update tolerances" step
  aPSentryForSameParam.Next();

  {
    // Start progress scope (no need to check if progress exists -- it is safe)
    Message_ProgressSentry aPSentry(theProgress, "Update tolerances", 0, aNbFaces, 1);

    //:i2 abv 21 Aug 98: ProSTEP TR8 Motor.rle face 710:
    // Update tolerance of edges on planes (no pcurves are stored)
    for ( TopExp_Explorer exp ( shape, TopAbs_FACE ); exp.More() && aPSentry.More(); exp.Next(), aPSentry.Next() )
    {
      TopoDS_Face face = TopoDS::Face ( exp.Current() );
      Handle(Geom_Surface) Surf = BRep_Tool::Surface ( face );
        
      Handle(Geom_Plane) plane = Handle(Geom_Plane)::DownCast ( Surf );
      if ( plane.IsNull() ) {
        Handle(Geom_RectangularTrimmedSurface) GRTS = 
          Handle(Geom_RectangularTrimmedSurface)::DownCast ( Surf );
        if ( ! GRTS.IsNull() ) 
          plane = Handle(Geom_Plane)::DownCast ( GRTS->BasisSurface() );
        if ( plane.IsNull() )
           continue;
      }

      Handle(GeomAdaptor_HSurface) AS = new GeomAdaptor_HSurface ( plane );
      for ( TopExp_Explorer ed ( face, TopAbs_EDGE ); ed.More(); ed.Next() ) {
        TopoDS_Edge edge = TopoDS::Edge ( ed.Current() );
        Standard_Real f, l;
        Handle(Geom_Curve) crv = BRep_Tool::Curve ( edge, f, l );
        if ( crv.IsNull() )
          continue;
  	
        Handle(Geom2d_Curve) c2d = BRep_Tool::CurveOnSurface ( edge, face, f, l );;
        if ( c2d.IsNull() ) continue;
        Handle(Geom2dAdaptor_HCurve) GHPC = new Geom2dAdaptor_HCurve ( c2d, f, l );
        Adaptor3d_CurveOnSurface ACS(GHPC,AS);
  	
        Standard_Real tol0 = BRep_Tool::Tolerance(edge);
        tol = tol0;
        Standard_Real tol2 = tol*tol;
        Standard_Boolean isChanged = Standard_False;
        const Standard_Integer NCONTROL = 23;
        for ( Standard_Integer i = 0; i < NCONTROL; i++ )
        {
          Standard_Real par = ( f * ( NCONTROL - 1 - i ) + l * i ) / ( NCONTROL - 1 );
          gp_Pnt pnt = crv->Value ( par );
          gp_Pnt prj = ACS.Value( par );
          Standard_Real dist = pnt.SquareDistance(prj);
          if ( tol2 < dist ) 
          {
            tol2 = dist;
            isChanged = Standard_True;
          }
        }
        if ( isChanged )
        {
          tol = 1.00005 * sqrt(tol2); // coeff: see trj3_pm1-ct-203.stp #19681, edge 10
          if ( tol >= tol0 )
          {
            B.UpdateEdge ( edge, tol );
            for ( TopoDS_Iterator itV(edge); itV.More(); itV.Next() )
            {
              TopoDS_Shape S = itV.Value();
              B.UpdateVertex ( TopoDS::Vertex ( S ), tol );
            }
          }
        }
      }

      // Halt algorithm in case of user's abort
      if ( !aPSentry.More() )
        return Standard_False;
    }
  }

  if (!status) {
#ifdef OCCT_DEBUG
    cout<<"** SameParameter not complete. On "<<numedge<<" Edges:";
    if (nbfail > 0) cout<<"  "<<nbfail<<" Failed";
    cout<<endl;
#endif
  }
  return status;
}


//=======================================================================
//function : EncodeRegularity
//purpose  : 
//=======================================================================

void ShapeFix::EncodeRegularity (const TopoDS_Shape& shape,
                                 const Standard_Real tolang)
{
  BRepLib::EncodeRegularity(shape, tolang);
}


//=======================================================================
//function : RemoveSmallEdges
//purpose  : 
//=======================================================================

TopoDS_Shape ShapeFix::RemoveSmallEdges (TopoDS_Shape& Shape,
					 const Standard_Real Tolerance,
					 Handle(ShapeBuild_ReShape)& context)
{
  Handle(ShapeFix_Shape) sfs = new ShapeFix_Shape;
  sfs->Init(Shape);
  sfs->SetPrecision(Tolerance);
  sfs->FixFaceTool()->FixMissingSeamMode() = Standard_False;
  sfs->FixFaceTool()->FixOrientationMode() = Standard_False;
  sfs->FixFaceTool()->FixSmallAreaWireMode() = Standard_False;
  sfs->FixWireTool()->ModifyTopologyMode() = Standard_True;
  //sfs.FixWireTool().FixReorderMode() = Standard_False;
  sfs->FixWireTool()->FixConnectedMode() = Standard_False;
  sfs->FixWireTool()->FixEdgeCurvesMode() = Standard_False;
  sfs->FixWireTool()->FixDegeneratedMode() = Standard_False;
  sfs->FixWireTool()->FixSelfIntersectionMode() = Standard_False; 
  sfs->FixWireTool()->FixLackingMode() = Standard_False; 
  sfs->FixWireTool()->FixSmallMode() = Standard_True;
  sfs->Perform();
  TopoDS_Shape result = sfs->Shape();
  context = sfs->Context();
  return result;
}


//=======================================================================
//function : ReplaceVertex
//purpose  : auxilary for FixVertexPosition
//=======================================================================
static TopoDS_Edge ReplaceVertex(const TopoDS_Edge& theEdge,
                                 const gp_Pnt theP,
                                 const Standard_Boolean theFwd)
{
  TopoDS_Vertex aNewVertex;
  BRep_Builder aB;
  aB.MakeVertex(aNewVertex,theP,Precision::Confusion());
  TopoDS_Vertex aV1,aV2;
  if(theFwd) {
    aV1 = aNewVertex;
    aV1.Orientation( TopAbs_FORWARD);
  }
  else {
    aV2 = aNewVertex;
    aV2.Orientation( TopAbs_REVERSED);
  }
  ShapeBuild_Edge aSbe;
  TopoDS_Edge e1 = theEdge;
  TopAbs_Orientation Ori = e1.Orientation();
  e1.Orientation(TopAbs_FORWARD);
  TopoDS_Edge aNewEdge = aSbe.CopyReplaceVertices(e1,aV1,aV2);
  aNewEdge.Orientation(Ori);
  return aNewEdge;
}


//=======================================================================
//function : getNearPoint
//purpose  : auxilary for FixVertexPosition
//=======================================================================
static Standard_Real getNearPoint(const TColgp_SequenceOfPnt& aSeq1,
                                  const TColgp_SequenceOfPnt& aSeq2,
                                  gp_XYZ& acent)
{
  Standard_Integer i =1;
  Standard_Integer ind1 =0,ind2 =0;
  Standard_Real mindist =RealLast();
  for( ; i <= aSeq1.Length(); i++) {
    gp_Pnt p1 = aSeq1.Value(i);
    Standard_Integer j=1;
    for( ; j <= aSeq2.Length(); j++) {
      gp_Pnt p2 = aSeq2.Value(j);
      Standard_Real d = p1.Distance(p2);
      if(fabs(d -mindist ) <= Precision::Confusion())
        continue;
      if(d < mindist) {
        mindist = d;
        ind1 =i;
        ind2 = j;
      }
      
    }
  }
  if(ind1 && ind2)
    acent = (aSeq1.Value(ind1).XYZ() + aSeq2.Value(ind2).XYZ())/2.0;
  return mindist;
}


//=======================================================================
//function : getNearestEdges
//purpose  : auxilary for FixVertexPosition
//=======================================================================
static Standard_Boolean getNearestEdges(TopTools_ListOfShape& theLEdges,
                                        const TopoDS_Vertex theVert,
                                        TopTools_SequenceOfShape& theSuitEdges,
                                        TopTools_SequenceOfShape& theRejectEdges,
                                        const Standard_Real theTolerance,
                                        gp_XYZ& thecentersuit,
                                        gp_XYZ& thecenterreject)
{
  if(theLEdges.IsEmpty())
    return Standard_False;
  TopTools_MapOfShape aMapEdges;
  
  TopTools_ListOfShape atempList;
  atempList= theLEdges;
  TopTools_ListIteratorOfListOfShape alIter(atempList);
  
  TopoDS_Edge aEdge1 = TopoDS::Edge(alIter.Value());
  TopoDS_Vertex aVert11,aVert12;
  TopExp::Vertices(aEdge1, aVert11,aVert12 );
  aMapEdges.Add(aEdge1);
  Standard_Real aFirst1,aLast1;
  Handle(Geom_Curve) aCurve1 = BRep_Tool::Curve(aEdge1,aFirst1,aLast1);
  gp_Pnt p11;
  gp_Pnt p12;
  Standard_Boolean isFirst1 = theVert.IsSame(aVert11);
  Standard_Boolean isSame1 = aVert11.IsSame(aVert12);
  if( !aCurve1.IsNull()) {
    if(isFirst1)
      p11 = aCurve1->Value(aFirst1);
    else if(!isSame1)
      p11 = aCurve1->Value(aLast1);
    if(isSame1)
      p12 = aCurve1->Value(aLast1);
  }
  else return Standard_False;
  alIter.Next();
  TopTools_SequenceOfShape aseqreject;
  TopTools_SequenceOfShape aseqsuit;
  
  Standard_Integer anumLoop =0;
  for( ; alIter.More(); ) {
    TopoDS_Edge aEdge = TopoDS::Edge(alIter.Value());
    if( aMapEdges.Contains(aEdge)) {
      atempList.Remove(alIter);
      continue;
    }
    
    TopoDS_Vertex aVert1,aVert2;
    TopExp::Vertices(aEdge, aVert1,aVert2 );
    Standard_Real isFirst = theVert.IsSame(aVert1);
    Standard_Boolean isSame = aVert1.IsSame(aVert2);
    
    Standard_Boolean isLoop = ((aVert1.IsSame(aVert11) && aVert2.IsSame(aVert12)) ||
                               (aVert1.IsSame(aVert12) && aVert2.IsSame(aVert11)));
    if(isLoop /*&& !aseqsuit.Length()*/ && (atempList.Extent() >anumLoop)) {
      atempList.Append(aEdge);
      atempList.Remove(alIter);
      anumLoop++;
      continue;
    }
    aMapEdges.Add(aEdge);
    Standard_Real aFirst,aLast;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(aEdge,aFirst,aLast);
    if( !aCurve.IsNull()) {
      gp_Pnt p1;
      gp_Pnt p2;
      if(isFirst)
        p1 = aCurve->Value(aFirst);
      else
        p1 = aCurve->Value(aLast);
      if(isSame)
        p2 = aCurve->Value(aLast);
      Standard_Real aMinDist = RealLast();
      gp_XYZ acent;
      if(!isSame && ! isSame1) {
        aMinDist =  p1.Distance(p11);
        acent = (p1.XYZ() + p11.XYZ())/2.0;
      }
      else {
        TColgp_SequenceOfPnt aSeq1;
        TColgp_SequenceOfPnt aSeq2;
        aSeq1.Append(p11);
        if(isSame1)
          aSeq1.Append(p12);
        aSeq2.Append(p1);
        if(isSame)
          aSeq2.Append(p2);
        aMinDist = getNearPoint(aSeq1,aSeq2,acent);
      }
      
      if(aMinDist > theTolerance) {
        if(!aseqreject.Length()) 
          thecenterreject = acent;
        aseqreject.Append(aEdge);
      }
      else {
        if(!aseqsuit.Length()) {
          thecentersuit = acent;
          aseqsuit.Append(aEdge);
        }
        else if(!isSame1)
          aseqsuit.Append(aEdge);
        else if((thecentersuit - acent).Modulus() < theTolerance)
          aseqsuit.Append(aEdge);
        else
          aseqreject.Append(aEdge);
      }
      
    }
    atempList.Remove(alIter);
  }

  Standard_Boolean isDone = (!aseqsuit.IsEmpty() || !aseqreject.IsEmpty());
  if(isDone) {
    if(aseqsuit.IsEmpty()) {
      theRejectEdges.Append(aEdge1);
      theLEdges.RemoveFirst();
      
      getNearestEdges(theLEdges,theVert,theSuitEdges,theRejectEdges,
                      theTolerance,thecentersuit,thecenterreject);
    }
    else {
      theSuitEdges.Append(aEdge1);
      theSuitEdges.Append(aseqsuit);
      theRejectEdges.Append(aseqreject);
    }
  }
  else
    theRejectEdges.Append(aEdge1);
  
  return isDone;
}


//=======================================================================
//function : FixVertexPosition
//purpose  : 
//=======================================================================

Standard_Boolean ShapeFix::FixVertexPosition(TopoDS_Shape& theshape, 
                                             const Standard_Real theTolerance,
                                             const Handle(ShapeBuild_ReShape)& thecontext)
{
  TopTools_IndexedDataMapOfShapeListOfShape aMapVertEdges;
  TopExp_Explorer aExp1(theshape,TopAbs_EDGE);
  for( ; aExp1.More(); aExp1.Next()) {
    TopoDS_Shape aVert1;
    Standard_Integer nV =1;
    TopoDS_Iterator aExp3(aExp1.Current());
    for( ; aExp3.More(); aExp3.Next(),nV++) {
      TopoDS_Shape aVert =  aExp3.Value();
      if(nV ==1)
        aVert1 = aVert;
      else if(aVert1.IsSame(aVert))
        continue;
      if(aMapVertEdges.Contains(aVert))
        aMapVertEdges.ChangeFromKey(aVert).Append(aExp1.Current());
      else {
        TopTools_ListOfShape alEdges;
        alEdges.Append(aExp1.Current());
        aMapVertEdges.Add(aVert,alEdges);
      }
    }
  }
  Standard_Boolean isDone = Standard_False;
  Standard_Integer i=1;
  for( ; i <= aMapVertEdges.Extent(); i++) {
    TopoDS_Vertex aVert = TopoDS::Vertex(aMapVertEdges.FindKey(i));
    Standard_Real aTolVert = BRep_Tool::Tolerance(aVert);
    if(aTolVert <= theTolerance)
      continue;
    
    BRep_Builder aB1;
    aB1.UpdateVertex(aVert,theTolerance);
    gp_Pnt aPvert = BRep_Tool::Pnt(aVert);
    gp_XYZ acenter(aPvert.XYZ()), acenterreject(aPvert.XYZ());
    
    TopTools_SequenceOfShape aSuitEdges;
    TopTools_SequenceOfShape aRejectEdges;
    TopTools_ListOfShape aledges;
    aledges= aMapVertEdges.FindFromIndex(i);
    if(aledges.Extent() ==1)
      continue;
    //if tolerance of vertex is more than specified tolerance 
    // check distance between curves and vertex
    
    if(!getNearestEdges(aledges,aVert,aSuitEdges,aRejectEdges,theTolerance,acenter,acenterreject))
      continue;

    //update vertex by nearest point
    Standard_Boolean isAdd = Standard_False;
    Standard_Integer k =1;
    for( ; k <= aSuitEdges.Length(); k++) {
    
      TopoDS_Edge aEdgeOld = TopoDS::Edge(aSuitEdges.Value(k));
      TopoDS_Vertex aVert1,aVert2;
      TopExp::Vertices(aEdgeOld, aVert1,aVert2 );
    
      Standard_Boolean isFirst = (aVert1.IsSame(aVert));
      Standard_Boolean isLast = (aVert2.IsSame(aVert));
      if(!isFirst && !isLast)
        continue;
      Standard_Real aFirst,aLast;
      Handle(Geom_Curve) aCurve;
      TopoDS_Edge aEdge = TopoDS::Edge(thecontext->Apply(aEdgeOld));

      TopoDS_Vertex aVert1n,aVert2n;
      TopExp::Vertices(aEdge, aVert1n,aVert2n );
      aCurve = BRep_Tool::Curve(aEdge,aFirst,aLast);
      if( !aCurve.IsNull()) {
        gp_Pnt p1 = aCurve->Value(aFirst);
        gp_Pnt p2 = aCurve->Value(aLast);
        
        //if distance between ends of curve more than specified tolerance
        //but vertices are the same that one of the vertex will be replaced.

        Standard_Boolean isReplace = (aVert1n.IsSame(aVert2n) && p1.Distance(p2) >theTolerance);
        
        //Standard_Real dd1 = (acenter - p1.XYZ()).Modulus();
        //Standard_Real dd2 = (acenter - p2.XYZ()).Modulus();
         if(isFirst) {
           if( k>2) {
             acenter += p1.XYZ();
             acenter /= 2.0;
           }
           if(isReplace) {
             TopoDS_Edge enew;
             if(p1.Distance(acenter) < p2.Distance(acenter))
               enew = ReplaceVertex(aEdge,p2,Standard_False);
             else
               enew = ReplaceVertex(aEdge,p1,Standard_True);
              thecontext->Replace(aEdge,enew);
              isDone = Standard_True;
            }
          }
          else {
            if( k>2) {
              acenter += p2.XYZ();
              acenter /= 2.0;
            }
            if(isReplace) {
              TopoDS_Edge enew;
             if(p1.Distance(acenter) < p2.Distance(acenter))
               enew = ReplaceVertex(aEdge,p2,Standard_False);
             else
               enew = ReplaceVertex(aEdge,p1,Standard_True);
              thecontext->Replace(aEdge,enew);
              isDone = Standard_True;
            }
          }
          
      
          
        isAdd = Standard_True;
       
      }
    }
    
  
    if(isAdd && aPvert.Distance(acenter) > theTolerance)
    {
      
      BRep_Builder aB;
     
      //  aB.UpdateVertex(aVert,Precision::Confusion());
      //else {
        isDone = Standard_True;
        TopoDS_Vertex aNewVertex;
        aB.MakeVertex(aNewVertex,acenter,Precision::Confusion());
        aNewVertex.Orientation(aVert.Orientation());
        thecontext->Replace(aVert,aNewVertex);
      
    }
    
    for( k =1; k <= aRejectEdges.Length(); k++) {
      TopoDS_Edge aEdgeOld = TopoDS::Edge( aRejectEdges.Value(k));
      TopoDS_Vertex aVert1,aVert2;
      TopExp::Vertices(aEdgeOld, aVert1,aVert2 );
    
      Standard_Boolean isFirst = (aVert1.IsSame(aVert));
      Standard_Boolean isLast = (aVert2.IsSame(aVert));
      if(!isFirst && !isLast)
        continue;
      Standard_Boolean isSame = aVert1.IsSame(aVert2);
      Handle(Geom_Curve) aCurve;
      TopoDS_Edge aEdge = TopoDS::Edge(thecontext->Apply(aEdgeOld));

      TopoDS_Vertex aVert1n,aVert2n;
      TopExp::Vertices(aEdge, aVert1n,aVert2n );
      
      Standard_Real aFirst,aLast;
      aCurve = BRep_Tool::Curve(aEdge,aFirst,aLast);
      if( !aCurve.IsNull()) {
        gp_Pnt p1 = aCurve->Value(aFirst);
        gp_Pnt p2 = aCurve->Value(aLast);
        TopoDS_Edge enew;
        if(isFirst) {
          enew = ReplaceVertex(aEdge,p1,Standard_True);
          if(isSame)
            enew = ReplaceVertex(enew,p2,Standard_False);
        }
        else {
          enew = ReplaceVertex(aEdge,p2,Standard_False);
          if(isSame)
            enew = ReplaceVertex(enew ,p1,Standard_True);
        }
        
        thecontext->Replace(aEdge,enew);
        isDone = Standard_True;
        
        
      }
      
    }
  }
  if(isDone)
    theshape = thecontext->Apply(theshape);
  return isDone;
}


//=======================================================================
//function : LeastEdgeSize
//purpose  : 
//=======================================================================

Standard_Real ShapeFix::LeastEdgeSize(TopoDS_Shape& theShape)
{
  Standard_Real aRes = RealLast();
  for(TopExp_Explorer exp(theShape,TopAbs_EDGE); exp.More(); exp.Next()) { 
    TopoDS_Edge edge = TopoDS::Edge ( exp.Current() );
    Standard_Real first,last;
    Handle(Geom_Curve) c3d = BRep_Tool::Curve(edge, first, last);
    if(!c3d.IsNull()) {
      Bnd_Box bb;
      bb.Add(c3d->Value(first));
      bb.Add(c3d->Value(last));
      bb.Add(c3d->Value((last+first)/2.));
      Standard_Real x1,x2,y1,y2,z1,z2,size;
      bb.Get(x1,y1,z1,x2,y2,z2);
      size = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1);
      if(size<aRes) aRes = size;
    }
  }
  aRes = sqrt(aRes);
  return aRes;
}
