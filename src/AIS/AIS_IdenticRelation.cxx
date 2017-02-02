// Created on: 1997-03-03
// Created by: Jean-Pierre COMBE
// Copyright (c) 1997-1999 Matra Datavision
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


#include <AIS.hxx>
#include <AIS_IdenticRelation.hxx>
#include <AIS_Shape.hxx>
#include <BRep_Tool.hxx>
#include <DsgPrs_IdenticPresentation.hxx>
#include <ElCLib.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Transformation.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Projector.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_ListIteratorOfListOfTransient.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_IdenticRelation,AIS_Relation)

// jfa 15/10/2000
static Standard_Real Modulo2PI(const Standard_Real ANGLE)
{
  if ( ANGLE < 0 )          return Modulo2PI(ANGLE + 2*M_PI);
  else if ( ANGLE >= 2*M_PI ) return Modulo2PI(ANGLE - 2*M_PI);
  return ANGLE;
}

static Standard_Boolean IsEqual2PI(const Standard_Real angle1,
				   const Standard_Real angle2, const Standard_Real precision)
{
  Standard_Real diff = Abs(angle1-angle2);
  if ( diff < precision )                return Standard_True;
  else if ( Abs(diff-2*M_PI) < precision ) return Standard_True;
  return Standard_False;
}
// jfa 15/10/2000 end

//=======================================================================
//function : AIS_Sort
//purpose  : sort an array of parameters <tab1> in increasing order
//           updates <tab2> and <tab3> according to <tab1>
//=======================================================================
static void AIS_Sort(Standard_Real tab1[4],
		     gp_Pnt tab2[4],
		     Standard_Integer tab3[4])
{
  Standard_Boolean found = Standard_True;
  Standard_Real cur; gp_Pnt cur1; Standard_Integer cur2;
  
  while (found) {
    found = Standard_False;
    for (Standard_Integer i=0; i< 3; i++) {
      if (tab1[i+1] < tab1[i]) {
	found = Standard_True;
	cur = tab1[i]; cur1 = tab2[i]; cur2 = tab3[i];
	tab1[i] = tab1[i+1]; tab2[i] = tab2[i+1]; tab3[i] = tab3[i+1]; 
	tab1[i+1] = cur; tab2[i+1] = cur1; tab3[i+1] = cur2; 
      }
    }
  }
}

//=======================================================================
//function : ConnectedEdges
//purpose  : 
//=======================================================================
static Standard_Boolean ConnectedEdges(const TopoDS_Wire& WIRE,
				       const TopoDS_Vertex& V, 
				       TopoDS_Edge& E1, 
				       TopoDS_Edge& E2)
{
  TopTools_IndexedDataMapOfShapeListOfShape  vertexMap;
  TopExp::MapShapesAndAncestors (WIRE,TopAbs_VERTEX,TopAbs_EDGE,vertexMap);
  
  Standard_Boolean found(Standard_False);
  TopoDS_Vertex theVertex;
  for (Standard_Integer i=1; i<=vertexMap.Extent() && !found; i++) {
    if (vertexMap.FindKey(i).IsSame(V)) {
      theVertex = TopoDS::Vertex(vertexMap.FindKey(i));
      found = Standard_True;
    }
  }
  if (!found) {
    E1.Nullify();
    E2.Nullify();
    return Standard_False;
  }
  
  TopTools_ListIteratorOfListOfShape iterator(vertexMap.FindFromKey(theVertex));
  if (iterator.More()) {
    E1 = TopoDS::Edge(iterator.Value());
    iterator.Next();
  }
  else {
    E1.Nullify();
    return Standard_False;
  }
  
  if (iterator.More()) {
    E2 = TopoDS::Edge(iterator.Value());
    iterator.Next();
  }
  else {
    E2.Nullify();
    return Standard_False;
  }
  
  if (iterator.More()) {
    E1.Nullify();
    E2.Nullify();
    return Standard_False;
  }
  return Standard_True;
}

// jfa 16/10/2000
//=======================================================================
//function : ComputeAttach
//purpose  : Compute a point on the arc of <thecirc>
//             between <aFAttach> and <aSAttach>
//             corresponding to <aPosition>
//           Returns result into <aPosition>
// Note    : This function is to be used only in the case of circles.
//           The <aPosition> parameter is in/out.
//=======================================================================
static Standard_Boolean ComputeAttach(const gp_Circ& thecirc,
				      const gp_Pnt& aFAttach,
				      const gp_Pnt& aSAttach,
				      gp_Pnt& aPosition)
{
  gp_Pnt curpos = aPosition;

  // Case of confusion between the current position and the center 
  // of the circle -> we move the current position
  Standard_Real confusion (Precision::Confusion());
  gp_Pnt aCenter = thecirc.Location();
  if ( aCenter.Distance(curpos) <= confusion )
    {
      gp_Vec vprec(aCenter, aFAttach);
      vprec.Normalize();
      curpos.Translate(vprec*1e-5);
    }
  
  Standard_Real pcurpos  = ElCLib::Parameter(thecirc,curpos);
  Standard_Real pFAttach = ElCLib::Parameter(thecirc,aFAttach);
  Standard_Real pSAttach = ElCLib::Parameter(thecirc,aSAttach);

  Standard_Real pSAttachM = pSAttach;
  Standard_Real deltap = pSAttachM - pFAttach;
  if ( deltap < 0 )
    {
      deltap += 2 * M_PI;
      pSAttachM += 2 * M_PI;
    }
  pSAttachM -= pFAttach;

  Standard_Real pmiddleout = pSAttachM/2.0 + M_PI;

  Standard_Real pcurpos1 = pcurpos;
  // define where curpos lays
  if ( pcurpos1 < pFAttach )
    {
      pcurpos1 = pcurpos1 + 2 * M_PI - pFAttach;
      if ( pcurpos1 > pSAttachM ) // out
	{
	  if ( pcurpos1 > pmiddleout ) pcurpos = pFAttach;
	  else pcurpos = pSAttach;
	}
    }
  else if ( pcurpos1 > (pFAttach + deltap) ) // out
    {
      pcurpos1 -= pFAttach;
      if ( pcurpos1 > pmiddleout ) pcurpos = pFAttach;
      else pcurpos = pSAttach;
    }

  aPosition = ElCLib::Value(pcurpos,thecirc);
  return Standard_True;
}

//=======================================================================
//function : ComputeAttach
//purpose  : Compute a point on the arc of ellipse <theEll>
//             between <aFAttach> and <aSAttach>
//             corresponding to <aPosition>
//           Returns result into <aPosition>
// Note    : This function is to be used only in the case of ellipses.
//           The <aPosition> parameter is in/out.
//=======================================================================
static Standard_Boolean ComputeAttach(const gp_Elips& theEll,
				      const gp_Pnt& aFAttach,
				      const gp_Pnt& aSAttach,
				      gp_Pnt& aPosition)
{
  gp_Pnt curpos = aPosition;

  // Case of confusion between the current position and the center 
  // of the circle -> we move the current position
  Standard_Real confusion (Precision::Confusion());
  gp_Pnt aCenter = theEll.Location();
  if ( aCenter.Distance(curpos) <= confusion )
    {
      gp_Vec vprec(aCenter, aFAttach);
      vprec.Normalize();
      curpos.Translate(vprec*1e-5);
    }
  
// for ellipses it's not good  Standard_Real pcurpos  = ElCLib::Parameter(theEll,curpos);
  Handle(Geom_Ellipse) theEllg = new Geom_Ellipse(theEll);
  GeomAPI_ProjectPointOnCurve aProj (curpos, theEllg);
  Standard_Real pcurpos  = aProj.LowerDistanceParameter();

  Standard_Real pFAttach = ElCLib::Parameter(theEll,aFAttach);
  Standard_Real pSAttach = ElCLib::Parameter(theEll,aSAttach);

  Standard_Real pSAttachM = pSAttach;
  Standard_Real deltap = pSAttachM - pFAttach;
  if ( deltap < 0 )
    {
      deltap += 2 * M_PI;
      pSAttachM += 2 * M_PI;
    }
  pSAttachM -= pFAttach;

  Standard_Real pmiddleout = pSAttachM/2.0 + M_PI;

  Standard_Real pcurpos1 = pcurpos;
  // define where curpos lays
  if ( pcurpos1 < pFAttach )
    {
      pcurpos1 = pcurpos1 + 2 * M_PI - pFAttach;
      if ( pcurpos1 > pSAttachM ) // out
	{
	  if ( pcurpos1 > pmiddleout ) pcurpos = pFAttach;
	  else pcurpos = pSAttach;
	}
    }
  else if ( pcurpos1 > (pFAttach + deltap) ) // out
    {
      pcurpos1 -= pFAttach;
      if ( pcurpos1 > pmiddleout ) pcurpos = pFAttach;
      else pcurpos = pSAttach;
    }

  aPosition = ElCLib::Value(pcurpos,theEll);
  return Standard_True;
}
// jfa 16/10/2000 end

