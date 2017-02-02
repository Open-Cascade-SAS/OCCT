// Created on: 1996-12-05
// Created by: Flore Lantheaume/Odile Olivier
// Copyright (c) 1996-1999 Matra Datavision
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
#include <AIS_FixRelation.hxx>
#include <AIS_Shape.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <DsgPrs_FixPresentation.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Transformation.hxx>
#include <gp_Ax1.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Projector.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>
#include <TColStd_ListIteratorOfListOfTransient.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_FixRelation,AIS_Relation)

static Standard_Boolean InDomain(const Standard_Real fpar,
					const Standard_Real lpar,
					const Standard_Real para) 
{
  if (fpar >= 0.) {
    return ((para >= fpar) && (para <= lpar));
  }
  if (para >= (fpar+2*M_PI)) return Standard_True;
  if (para <= lpar) return Standard_True;
  return Standard_False;
}

//=======================================================================
//function : Constructor
//purpose  : vertex Fix Relation
//=======================================================================

AIS_FixRelation::AIS_FixRelation(const TopoDS_Shape& aShape, 
				 const Handle(Geom_Plane)& aPlane, 
				 const TopoDS_Wire& aWire)
:AIS_Relation(),
 myWire(aWire)
{
  myFShape = aShape;
  myPlane = aPlane;
  myAutomaticPosition = Standard_True;
  myArrowSize = 5.;
}

//=======================================================================
//function : Constructor
//purpose  : vertex Fix Relation
//=======================================================================

AIS_FixRelation::AIS_FixRelation(const TopoDS_Shape& aShape, 
				 const Handle(Geom_Plane)& aPlane, 
				 const TopoDS_Wire& aWire, 
				 const gp_Pnt& aPosition, 
				 const Standard_Real anArrowSize)
:AIS_Relation(),
 myWire(aWire)
{
  myFShape = aShape;
  myPlane = aPlane;
  myPosition = aPosition;
  SetArrowSize( anArrowSize );
  myAutomaticPosition = Standard_False;
}


//=======================================================================
//function : Constructor
//purpose  : edge (line or circle) Fix Relation
//=======================================================================

AIS_FixRelation::AIS_FixRelation(const TopoDS_Shape& aShape, 
                                 const Handle(Geom_Plane)& aPlane)
{
  myFShape = aShape;
  myPlane = aPlane;
  myAutomaticPosition = Standard_True;
  myArrowSize = 5.;
}

//=======================================================================
//function : Constructor
//purpose  : edge (line or circle) Fix Relation
//=======================================================================

AIS_FixRelation::AIS_FixRelation(
	const TopoDS_Shape& aShape, 
	const Handle(Geom_Plane)& aPlane, 
	const gp_Pnt& aPosition, 
	const Standard_Real anArrowSize)
{
  myFShape = aShape;
  myPlane = aPlane;
  myPosition = aPosition;
  SetArrowSize( anArrowSize );
  myAutomaticPosition = Standard_False;
}

//=======================================================================
//function : Wire
//purpose  : 
//=======================================================================

TopoDS_Wire AIS_FixRelation::Wire()
{
  return myWire;
}

//=======================================================================
//function : SetWire
//purpose  : 
//=======================================================================

void AIS_FixRelation::SetWire(const TopoDS_Wire& aWire)
{
  myWire = aWire;
}


//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_FixRelation::Compute(const Handle(PrsMgr_PresentationManager3d)&, 
			      const Handle(Prs3d_Presentation)& aPresentation, 
			      const Standard_Integer)
{
  // Calculate position of the symbol and
  // point of attach of the segment on the shape
  gp_Pnt curpos;
  if (myFShape.ShapeType() == TopAbs_VERTEX)
    ComputeVertex(TopoDS::Vertex(myFShape), curpos);
  else if (myFShape.ShapeType() == TopAbs_EDGE)
    ComputeEdge(TopoDS::Edge(myFShape), curpos);

  const gp_Dir& nor = myPlane->Axis().Direction();

  
  // calculate presentation
  // definition of the symbol size
  if( !myArrowSizeIsDefined )
    myArrowSize = 5.;

    //creation of the presentation
  DsgPrs_FixPresentation::Add(aPresentation,
			      myDrawer,
			      myPntAttach,
			      curpos,
			      nor,
			      myArrowSize);
}

//=======================================================================
//function : Compute
//purpose  : to avoid warning
//=======================================================================

