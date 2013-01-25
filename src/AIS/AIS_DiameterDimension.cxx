// Created on: 1996-12-05
// Created by: Jacques MINOT/Odile Olivier/Sergey ZARITCHNY
// Copyright (c) 1996-1999 Matra Datavision
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

// Modified     Mon 12-january-98
//              <ODL>, <SZY>


#define BUC60915        //GG 05/06/01 Enable to compute the requested arrow size
//                      if any in all dimensions.

#include <Standard_NotImplemented.hxx>

#include <AIS_DiameterDimension.ixx>
#include <AIS_DimensionOwner.hxx>
#include <DsgPrs_DiameterPresentation.hxx>
#include <DsgPrs_RadiusPresentation.hxx>

#include <TCollection_ExtendedString.hxx>

#include <Prs3d_LengthAspect.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Prs3d_Text.hxx>

#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <SelectMgr_EntityOwner.hxx>

#include <ElCLib.hxx>
#include <ElSLib.hxx>

#include <TopoDS.hxx>

#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <Adaptor3d_HCurve.hxx>

#include <Geom_Circle.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>

#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Lin.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>

#include <AIS.hxx>
#include <AIS_Drawer.hxx>

#include <GC_MakeCircle.hxx>

#include <Precision.hxx>

#include <TopExp_Explorer.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

AIS_DiameterDimension::AIS_DiameterDimension(const TopoDS_Shape& aShape, 
					     const Standard_Real aVal, 
					     const TCollection_ExtendedString& aText)