//=======================================================================
//function : AIS_IdenticRelation
//purpose  : 
//=======================================================================
AIS_IdenticRelation::AIS_IdenticRelation(const TopoDS_Shape& FirstShape, 
					 const TopoDS_Shape& SecondShape, 
					 const Handle(Geom_Plane)& aPlane)
  :isCircle(Standard_False)
{
  myFShape = FirstShape;
  mySShape = SecondShape;
  myPlane  = aPlane;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void AIS_IdenticRelation::Compute(const Handle(PrsMgr_PresentationManager3d)&, 
				  const Handle(Prs3d_Presentation)& aprs, 
				  const Standard_Integer)
{
  switch ( myFShape.ShapeType() ) {
    
  case TopAbs_VERTEX:
    {
      switch ( mySShape.ShapeType() ) {
      case TopAbs_VERTEX:
	{
	  ComputeTwoVerticesPresentation(aprs);
	}
      break;
      case TopAbs_EDGE:
	{
	  ComputeOneEdgeOVertexPresentation(aprs);
	}
      break;
      default:
	break;
      }
    }
    break;
    
  case TopAbs_EDGE:
    {
      switch ( mySShape.ShapeType() ) {
      case TopAbs_VERTEX:
	{
	  ComputeOneEdgeOVertexPresentation(aprs);
	}
      break;
      case TopAbs_EDGE:
	{
	  ComputeTwoEdgesPresentation(aprs);	  
	}
      break;
      default:
	break;
      }
    }
  break;
  default: break;
  }
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void AIS_IdenticRelation::Compute(const Handle(Prs3d_Projector)& aProjector,
				  const Handle(Prs3d_Presentation)& aPresentation)
{
// throw Standard_NotImplemented("AIS_IdenticRelation::Compute(const Handle(Prs3d_Projector)&,const Handle(Prs3d_Presentation)&)");
 PrsMgr_PresentableObject::Compute( aProjector , aPresentation ) ;
}

void AIS_IdenticRelation::Compute(const Handle(Prs3d_Projector)& aProjector, const Handle(Geom_Transformation)& aTransformation, const Handle(Prs3d_Presentation)& aPresentation)
{
// throw Standard_NotImplemented("AIS_IdenticRelation::Compute(const Handle(Prs3d_Projector)&, const Handle(Geom_Transformation)&, const Handle(Prs3d_Presentation)&)");
 PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation ) ;
}

//=======================================================================
//function : ComputeSelection
//purpose  : function used to compute the selection associated to the 
//           "identic" presentation
// note    : if we are in the case of lines, we create a segment between
//           myFAttach and mySAttach. In the case of Circles, we create
//           an arc of circle between the sames points. We Add a segment
//           to link Position to its projection on the curve described
//           before.
//=======================================================================

void AIS_IdenticRelation::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection, 
					   const Standard_Integer)
{
  Handle(SelectMgr_EntityOwner) own = new SelectMgr_EntityOwner(this,7);

  Handle(Select3D_SensitiveSegment) seg;
  // attachement point of the segment linking position to the curve
  gp_Pnt attach; 
  Standard_Real confusion (Precision::Confusion());
    
  if ( myFAttach.IsEqual(mySAttach, confusion) )
    {
      attach = myFAttach;
    }
  else
    {    
// jfa 24/10/2000
      if ( myFShape.ShapeType() == TopAbs_EDGE )
	{
	  Handle(Geom_Curve) curv1,curv2;
	  gp_Pnt firstp1,lastp1,firstp2,lastp2;
	  Standard_Boolean isInfinite1,isInfinite2;
	  Handle(Geom_Curve) extCurv;
	  if ( !AIS::ComputeGeometry(TopoDS::Edge(myFShape),TopoDS::Edge(mySShape),
				     myExtShape,curv1,curv2,
				     firstp1,lastp1,firstp2,lastp2,
				     extCurv,isInfinite1,isInfinite2,myPlane) ) return;

	  if ( isCircle ) // case of Circles
	    {
	      Handle(Geom_Circle) thecirc = Handle(Geom_Circle)::DownCast (curv1);
	      Standard_Real udeb = ElCLib::Parameter(thecirc->Circ(),myFAttach);
	      Standard_Real ufin = ElCLib::Parameter(thecirc->Circ(),mySAttach);
	      Handle(Geom_Curve) thecu = new Geom_TrimmedCurve(thecirc,udeb,ufin);
      
	      Handle(Select3D_SensitiveCurve) scurv = new Select3D_SensitiveCurve(own, thecu);
	      aSelection->Add(scurv);
      
	      attach = myPosition;
	      ComputeAttach(thecirc->Circ(),myFAttach,mySAttach,attach);
	    }
	  else if ( curv1->IsInstance(STANDARD_TYPE(Geom_Ellipse)) ) // case of ellipses
	    {
	      Handle(Geom_Ellipse) theEll = Handle(Geom_Ellipse)::DownCast (curv1);

	      Standard_Real udeb = ElCLib::Parameter(theEll->Elips(),myFAttach);
	      Standard_Real ufin = ElCLib::Parameter(theEll->Elips(),mySAttach);
	      Handle(Geom_Curve) thecu = new Geom_TrimmedCurve(theEll,udeb,ufin);
      
	      Handle(Select3D_SensitiveCurve) scurv = new Select3D_SensitiveCurve(own, thecu);
	      aSelection->Add(scurv);
      
	      attach = myPosition;
	      ComputeAttach(theEll->Elips(),myFAttach,mySAttach,attach);
	    }
	  else if ( curv1->IsInstance(STANDARD_TYPE(Geom_Line)) ) // case of Lines
	    {
	      seg = new Select3D_SensitiveSegment(own, myFAttach, mySAttach);
	      aSelection->Add(seg);
	      
	      //attach = projection of Position() on the curve;
	      gp_Vec v1 (myFAttach, mySAttach);
	      gp_Vec v2 (myFAttach, myPosition);
	      if ( v1.IsParallel(v2, Precision::Angular()) )
		{
		  attach = mySAttach;
		}
	      else
		{
		  gp_Lin ll (myFAttach, gp_Dir(v1));
		  attach = ElCLib::Value(ElCLib::Parameter(ll,myPosition), ll);
		}
	    }
	  else return;
	}
//      else if ( myFShape.ShapeType() == TopAbs_VERTEX )
//	{
//	}
// jfa 24/10/2000 end
    }

  // Creation of the segment linking the attachement point with the
  // position
  if ( !attach.IsEqual(myPosition, confusion) )
    {
      seg = new Select3D_SensitiveSegment(own, attach, myPosition);
      aSelection->Add(seg);
    }
}