void AIS_FixRelation::Compute(const Handle(Prs3d_Projector)& aProjector,
			      const Handle(Prs3d_Presentation)& aPresentation)
{
// throw Standard_NotImplemented("AIS_FixRelation::Compute(const Handle(Prs3d_Projector)&,const Handle(Prs3d_Presentation)&)");
 PrsMgr_PresentableObject::Compute( aProjector , aPresentation ) ; 
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_FixRelation::Compute(const Handle(Prs3d_Projector)& aProjector,
                              const Handle(Geom_Transformation)& aTransformation,
                              const Handle(Prs3d_Presentation)& aPresentation)
{
// throw Standard_NotImplemented("AIS_FixRelation::Compute(const Handle(Prs3d_Projector)&, const Handle(Geom_Transformation)&, const Handle(Prs3d_Presentation)&)");
 PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation ) ;
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================

void AIS_FixRelation::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection, 
				       const Standard_Integer)
{
  Handle(SelectMgr_EntityOwner) own = new SelectMgr_EntityOwner(this,7);

  // creation of segment sensible for the linked segment  
  // of the shape fixed to symbol 'Fix'
  Handle(Select3D_SensitiveSegment) seg;
  seg = new Select3D_SensitiveSegment(own,
				      myPntAttach,
				      myPosition);
  aSelection->Add(seg);
  
  // Creation of the sensible zone of symbol 'Fix'
  gp_Dir norm = myPlane->Axis().Direction();
    
  gp_Vec dirac(myPntAttach,myPosition);
  dirac.Normalize();
  gp_Vec norac = dirac.Crossed(gp_Vec(norm));
  gp_Ax1 ax(myPosition, norm);
  norac.Rotate(ax, M_PI/8);

  norac*=(myArrowSize/2);
  gp_Pnt P1 = myPosition.Translated(norac);
  gp_Pnt P2 = myPosition.Translated(-norac);
  seg = new Select3D_SensitiveSegment(own,
				      P1,
				      P2);
  aSelection->Add(seg);

  norac*=0.8;
  P1 = myPosition.Translated(norac);
  P2 = myPosition.Translated(-norac);
  dirac*=(myArrowSize/2);
  gp_Pnt PF(P1.XYZ());
  gp_Pnt PL = PF.Translated(dirac);
  PL.Translate(norac);
  seg = new Select3D_SensitiveSegment(own,
				      PF,
				      PL);
  aSelection->Add(seg);


  PF.SetXYZ(P2.XYZ());
  PL = PF.Translated(dirac);
  PL.Translate(norac);
  seg = new Select3D_SensitiveSegment(own,
				      PF,
				      PL);
  aSelection->Add(seg);

  PF.SetXYZ( (P1.XYZ() + P2.XYZ()) /2 );
  PL = PF.Translated(dirac);
  PL.Translate(norac);
  seg = new Select3D_SensitiveSegment(own,
				      PF,
				      PL);
}

//=======================================================================
//function : ComputeVertex
//purpose  : computes myPntAttach and the position of the presentation 
//           when you fix a vertex
//=======================================================================

void AIS_FixRelation::ComputeVertex(const TopoDS_Vertex& /*FixVertex*/,
				    gp_Pnt& curpos)
{
  myPntAttach = BRep_Tool::Pnt(TopoDS::Vertex(myFShape));
  curpos = myPosition;
  if (myAutomaticPosition) {
      gp_Pln pln(myPlane->Pln());
      gp_Dir dir(pln.XAxis().Direction());
      gp_Vec transvec = gp_Vec(dir)*myArrowSize;
      curpos = myPntAttach.Translated(transvec);;
      myPosition = curpos;
      myAutomaticPosition = Standard_True;
  }
}

//=======================================================================
//function : ComputePosition
//purpose  : 
//=======================================================================

