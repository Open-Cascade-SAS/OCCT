// Created on: 1998-01-23
// Created by: Sergey ZARITCHNY
// Copyright (c) 1998-1999 Matra Datavision
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


#define BUC60915        //GG 05/06/01 Enable to compute the requested arrow size
//                      if any in all dimensions.

#include <Standard_NotImplemented.hxx>

#include <AIS_MaxRadiusDimension.ixx>
#include <AIS_EllipseRadiusDimension.hxx>
#include <TCollection_ExtendedString.hxx>

#include <Prs3d_LengthAspect.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Prs3d_Text.hxx> 

#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <SelectMgr_EntityOwner.hxx>

#include <ElCLib.hxx>
#include <ElSLib.hxx>

#include <TopoDS.hxx>

#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve.hxx>

#include <Geom_Ellipse.hxx>
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

#include <Precision.hxx>
#include <DsgPrs_EllipseRadiusPresentation.hxx>

//=======================================================================
//function : AIS_MaxRadiusDimension
//purpose  : 
//=======================================================================

AIS_MaxRadiusDimension::AIS_MaxRadiusDimension(const TopoDS_Shape& aShape, 
					       const Standard_Real aVal, 
					       const TCollection_ExtendedString& aText)
:AIS_EllipseRadiusDimension(aShape, aText)
{
  myVal = aVal;
  mySymbolPrs = DsgPrs_AS_LASTAR;
  myAutomaticPosition = Standard_True;
  myArrowSize = myVal / 100.;
}

//=======================================================================
//function : AIS_MaxRadiusDimension
//purpose  : 
//=======================================================================

AIS_MaxRadiusDimension::AIS_MaxRadiusDimension(const TopoDS_Shape& aShape, 
					       const Standard_Real aVal, 
					       const TCollection_ExtendedString& aText,
					       const gp_Pnt& aPosition, 
					       const DsgPrs_ArrowSide aSymbolPrs,
					       const Standard_Real anArrowSize)