//=======================================================================
//function : ComputeTwoEdgesPresentation
//purpose  : 
//=======================================================================
void AIS_IdenticRelation::ComputeTwoEdgesPresentation(const Handle(Prs3d_Presentation)& aPrs)
{
  Handle(Geom_Curve) curv1,curv2;
  gp_Pnt firstp1,lastp1,firstp2,lastp2;
  Standard_Boolean isInfinite1,isInfinite2;

  Handle(Geom_Curve) extCurv;
  if (!AIS::ComputeGeometry(TopoDS::Edge(myFShape),
			    TopoDS::Edge(mySShape),
			    myExtShape,
			    curv1,
			    curv2,
			    firstp1,
			    lastp1,
			    firstp2,
			    lastp2,
			    extCurv,
			    isInfinite1,isInfinite2,
			    myPlane))
    return;
  aPrs->SetInfiniteState((isInfinite1 || isInfinite2) && myExtShape != 0);

  // Treatement of the case of lines
  if ( curv1->IsInstance(STANDARD_TYPE(Geom_Line)) && curv2->IsInstance(STANDARD_TYPE(Geom_Line)) ) {
    // we take the line curv1 like support
    Handle(Geom_Line) thelin;
    if (isInfinite1 && !isInfinite2) thelin = Handle(Geom_Line)::DownCast (curv2);
    else if (!isInfinite1 && isInfinite2) thelin = Handle(Geom_Line)::DownCast (curv1);
    else thelin = Handle(Geom_Line)::DownCast (curv1);
    ComputeTwoLinesPresentation(aPrs, thelin, firstp1, lastp1, firstp2, lastp2, isInfinite1, isInfinite2);
  }

  //  Treatement of the case of circles
  else if ( curv1->IsInstance(STANDARD_TYPE(Geom_Circle)) && curv2->IsInstance(STANDARD_TYPE(Geom_Circle)) ) {
    //gp_Pnt curpos;
    isCircle = Standard_True; // useful for ComputeSelection
    Handle(Geom_Circle) thecirc (Handle(Geom_Circle)::DownCast (curv1));
    ComputeTwoCirclesPresentation(aPrs, thecirc, firstp1, lastp1, firstp2, lastp2);
  }

  // jfa 10/10/2000
  //  Treatement of the case of ellipses
  else if ( curv1->IsInstance(STANDARD_TYPE(Geom_Ellipse)) && curv2->IsInstance(STANDARD_TYPE(Geom_Ellipse)) )
      {
	Handle(Geom_Ellipse) theEll (Handle(Geom_Ellipse)::DownCast (curv1));
	ComputeTwoEllipsesPresentation(aPrs, theEll, firstp1, lastp1, firstp2, lastp2);
      }
  // jfa 10/10/2000 end
  else
    return;

 // Calculate presentation of projected edges
 if ( (myExtShape != 0) &&  !extCurv.IsNull()) {
   if (myExtShape == 1 )
     ComputeProjEdgePresentation(aPrs, TopoDS::Edge(myFShape), curv1, firstp1, lastp1);
   else
     ComputeProjEdgePresentation(aPrs, TopoDS::Edge(mySShape), curv2, firstp2, lastp2);
 }
}

//=======================================================================
//function : ComputeTwoLinesPresentation
//purpose  : Compute the presentation of the 'identic' constraint
//           between two lines ( which are equal)
//input    : <thelin> : the 
//           <firstp1>: first extremity of the 1st curve of the constraint
//           <lastp1> : last extremity of the 1st curve of the constraint
//           <firstp2>: first extremity of the 2nd curve of the constraint
//           <lastp2> :last extremity of the 2nd curve of the constraint
//=======================================================================
void AIS_IdenticRelation::ComputeTwoLinesPresentation(const Handle(Prs3d_Presentation)& aPrs, 
						      const Handle(Geom_Line)& thelin,
						      gp_Pnt& firstp1,
						      gp_Pnt& lastp1,
						      gp_Pnt& firstp2,
						      gp_Pnt& lastp2,
						      const Standard_Boolean isInfinite1,
						      const Standard_Boolean isInfinite2)
{
  if (isInfinite1 && isInfinite2) {
    if ( myAutomaticPosition ) {
      myFAttach = mySAttach = thelin->Lin().Location();
      gp_Pnt curpos;
      gp_Pln pln(myPlane->Pln());
      gp_Dir dir(pln.XAxis().Direction());
      gp_Vec transvec = gp_Vec(dir)*myArrowSize;
      curpos = myFAttach.Translated(transvec);;
      myPosition = curpos;
      myAutomaticPosition = Standard_True;
    }
    else {
      myFAttach = mySAttach = ElCLib::Value(ElCLib::Parameter(thelin->Lin(),myPosition),thelin->Lin());            
    }
    TCollection_ExtendedString vals(" ==");
    DsgPrs_IdenticPresentation::Add(aPrs,
				    myDrawer,
				    vals,
				    myFAttach,
				    myPosition);    
  }
  else {
    // Computation of the parameters of the 4 points on the line <thelin>
    Standard_Real pf1, pf2, pl1, pl2;    

    pf1 = ElCLib::Parameter(thelin->Lin(), firstp1);
    pl1 = ElCLib::Parameter(thelin->Lin(), lastp1);

    pf2 = ElCLib::Parameter(thelin->Lin(), firstp2);
    pl2 = ElCLib::Parameter(thelin->Lin(), lastp2);

    if (isInfinite1) {
      pf1 = pf2;
      pl1 = pl2;
      firstp1 = firstp2;
      lastp1 = lastp2;
    }
    else if (isInfinite2) {
      pf2 = pf1;
      pl2 = pl1;
      firstp2 = firstp1;
      lastp2 = lastp1;
    }

    Standard_Real tabRang1[4];      // array taht contains the parameters of the 4 points
    // ordered by increasing abscisses.

    gp_Pnt tabRang2[4];             // array containing the points corresponding to the
    // parameters in tabRang1

    Standard_Integer tabRang3[4];   // array containing the number of the curve( 1 or 2)
    // of which belongs each point of tabRang2

    // Filling of the arrays
    tabRang1[0] = pf1; tabRang2[0] = firstp1; tabRang3[0] = 1;
    tabRang1[1] = pf2; tabRang2[1] = firstp2; tabRang3[1] = 2;
    tabRang1[2] = pl1; tabRang2[2] = lastp1;  tabRang3[2] = 1;
    tabRang1[3] = pl2; tabRang2[3] = lastp2;  tabRang3[3] = 2;

  // Sort of the array of parameters (tabRang1)
    AIS_Sort(tabRang1, tabRang2, tabRang3);

    // Computation of myFAttach and mySAttach according to the
    // position of the 2 linear edges
    gp_Pnt curpos;
    gp_Pnt middle;
  
    if ( (tabRang1[0] == tabRang1[1]) && (tabRang1[2] == tabRang1[3]) ) {
      middle.SetXYZ((tabRang2[1].XYZ() + tabRang2[2].XYZ())/2. );
      Standard_Real pmiddle = (tabRang1[1] + tabRang1[2]) / 2.;
      Standard_Real delta = (tabRang1[3] - tabRang1[0])/ 5.;
      myFAttach = ElCLib::Value(pmiddle-delta, thelin->Lin());
      mySAttach = ElCLib::Value(pmiddle+delta, thelin->Lin());
    }
  
    else if ( tabRang1[1] == tabRang1[2] ) {
      middle = tabRang2[1];
      Standard_Real delta1 = tabRang1[1] - tabRang1[0];
      Standard_Real delta2 = tabRang1[3] - tabRang1[2];
      if ( delta1 > delta2 ) delta1 = delta2;
      myFAttach = ElCLib::Value(tabRang1[1]-delta1/2., thelin->Lin());
      mySAttach = ElCLib::Value(tabRang1[1]+delta1/2., thelin->Lin());
    }
  
    // Case of 2 disconnected segments -> the symbol completes the gap 
    //                                    between the 2 edges
    //--------------------------------
    else if ( (tabRang3[0] == tabRang3[1]) && (tabRang1[1] != tabRang1[2])) {
      middle.SetXYZ((tabRang2[1].XYZ() + tabRang2[2].XYZ())/2. );
      myFAttach = tabRang2[1];
      mySAttach = tabRang2[2];
    }
    else if ( (tabRang3[0] != tabRang3[1]) 
	      && (tabRang3[1] != tabRang3[2])    // Intersection
	      && (tabRang1[1] != tabRang1[2]) ) { 
      middle.SetXYZ((tabRang2[1].XYZ() + tabRang2[2].XYZ())/2. );
      myFAttach = tabRang2[1];
      mySAttach = tabRang2[2];
    }
    else {                                     // Inclusion
      myFAttach.SetXYZ((tabRang2[0].XYZ() + tabRang2[1].XYZ())/2. );
      mySAttach.SetXYZ((tabRang2[1].XYZ() + tabRang2[2].XYZ())/2. );
      middle.SetXYZ( (myFAttach.XYZ() + mySAttach.XYZ() )/2.);
    }
  

    if ( myAutomaticPosition ) {
    
      gp_Vec vtrans(myFAttach, mySAttach);
      vtrans.Normalize();
      vtrans.Cross(gp_Vec(myPlane->Pln().Axis().Direction()));
      vtrans *= ComputeSegSize();
      curpos = middle.Translated(vtrans);
      myPosition = curpos;
      myAutomaticPosition = Standard_True;
    }

    else {

      curpos = myPosition;
      Standard_Real pcurpos = ElCLib::Parameter(thelin->Lin() ,curpos);
      Standard_Real dist = thelin->Lin().Distance(curpos);
      gp_Pnt proj = ElCLib::Value( pcurpos, thelin->Lin());
      gp_Vec  trans;
      Standard_Real confusion(Precision::Confusion());
      if ( dist >= confusion ) {
	trans = gp_Vec(proj, curpos);
	trans.Normalize();
      }
      Standard_Real pf = ElCLib::Parameter(thelin->Lin() ,myFAttach);
      Standard_Real pl = ElCLib::Parameter(thelin->Lin() ,mySAttach);
      if ( pcurpos <= pf ) {
	pcurpos = pf + 1e-5;
	curpos = ElCLib::Value( pcurpos, thelin->Lin());
	if ( dist >= confusion ) curpos.Translate(trans*dist);
      }
      else if ( pcurpos >= pl ) {
	pcurpos = pl - 1e-5;
	curpos = ElCLib::Value( pcurpos, thelin->Lin());
	if ( dist >= confusion ) curpos.Translate(trans*dist);
      }
      SetPosition(curpos);
    }

    // Display of the presentation
    TCollection_ExtendedString vals(" ==");
    DsgPrs_IdenticPresentation::Add(aPrs,
				    myDrawer,
				    vals,
				    myFAttach,
				    mySAttach,
				    curpos);
  }
}