gp_Pnt AIS_FixRelation::ComputePosition(const Handle(Geom_Curve)& curv1, 
					const Handle(Geom_Curve)& curv2, 
					const gp_Pnt& firstp1, 
					const gp_Pnt& lastp1, 
					const gp_Pnt& firstp2, 
					const gp_Pnt& lastp2) const 
{
  //---------------------------------------------------------
  // calculate the point of attach
  //---------------------------------------------------------
  gp_Pnt curpos;

  if (curv1->IsInstance(STANDARD_TYPE(Geom_Circle)) || curv2->IsInstance(STANDARD_TYPE(Geom_Circle))) {    
    Handle(Geom_Circle) gcirc = Handle(Geom_Circle)::DownCast(curv1);
    if (gcirc.IsNull()) gcirc = Handle(Geom_Circle)::DownCast(curv2);
    gp_Dir dir( gcirc->Location().XYZ() + myPntAttach.XYZ() );
    gp_Vec transvec = gp_Vec(dir)*myArrowSize;
    curpos = myPntAttach.Translated(transvec);    
  }

  else {
    gp_Vec vec1(firstp1,lastp1);
    gp_Vec vec2(firstp2,lastp2);
    
    if (!vec1.IsParallel(vec2, Precision::Angular()) ) {
      gp_Dir dir;
      Standard_Real conf =Precision::Confusion();
      if (lastp1.IsEqual(firstp2,conf) || firstp1.IsEqual(lastp2,conf)) dir.SetXYZ(vec1.XYZ() - vec2.XYZ());
      else dir.SetXYZ(vec1.XYZ() + vec2.XYZ());
      gp_Vec transvec = gp_Vec(dir)*myArrowSize;
      curpos = myPntAttach.Translated(transvec);
    }
    else {
      gp_Vec crossvec = vec1.Crossed(vec2);
      vec1.Cross(crossvec);
      gp_Dir dir(vec1);
      curpos = myPntAttach.Translated(gp_Vec(dir)*myArrowSize);
    }
  }

  return curpos;
}

//=======================================================================
//function : ComputePosition
//purpose  : Computes the position of the "fix dimension" when the 
//           fixed object is a vertex which is set at the intersection
//           of two curves.
//           The "dimension" is in the "middle" of the two edges.
//=======================================================================

gp_Pnt AIS_FixRelation::ComputePosition(const Handle(Geom_Curve)& curv, 
					const gp_Pnt& firstp, 
					const gp_Pnt& lastp) const 
{
  //---------------------------------------------------------
  // calculate the point of attach
  //---------------------------------------------------------
  gp_Pnt curpos;

  if (curv->IsKind(STANDARD_TYPE(Geom_Circle))) {
    
    Handle(Geom_Circle) gcirc = Handle(Geom_Circle)::DownCast(curv);
    gp_Dir dir( gcirc->Location().XYZ() + myPntAttach.XYZ() );
    gp_Vec transvec = gp_Vec(dir)*myArrowSize;
    curpos = myPntAttach.Translated(transvec);
    
  } //if (curv->IsKind(STANDARD_TYPE(Geom_Circle))

  else {
//    gp_Pln pln(Component()->WorkingPlane()->Plane()->GetValue()->Pln());
    gp_Pln pln(myPlane->Pln());
    gp_Dir NormPln = pln.Axis().Direction();
    gp_Vec vec(firstp,lastp);
    vec.Cross( gp_Vec(NormPln));
    vec.Normalize();
    gp_Vec transvec = vec*myArrowSize;
    curpos = myPntAttach.Translated(transvec);
    gp_Ax1 RotAx( myPntAttach, NormPln);
    curpos.Rotate(RotAx, M_PI/10);
  }

  return curpos;
 }

//=======================================================================
//function : ComputeEdge 
//purpose  : computes myPntAttach and the position of the presentation 
//           when you fix an edge
//=======================================================================

void AIS_FixRelation::ComputeEdge(const TopoDS_Edge& FixEdge, gp_Pnt& curpos)
{
  Handle(Geom_Curve) curEdge;
  gp_Pnt ptbeg,ptend;
  if (!AIS::ComputeGeometry(FixEdge,curEdge,ptbeg,ptend)) return;

  //---------------------------------------------------------
  // calcul du point de positionnement du symbole 'fix'
  //---------------------------------------------------------
        //--> In case of a straight line
  if (curEdge->IsKind(STANDARD_TYPE(Geom_Line))){
    gp_Lin glin = Handle(Geom_Line)::DownCast(curEdge)->Lin();
    Standard_Real pfirst(ElCLib::Parameter(glin,ptbeg));
    Standard_Real plast(ElCLib::Parameter(glin,ptend));
    ComputeLinePosition(glin, curpos, pfirst, plast);
  }
  
        //--> In case of a circle
  else if (curEdge->IsKind(STANDARD_TYPE(Geom_Circle))) {
    gp_Circ  gcirc = Handle(Geom_Circle)::DownCast(curEdge)->Circ();
    Standard_Real pfirst, plast;
    BRepAdaptor_Curve cu(FixEdge);
    pfirst = cu.FirstParameter();
    plast = cu.LastParameter();
    ComputeCirclePosition(gcirc, curpos, pfirst, plast);
  }
  
  else
    return;
    
}

