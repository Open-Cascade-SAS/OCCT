// Created on: 1998-01-20
// Created by: Julia GERASIMOVA
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

#include <AIS_EqualRadiusRelation.ixx>

#include <AIS.hxx>
#include <DsgPrs_EqualRadiusPresentation.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <TopoDS.hxx>
#include <gp_Circ.hxx>
#include <Geom_Circle.hxx>
#include <ElCLib.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <Precision.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <Prs3d_LengthAspect.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <AIS_Drawer.hxx>

//=======================================================================
//function : AIS_EqualRadiusRelation
//purpose  : 
//=======================================================================

AIS_EqualRadiusRelation::AIS_EqualRadiusRelation( const TopoDS_Edge& aFirstEdge,
						  const TopoDS_Edge& aSecondEdge,
						  const Handle( Geom_Plane )& aPlane )
     :AIS_Relation()
{
  myFShape = aFirstEdge;
  mySShape = aSecondEdge;
  myPlane  = aPlane;
}

//=======================================================================
//function : Compute
//purpose  :
//=======================================================================

void AIS_EqualRadiusRelation::Compute( const Handle( PrsMgr_PresentationManager3d )&,
				       const Handle( Prs3d_Presentation )& aPresentation,
				       const Standard_Integer ) 
{
  aPresentation->Clear();

  BRepAdaptor_Curve FirstCurve( TopoDS::Edge( myFShape ) ), SecondCurve( TopoDS::Edge( mySShape ) );

  Standard_Real FirstPar1 = FirstCurve.FirstParameter(), LastPar1 = FirstCurve.LastParameter(),
                FirstPar2 = SecondCurve.FirstParameter(), LastPar2 = SecondCurve.LastParameter();

  Handle( Geom_Curve ) FirstProjCurve = FirstCurve.Curve().Curve(),
                       SecondProjCurve = SecondCurve.Curve().Curve();
  gp_Pnt FirstPoint1, LastPoint1, FirstPoint2, LastPoint2;
  Standard_Boolean isFirstOnPlane, isSecondOnPlane;

  AIS::ComputeGeomCurve( FirstProjCurve, FirstPar1, LastPar1, FirstPoint1, LastPoint1, myPlane, isFirstOnPlane );
  AIS::ComputeGeomCurve( SecondProjCurve, FirstPar2, LastPar2, FirstPoint2, LastPoint2, myPlane, isSecondOnPlane );

  if (!isFirstOnPlane)
    ComputeProjEdgePresentation( aPresentation, TopoDS::Edge( myFShape ), FirstProjCurve, FirstPoint1, LastPoint1 );
  if (! isSecondOnPlane)
    ComputeProjEdgePresentation( aPresentation, TopoDS::Edge( mySShape ), SecondProjCurve, FirstPoint2, LastPoint2 );

  gp_Circ FirstCirc = (Handle( Geom_Circle )::DownCast( FirstProjCurve ))->Circ();
  gp_Circ SecondCirc = (Handle( Geom_Circle )::DownCast( SecondProjCurve ))->Circ();
  
  myFirstCenter = FirstCirc.Location();
  mySecondCenter = SecondCirc.Location();
  
  //ota -- begin --
  if (myAutomaticPosition)
    {
      myFirstPoint = ElCLib::Value( (FirstPar1 + LastPar1)*0.5, FirstCirc );
      mySecondPoint = ElCLib::Value( (FirstPar2 + LastPar2)*0.5, SecondCirc );
    }
  else {
    Standard_Real aPar =  ElCLib::Parameter(FirstCirc, myFirstPoint);
    if (IntegerPart(0.5*LastPar1/M_PI) != 0 &&  aPar < FirstPar1 )
      aPar +=2*M_PI*IntegerPart(0.5*LastPar1/M_PI);
    Standard_Real aRadius = FirstCirc.Radius();

    if (Abs(myFirstPoint.Distance(myFirstCenter) - aRadius) >= Precision::Confusion())
      myFirstPoint = ElCLib::Value(aPar, FirstCirc);
    if ( FirstPoint1.Distance(LastPoint1) > Precision::Confusion()){
      //check where is myFirstPoint
      if (aPar > LastPar1 || aPar < FirstPar1)
	{
	  //myFirstPoint is out of Arc of FirstCircle
	  if (FirstPoint1.Distance(myFirstPoint)< LastPoint1.Distance(myFirstPoint))
	    myFirstPoint = FirstPoint1; 
	  else
	    myFirstPoint = LastPoint1; 
	}
    }
  
    
    aPar =  ElCLib::Parameter(SecondCirc, mySecondPoint);
    if (IntegerPart(0.5*LastPar2/M_PI) != 0 &&  aPar < FirstPar2 )
      aPar +=2*M_PI*IntegerPart(0.5*LastPar2/M_PI);
    
    aRadius = SecondCirc.Radius();
    if (Abs(mySecondPoint.Distance(mySecondCenter) - aRadius) >= Precision::Confusion())
      mySecondPoint =  ElCLib::Value(aPar, SecondCirc);
    if (FirstPoint2.Distance(LastPoint2) > Precision::Confusion()){
      if (aPar > LastPar2 || aPar < FirstPar2)
	{ //mySecondPoint is out of Arc of mySecondCircle
	  if (FirstPoint2.Distance(mySecondPoint)< LastPoint2.Distance(mySecondPoint))
	    mySecondPoint = FirstPoint2; 
	  else
	    mySecondPoint = LastPoint2;
	}
    }
  }
#ifdef BUC60915
  if( !myArrowSizeIsDefined )
#endif
    myArrowSize = (Min(myFirstCenter.Distance(myFirstPoint),
		     mySecondCenter.Distance(mySecondPoint)))*0.05;
  
  Handle(Prs3d_LengthAspect) la = myDrawer->LengthAspect();
  Handle(Prs3d_ArrowAspect) arr = la->Arrow1Aspect();
  arr->SetLength(myArrowSize);
  
  //ota -- end --
  
  DsgPrs_EqualRadiusPresentation::Add(aPresentation, myDrawer, 
				      myFirstCenter, mySecondCenter, myFirstPoint, mySecondPoint, myPlane );
}