// jfa 17/10/2000
//=======================================================================
//function : ComputeTwoCirclesPresentation
//purpose  : Compute the presentation of the 'identic' constraint
//           between two circles ( which are equal)
//input    : <thecirc>: the circle
//           <firstp1>: first extremity of the 1st curve of the constraint
//           <lastp1> : last extremity of the 1st curve of the constraint
//           <firstp2>: first extremity of the 2nd curve of the constraint
//           <lastp2> :last extremity of the 2nd curve of the constraint
//=======================================================================
void AIS_IdenticRelation::ComputeTwoCirclesPresentation(const Handle(Prs3d_Presentation)& aPrs,
							const Handle(Geom_Circle)& thecirc,
							const gp_Pnt& firstp1,
							const gp_Pnt& lastp1,
							const gp_Pnt& firstp2,
							const gp_Pnt& lastp2)
{
  Standard_Real confusion (Precision::Confusion());

  // Searching of complete circles
  Standard_Boolean circ1complete = (firstp1.IsEqual(lastp1, confusion));
  Standard_Boolean circ2complete = (firstp2.IsEqual(lastp2, confusion));
    
  myCenter = thecirc->Location();
  Standard_Real aSegSize = thecirc->Radius()/5.0;
  Standard_Real rad = M_PI / 5.0;
    
  // I. Case of 2 complete circles
  if ( circ1complete && circ2complete )
    {
      if (myAutomaticPosition)
	{
	  Standard_Real pfirst1 = ElCLib::Parameter(thecirc->Circ(), firstp1);
	  myFAttach = ElCLib::Value(Modulo2PI(pfirst1-rad), thecirc->Circ());
	  mySAttach = ElCLib::Value(Modulo2PI(pfirst1+rad), thecirc->Circ());

	  gp_Pnt curpos = ElCLib::Value(pfirst1,thecirc->Circ());
	  gp_Vec vtrans(myCenter, curpos);
	  vtrans.Normalize();
	  vtrans *= aSegSize;
	  curpos.Translate(vtrans);
	  myPosition = curpos;
	}
      else ComputeNotAutoCircPresentation(thecirc);
    }

  // II. Case of one complete circle and one arc
  else if ( (circ1complete && !circ2complete) || (!circ1complete && circ2complete) )
    {
      gp_Pnt firstp, lastp;
      if ( circ1complete && !circ2complete)
	{
	  firstp = firstp2;
	  lastp  = lastp2;
	}
      else
	{
	  firstp = firstp1;
	  lastp  = lastp1;
	}

      if (myAutomaticPosition)
	{
	  ComputeAutoArcPresentation(thecirc, firstp, lastp);
	}
      else
	{
	  ComputeNotAutoArcPresentation(thecirc, firstp, lastp);
	}
    }

  // III and IV. Case of two arcs
  else if ( !circ1complete && !circ2complete )
    {
      // We project all the points on the circle
      Standard_Real pf1, pf2, pl1, pl2;
      pf1 = ElCLib::Parameter(thecirc->Circ(), firstp1);
      pf2 = ElCLib::Parameter(thecirc->Circ(), firstp2);
      pl1 = ElCLib::Parameter(thecirc->Circ(), lastp1);
      pl2 = ElCLib::Parameter(thecirc->Circ(), lastp2);

      // III. Arcs with common ends
      // III.1. First of one and last of another
      if ( IsEqual2PI(pl1,pf2,confusion) || IsEqual2PI(pf1,pl2,confusion) )
	{
	  gp_Pnt curpos(0.,0.,0.);
	  Standard_Real att=0.;
	  if ( IsEqual2PI(pl1,pf2,confusion) )
	    {
	      att = pl1;
	      curpos = lastp1;
	    }
	  else if ( IsEqual2PI(pf1,pl2,confusion) )
	    {
	      att = pf1;
	      curpos = firstp1;
	    }
	  Standard_Real maxrad = Min(Modulo2PI(pl1 - pf1),Modulo2PI(pl2 - pf2))*3/4;
	  if ( rad > maxrad ) rad = maxrad;
	  Standard_Real pFAttach = Modulo2PI(att - rad);
	  Standard_Real pSAttach = Modulo2PI(att + rad);
	  myFAttach = ElCLib::Value(pFAttach, thecirc->Circ());
	  mySAttach = ElCLib::Value(pSAttach, thecirc->Circ());
	  if ( myAutomaticPosition )
	    {
	      gp_Vec vtrans(myCenter,curpos);
	      vtrans.Normalize();
	      vtrans *= aSegSize;
	      curpos.Translate(vtrans);
	      myPosition = curpos;
	    }
	}
      // III.2. Two first or two last
      else if ( IsEqual2PI(pf1,pf2,confusion) || IsEqual2PI(pl1,pl2,confusion) )
	{
	  Standard_Real l1 = Modulo2PI(pl1 - pf1);
	  Standard_Real l2 = Modulo2PI(pl2 - pf2);
	  gp_Pnt firstp,lastp;
	  if ( l1 < l2 )
	    {
	      firstp = firstp1;
	      lastp = lastp1;
	    }
	  else
	    {
	      firstp = firstp2;
	      lastp = lastp2;
	    }

	  if ( myAutomaticPosition )
	    {
	      ComputeAutoArcPresentation(thecirc, firstp, lastp);
	    }
	  else
	    {
	      ComputeNotAutoArcPresentation(thecirc, firstp, lastp);
	    }
	}
      // IV. All others arcs (without common ends)
      else
	{
	  // order the parameters; first will be pf1
	  Standard_Real pl1m = Modulo2PI(pl1 - pf1);
	  Standard_Real pf2m = Modulo2PI(pf2 - pf1);
	  Standard_Real pl2m = Modulo2PI(pl2 - pf1);

	  Standard_Boolean case1 = Standard_False;
	  // 1 - not intersecting arcs
	  // 2 - intersecting arcs, but one doesn't contain another
	  // 3a - first arc contains the second one
	  // 3b - second arc contains the first one
	  // 4 - two intersections

	  gp_Pnt firstp, lastp;

	  if ( pl1m < pf2m ) // 1 or 2b or 3b
	    {
	      if ( pl1m < pl2m ) // 1 or 3b
		{
		  if ( pl2m < pf2m ) // 3b
		    {
		      firstp = firstp1;
		      lastp  = lastp1;
		    }
		  else // 1
		    {
		      case1 = Standard_True;
		      Standard_Real deltap1 = Modulo2PI(pf1 - pl2);
		      Standard_Real deltap2 = Modulo2PI(pf2 - pl1);
		      if ( ((deltap1 < deltap2) && (deltap1 < 2*rad)) ||
			   ((deltap2 < deltap1) && (deltap2 > 2*rad)) ) // deltap2
			{
			  firstp = lastp1;
			  lastp  = firstp2;
			}
		      else // deltap1
			{
			  firstp = lastp2;
			  lastp  = firstp1;
			}
		    }
		}
	      else // 2b
		{
		  firstp = firstp1;
		  lastp  = lastp2;
		}
	    }
	  else // 2a or 3a or 4
	    {
	      if ( pl1m < pl2m ) // 2a
		{
		  firstp = firstp2;
		  lastp  = lastp1;
		}
	      else // 3a or 4
		{
		  if ( pl2m > pf2m ) // 3a
		    {
		      firstp = firstp2;
		      lastp  = lastp2;
		    }
		  else // 4
		    {
		      Standard_Real deltap1 = Modulo2PI(pl1 - pf2);
		      Standard_Real deltap2 = Modulo2PI(pl2 - pf1);
		      if ( ((deltap1 < deltap2) && (deltap1 < 2*rad)) ||
			   ((deltap2 < deltap1) && (deltap2 > 2*rad)) ) // deltap2
			{
			  firstp = firstp1;
			  lastp  = lastp2;
			}
		      else // deltap1
			{
			  firstp = firstp2;
			  lastp  = lastp1;
			}
		    }
		}
	    }

	  if ( myAutomaticPosition )
	    {
	      ComputeAutoArcPresentation(thecirc,firstp,lastp,case1);
	    }
	  else
	    {
	      if ( case1 )
		{
		  myFAttach = firstp;
		  mySAttach = lastp;
		}
	      else ComputeNotAutoArcPresentation(thecirc, firstp, lastp);
	    }
	}
    }

  // Display of the presentation
  TCollection_ExtendedString vals(" ==");
  gp_Pnt attach = myPosition;
  ComputeAttach(thecirc->Circ(),myFAttach,mySAttach,attach);
  DsgPrs_IdenticPresentation::Add(aPrs,
				  myDrawer,
				  vals,
				  myPlane->Pln().Position().Ax2(),
				  myCenter,
				  myFAttach,
				  mySAttach,
				  myPosition,
				  attach);
}