:AIS_EllipseRadiusDimension(aShape, aText)
{
  myVal = aVal;
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

void AIS_MaxRadiusDimension::Compute(const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
				     const Handle(Prs3d_Presentation)& aPresentation, 
				     const Standard_Integer /*aMode*/)
{
  aPresentation->Clear();

//  if( myAutomaticPosition )
    {//ota : recompute in any case 
      ComputeGeometry();
      myEllipse.SetMajorRadius(myVal);
      gp_Vec v1(myEllipse.XAxis().Direction());
      v1 *=myVal;
      myApexP = myEllipse.Location().Translated(v1); 
      myApexN = myEllipse.Location().Translated(-v1); 
    }
  if(myIsAnArc) ComputeArcOfEllipse(aPresentation);
  else 
    ComputeEllipse(aPresentation);
}

//=======================================================================
//function : Compute
//purpose  : to avoid warning
//=======================================================================

void  AIS_MaxRadiusDimension::Compute(const Handle(Prs3d_Projector)& aProjector,
                                      const Handle(Prs3d_Presentation)& aPresentation)
{
// Standard_NotImplemented::Raise("AIS_MaxRadiusDimension::Compute(const Handle(Prs3d_Projector)& aProjector, const Handle(Prs3d_Presentation)& aPresentation)");
 PrsMgr_PresentableObject::Compute( aProjector , aPresentation ) ;
}

//=======================================================================
//function : Compute
//purpose  : to avoid warning
//=======================================================================

void  AIS_MaxRadiusDimension::Compute(const Handle(PrsMgr_PresentationManager2d)& aPresentationManager,
                                      const Handle(Graphic2d_GraphicObject)& aPresentation,
                                      const Standard_Integer aMode)
{
// Standard_NotImplemented::Raise("AIS_MaxRadiusDimension::Compute(const Handle(PrsMgr_PresentationManager2d)& aPresentationManager, const Handle(Graphic2d_GraphicObject)& aPresentation, const Standard_Integer aMode)");
 PrsMgr_PresentableObject::Compute( aPresentationManager ,aPresentation,aMode) ;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_MaxRadiusDimension::Compute(const Handle_Prs3d_Projector& aProjector,
                                     const Handle_Geom_Transformation& aTransformation,
                                     const Handle_Prs3d_Presentation& aPresentation)
{
// Standard_NotImplemented::Raise("AIS_MaxRadiusDimension::Compute(const Handle_Prs3d_Projector&, const Handle_Geom_Transformation&, const Handle_Prs3d_Presentation&)");
 PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation ) ;
}


//=======================================================================
//function : ComputeEllipse
//purpose  : 
//=======================================================================

void AIS_MaxRadiusDimension::ComputeEllipse(const Handle(Prs3d_Presentation)& aPresentation)
{

  Handle(Prs3d_LengthAspect) la = myDrawer->LengthAspect();
  Handle(Prs3d_ArrowAspect) arr = la->Arrow1Aspect();
  
  // size
#ifdef BUC60915
  if( !myArrowSizeIsDefined ) {
    myArrowSize = Min(myArrowSize,myVal / 5.);
  }
  arr->SetLength(myArrowSize);
#else
  if (myVal / 5. > myArrowSize) {
    arr->SetLength(myArrowSize);
  }
  else {
    arr->SetLength(myVal / 5.);
  }
#endif

  Standard_Real U;//,V;
  gp_Pnt curPos, Center;
  Center = myEllipse.Location();
  if( myAutomaticPosition )
    {
      myPosition = Center;
      myEndOfArrow = myApexP;
      myAutomaticPosition = Standard_True;   
      
      if ( myIsSetBndBox )
	myPosition = AIS::TranslatePointToBound(myPosition, gp_Dir(gp_Vec( Center, myPosition )),
						myBndBox );	   
      curPos = myPosition;  
    }
  else //!AutomaticPosition
    {
      curPos = myPosition;
      gp_Lin L1(myEllipse.XAxis());
      U = ElCLib::Parameter ( L1, curPos );
      curPos = ElCLib::Value (U, L1);
      if (curPos.Distance(myApexP) < curPos.Distance(myApexN)) 
	myEndOfArrow = myApexP ;
      else
	myEndOfArrow = myApexN ;
    }
 
  // Presenatation  
  DsgPrs_EllipseRadiusPresentation::Add(aPresentation, myDrawer, myVal, myText, curPos,
					myEndOfArrow, Center, Standard_True, mySymbolPrs);

}

//=======================================================================
//function : ComputeArcOfEllipse
//purpose  : 
//=======================================================================

void AIS_MaxRadiusDimension::ComputeArcOfEllipse(const Handle(Prs3d_Presentation)& aPresentation)
{

  Handle(Prs3d_LengthAspect) la = myDrawer->LengthAspect();
  Handle(Prs3d_ArrowAspect) arr = la->Arrow1Aspect();
  
  // size
#ifdef BUC60915
  if( !myArrowSizeIsDefined ) {
    myArrowSize = Min(myArrowSize,myVal / 5.);
  }
  arr->SetLength(myArrowSize);
#else
  if (myVal / 5. > myArrowSize) {
    arr->SetLength(myArrowSize);
  }
  else {
    arr->SetLength(myVal / 5.);
  }
#endif
  
  Standard_Real par;
  gp_Pnt curPos, Center;
  Center = myEllipse.Location();
  Standard_Boolean IsInDomain = Standard_True;
  if( myAutomaticPosition )
    {
      myEndOfArrow = AIS::NearestApex(myEllipse, myApexP, myApexN,
				      myFirstPar, myLastPar, IsInDomain);
      myPosition = Center;
      myAutomaticPosition = Standard_True;
      if ( myIsSetBndBox )
	myPosition = AIS::TranslatePointToBound(myPosition, gp_Dir(gp_Vec( Center, myPosition )),
						myBndBox );
      curPos = myPosition;  
    }
  else //!AutomaticPosition
    {
      curPos = myPosition;
//      ElSLib::Parameters ( myPlane->Pln(), curPos, U, V );
//      curPos = ElSLib::Value (U, V, myPlane->Pln());
      gp_Lin L1(myEllipse.XAxis());
      par = ElCLib::Parameter ( L1, curPos );
      curPos = ElCLib::Value (par, L1);
      if (curPos.Distance(myApexP) < curPos.Distance(myApexN)) 
	myEndOfArrow = myApexP ;
      else
	myEndOfArrow = myApexN ;
      par = ElCLib::Parameter ( myEllipse, myEndOfArrow );
      IsInDomain = AIS::InDomain( myFirstPar, myLastPar, par );
      myPosition = curPos;
    }

//  Standard_Real parEnd = ElCLib::Parameter ( myEllipse, myEndOfArrow );
  Standard_Real parStart = 0.;
  if( !IsInDomain )
    {
      if(AIS::DistanceFromApex (myEllipse, myEndOfArrow, myFirstPar) <
	 AIS::DistanceFromApex (myEllipse, myEndOfArrow, myLastPar))
	parStart = myFirstPar;
      else
	parStart = myLastPar;
    }
  if(!myIsOffset)
    DsgPrs_EllipseRadiusPresentation::Add(aPresentation, myDrawer, myVal, myText, myEllipse,
					  curPos, myEndOfArrow, Center, parStart, IsInDomain,
					  Standard_True, mySymbolPrs);
  else 
    DsgPrs_EllipseRadiusPresentation::Add(aPresentation, myDrawer, myVal, myText, myOffsetCurve,
					  curPos, myEndOfArrow, Center, parStart, IsInDomain,
					  Standard_True, mySymbolPrs);
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================

void AIS_MaxRadiusDimension::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection, 
					      const Standard_Integer /*aMode*/)
{

    gp_Pnt        center  = myEllipse.Location();
    gp_Pnt AttachmentPoint = myPosition;
    Standard_Real dist    = center.Distance(AttachmentPoint);
    Standard_Real aRadius = myVal;
    //Standard_Real inside  = Standard_False;
    gp_Pnt pt1;
    if (dist > aRadius) pt1 = AttachmentPoint; 
    else 
      pt1 = myEndOfArrow;
    Handle(SelectMgr_EntityOwner) own = new SelectMgr_EntityOwner(this,7);
    Handle(Select3D_SensitiveSegment) 
      seg = new Select3D_SensitiveSegment(own, center , pt1);
    aSelection->Add(seg);

    // Text
    Standard_Real size(Min(myVal/100.+1.e-6,myArrowSize+1.e-6));
    Handle( Select3D_SensitiveBox ) box = new Select3D_SensitiveBox( own,
								     AttachmentPoint.X(),
								     AttachmentPoint.Y(),
								     AttachmentPoint.Z(),
								     AttachmentPoint.X()+size,
								     AttachmentPoint.Y()+size,
								     AttachmentPoint.Z()+size);
  aSelection->Add(box);

  // Arc of Ellipse
    if(myIsAnArc)
      {
	
	Standard_Real parEnd = ElCLib::Parameter ( myEllipse, myEndOfArrow );
	if(!AIS::InDomain(myFirstPar, myLastPar, parEnd))
	  {
	    Standard_Real parStart, par;
	    if(AIS::DistanceFromApex (myEllipse, myEndOfArrow, myFirstPar) <
	       AIS::DistanceFromApex (myEllipse, myEndOfArrow, myLastPar))
	      par = myFirstPar;
	    else
	      par = myLastPar;
	    gp_Vec Vapex(center, ElCLib::Value( parEnd, myEllipse )) ;
	    gp_Vec Vpnt (center, ElCLib::Value( par, myEllipse )) ;
	    gp_Dir dir(Vpnt ^ Vapex);
	    if(myEllipse.Position().Direction().IsOpposite( dir, Precision::Angular())) {
	      parStart = parEnd;
	      parEnd   = par;
	    }
	    else 
	      parStart = par;

	    Handle(Geom_TrimmedCurve)TrimCurve;
	    if(myIsOffset)
	      {
		Handle(Geom_Curve) aCurve = myOffsetCurve;
		TrimCurve = new Geom_TrimmedCurve( aCurve,  parStart, parEnd );
	      }
	    else
	      {
		Handle(Geom_Ellipse) Ellipse = new Geom_Ellipse( myEllipse );
		TrimCurve = new Geom_TrimmedCurve( Ellipse,  parStart, parEnd );
	      }
	    Handle( Select3D_SensitiveCurve ) SensArc;
	    SensArc = new Select3D_SensitiveCurve( own, TrimCurve );
	    aSelection->Add( SensArc );
	  }
    }

}