//=======================================================================
//function : Compute
//purpose  : to avoid warning at compilation (SUN)
//=======================================================================

void AIS_EqualRadiusRelation::Compute( const Handle( Prs3d_Projector )& /*aProjector*/,
				       const Handle( Prs3d_Presentation )& /*aPresentation*/)
{
// Standard_NotImplemented::Raise("AIS_EqualRadiusRelation::Compute( const Handle( Prs3d_Projector )&,const Handle( Prs3d_Presentation )& )");
// PrsMgr_PresentableObject::Compute( aProjector , aPresentation ) ;
}

//=======================================================================
//function : Compute
//purpose  : to avoid warning at compilation (SUN)
//=======================================================================

void AIS_EqualRadiusRelation::Compute( const Handle( PrsMgr_PresentationManager2d )& /*aPresentationManager2d*/,
				       const Handle( Graphic2d_GraphicObject )& /*aGraphicObject*/,
				       const Standard_Integer /*anInteger*/)
{
// Standard_NotImplemented::Raise("AIS_EqualRadiusRelation::Compute( const Handle( PrsMgr_PresentationManager2d )&,const Handle( Graphic2d_GraphicObject )&,const Standard_Integer )");
// PrsMgr_PresentableObject::Compute( aPresentationManager2d ,aGraphicObject,anInteger) ;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_EqualRadiusRelation::Compute(const Handle_Prs3d_Projector& aProjector,
                                      const Handle_Geom_Transformation& aTransformation,
                                      const Handle_Prs3d_Presentation& aPresentation)
{
// Standard_NotImplemented::Raise("AIS_EqualRadiusRelation::Compute(const Handle_Prs3d_Projector&, const Handle_Geom_Transformation&, const Handle_Prs3d_Presentation&)");
 PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation ) ;
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================

void AIS_EqualRadiusRelation::ComputeSelection( const Handle( SelectMgr_Selection )& aSelection,
					        const Standard_Integer ) 
{
  Handle( SelectMgr_EntityOwner ) own = new SelectMgr_EntityOwner( this, 7 );
  Handle( Select3D_SensitiveSegment ) seg;

  seg = new Select3D_SensitiveSegment( own, myFirstCenter, myFirstPoint );
  aSelection->Add( seg );
  
  if(!myAutomaticPosition) 
    ComputeRadiusPosition();
  
  seg = new Select3D_SensitiveSegment( own, mySecondCenter, mySecondPoint );
  aSelection->Add( seg );
  
  seg = new Select3D_SensitiveSegment( own, myFirstCenter, mySecondCenter );
  aSelection->Add( seg );
  
  
  // Two small lines
  gp_Pnt Middle( (myFirstCenter.XYZ() + mySecondCenter.XYZ())*0.5 );

  Standard_Real SmallDist = .001;
  //Should be changed as the domain of small lines could be changed.
  Handle( Select3D_SensitiveBox ) box = new Select3D_SensitiveBox(own,
								  Middle.X() - SmallDist,
								  Middle.Y() - SmallDist,
								  Middle.Z() - SmallDist,
								  Middle.X() + SmallDist,
								  Middle.Y() + SmallDist,
								  Middle.Z() + SmallDist );
  aSelection->Add(box);
}

//=================================================================
//function : ComputeRadiusPosition
//purpose  :
//=================================================================
void AIS_EqualRadiusRelation::ComputeRadiusPosition() 
{
  if (myAutomaticPosition ||
      myFirstCenter.Distance(myPosition) < Precision::Confusion() ||
      mySecondCenter.Distance(myPosition) <  Precision::Confusion())
      return;

  gp_Pnt aPosition;

  //project myPosition to the plane of constraint
  GeomAPI_ProjectPointOnSurf aProj(myPosition, myPlane);
  aPosition =  aProj.NearestPoint();
		
  Standard_Real aDist1 = myFirstPoint.Distance(aPosition);
  Standard_Real aDist2 = mySecondPoint.Distance(aPosition);
  
  if(aDist1<aDist2)
    {
      Standard_Real Rad1 = myFirstPoint.Distance(myFirstCenter);
      const gp_Dir aNewDir1(aPosition.XYZ() - myFirstCenter.XYZ());
      const gp_Vec aTVec (aNewDir1.XYZ()*Rad1); 
      myFirstPoint = myFirstCenter.Translated(aTVec);
    }
  else {
    Standard_Real Rad2 = mySecondPoint.Distance(mySecondCenter);
    const gp_Dir aNewDir2(aPosition.XYZ() - mySecondCenter.XYZ());
    gp_Vec aTVec (aNewDir2.XYZ()*Rad2); 
    mySecondPoint = mySecondCenter.Translated(aTVec);
  }
  
}