//=======================================================================
//function : ComputeAutoArcPresentation
//purpose  : Compute the presentation of the constraint where we are
//           not in the case of dragging.
//=======================================================================
void AIS_IdenticRelation::ComputeAutoArcPresentation(const Handle(Geom_Circle)& thecirc,
						     const gp_Pnt& firstp,
						     const gp_Pnt& lastp,
						     const Standard_Boolean isstatic)
{
  Standard_Real aSegSize = thecirc->Radius()/5.0;
  Standard_Real rad = M_PI / 5.0;

  Standard_Real pFA = ElCLib::Parameter(thecirc->Circ(),firstp);
  Standard_Real pSA = ElCLib::Parameter(thecirc->Circ(),lastp);
  Standard_Real maxrad = Modulo2PI(pSA - pFA)/2.0;
  
  if ( (rad > maxrad) || isstatic ) rad = maxrad;
  Standard_Real pmiddle = Modulo2PI(pFA + Modulo2PI(pSA - pFA)/2.0);
  
  myFAttach = ElCLib::Value(Modulo2PI(pmiddle - rad),thecirc->Circ());
  mySAttach = ElCLib::Value(Modulo2PI(pmiddle + rad),thecirc->Circ());
  
  gp_Pnt curpos = ElCLib::Value(pmiddle,thecirc->Circ());
  gp_Vec vtrans(myCenter, curpos);
  vtrans.Normalize();
  vtrans *= aSegSize;
  myPosition = curpos.Translated(vtrans);
}

//=======================================================================
//function : ComputeNotAutoCircPresentation
//purpose  : Compute the presentation of the constraint where we are
//           in the case of dragging.
// Note    : This function is to be used only in the case of full circles.
//           The symbol of the constraint moves together with arc
//           representing the constraint around all the circle.
//=======================================================================
void AIS_IdenticRelation::ComputeNotAutoCircPresentation(const Handle(Geom_Circle)& thecirc)
{
  gp_Pnt curpos = myPosition;

  Handle(Geom_Circle) cirNotAuto = new Geom_Circle(thecirc->Circ());
  
  // Case of confusion between the current position and the center 
  // of the circle -> we move the current position
  Standard_Real confusion (Precision::Confusion());
  if ( myCenter.Distance(curpos) <= confusion )
    {
      gp_Vec vprec(myCenter, myFAttach);
      vprec.Normalize();
      curpos.Translate(vprec*1e-5);
    }
  
  Standard_Real rad = M_PI / 5.0;
  Standard_Real pcurpos = ElCLib::Parameter(cirNotAuto->Circ(),curpos);
  Standard_Real pFAttach = pcurpos - rad;
  Standard_Real pSAttach = pcurpos + rad;
  myFAttach = ElCLib::Value(pFAttach,cirNotAuto->Circ());
  mySAttach = ElCLib::Value(pSAttach,cirNotAuto->Circ());
}

//=======================================================================
//function : ComputeNotAutoArcPresentation
//purpose  : Compute the presentation of the constraint where we are
//           in the case of dragging.
// Note    : This function is to be used only in the case of circles.
//           The symbol of the constraint moves only between myFAttach
//           and mySAttach.
//=======================================================================
void AIS_IdenticRelation::ComputeNotAutoArcPresentation(const Handle(Geom_Circle)& thecirc,
							const gp_Pnt& pntfirst,
							const gp_Pnt& pntlast)
{
  gp_Pnt curpos = myPosition;

  gp_Circ cirNotAuto = thecirc->Circ();

  Standard_Real pFPnt = ElCLib::Parameter(cirNotAuto, pntfirst);
  Standard_Real pSPnt = ElCLib::Parameter(cirNotAuto, pntlast);
  Standard_Real deltap = Modulo2PI(pSPnt - pFPnt)/2.0;

  Standard_Real rad = M_PI / 5;
  if ( deltap < rad )
    {
      myFAttach = pntfirst;
      mySAttach = pntlast;
    }
  else
    {
      gp_Pnt aFPnt = ElCLib::Value(Modulo2PI(pFPnt + rad), cirNotAuto);
      gp_Pnt aSPnt = ElCLib::Value(Modulo2PI(pSPnt - rad), cirNotAuto);

      ComputeAttach(cirNotAuto,aFPnt,aSPnt,curpos);

      Standard_Real pcurpos = ElCLib::Parameter(cirNotAuto,curpos);
      myFAttach = ElCLib::Value(pcurpos - rad, cirNotAuto);
      mySAttach = ElCLib::Value(pcurpos + rad, cirNotAuto);
    }
}
// jfa 17/10/2000 end