:AIS_Relation(),
 myDiamSymbol(Standard_True)
{
  myPosition  = gp_Pnt(0.,0.,0.);
  myFShape = aShape;
  myVal = aVal;
  myText = aText;
  mySymbolPrs = DsgPrs_AS_LASTAR;
  myAutomaticPosition = Standard_True;
  myArrowSize = myVal / 100.;
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

AIS_DiameterDimension::AIS_DiameterDimension(const TopoDS_Shape& aShape, 
					     const Standard_Real aVal, 
					     const TCollection_ExtendedString& aText, 
					     const gp_Pnt& aPosition, 
					     const DsgPrs_ArrowSide aSymbolPrs,
					     const Standard_Boolean aDiamSymbol,
					     const Standard_Real anArrowSize)
:AIS_Relation(),
 myDiamSymbol(aDiamSymbol)
{
  myFShape = aShape;
  myVal = aVal;
  myText = aText;
  mySymbolPrs = aSymbolPrs;
  myPosition = aPosition;
  myAutomaticPosition = Standard_False;
#ifdef BUC60915
  SetArrowSize( anArrowSize );
#else
  myArrowSize = anArrowSize;
#endif
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_DiameterDimension::Compute(
       const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
       const Handle(Prs3d_Presentation)& aPresentation, 
       const Standard_Integer /*aMode*/)
{
  aPresentation->Clear();

  switch (myFShape.ShapeType()) {
  case TopAbs_FACE :
    {
      // compute one face case
      ComputeOneFaceDiameter (aPresentation);
      break;
    }
  case TopAbs_EDGE:
    {
      ComputeOneEdgeDiameter (aPresentation);
      break;
    }
  default:
    break;
  }

}

//=======================================================================
//function : Compute
//purpose  : to avoid warning
//=======================================================================

void AIS_DiameterDimension::Compute(const Handle(Prs3d_Projector)& aProjector,
                                    const Handle(Prs3d_Presentation)& aPresentation)
{
// Standard_NotImplemented::Raise("AIS_DiameterDimension::Compute(const Handle(Prs3d_Projector)& aProjector, const Handle(Prs3d_Presentation)& aPresentation)");
 PrsMgr_PresentableObject::Compute( aProjector , aPresentation ) ;
}

void AIS_DiameterDimension::Compute(const Handle_Prs3d_Projector& aProjector, const Handle_Geom_Transformation& aTransformation, const Handle_Prs3d_Presentation& aPresentation)
{
// Standard_NotImplemented::Raise("AIS_DiameterDimension::Compute(const Handle_Prs3d_Projector&, const Handle_Geom_Transformation&, const Handle_Prs3d_Presentation&)");
  PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation ) ;
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================

void AIS_DiameterDimension::ComputeSelection(
	const Handle(SelectMgr_Selection)& aSelection, 
        const Standard_Integer /*aMode*/)
{
  Handle(AIS_DimensionOwner) own = new AIS_DimensionOwner(this,7);
  own->SetShape(myFShape);

  if (!myIsAnArc) {
    gp_Pnt        AttachmentPoint = myPosition;
    Standard_Real parat    = ElCLib::Parameter(myCircle,AttachmentPoint);
    gp_Pnt        ptoncirc = ElCLib::Value    (parat,myCircle);
    
    // ligne de cote
    
    gp_Pnt        center  = myCircle.Location();
    gp_Vec        vecrap  (ptoncirc,center);
    
    Standard_Real dist    = center.Distance(AttachmentPoint);
    Standard_Real aRadius = myCircle.Radius();
    Standard_Real inside  = Standard_False;
    
    gp_Pnt pt1 = AttachmentPoint;
    if (dist < aRadius) {
      pt1 = ptoncirc;
      dist = aRadius;
      inside = Standard_True;
    }
    vecrap.Normalize();
    vecrap *= (dist+aRadius);
    gp_Pnt        OppositePoint = pt1.Translated(vecrap);
    
    if ( pt1.Distance(OppositePoint)>=Precision::Confusion()) {
      Handle(Select3D_SensitiveSegment) 
	seg = new Select3D_SensitiveSegment(own,pt1 ,OppositePoint);
      aSelection->Add(seg);
    }
  }
  else
    ComputeArcSelection(aSelection);

  // Text
  Standard_Real size(Min(myVal/100.+1.e-6,myArrowSize+1.e-6));
  Handle( Select3D_SensitiveBox ) box = new Select3D_SensitiveBox( own,
								   myPosition.X(),
								   myPosition.Y(),
								   myPosition.Z(),
								   myPosition.X() + size,
								   myPosition.Y() + size,
								   myPosition.Z() + size);
  aSelection->Add(box);
}
//==========================================================================
// function : ComputeArcSelection
// purpose  : 
//           
//==========================================================================

void AIS_DiameterDimension::ComputeArcSelection(const Handle(SelectMgr_Selection)& aSelection)
{

  Standard_Real fpara; 
  Standard_Real lpara; 
  fpara = myFirstPar;
  lpara = myLastPar;

  Handle(SelectMgr_EntityOwner) own = new SelectMgr_EntityOwner(this,7);
  gp_Pnt theCenter = myCircle.Location();
  while (lpara > 2*M_PI) {
    fpara -= 2*M_PI;
    lpara -= 2*M_PI;
  }
  Standard_Real parat = ElCLib::Parameter(myCircle,myPosition);
  Standard_Boolean otherside(Standard_False);
  gp_Pnt attpoint = myPosition;

  if (!AIS::InDomain(fpara,lpara,parat)) {
    Standard_Real otherpar = parat + M_PI;
    if (otherpar > 2*M_PI) otherpar -= 2*M_PI;
    if (AIS::InDomain(fpara,lpara,otherpar)) {
      parat = otherpar;
      otherside = Standard_True;
    }
    else {
      Standard_Real ecartpar = Min(Abs(fpara-parat),
				   Abs(lpara-parat));
      Standard_Real ecartoth = Min(Abs(fpara-otherpar),
				   Abs(lpara-otherpar));
      if (ecartpar <= ecartoth) {
	if (parat < fpara) parat = fpara;
	else parat = lpara;
      }
      else {
	otherside = Standard_True;
	if (otherpar < fpara) parat = fpara;
	else parat = lpara;
      }
      gp_Pnt ptdir = ElCLib::Value(parat,myCircle);
      gp_Lin lsup(theCenter,
		  gp_Dir(ptdir.XYZ()-theCenter.XYZ()));
      Standard_Real parpos = ElCLib::Parameter(lsup,myPosition);
      attpoint = ElCLib::Value(parpos,lsup);
    }
  }
  gp_Pnt ptoncirc = ElCLib::Value(parat,myCircle);
  gp_Lin L (theCenter,gp_Dir(attpoint.XYZ()-theCenter.XYZ()));
  gp_Pnt firstpoint = attpoint;
  gp_Pnt drawtopoint = ptoncirc;

  if (!otherside) {
    Standard_Real uatt = ElCLib::Parameter(L,attpoint);
    Standard_Real uptc = ElCLib::Parameter(L,ptoncirc);
    if (Abs(uatt) > Abs(uptc)) {
      drawtopoint = theCenter;
    }
    else {
      firstpoint = theCenter;
    }
  }

  Handle(Select3D_SensitiveSegment) seg = new Select3D_SensitiveSegment(own,firstpoint,drawtopoint);
  aSelection->Add(seg);
}

//=======================================================================
//function : ComputeOneFaceDiameter
//purpose  : 
//=======================================================================

void AIS_DiameterDimension::ComputeOneFaceDiameter(
	const Handle(Prs3d_Presentation)& aPresentation)
{
  //cout<<"AIS_DiameterDimension::ComputeOneFaceDiameter"<<endl;

  gp_Pln aPln;
  Handle( Geom_Surface ) aBasisSurf;
  AIS_KindOfSurface aSurfType = AIS_KOS_OtherSurface;
  Standard_Real Offset;
  if( myAutomaticPosition )
    AIS::GetPlaneFromFace( TopoDS::Face(  myFShape),
			  aPln,
			  aBasisSurf,
			  aSurfType,
			  Offset ) ;
  
  if ( aSurfType == AIS_KOS_Plane )
    ComputeOnePlanarFaceDiameter( aPresentation );
  else 
    ComputeOneCylFaceDiameter( aPresentation, aSurfType, aBasisSurf );

}


//=======================================================================
//function : ComputeOneCylFaceDiameter
//purpose  : 
//=======================================================================

void AIS_DiameterDimension::ComputeOneCylFaceDiameter
  (const Handle(Prs3d_Presentation)& aPresentation,
   const AIS_KindOfSurface  aSurfType,
   const Handle( Geom_Surface )&  aBasisSurf )
{
  gp_Pnt curPos;
  if( myAutomaticPosition )
    {
      BRepAdaptor_Surface surf1(TopoDS::Face(myFShape));
      Standard_Real uFirst, uLast, vFirst, vLast;
      uFirst = surf1.FirstUParameter();
      uLast  = surf1.LastUParameter();
      vFirst = surf1.FirstVParameter();
      vLast  = surf1.LastVParameter();
      Standard_Real uMid = (uFirst + uLast)*0.5;
      Standard_Real vMid = (vFirst + vLast)*0.5;
      surf1.D0(uMid, vMid, curPos);
      Handle( Adaptor3d_HCurve ) BasisCurve;
      //Standard_Real Param;
      Standard_Boolean ExpectedType = Standard_False;
      if (aSurfType == AIS_KOS_Cylinder)
	{
	  ExpectedType = Standard_True;
	}
      else 
	if (aSurfType == AIS_KOS_Revolution)
	{
	  BasisCurve = surf1.BasisCurve();
	  if (BasisCurve->GetType() == GeomAbs_Line) 
	    ExpectedType = Standard_True;
	}
      else if (aSurfType == AIS_KOS_Extrusion)
	{
	  BasisCurve = surf1.BasisCurve();
	  if ( BasisCurve->GetType() == GeomAbs_Circle )
	    ExpectedType = Standard_True;
	}
      if(!ExpectedType) {
	Standard_ConstructionError::Raise("AIS:: Not expected type of surface") ;
	  return;
	}
      //      
      Handle(Geom_Curve) aCurve;
      aCurve =   aBasisSurf->VIso(vMid);
      if (aCurve->DynamicType() == STANDARD_TYPE(Geom_Circle)) 
	{
	  myCircle = Handle(Geom_Circle)::DownCast(aCurve)->Circ();//gp_Circ
	}
      else if (aCurve->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve)) {
	Handle(Geom_TrimmedCurve) tCurve = Handle(Geom_TrimmedCurve)::DownCast(aCurve); 
	aCurve = tCurve->BasisCurve();
	uFirst = tCurve->FirstParameter();
	uLast  = tCurve->LastParameter();
	if (aCurve->DynamicType() == STANDARD_TYPE(Geom_Circle))
	  myCircle = Handle(Geom_Circle)::DownCast(aCurve)->Circ();//gp_Circ
      }
      else {
	// compute a circle from 3 points on "aCurve"
	gp_Pnt P1, P2;
	surf1.D0(uFirst, vMid, P1);
	surf1.D0(uLast, vMid, P2);
	GC_MakeCircle mkCirc(P1, curPos, P2);
	myCircle = mkCirc.Value()->Circ();
      } 
      myCircle.SetRadius(myVal/2.);
      myPlane = new Geom_Plane(gp_Pln(gp_Ax3(myCircle.Position())));//gp_Circ
      gp_Vec v1(myCircle.Location(), curPos);
      v1.Normalize();
      v1 = v1 * myVal*1.2;
      myPosition = myCircle.Location().Translated(v1); 
//    IsArc ?
      gp_Pnt p1, p2;
      p1 = ElCLib::Value (uFirst, myCircle);
      p2 = ElCLib::Value (uLast,  myCircle);
      if ( p1.IsEqual(p2, Precision::Confusion()) ) 
	myIsAnArc = Standard_False;
      else myIsAnArc = Standard_True;
      myFirstPar =  uFirst;
      myLastPar  =  uLast;
//    myPosition = curPos;
      myAutomaticPosition = Standard_True;   
      if ( myIsSetBndBox )
	myPosition = AIS::TranslatePointToBound( myPosition, gp_Dir( gp_Vec( myCircle.Location(),
									    myPosition ) ), myBndBox );
    }
  else { // !AutomaticPosition
    curPos = myPosition;
    curPos = AIS::ProjectPointOnPlane( curPos, myPlane->Pln() );
    myPosition = curPos;
  }
 
  Handle(Prs3d_LengthAspect) la = myDrawer->LengthAspect();
  Handle(Prs3d_ArrowAspect) arr = la->Arrow1Aspect();
  
  // size
#ifdef BUC60915
  if( !myArrowSizeIsDefined ) {
    myArrowSize = Min(myArrowSize,myCircle.Radius()/5.);
  }
  arr->SetLength(myArrowSize);
#else
  if (myCircle.Radius()/5. > myArrowSize) {
    arr->SetLength(myArrowSize);
  }
  else {
    arr->SetLength(myCircle.Radius()/5.);
  }
#endif
  
  //cout<<"AIS_DiameterDimension:: add Prs"<<endl;
  if( myIsAnArc ) 
    DsgPrs_DiameterPresentation::Add(aPresentation, myDrawer, myText, myPosition,
				     myCircle, myFirstPar, myLastPar, mySymbolPrs, myDiamSymbol);
  else
    DsgPrs_DiameterPresentation::Add(aPresentation, myDrawer, myText, myPosition,
				     myCircle, DsgPrs_AS_BOTHAR, myDiamSymbol);


}


//=======================================================================
//function : ComputeCircleDiameter
//purpose  : 
//=======================================================================

void AIS_DiameterDimension::ComputeCircleDiameter(const Handle(Prs3d_Presentation)& aPresentation)
{
  gp_Pnt center = myCircle.Location();
  Standard_Real rad = myCircle.Radius();
  gp_Pnt curpos;

  if (myAutomaticPosition) {
// we compute 1 point on the circle
    myPlane = new Geom_Plane(gp_Pln(gp_Ax3(myCircle.Position())));//gp_Circ
    gp_Dir xdir = myCircle.XAxis().Direction();
    Standard_Real deport = rad *1.2;
    curpos = center.Translated( gp_Vec(xdir)*deport );
    SetPosition (curpos);// myPosition = curpos
    myAutomaticPosition = Standard_True; 
    if (myIsSetBndBox)
      myPosition = AIS::TranslatePointToBound( myPosition, gp_Dir( gp_Vec( myCircle.Location(),
									  myPosition ) ), myBndBox );
  }
  else {
    curpos = myPosition;
	// VRO (2007-05-17) inserted this IF.
	if (myPlane.IsNull())
		myPlane = new Geom_Plane(gp_Pln(gp_Ax3(myCircle.Position())));
    myPosition = AIS::ProjectPointOnPlane( curpos, myPlane->Pln() );
  }

  // size
  Handle(Prs3d_LengthAspect) LA = myDrawer->LengthAspect();
  Handle(Prs3d_ArrowAspect) arr = LA->Arrow1Aspect();

#ifdef BUC60915
  if( !myArrowSizeIsDefined ) {
    myArrowSize = Min(myArrowSize,myCircle.Radius()/5.);
  }
  arr->SetLength(myArrowSize);
#else
  if (myCircle.Radius()/5. > myArrowSize) {
    arr->SetLength(myArrowSize);
  }
  else {
    arr->SetLength(myCircle.Radius()/5.);
  }
#endif
  
  DsgPrs_DiameterPresentation::Add(aPresentation, myDrawer, myText, myPosition, myCircle,
				   DsgPrs_AS_BOTHAR, myDiamSymbol );

}

//==========================================================================
// function : ComputeArcDiameter
// purpose  : 
//           
//==========================================================================

void AIS_DiameterDimension::ComputeArcDiameter(
                        const Handle(Prs3d_Presentation)& aPresentation,
			const gp_Pnt& pfirst,
                        const gp_Pnt& pend)
{

  gp_Pnt center = myCircle.Location();
  Standard_Real rad = myCircle.Radius();

  gp_Pnt curpos;
  Standard_Real parfirst, parend;

  parfirst = ElCLib::Parameter(myCircle, pfirst);
  parend   = ElCLib::Parameter(myCircle, pend);
  myFirstPar = parfirst;
  myLastPar  = parend;
  if ( parfirst > parend) {
    parfirst -= 2*M_PI;
  }
  if (myAutomaticPosition) {
    Standard_Real pcurpos = (parfirst + parend)/2.;
    curpos = ElCLib::Value(pcurpos, myCircle);
    myPlane = new Geom_Plane(gp_Pln(gp_Ax3(myCircle.Position())));//gp_Circ
    gp_Dir vdir(gp_Vec(myCircle.Location(),curpos));
    Standard_Real deport = rad * 1.2;
    curpos = center.Translated( gp_Vec(vdir)*deport );

    SetPosition (curpos);
    myAutomaticPosition = Standard_True; 
  
    if ( myIsSetBndBox )
      myPosition = AIS::TranslatePointToBound( myPosition, gp_Dir( gp_Vec( myCircle.Location(),
									  myPosition ) ), myBndBox );
 
  }
  else {
    curpos = myPosition;
    myPosition = AIS::ProjectPointOnPlane( curpos, myPlane->Pln() );
  }
  
  // size

  Handle(Prs3d_LengthAspect) LA = myDrawer->LengthAspect();
  Handle(Prs3d_ArrowAspect) arr = LA->Arrow1Aspect();

#ifdef BUC60915
  if( !myArrowSizeIsDefined ) {
    myArrowSize = Min(myArrowSize,myCircle.Radius()/5.);
  }
  arr->SetLength(myArrowSize);
#else
  if (myCircle.Radius()/5. > myArrowSize) {
    arr->SetLength(myArrowSize);
  }
  else {
    arr->SetLength(myCircle.Radius()/5.);
  }
#endif
    
  // Display
  DsgPrs_DiameterPresentation::Add (aPresentation, myDrawer, myText, myPosition, myCircle, 
				  parfirst, parend, mySymbolPrs, myDiamSymbol);
 
}


//==========================================================================
// function : ComputeOneEdgeDiameter
// purpose  : 
//           
//==========================================================================

void AIS_DiameterDimension::ComputeOneEdgeDiameter(const Handle(Prs3d_Presentation)& aPresentation)
{
  gp_Pnt ptfirst,ptend;
  Handle(Geom_Curve) curv;
  if (!AIS::ComputeGeometry(TopoDS::Edge(myFShape),curv,ptfirst,ptend)) return;

  Handle(Geom_Circle) circ = Handle(Geom_Circle)::DownCast(curv);
  if ( circ.IsNull()) return;
  
  myCircle = circ->Circ();
  myCircle.SetRadius(myVal/2.);
  if ( ptfirst.IsEqual(ptend, Precision::Confusion()) ) {
    myIsAnArc = Standard_False;
    ComputeCircleDiameter(aPresentation);
  }
  else {
    myIsAnArc = Standard_True;
    ComputeArcDiameter(aPresentation,ptfirst,ptend );
  }
}

//===================================================================
//function : CircleFromPlanarFace
//purpose  : if possible computes circle from planar face
//=======================================================================
static Standard_Boolean CircleFromPlanarFace(const TopoDS_Face& aFace,
                                             Handle(Geom_Curve)& aCurve, 
                                             gp_Pnt & ptfirst,  gp_Pnt & ptend)
{
  TopExp_Explorer ExploEd( aFace, TopAbs_EDGE );
  for ( ; ExploEd.More(); ExploEd.Next())
    {
      TopoDS_Edge curedge =  TopoDS::Edge( ExploEd.Current() );
      if (AIS::ComputeGeometry(curedge, aCurve, ptfirst, ptend))
	if(aCurve->IsInstance(STANDARD_TYPE(Geom_Circle))  && 
	   !Handle(Geom_Circle)::DownCast(aCurve).IsNull())
	  return Standard_True;
    } 
  return Standard_False;
}

//=======================================================================
//function : ComputeOnePlanarFaceDiameter
//purpose  : 
//=======================================================================

void AIS_DiameterDimension::ComputeOnePlanarFaceDiameter(const Handle(Prs3d_Presentation)& aPresentation)
{
  gp_Pnt curPos ;
  Standard_Real parfirst =0., parend =0.; 
  if (myAutomaticPosition) {
    Handle(Geom_Curve) curv;
    gp_Pnt ptfirst,ptend;

    if( !CircleFromPlanarFace( TopoDS::Face( myFShape ), curv, ptfirst, ptend) )  {
      Standard_ConstructionError::Raise("AIS:: Curve is not a circle or is Null") ;
	return;
      }

    myCircle = Handle(Geom_Circle)::DownCast(curv)->Circ();
    if ( ptfirst.IsEqual(ptend, Precision::Confusion()) )
      myIsAnArc = Standard_False;
    else
      myIsAnArc = Standard_True;
    myCircle.SetRadius(myVal/2.);//
    BRepAdaptor_Surface surfAlgo (TopoDS::Face(myFShape));
    myPlane = new Geom_Plane(gp_Pln(gp_Ax3(myCircle.Position())));//gp_Circ
    gp_Pnt center = myCircle.Location();
    Standard_Real rad = myCircle.Radius();
    Standard_Real deport = rad * 1.2;
    if(! myIsAnArc ) { // Circle
      gp_Dir xdir = myCircle.XAxis().Direction();
      curPos = center.Translated( gp_Vec(xdir)*deport );
    }
    else { // Arc
      parfirst = ElCLib::Parameter(myCircle, ptfirst);
      parend   = ElCLib::Parameter(myCircle, ptend);
      if ( parfirst > parend) {
	parfirst -= 2*M_PI;
      }
      Standard_Real parcurPos = (parfirst + parend) * 0.5;
      curPos = ElCLib::Value(parcurPos, myCircle);
      gp_Vec v1( center, curPos );
      v1.Normalize();
      curPos = center.Translated( v1 * deport );
    }
    myFirstPar = parfirst;
    myLastPar  = parend;
    myPosition = curPos;
    myAutomaticPosition = Standard_True;    
    if ( myIsSetBndBox )
      myPosition = AIS::TranslatePointToBound( myPosition, gp_Dir( gp_Vec( myCircle.Location(),
									  myPosition ) ), myBndBox );
  } else 
    {
      // !myAutomaticPosition
      // Project point on the plane of face
      curPos = myPosition;
      curPos = AIS::ProjectPointOnPlane( curPos, myPlane->Pln() );
      myPosition = curPos;
    }

  
  Handle(Prs3d_LengthAspect) la = myDrawer->LengthAspect();
  Handle(Prs3d_ArrowAspect) arr = la->Arrow1Aspect();

  // size
#ifdef BUC60915
  if( !myArrowSizeIsDefined ) {
    myArrowSize = Min(myArrowSize,myCircle.Radius()/5.);
  }
  arr->SetLength(myArrowSize);
#else
  if (myCircle.Radius()/5. > myArrowSize) {
    arr->SetLength(myArrowSize);
  }
  else {
    arr->SetLength(myCircle.Radius()/5.);
  }
#endif
  	
  if(! myIsAnArc )
    DsgPrs_DiameterPresentation::Add(aPresentation, myDrawer, myText, myPosition,
				     myCircle, DsgPrs_AS_BOTHAR, myDiamSymbol);
  else 
    DsgPrs_DiameterPresentation::Add(aPresentation, myDrawer, myText, myPosition,
				     myCircle, myFirstPar, myLastPar, mySymbolPrs, myDiamSymbol );
}