//=======================================================================
//function : ComputeLinePosition
//purpose  : compute the values of myPntAttach and the position <pos> of
//           the symbol when the fixed edge has a geometric support equal
//           to a line.
//=======================================================================

void AIS_FixRelation::ComputeLinePosition(const gp_Lin& glin, 
					  gp_Pnt& pos, 
					  Standard_Real& pfirst, 
					  Standard_Real& plast)
{
  if (myAutomaticPosition) {
    // point of attach is chosen as middle of the segment
    myPntAttach = ElCLib::Value((pfirst+ plast)/2, glin);
    
    gp_Dir norm = myPlane ->Axis().Direction();
     
    norm.Cross(glin.Position().Direction());
    pos = myPntAttach.Translated(gp_Vec(norm)*myArrowSize);
    myAutomaticPosition = Standard_True;
  } // if (myAutomaticPosition)

  else {
    pos = myPosition;
    Standard_Real linparam = ElCLib::Parameter(glin, pos);

    // case if the projection of position is located between 2 vertices
    // de l'edge
    if ( (linparam >= pfirst) && (linparam <= plast) )
      myPntAttach = ElCLib::Value(linparam,glin);
    
    // case if the projection of Position is outside of the limits
    // of the edge : the point closest to the projection is chosen 
    // as the attach point
    else {
      Standard_Real pOnLin;
      if (linparam > plast)
	pOnLin = plast;
      else
	pOnLin = pfirst;
      myPntAttach = ElCLib::Value(pOnLin,glin);
      gp_Dir norm = myPlane->Axis().Direction();
	
      norm.Cross(glin.Position().Direction());
      gp_Lin lsup(myPntAttach, norm);
      Standard_Real parpos = ElCLib::Parameter(lsup,myPosition);
      pos =  ElCLib::Value(parpos,lsup);
    }

  }
  myPosition = pos;
}

//=======================================================================
//function : ComputeCirclePosition 
//purpose  : compute the values of myPntAttach and the position <pos> of
//           the symbol when the fixed edge has a geometric support equal
//           to a circle. 
//=======================================================================

void AIS_FixRelation::ComputeCirclePosition(
	const gp_Circ& gcirc, 
	gp_Pnt& pos, 
	Standard_Real& pfirst, 
	Standard_Real& plast)
{
  // readjust parametres on the circle
  if (plast > 2*M_PI ) {
    Standard_Real nbtours = Floor(plast / (2*M_PI));
    plast -= nbtours*2*M_PI;
    pfirst -= nbtours*2*M_PI;
  }

  if (myAutomaticPosition) {
    // the point attach is the "middle" of the segment (relatively
    // to the parametres of start and end vertices of the edge
    
    Standard_Real circparam = (pfirst + plast)/2.;

    if ( !InDomain(pfirst,plast,circparam)) {
      Standard_Real otherpar = circparam + M_PI;
      if (otherpar > 2*M_PI) otherpar -= 2*M_PI;
      circparam = otherpar;
    }

    myPntAttach = ElCLib::Value(circparam, gcirc );

    gp_Vec dir( gcirc.Location().XYZ(), myPntAttach.XYZ() );
    dir.Normalize();
    gp_Vec transvec = dir*myArrowSize;
    pos = myPntAttach.Translated(transvec);
    myPosition = pos;
    myAutomaticPosition = Standard_True;
  } // if (myAutomaticPosition)

  else {
    // case if the projection of myPosition is outside of 2
    // vertices of the edge. In this case the parameter is readjusted
    // in the valid part of the circle
    pos = myPosition;

    Standard_Real circparam = ElCLib::Parameter(gcirc, pos);

    if ( !InDomain(pfirst,plast,circparam)) {
      Standard_Real otherpar = circparam + M_PI;
      if (otherpar > 2*M_PI) otherpar -= 2*M_PI;
      circparam = otherpar;
    }
    
    myPntAttach = ElCLib::Value(circparam,gcirc);
  }
}

//=======================================================================
//function : ConnectedEdges
//purpose  : 
//=======================================================================
Standard_Boolean AIS_FixRelation::ConnectedEdges(const TopoDS_Wire& WIRE,
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
    BRepAdaptor_Curve curv(E1);
    iterator.Next();
  }
  else {
    E1.Nullify();
    return Standard_False;
  }

  if (iterator.More()) {
    E2 = TopoDS::Edge(iterator.Value());
    BRepAdaptor_Curve curv(E2);
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