// jfa 18/10/2000
//=======================================================================
//function : ComputeTwoEllipsesPresentation
//purpose  : Compute the presentation of the 'identic' constraint
//           between two ellipses (which are equal)
//input    : <theEll>: the ellipse
//           <firstp1>: first extremity of the 1st curve of the constraint
//           <lastp1> : last extremity of the 1st curve of the constraint
//           <firstp2>: first extremity of the 2nd curve of the constraint
//           <lastp2> :last extremity of the 2nd curve of the constraint
//=======================================================================
void AIS_IdenticRelation::ComputeTwoEllipsesPresentation(const Handle(Prs3d_Presentation)& aPrs,
							 const Handle(Geom_Ellipse)& theEll,
							 const gp_Pnt& firstp1,
							 const gp_Pnt& lastp1,
							 const gp_Pnt& firstp2,
							 const gp_Pnt& lastp2)
{
  Standard_Real confusion (Precision::Confusion());

  // Searching of complete ellipses
  Standard_Boolean circ1complete = (firstp1.IsEqual(lastp1, confusion));
  Standard_Boolean circ2complete = (firstp2.IsEqual(lastp2, confusion));
    
  myCenter = theEll->Location();
  Standard_Real aSegSize = theEll->MajorRadius()/5.0;
  Standard_Real rad = M_PI / 5.0;
    
  // I. Case of 2 complete ellipses
  if ( circ1complete && circ2complete )
    {
      if (myAutomaticPosition)
	{
	  Standard_Real pfirst1 = ElCLib::Parameter(theEll->Elips(), firstp1);
	  myFAttach = ElCLib::Value(Modulo2PI(pfirst1-rad), theEll->Elips());
	  mySAttach = ElCLib::Value(Modulo2PI(pfirst1+rad), theEll->Elips());

	  gp_Pnt curpos = ElCLib::Value(pfirst1,theEll->Elips());
	  gp_Vec vtrans(myCenter, curpos);
	  vtrans.Normalize();
	  vtrans *= aSegSize;
	  curpos.Translate(vtrans);
	  myPosition = curpos;
	}
      else ComputeNotAutoElipsPresentation(theEll);
    }

  // II. Case of one complete circle and one arc
  else if ( (circ1complete && !circ2complete) || (!circ1complete && circ2complete) )
    {
      gp_Pnt firstp, lastp;
      if ( circ1complete && !circ2complete)
	{
	  firstp = firstp2;
	  lastp  = lastp2;
	}
      else
	{
	  firstp = firstp1;
	  lastp  = lastp1;
	}

      if (myAutomaticPosition)
	{
	  ComputeAutoArcPresentation(theEll, firstp, lastp);
	}
      else
	{
	  ComputeNotAutoArcPresentation(theEll, firstp, lastp);
	}
    }

  // III and IV. Case of two arcs
  else if ( !circ1complete && !circ2complete )
    {
      // We project all the points on the circle
      Standard_Real pf1, pf2, pl1, pl2;
      pf1 = ElCLib::Parameter(theEll->Elips(), firstp1);
      pf2 = ElCLib::Parameter(theEll->Elips(), firstp2);
      pl1 = ElCLib::Parameter(theEll->Elips(), lastp1);
      pl2 = ElCLib::Parameter(theEll->Elips(), lastp2);

      // III. Arcs with common ends
      // III.1. First of one and last of another
      if ( IsEqual2PI(pl1,pf2,confusion) || IsEqual2PI(pf1,pl2,confusion) )
	{
	  gp_Pnt curpos;
	  Standard_Real att=0.;
	  if ( IsEqual2PI(pl1,pf2,confusion) )
	    {
	      att = pl1;
	      curpos = lastp1;
	    }
	  else if ( IsEqual2PI(pf1,pl2,confusion) )
	    {
	      att = pf1;
	      curpos = firstp1;
	    }
	  Standard_Real maxrad = Min(Modulo2PI(pl1 - pf1),Modulo2PI(pl2 - pf2))*3/4;
	  if ( rad > maxrad ) rad = maxrad;
	  Standard_Real pFAttach = Modulo2PI(att - rad);
	  Standard_Real pSAttach = Modulo2PI(att + rad);
	  myFAttach = ElCLib::Value(pFAttach, theEll->Elips());
	  mySAttach = ElCLib::Value(pSAttach, theEll->Elips());
	  if ( myAutomaticPosition )
	    {
	      gp_Vec vtrans(myCenter,curpos);
	      vtrans.Normalize();
	      vtrans *= aSegSize;
	      curpos.Translate(vtrans);
	      myPosition = curpos;
	    }
	}
      // III.2. Two first or two last
      else if ( IsEqual2PI(pf1,pf2,confusion) || IsEqual2PI(pl1,pl2,confusion) )
	{
	  Standard_Real l1 = Modulo2PI(pl1 - pf1);
	  Standard_Real l2 = Modulo2PI(pl2 - pf2);
	  gp_Pnt firstp,lastp;
	  if ( l1 < l2 )
	    {
	      firstp = firstp1;
	      lastp = lastp1;
	    }
	  else
	    {
	      firstp = firstp2;
	      lastp = lastp2;
	    }

	  if ( myAutomaticPosition )
	    {
	      ComputeAutoArcPresentation(theEll, firstp, lastp);
	    }
	  else
	    {
	      ComputeNotAutoArcPresentation(theEll, firstp, lastp);
	    }
	}
      // IV. All others arcs (without common ends)
      else
	{
	  // order the parameters; first will be pf1
	  Standard_Real pl1m = Modulo2PI(pl1 - pf1);
	  Standard_Real pf2m = Modulo2PI(pf2 - pf1);
	  Standard_Real pl2m = Modulo2PI(pl2 - pf1);

	  Standard_Boolean case1 = Standard_False;
	  // 1 - not intersecting arcs
	  // 2 - intersecting arcs, but one doesn't contain another
	  // 3a - first arc contains the second one
	  // 3b - second arc contains the first one
	  // 4 - two intersections

	  gp_Pnt firstp, lastp;

	  if ( pl1m < pf2m ) // 1 or 2b or 3b
	    {
	      if ( pl1m < pl2m ) // 1 or 3b
		{
		  if ( pl2m < pf2m ) // 3b
		    {
		      firstp = firstp1;
		      lastp  = lastp1;
		    }
		  else // 1
		    {
		      case1 = Standard_True;
		      Standard_Real deltap1 = Modulo2PI(pf1 - pl2);
		      Standard_Real deltap2 = Modulo2PI(pf2 - pl1);
		      if ( ((deltap1 < deltap2) && (deltap1 < 2*rad)) ||
			   ((deltap2 < deltap1) && (deltap2 > 2*rad)) ) // deltap2
			{
			  firstp = lastp1;
			  lastp  = firstp2;
			}
		      else // deltap1
			{
			  firstp = lastp2;
			  lastp  = firstp1;
			}
		    }
		}
	      else // 2b
		{
		  firstp = firstp1;
		  lastp  = lastp2;
		}
	    }
	  else // 2a or 3a or 4
	    {
	      if ( pl1m < pl2m ) // 2a
		{
		  firstp = firstp2;
		  lastp  = lastp1;
		}
	      else // 3a or 4
		{
		  if ( pl2m > pf2m ) // 3a
		    {
		      firstp = firstp2;
		      lastp  = lastp2;
		    }
		  else // 4
		    {
		      Standard_Real deltap1 = Modulo2PI(pl1 - pf2);
		      Standard_Real deltap2 = Modulo2PI(pl2 - pf1);
		      if ( ((deltap1 < deltap2) && (deltap1 < 2*rad)) ||
			   ((deltap2 < deltap1) && (deltap2 > 2*rad)) ) // deltap2
			{
			  firstp = firstp1;
			  lastp  = lastp2;
			}
		      else // deltap1
			{
			  firstp = firstp2;
			  lastp  = lastp1;
			}
		    }
		}
	    }

	  if ( myAutomaticPosition )
	    {
	      ComputeAutoArcPresentation(theEll,firstp,lastp,case1);
	    }
	  else
	    {
	      if ( case1 )
		{
		  myFAttach = firstp;
		  mySAttach = lastp;
		}
	      else ComputeNotAutoArcPresentation(theEll, firstp, lastp);
	    }
	}
    }

  // Display of the presentation
  TCollection_ExtendedString vals(" ==");
  gp_Pnt attach = myPosition;
  ComputeAttach(theEll->Elips(),myFAttach,mySAttach,attach);
  DsgPrs_IdenticPresentation::Add(aPrs,
				  myDrawer,
				  vals,
				  theEll->Elips(),
				  myFAttach,
				  mySAttach,
				  myPosition,
				  attach);
}

//=======================================================================
//function : ComputeAutoArcPresentation
//purpose  : Compute the presentation of the constraint where we are
//           not in the case of dragging.
//=======================================================================
void AIS_IdenticRelation::ComputeAutoArcPresentation(const Handle(Geom_Ellipse)& theEll,
						     const gp_Pnt& firstp,
						     const gp_Pnt& lastp,
						     const Standard_Boolean isstatic)
{
  Standard_Real aSegSize = theEll->MajorRadius()/5.0;
  Standard_Real rad = M_PI / 5.0;

  gp_Elips anEll = theEll->Elips();
  
  Standard_Real pFA = ElCLib::Parameter(anEll,firstp);
  Standard_Real pSA = ElCLib::Parameter(anEll,lastp);
  Standard_Real maxrad = Modulo2PI(pSA - pFA)/2.0;
  
  if ( (rad > maxrad) || isstatic ) rad = maxrad;
  Standard_Real pmiddle = Modulo2PI(pFA + Modulo2PI(pSA - pFA)/2.0);
  
  myFAttach = ElCLib::Value(Modulo2PI(pmiddle - rad),anEll);
  mySAttach = ElCLib::Value(Modulo2PI(pmiddle + rad),anEll);
  
  gp_Pnt curpos = ElCLib::Value(pmiddle,anEll);
  gp_Vec vtrans(myCenter, curpos);
  vtrans.Normalize();
  vtrans *= aSegSize;
  myPosition = curpos.Translated(vtrans);
}

//=======================================================================
//function : ComputeNotAutoElipsPresentation
//purpose  : Compute the presentation of the constraint where we are
//           in the case of dragging.
// Note    : This function is to be used only in the case of ellipses.
//           The symbol of the constraint moves only between myFAttach
//           and mySAttach.
//=======================================================================
void AIS_IdenticRelation::ComputeNotAutoElipsPresentation(const Handle(Geom_Ellipse)& theEll)
{
  gp_Pnt curpos = myPosition;

  gp_Elips anEll = theEll->Elips();
  
  // Case of confusion between the current position and the center 
  // of the ellipse -> we move the current position
  Standard_Real confusion (Precision::Confusion());
  if ( myCenter.Distance(curpos) <= confusion )
    {
      gp_Vec vprec(myCenter, myFAttach);
      vprec.Normalize();
      curpos.Translate(vprec*1e-5);
    }
  
  Standard_Real rad = M_PI / 5.0;
//  Standard_Real pcurpos = ElCLib::Parameter(anEll,curpos);
  GeomAPI_ProjectPointOnCurve aProj (curpos, theEll);
  Standard_Real pcurpos  = aProj.LowerDistanceParameter();

  Standard_Real pFAttach = pcurpos - rad;
  Standard_Real pSAttach = pcurpos + rad;
  myFAttach = ElCLib::Value(pFAttach,anEll);
  mySAttach = ElCLib::Value(pSAttach,anEll);
}

//=======================================================================
//function : ComputeNotAutoArcPresentation
//purpose  : Compute the presentation of the constraint where we are
//           in the case of dragging.
// Note    : This function is to be used only in the case of ellipses.
//           The symbol of the constraint moves only between myFAttach
//           and mySAttach.
//=======================================================================
void AIS_IdenticRelation::ComputeNotAutoArcPresentation(const Handle(Geom_Ellipse)& theEll,
							const gp_Pnt& pntfirst,
							const gp_Pnt& pntlast)
{
  gp_Pnt curpos = myPosition;

  gp_Elips anEll = theEll->Elips();

  Standard_Real pFPnt = ElCLib::Parameter(anEll, pntfirst);
  Standard_Real pSPnt = ElCLib::Parameter(anEll, pntlast);
  Standard_Real deltap = Modulo2PI(pSPnt - pFPnt)/2.0;

  Standard_Real rad = M_PI / 5;
  if ( deltap < rad )
    {
      myFAttach = pntfirst;
      mySAttach = pntlast;
    }
  else
    {
      gp_Pnt aFPnt = ElCLib::Value(Modulo2PI(pFPnt + rad), anEll);
      gp_Pnt aSPnt = ElCLib::Value(Modulo2PI(pSPnt - rad), anEll);

      ComputeAttach(anEll,aFPnt,aSPnt,curpos);

//      Standard_Real pcurpos = ElCLib::Parameter(anEll,curpos);
      GeomAPI_ProjectPointOnCurve aProj (curpos, theEll);
      Standard_Real pcurpos  = aProj.LowerDistanceParameter();

      myFAttach = ElCLib::Value(pcurpos - rad, anEll);
      mySAttach = ElCLib::Value(pcurpos + rad, anEll);
    }
}
// jfa 18/10/2000 end

//=======================================================================
//function : ComputeTwoVerticesPresentation
//purpose  : 
//=======================================================================
void AIS_IdenticRelation::ComputeTwoVerticesPresentation(const Handle(Prs3d_Presentation)& aPrs)
{
  Standard_Boolean isOnPlane1, isOnPlane2;
  const TopoDS_Vertex& FVertex = TopoDS::Vertex(myFShape);
  const TopoDS_Vertex& SVertex = TopoDS::Vertex(mySShape);
  
  AIS::ComputeGeometry(FVertex, myFAttach, myPlane, isOnPlane1);
  AIS::ComputeGeometry(SVertex, mySAttach, myPlane, isOnPlane2);
  
  if (isOnPlane1 && isOnPlane2)
    myExtShape = 0;
  else if ( isOnPlane1 && !isOnPlane2)
    myExtShape = 2;
  else if (!isOnPlane1 && isOnPlane2)
    myExtShape = 1;
  else
    return ;

  
  // The attachement points are the points themselves that must be 
  //identical
  myFAttach = BRep_Tool::Pnt(FVertex);
  mySAttach = myFAttach;

  gp_Pnt curpos;
  if (myAutomaticPosition)
    {
      //Computation of the size of the symbol
      Standard_Real symbsize = ComputeSegSize();
      if (symbsize <= Precision::Confusion()) symbsize = 1.;
      symbsize*=5;
      // Computation of the direction of the segment of the presentation 
      // we take the median of the edges connected to vertices
      gp_Dir dF, dS;
      gp_Dir myDir;
      TColStd_ListIteratorOfListOfTransient it(Users());
      if (it.More())
	{
	  Handle(AIS_Shape) USER (Handle(AIS_Shape)::DownCast(it.Value()));
	  if (!USER.IsNull())
	    {
	      const TopoDS_Shape& SH =USER->Shape();
	      if ( (!SH.IsNull()) && (SH.ShapeType() == TopAbs_WIRE) )
		{
		  const TopoDS_Wire& WIRE = TopoDS::Wire(USER->Shape());
		  Standard_Boolean done = ComputeDirection(WIRE,FVertex,dF);
		  if (!done) return;
		  done =  ComputeDirection(WIRE,SVertex,dS);
		  if (!done) return;
		}
	      else return;
	    }
	  else return;

	  // computation of the segment direction like average 
	  // of the 2 computed directions.
	  if ( dF.IsParallel(dS, Precision::Angular()) )
	    {
	      myDir = dF.Crossed(myPlane->Pln().Axis().Direction());
	    }
	  else
	    {
	      myDir.SetXYZ(dF.XYZ() + dS.XYZ());
	    }
	  curpos = myFAttach.Translated(gp_Vec(myDir)*symbsize) ;
	}
// jfa 11/10/2000
      else
	{
	  curpos = myFAttach;
	}
// jfa 11/10/2000 end

      myPosition = curpos;
      myAutomaticPosition = Standard_False;
    }
  else
    {
      curpos = myPosition;
    }

  // Presentation computation
  TCollection_ExtendedString vals(" ++");
  DsgPrs_IdenticPresentation::Add(aPrs,
				  myDrawer,
				  vals,
				  myFAttach,
				  curpos);
  // Calculate the projection of vertex
  if ( myExtShape == 1)
    ComputeProjVertexPresentation(aPrs,FVertex,myFAttach);
  else if ( myExtShape == 2)
    ComputeProjVertexPresentation(aPrs,SVertex,mySAttach);
}



//=======================================================================
//function : ComputeSegSize
//purpose  : 
//=======================================================================
Standard_Real AIS_IdenticRelation::ComputeSegSize() const 
{
  return 1.;
}

//=======================================================================
//function : ComputeDirection
//purpose  : Compute a direction according to the different geometric
//           elements connected to the vertex <VERT>, in way to not have
//           overlap between the symbol and them.
//=======================================================================
Standard_Boolean AIS_IdenticRelation::ComputeDirection(const TopoDS_Wire& aWire, 
						       const TopoDS_Vertex& VERT, 
						       gp_Dir& dF) const 
{
   // we take the median of the edges connected to vertices
  TopoDS_Edge edg1,edg2;
  ConnectedEdges(aWire,VERT,edg1,edg2);

  if ( edg1.IsNull() && edg2.IsNull() ) {
    return Standard_False;
  }
  
  Handle(Geom_Curve) curv1,curv2;
  gp_Pnt firstp1,lastp1,firstp2,lastp2;

  // Case with 2 edges connected to the vertex <VERT>
  if ( !edg1.IsNull() && !edg2.IsNull() ) {
    if ( !AIS::ComputeGeometry(edg1,edg2,
			       curv1,curv2,
			       firstp1, lastp1,
			       firstp2, lastp2,myPlane))
      return Standard_False;

    gp_Dir d1, d2;
    if ( curv1->IsInstance(STANDARD_TYPE(Geom_Circle)) ) {
      d1 = ComputeCircleDirection(Handle(Geom_Circle)::DownCast (curv1), VERT);
    }
    else if (curv1->IsInstance(STANDARD_TYPE(Geom_Line)) ) {
      d1 = ComputeLineDirection(Handle(Geom_Line)::DownCast (curv1), firstp1);
    }
    else 
      return Standard_False;
    
    if ( curv2->IsInstance(STANDARD_TYPE(Geom_Circle)) ) {
      d2 = ComputeCircleDirection( Handle(Geom_Circle)::DownCast (curv2), VERT);
    }
    else if (curv2->IsInstance(STANDARD_TYPE(Geom_Line)) ) {
      d2 =ComputeLineDirection( Handle(Geom_Line)::DownCast (curv2), firstp2);
    }
    else 
      return Standard_False;
    
    if ( !d1.IsParallel(d2, Precision::Angular() ))
      dF.SetXYZ( (d1.XYZ() + d2.XYZ())/2 );
    else {
      dF= d1.Crossed(myPlane->Pln().Axis().Direction());
    }
  }

  // Case where <VERT> is at an extremity of a wire.
  else {
    TopoDS_Edge VEdge;
    if ( !edg1.IsNull() )  
      VEdge = edg1;
    else if (!edg2.IsNull() )
       VEdge = edg2;
    else 
      return Standard_False;

    if ( !AIS::ComputeGeometry(VEdge, curv1, firstp1, lastp1) )
      return Standard_False; 
    if ( curv1->IsInstance(STANDARD_TYPE(Geom_Circle)) ) {
      dF = ComputeCircleDirection( Handle(Geom_Circle)::DownCast (curv1), VERT);
    }
    else if (curv1->IsInstance(STANDARD_TYPE(Geom_Line)) ) {
      dF = ComputeLineDirection( Handle(Geom_Line)::DownCast (curv1), firstp1);
    }
    else
      return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
//function : ComputeLineDirection
//purpose  : 
//=======================================================================
gp_Dir AIS_IdenticRelation::ComputeLineDirection(const Handle(Geom_Line)& lin, 
						 const gp_Pnt& firstP) const 
{
  gp_Dir dir;
  dir = lin->Lin().Direction();
  if ( !myFAttach.IsEqual(firstP, Precision::Confusion()) )
    dir.Reverse();
  return dir;
}

//=======================================================================
//function : ComputeCircleDirection
//purpose  : 
//=======================================================================
gp_Dir AIS_IdenticRelation::ComputeCircleDirection(const Handle(Geom_Circle)& circ, 
						   const TopoDS_Vertex& VERT) const 
{
  gp_Vec V(circ->Location(),BRep_Tool::Pnt(VERT));
  return gp_Dir(V);
}

//=======================================================================
//function : ComputeOneEdgeOVertexPresentation
//purpose  : 
//=======================================================================
void AIS_IdenticRelation::ComputeOneEdgeOVertexPresentation(const Handle(Prs3d_Presentation)& aPrs)
{
  TopoDS_Vertex V;
  TopoDS_Edge E;
  Standard_Integer numedge;
  
  if (myFShape.ShapeType() == TopAbs_VERTEX) {
    V = TopoDS::Vertex(myFShape);
    E = TopoDS::Edge(mySShape);
    numedge = 2;// edge = 2nd shape
  }
  else {
    V = TopoDS::Vertex(mySShape);
    E   = TopoDS::Edge(myFShape);
    numedge = 1;  // edge = 1st shape
  }
  gp_Pnt ptonedge1,ptonedge2;
  Handle(Geom_Curve) aCurve;
  Handle(Geom_Curve) extCurv;
  Standard_Boolean isInfinite;
  Standard_Boolean isOnPlanEdge, isOnPlanVertex;
  if (!AIS::ComputeGeometry(E,aCurve,ptonedge1,ptonedge2,extCurv,isInfinite,isOnPlanEdge,myPlane))
    return;
  aPrs->SetInfiniteState(isInfinite);
  AIS::ComputeGeometry(V, myFAttach, myPlane, isOnPlanVertex);

  // only the curve can be projected 
  if (!isOnPlanEdge && !isOnPlanVertex) return;

  if (!isOnPlanEdge) {
    if (numedge == 1) myExtShape = 1;
    else myExtShape = 2;
  }
  else if (!isOnPlanVertex) {
    if (numedge == 1) myExtShape = 2;
    else myExtShape = 1;
  }
  // The attachement points are the point 
  myFAttach = BRep_Tool::Pnt(V);
  mySAttach = myFAttach;

  gp_Pnt curpos;
  if (myAutomaticPosition) {
    //Computation of the size of the symbol
    Standard_Real symbsize = ComputeSegSize();
    symbsize*=5;
    // Computation of the direction of the segment of the presentation 
    // we take the median of the edges connected to vertices
    gp_Dir myDir;
    if ( aCurve->IsKind(STANDARD_TYPE(Geom_Line))) {
      myDir = Handle(Geom_Line)::DownCast (aCurve)->Lin().Direction();
      myDir.Cross(myPlane->Pln().Axis().Direction());
    }
    else if (aCurve->IsKind(STANDARD_TYPE(Geom_Circle))) {
      Handle(Geom_Circle) CIR = Handle(Geom_Circle)::DownCast (aCurve);
      myDir.SetXYZ(myFAttach.XYZ() - CIR->Location().XYZ());
    }
    // jfa 10/10/2000
    else if (aCurve->IsKind(STANDARD_TYPE(Geom_Ellipse))) {
      Handle(Geom_Ellipse) CIR = Handle(Geom_Ellipse)::DownCast (aCurve);
      myDir.SetXYZ(myFAttach.XYZ() - CIR->Location().XYZ());
    }
    // jfa 10/10/2000 end

    curpos = myFAttach.Translated(gp_Vec(myDir)*symbsize) ;
    myPosition = curpos;
    myAutomaticPosition = Standard_True;
  }
  else {
    curpos = myPosition;
  }

  // Presentation computation
  TCollection_ExtendedString vals(" -+-");
  DsgPrs_IdenticPresentation::Add(aPrs,
				  myDrawer,
				  vals,
				  myFAttach,
				  curpos);
  if (myExtShape != 0) {
    if (!extCurv.IsNull()) { // the edge is not in the WP
     ComputeProjEdgePresentation(aPrs,E,Handle(Geom_Line)::DownCast (aCurve),ptonedge1,ptonedge2);
    }
  }
}
