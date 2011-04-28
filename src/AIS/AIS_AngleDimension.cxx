// File:	AIS_AngleDimension.cdl
// Created:	Tue Dec  5 15:09:04 1996
// Author:	Arnaud BOUZY/Odile Olivier
//              <ODL>

#define BUC60655	//GG 22/03/00 Enable to compute correctly
//			the arrow size at object creation time.

#define BUC60915        //GG 05/06/01 Enable to compute the requested arrow size
//                      if any in all dimensions.

#include <Standard_NotImplemented.hxx>

#include <AIS_AngleDimension.ixx>

#include <AIS.hxx>
#include <AIS_DimensionOwner.hxx>
#include <AIS_Drawer.hxx>

#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRep_Tool.hxx>

#include <DsgPrs.hxx>
#include <DsgPrs_AnglePresentation.hxx>

#include <ElCLib.hxx>
#include <ElSLib.hxx>

#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <GeomAPI.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_OffsetSurface.hxx>

#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <IntAna_QuadQuadGeo.hxx>
#include <IntAna_ResultType.hxx>

#include <Precision.hxx>

#include <ProjLib.hxx>

#include <Prs3d_AngleAspect.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_Drawer.hxx>

#include <Select3D_SensitiveCurve.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <SelectMgr_EntityOwner.hxx>

#include <TColStd_Array1OfReal.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

#include <UnitsAPI.hxx>

#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Lin.hxx>
#include <gp_Cone.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx> 
#include <gp_XYZ.hxx>

#include <GC_MakeCircle.hxx>
#include <GC_MakeConicalSurface.hxx>
#include <gce_MakePln.hxx>
#include <gce_MakeCone.hxx>
#include <Graphic3d_Array1OfVertex.hxx>  

//=======================================================================
//function : Constructor
//purpose  : ConeAngle dimension 
//=======================================================================

AIS_AngleDimension::AIS_AngleDimension( const TopoDS_Face& aCone, 
					const Standard_Real aVal,
				        const TCollection_ExtendedString& aText,
					const gp_Pnt& aPosition,
					const DsgPrs_ArrowSide /*aSymbolPrs*/,
					const Standard_Real anArrowSize):
myNbShape(1)
{
  myCone = aCone;
  myVal = aVal;
  myText = aText;
  myPosition = aPosition; 
  mySymbolPrs = DsgPrs_AS_BOTHAR;
  myAutomaticPosition = Standard_True;
#ifdef BUC60915
  SetArrowSize( anArrowSize );
#else
  myArrowSize = anArrowSize;
#endif
}

//=======================================================================
//function : Constructor
//purpose  : ConeAngle dimension 
//=======================================================================

AIS_AngleDimension::AIS_AngleDimension( const TopoDS_Face& aCone, 
					const Standard_Real aVal,
					const TCollection_ExtendedString& aText):
myNbShape(1)
{
//#ifdef DEB
  cout << "Call new AngleDimension for cone's angle" << endl;
//#endif

  gp_Pnt tmpPnt(0., 0., 0.); 

  myCone = aCone;
  myVal = aVal;
  myText = aText;
  myPosition = tmpPnt; 
  mySymbolPrs = DsgPrs_AS_BOTHAR;
  myAutomaticPosition = Standard_True;
 
  myArrowSize = 0.0;

}


//=======================================================================
//function : Constructor
//purpose  : TwoEdgesAngle dimension 
//=======================================================================


AIS_AngleDimension::AIS_AngleDimension(const TopoDS_Edge& aFirstEdge,
				       const TopoDS_Edge& aSecondEdge,
				       const Handle (Geom_Plane)& aPlane, 
				       const Standard_Real aVal, 
				       const TCollection_ExtendedString& aText)
:AIS_Relation(),
 myNbShape(2)
{
#ifdef DEB
  cout << endl << "Call new AngleDimension for edges, default" << endl;
#endif

  myFShape = aFirstEdge;
  mySShape = aSecondEdge;
  myVal = aVal;
  myPlane = aPlane;
  myText = aText;
  mySymbolPrs = DsgPrs_AS_BOTHAR;
  myAutomaticPosition = Standard_True;

  myArrowSize = myVal / 100.;
}

//=======================================================================
//function : Constructor
//purpose  : TwoEdgesAngle dimension (avec position et texte)
//=======================================================================

AIS_AngleDimension::AIS_AngleDimension( const TopoDS_Edge& aFirstEdge, 
					const TopoDS_Edge& aSecondEdge, 
					const Handle (Geom_Plane)& aPlane,  
					const Standard_Real aVal, 
					const TCollection_ExtendedString& aText, 
					const gp_Pnt& aPosition,
					const DsgPrs_ArrowSide aSymbolPrs,
					const Standard_Real anArrowSize):
myNbShape(2)
{
#ifdef DEB
  cout << endl << "Call new AngleDimension for edges" << endl;
#endif

  myFShape = aFirstEdge;
  mySShape = aSecondEdge;
  myVal = aVal;
  myPlane = aPlane;
  myText = aText;
  mySymbolPrs = aSymbolPrs;
  myAutomaticPosition = Standard_False;
#ifdef BUC60915
  SetArrowSize( anArrowSize );
#else
  myArrowSize = anArrowSize;
#endif
  myPosition = aPosition;

}

//=======================================================================
//function : Constructor
//purpose  : TwoPlanarFacesAngle dimension 
//=======================================================================

AIS_AngleDimension::AIS_AngleDimension( const TopoDS_Face& aFirstFace, 
					const TopoDS_Face& aSecondFace, 
					const gp_Ax1& anAxis, 
					const Standard_Real aVal,
					const TCollection_ExtendedString& aText):
myNbShape(2),
myAxis(anAxis)
{
#ifdef DEB
  cout << endl << "Call new AngleDimension for planar faces, default" << endl;
#endif

  myFShape = aFirstFace;
  mySShape = aSecondFace;

  AIS::GetPlaneFromFace( aFirstFace, myFirstPlane, myFirstBasisSurf, myFirstSurfType, myFirstOffset );
  AIS::GetPlaneFromFace( aSecondFace, mySecondPlane, mySecondBasisSurf, mySecondSurfType, mySecondOffset );

//POP init champ myPlane
  myPlane = new Geom_Plane(myFirstPlane);

  myVal = aVal;
  myText = aText;
  mySymbolPrs = DsgPrs_AS_BOTHAR;
  myAutomaticPosition = Standard_True;

  myArrowSize = myVal / 100.;
}

//=======================================================================
//function : Constructor
//purpose  : TwoPlanarFacesAngle dimension  (avec position et texte)
//=======================================================================

AIS_AngleDimension::AIS_AngleDimension( const TopoDS_Face& aFirstFace, 
					const TopoDS_Face& aSecondFace, 
					const gp_Ax1& anAxis, 
					const Standard_Real aVal, 
					const TCollection_ExtendedString& aText, 
					const gp_Pnt& aPosition, 
					const DsgPrs_ArrowSide aSymbolPrs,
					const Standard_Real anArrowSize):
myNbShape(2),
myAxis(anAxis)
{
#ifdef DEB
  cout << endl << "Call new AngleDimension for planar faces" << endl;
#endif

  myFShape = aFirstFace;
  mySShape = aSecondFace;

  AIS::GetPlaneFromFace( aFirstFace, myFirstPlane, myFirstBasisSurf, myFirstSurfType, myFirstOffset );
  AIS::GetPlaneFromFace( aSecondFace, mySecondPlane, mySecondBasisSurf, mySecondSurfType, mySecondOffset );

//POP init champ myPlane
  myPlane = new Geom_Plane(myFirstPlane);

  myVal = aVal;
  myText = aText;
  mySymbolPrs = aSymbolPrs;
  myAutomaticPosition = Standard_False;
#ifdef BUC60915
  SetArrowSize( anArrowSize );
#else
  myArrowSize = anArrowSize;
#endif
  myPosition = aPosition;
}


//=======================================================================
//function : AIS_AngleDimension
//purpose  : Two curvilinear faces dimension
//=======================================================================

AIS_AngleDimension::AIS_AngleDimension( const TopoDS_Face& aFFace, 
					const TopoDS_Face& aSFace, 
					const Standard_Real aVal,
					const TCollection_ExtendedString& aText ):
myNbShape(2)
{
#ifdef DEB
  cout << endl << "Call new AngleDimension for curvilinear faces, default" << endl;
#endif

  SetFirstShape( aFFace );
  SetSecondShape( aSFace );
  myVal = aVal;

  myText = aText;
  mySymbolPrs = DsgPrs_AS_BOTHAR;
  myAutomaticPosition = Standard_True;

  myArrowSize = myVal / 100.;
}

//=======================================================================
//function : AIS_AngleDimension
//purpose  : 
//=======================================================================

AIS_AngleDimension::AIS_AngleDimension( const TopoDS_Face& aFFace, 
					const TopoDS_Face& aSFace, 
					const Standard_Real aVal,
					const TCollection_ExtendedString& aText,
					const gp_Pnt& aPosition, 
					const DsgPrs_ArrowSide aSymbolPrs,
					const Standard_Real anArrowSize):
myNbShape(2)
{
#ifdef DEB
  cout << endl << "Call new AngleDimension for curvilinear faces" << endl;
#endif

  SetFirstShape( aFFace );
  SetSecondShape( aSFace );
  myVal = aVal;

  myText = aText;
  mySymbolPrs = DsgPrs_AS_BOTHAR;
  myAutomaticPosition = Standard_True;

  mySymbolPrs = aSymbolPrs;
  myAutomaticPosition = Standard_False;
#ifdef BUC60915
  SetArrowSize( anArrowSize );
#else
  myArrowSize = anArrowSize;
#endif
  myPosition = aPosition;
}


//=======================================================================
//function : SetConeFace
//purpose  : 
//=======================================================================

void AIS_AngleDimension::SetConeFace( const TopoDS_Face& aConeFace )
{
  myCone = aConeFace;
  myAutomaticPosition = Standard_True;
}


//=======================================================================
//function : SetFirstShape
//purpose  : 
//=======================================================================

void AIS_AngleDimension::SetFirstShape( const TopoDS_Shape& aFShape )
{
  myFShape = aFShape;

  if (myFShape.ShapeType() == TopAbs_FACE)
    {
      AIS::GetPlaneFromFace( TopoDS::Face( myFShape ),
			     myFirstPlane,
			     myFirstBasisSurf,
			     myFirstSurfType,
			     myFirstOffset );

      if (myFirstSurfType == AIS_KOS_Cylinder)
	myAxis = (Handle( Geom_CylindricalSurface )::DownCast( myFirstBasisSurf ))->Cylinder().Axis();
      else if (myFirstSurfType == AIS_KOS_Cone)
	myAxis = (Handle( Geom_ConicalSurface )::DownCast( myFirstBasisSurf ))->Cone().Axis();
      else if (myFirstSurfType == AIS_KOS_Revolution)
	myAxis = (Handle( Geom_SurfaceOfRevolution )::DownCast( myFirstBasisSurf ))->Axis();
      else if (myFirstSurfType == AIS_KOS_Extrusion)
	{
	  myAxis.SetDirection((Handle( Geom_SurfaceOfLinearExtrusion )::DownCast( myFirstBasisSurf ))
			      ->Direction() );
	  //myAxis.SetLocation( ??? );
	}
    }
}

//=======================================================================
//function : SetSecondShape
//purpose  : 
//=======================================================================

void AIS_AngleDimension::SetSecondShape( const TopoDS_Shape& aSShape )
{
  mySShape = aSShape;

  if (myFShape.ShapeType() == TopAbs_FACE)
    AIS::GetPlaneFromFace( TopoDS::Face( mySShape ),
			   mySecondPlane,
			   mySecondBasisSurf,
			   mySecondSurfType,
			   mySecondOffset );
}




///=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_AngleDimension::Compute(const Handle(PrsMgr_PresentationManager3d)&, 
				 const Handle(Prs3d_Presentation)& aPresentation, 
				 const Standard_Integer)
{
  aPresentation->Clear();

  if( myNbShape == 1 ) 
    {
     //  cout << "Computing for cone' angle "   << endl;
     ComputeConeAngle(aPresentation);
     return;
    }
  switch (myFShape.ShapeType()) {
  case TopAbs_FACE :
    {
      // cas angle entre deux faces
      ComputeTwoFacesAngle(aPresentation);
    }
    break;
  case TopAbs_EDGE :
    {
      // cas angle entre deux edges
      ComputeTwoEdgesAngle(aPresentation);
    }
    break;
  default:
    break;
  }
  
}

//=======================================================================
//function : Compute
//purpose  : : to avoid warning
//=======================================================================

void AIS_AngleDimension::Compute(const Handle(Prs3d_Projector)& aProjector, 
				 const Handle(Prs3d_Presentation)& aPresentation)
{
// Standard_NotImplemented::Raise("AIS_AngleDimension::Compute(const Handle(Prs3d_Projector)&,const Handle(Prs3d_Presentation)&)");
 PrsMgr_PresentableObject::Compute( aProjector , aPresentation ) ;
}

//=======================================================================
//function : Compute
//purpose  : : to avoid warning
//=======================================================================

void AIS_AngleDimension::Compute(const Handle(PrsMgr_PresentationManager2d)& aPresentationManager2d, 
				 const Handle(Graphic2d_GraphicObject)& aGraphicObject, 
				 const Standard_Integer anInteger)
{
// Standard_NotImplemented::Raise("AIS_AngleDimension::Compute(const Handle(PrsMgr_PresentationManager2d)&,const Handle(Graphic2d_GraphicObject)&,const Standard_Integer)");
 PrsMgr_PresentableObject::Compute( aPresentationManager2d ,aGraphicObject,anInteger) ;
}

void AIS_AngleDimension::Compute(const Handle_Prs3d_Projector& aProjector, const Handle_Geom_Transformation& aTransformation, const Handle_Prs3d_Presentation& aPresentation)
{
// Standard_NotImplemented::Raise("AIS_AngleDimension::Compute(const Handle_Prs3d_Projector&, const Handle_Geom_Transformation&, const Handle_Prs3d_Presentation&)");
 PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation ) ;
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================

void AIS_AngleDimension::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection, 
					  const Standard_Integer)
{

  if ( myNbShape == 1 ) 
    {
    // cout << "Computing selection for cone's angle "   << endl;
     ComputeConeAngleSelection(aSelection);
     return;
    }


  if (myFShape.IsNull()) return;

  if (myFShape.ShapeType() == TopAbs_FACE )
    Compute3DSelection(aSelection);
  else
    Compute2DSelection(aSelection);

  // Text
  Handle( SelectMgr_EntityOwner ) own = new SelectMgr_EntityOwner( this, 7 );
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

//=======================================================================
//function : ComputeConeAngle
//purpose  : 
//=======================================================================

void AIS_AngleDimension::ComputeConeAngle(const Handle(Prs3d_Presentation)& aPresentation)
{
  if( myCone.IsNull() ) return;
  
  gp_Pln aPln;
  gp_Cone aCone;
  gp_Circ myCircle;
  gp_Pnt Apex;
  Handle( Geom_Surface ) aSurf;         //a surface from the Face
  Handle( Geom_OffsetSurface ) aOffsetSurf; 
  Handle( Geom_ConicalSurface ) aConicalSurf;
  Handle( Geom_SurfaceOfRevolution ) aRevSurf; 
  Handle( Geom_Line ) aLine;
  BRepAdaptor_Surface tmpSurf(myCone);  
  TopoDS_Face aFace;
  AIS_KindOfSurface aSurfType;
  Standard_Real Offset = 0. ;
  Handle( Standard_Type ) aType;

  Standard_Real maxV = tmpSurf.FirstVParameter();
  Standard_Real minV = tmpSurf.LastVParameter();


 AIS::GetPlaneFromFace( myCone, aPln, aSurf, aSurfType, Offset );

 if ( aSurfType == AIS_KOS_Revolution ) {                                    //surface of revolution

   aRevSurf = Handle( Geom_SurfaceOfRevolution )::DownCast( aSurf ); 
   gp_Lin ln( aRevSurf->Axis() );
   Handle( Geom_Curve ) tmpCrv = aRevSurf->BasisCurve();
   if ( tmpCrv ->DynamicType() != STANDARD_TYPE(Geom_Line) )  return;        //Must be a part of line

   Standard_Real par;
   gp_Pnt fst = tmpSurf.Value(0., minV);
   gp_Pnt lst = tmpSurf.Value(0., maxV);
 
   gp_Vec vec1(fst, lst);
   
   par = ElCLib::Parameter( ln, fst );
   gp_Pnt fst2 = ElCLib::Value( par, ln );                         //projection fst on ln
   par = ElCLib::Parameter( ln, lst );
   gp_Pnt lst2 = ElCLib::Value( par, ln );                         //projection lst on ln

   gp_Vec vec2(fst2, lst2);

   // Check if two parts of revolution are parallel ( it's a cylinder ) or normal (it's a circle ) 
  if( vec1.IsParallel( vec2,Precision::Angular() ) || vec1.IsNormal( vec2,Precision::Angular() ) ) return; 
 
  gce_MakeCone mkCone(aRevSurf->Axis(), fst, lst);
  aCone =  mkCone.Value();
  Apex = aCone.Apex();
 }
 else {  
   aType = aSurf->DynamicType();
   if ( aType == STANDARD_TYPE(Geom_OffsetSurface) || Offset > 0.01 ) {            //offset surface
     aOffsetSurf = new Geom_OffsetSurface (aSurf, Offset);
     aSurf = aOffsetSurf->Surface();
     BRepBuilderAPI_MakeFace mkFace(aSurf);
     mkFace.Build();
     if( !mkFace.IsDone() ) return;
     tmpSurf.Initialize( mkFace.Face() );
   }  
 
   aCone = tmpSurf.Cone();
   aConicalSurf = Handle( Geom_ConicalSurface)::DownCast( aSurf );
   Apex =  aConicalSurf->Apex();
 }
 
   Handle(Geom_Curve) aCurve;                 //A circle where the angle is drawn
   if ( myAutomaticPosition ) {
    Standard_Real midV = ( minV + maxV ) / 2.5; 

    aCurve =   aSurf->VIso(midV);
    myCircle = Handle(Geom_Circle)::DownCast(aCurve)->Circ();

    myPosition = ElCLib::Value(Standard_PI/2.0, myCircle);
    myAutomaticPosition = Standard_False;
  }
  else {
    Standard_Real U, V;
    ElSLib::Parameters(aCone, myPosition, U, V);
    aCurve = aSurf->VIso(V); 
    myCircle = Handle(Geom_Circle)::DownCast(aCurve)->Circ();
  }
  
 //__________________________________________________________________
  aCurve = aSurf->VIso(maxV);
  gp_Circ CircVmax = Handle(Geom_Circle)::DownCast(aCurve)->Circ();
  aCurve = aSurf->VIso(minV);
  gp_Circ CircVmin = Handle(Geom_Circle)::DownCast(aCurve)->Circ();
 //__________________________________________________________________

  if( CircVmax.Radius() < CircVmin.Radius() ) {
   gp_Circ tmpCirc = CircVmax;
   CircVmax = CircVmin;
   CircVmin = tmpCirc;
  }

  DsgPrs_AnglePresentation::Add(aPresentation, myDrawer, myVal, 
				myText, myCircle, myPosition, Apex, CircVmin, CircVmax, myArrowSize);
// cout << "ComputeConeAngle is over"   << endl;				
}


//=======================================================================
//function : ComputeTwoFacesAngle
//purpose  : 
//=======================================================================

void AIS_AngleDimension::ComputeTwoFacesAngle(const Handle(Prs3d_Presentation)& aPresentation)
{
  if (myFirstSurfType == AIS_KOS_Plane)
    ComputeTwoPlanarFacesAngle( aPresentation );
  else
    ComputeTwoCurvilinearFacesAngle( aPresentation );
}

//=======================================================================
//function : ComputeTwoCurvilinearFacesAngle
//purpose  : 
//=======================================================================

void AIS_AngleDimension::ComputeTwoCurvilinearFacesAngle(const Handle(Prs3d_Presentation)& aPresentation) 
{
  AIS::ComputeAngleBetweenCurvilinearFaces( TopoDS::Face( myFShape ),
					    TopoDS::Face( mySShape ),
					    myFirstBasisSurf,
					    mySecondBasisSurf,
					    myFirstSurfType,
					    mySecondSurfType,
					    myAxis,
					    myVal,
					    myAutomaticPosition,
					    myPosition,
					    myCenter,
					    myFAttach,
					    mySAttach,
					    myFDir,
					    mySDir,
					    myPlane );
  if (myAutomaticPosition && myIsSetBndBox)
    myPosition = AIS::TranslatePointToBound( myPosition, gp_Dir( gp_Vec( myCenter, myPosition ) ), myBndBox );

  Handle(Prs3d_AngleAspect) la = myDrawer->AngleAspect();
  Handle(Prs3d_ArrowAspect) arr = la->ArrowAspect();
#ifdef BUC60915
  if( !myArrowSizeIsDefined ) {
#endif
    Standard_Real arrsize = myCenter.Distance( myPosition );
      
    if ( (myArrowSize-arrsize) < 0.1 ) arrsize = myArrowSize;
    if (arrsize == 0.) arrsize = 1.;
#ifdef BUC60915
    myArrowSize = arrsize;
  }
  arr->SetLength( myArrowSize );
#else
  arr->SetLength(arrsize);
#endif
      

  if (myVal <= Precision::Angular() || Abs( PI-myVal ) <= Precision::Angular())
    DsgPrs_AnglePresentation::Add(aPresentation,
				  myDrawer,
				  myVal,
				  myText,
				  myCenter,
				  myFAttach,
				  mySAttach,
				  myFDir,
				  mySDir,
				  myPlane->Pln().Axis().Direction(),
				  Standard_False, // not plane
				  myAxis,
				  myPosition,
				  mySymbolPrs);
  else
    DsgPrs_AnglePresentation::Add(aPresentation,
				  myDrawer,
				  myVal,
				  myText,
				  myCenter,
				  myFAttach,
				  mySAttach,
				  myFDir,
				  mySDir,
				  myFDir ^ mySDir,
				  Standard_False, // not plane
				  myAxis,
				  myPosition,
				  mySymbolPrs);
}

//=======================================================================
//function : ComputeTwoPlanarFacesAngle
//purpose  : 
//=======================================================================

void AIS_AngleDimension::ComputeTwoPlanarFacesAngle( const Handle( Prs3d_Presentation )& aPresentation )
{
  AIS::ComputeAngleBetweenPlanarFaces( TopoDS::Face( myFShape ),
				       TopoDS::Face( mySShape ),
				       mySecondBasisSurf,
				       myAxis,
				       myVal,
				       myAutomaticPosition,
				       myPosition,
				       myCenter,
				       myFAttach,
				       mySAttach,
				       myFDir,
				       mySDir );
  if (myAutomaticPosition && myIsSetBndBox)
    myPosition = AIS::TranslatePointToBound( myPosition, gp_Dir( gp_Vec( myCenter, myPosition ) ), myBndBox );

  Handle(Prs3d_AngleAspect) la = myDrawer->AngleAspect();
  Handle(Prs3d_ArrowAspect) arr = la->ArrowAspect();
#ifdef BUC60915
  if( !myArrowSizeIsDefined ) {
#endif
    Standard_Real arrsize = myCenter.Distance( myPosition );
      
    if ( (myArrowSize-arrsize) < 0.1 ) arrsize = myArrowSize;
    if (arrsize == 0.) arrsize = 1.;
#ifdef BUC60915
    myArrowSize = arrsize;
  }
  arr->SetLength( myArrowSize );
#else
  arr->SetLength(arrsize);
#endif
      

  DsgPrs_AnglePresentation::Add(aPresentation,
				myDrawer,
				myVal,
				myText,
				myCenter,
				myFAttach,
				mySAttach,
				myFDir,
				mySDir,
				myAxis.Direction(),
				Standard_True,
				myAxis,
				myPosition,
				mySymbolPrs);
     
}

//=======================================================================
//function : ComputeTwoEdgesAngle
//purpose  : 
//=======================================================================

void AIS_AngleDimension::ComputeTwoEdgesAngle(const Handle(Prs3d_Presentation)& aPresentation)
{
  BRepAdaptor_Curve cu1(TopoDS::Edge(myFShape));
  BRepAdaptor_Curve cu2(TopoDS::Edge(mySShape));
  if ((cu1.GetType() != GeomAbs_Line) || (cu2.GetType() != GeomAbs_Line)) return;

  // current face
  BRepBuilderAPI_MakeFace makeface(myPlane->Pln());
  TopoDS_Face face(makeface.Face());  
  BRepAdaptor_Surface adp(makeface.Face());
    
  // 3d lines
  Handle(Geom_Line) geom_lin1,geom_lin2;
  gp_Pnt ptat11,ptat12,ptat21,ptat22;//,pint3d;
  Standard_Boolean isInfinite1,isInfinite2;
  Handle(Geom_Curve) extCurv;
  Standard_Integer copyOfMyExtShape = myExtShape;
  if (!AIS::ComputeGeometry(TopoDS::Edge(myFShape),
			    TopoDS::Edge(mySShape),
			    myExtShape,
			    geom_lin1,
			    geom_lin2,
			    ptat11,
			    ptat12,
			    ptat21,
			    ptat22,
			    extCurv,
			    isInfinite1,
			    isInfinite2,
			    myPlane)) {
    return;
  }
  // Temporary: computation of myVal
  //  myVal = Abs(geom_lin1->Lin().Angle( geom_lin2->Lin())); // Pb with angles JPR

  if (copyOfMyExtShape != 0) myExtShape = copyOfMyExtShape;  

  // 2d lines => projection of 3d on current plane

//POP pour NT
  Handle(Geom2d_Curve) geoC1 = GeomAPI::To2d(geom_lin1,myPlane->Pln());
  Handle(Geom2d_Line) lin1_2d = *((Handle(Geom2d_Line)*)& geoC1);
  Handle(Geom2d_Curve) geoC2 = GeomAPI::To2d(geom_lin2,myPlane->Pln());
  Handle(Geom2d_Line) lin2_2d = *((Handle(Geom2d_Line)*)& geoC2);

#ifdef BUC60915
  if( !myArrowSizeIsDefined ) {
#endif
    Standard_Real arrSize1(myArrowSize),arrSize2(myArrowSize);
    if (!isInfinite1) arrSize1 = ptat11.Distance(ptat12)/100.;
    if (!isInfinite2) arrSize2 = ptat21.Distance(ptat22)/100.;
#ifdef BUC60655
    myArrowSize = Min(myArrowSize,Max(arrSize1,arrSize2));
#else
    myArrowSize = Min(myArrowSize,Min(arrSize1,arrSize2));
#endif
#ifdef BUC60915
  }
#endif


  // Processing in  case of 2 parallel straight lines
  if (lin1_2d->Lin2d().Direction()
      .IsParallel(lin2_2d->Lin2d().Direction(),Precision::Angular())) {    
    ComputeTwoEdgesNullAngle(aPresentation,
			     geom_lin1,
			     geom_lin2,
			     ptat11,ptat12,
			     ptat21,ptat22,
			     isInfinite1,isInfinite2);
  }
  
  // Processing in case of 2 non-parallel straight lines
  else {
    ComputeTwoEdgesNotNullAngle(aPresentation,
				geom_lin1,
				geom_lin2,
				ptat11,
				ptat12,
				ptat21,
				ptat22,
				isInfinite1,isInfinite2);
  }
  if ( (myExtShape != 0) &&  !extCurv.IsNull()) {
    gp_Pnt pf, pl;
    if ( myExtShape == 1 ) {
      if (!isInfinite1) {
	pf = ptat11; 
	pl = ptat12;
      }
      aPresentation->SetInfiniteState(isInfinite1);
      ComputeProjEdgePresentation(aPresentation,TopoDS::Edge(myFShape),geom_lin1,pf,pl);
    }
    else {
      if (!isInfinite2) {
	pf = ptat21; 
	pl = ptat22;
      }
      aPresentation->SetInfiniteState(isInfinite2);
      ComputeProjEdgePresentation(aPresentation,TopoDS::Edge(mySShape),geom_lin2,pf,pl);
    }
  }
}


//=======================================================================
//function : ComputeTwoEdgesNotNullAngle
//purpose  : 
//=======================================================================

void AIS_AngleDimension::ComputeTwoEdgesNotNullAngle(const Handle(Prs3d_Presentation)& aPresentation,
						     const Handle(Geom_Line)& l1,
						     const Handle(Geom_Line)& l2,
						     const gp_Pnt& ptat11,
						     const gp_Pnt& ptat12,
						     const gp_Pnt& ptat21,
						     const gp_Pnt& ptat22,
						     const Standard_Boolean isInfinite1,
						     const Standard_Boolean isInfinite2)
{
  // current face
  BRepBuilderAPI_MakeFace makeface(myPlane->Pln());
  TopoDS_Face face(makeface.Face());  
  BRepAdaptor_Surface adp(makeface.Face());
  // 2d lines => projection of 3d on current plane
  Handle(Geom2d_Curve) geoC1 = GeomAPI::To2d(l1,myPlane->Pln());
  const Handle(Geom2d_Line)& l1_2d = *((Handle(Geom2d_Line)*)& geoC1);
  Handle(Geom2d_Curve) geoC2 = GeomAPI::To2d(l2,myPlane->Pln());
  const Handle(Geom2d_Line)& l2_2d = *((Handle(Geom2d_Line)*)& geoC2);

  //----------------------------------------------------------
  //          Computation of myCenter
  //----------------------------------------------------------
  IntAna2d_AnaIntersection inter(l1_2d->Lin2d(),l2_2d->Lin2d());
  if (!inter.IsDone()) return;
  if (!inter.NbPoints()) return;
  
  gp_Pnt2d pint(inter.Point(1).Value());
  myCenter = adp.Value(pint.X(),pint.Y());

  //----------------------------------------------------------
  //         Computation of the 2 directions
  //----------------------------------------------------------
  gp_Dir d1,d2;
  if (!isInfinite1) {
    if (myCenter.SquareDistance(ptat11) > myCenter.SquareDistance(ptat12)) d1 = gp_Dir(gp_Vec(myCenter,ptat11));
    else d1 = gp_Dir(gp_Vec(myCenter,ptat12));  
  }
  else d1 = l1->Lin().Direction();

  if (!isInfinite2) {
    if (myCenter.SquareDistance(ptat21) > myCenter.SquareDistance(ptat22)) d2 = gp_Dir(gp_Vec(myCenter,ptat21));
    else d2 = gp_Dir(gp_Vec(myCenter,ptat22));
  }
  else d2 = l2->Lin().Direction();
  if (!isInfinite1) {
    Standard_Boolean In1(Standard_False);
    Standard_Boolean In2(Standard_False);
    if ( !(Abs(d1.Angle(d2) - Abs(myVal)) <= Precision::Confusion())
	 &&  (Abs(myVal) <  PI) ) {
      Standard_Real parcent1 = ElCLib::Parameter(l1->Lin(), myCenter);
      Standard_Real par11 = ElCLib::Parameter(l1->Lin(), ptat11);
      Standard_Real par12 = ElCLib::Parameter(l1->Lin(), ptat12);
      if ( par11 < par12) {
	if ( ( parcent1> par11) && (parcent1< par12)) {
	  In1 = Standard_True;
	  d1.Reverse();
	}
      }
      else {
      if ( ( parcent1> par12) && (parcent1< par11)) {
	In1 = Standard_True;
	d1.Reverse();
      }
      }
      if ( !In1) {
	In2 = Standard_True;
	d2.Reverse();
      }
    }
  }

  myFDir = d1;
  mySDir = d2;
  gp_Lin theaxis;
  gp_Lin gpl1 = l1->Lin();
  gp_Lin gpl2 = l2->Lin();
  theaxis = gp_Lin(myCenter,myFDir^mySDir);

  if (myVal >  PI) {
    theaxis.Reverse();
  }
  
  gp_Pnt curpos;  
  TColStd_Array1OfReal tabdist(1,4);
  if (!isInfinite1) { 
    tabdist(1) = theaxis.Distance(ptat11);
    tabdist(2) = theaxis.Distance(ptat12);
  }
  else {
    tabdist(1) = tabdist(2) = 0.;
  }

  if (!isInfinite2) { 
    tabdist(3) = theaxis.Distance(ptat21);
    tabdist(4) = theaxis.Distance(ptat22);
  }
  else {
    tabdist(3) = tabdist(4) = 0.;
  }

  if (myAutomaticPosition) {
    Standard_Real length_1(RealLast());
    if (!isInfinite1) length_1 = .75*Abs(tabdist(2)-tabdist(1))+Min(tabdist(1),tabdist(2));

    Standard_Real length_2(RealLast());
    if (!isInfinite2) length_2 = .75*Abs(tabdist(4)-tabdist(3))+Min(tabdist(3),tabdist(4));
    Standard_Real theLength(Min(length_1,length_2));
    if (Precision::IsInfinite(theLength)) theLength = 50.;

    myFAttach = myCenter.Translated(gp_Vec(d1)*theLength);
    mySAttach = myCenter.Translated(gp_Vec(d2)*theLength);
       
    if (!isInfinite1) {
      Standard_Real par_p1_attach(ElCLib::Parameter(gpl1,myFAttach));
      Standard_Real par11 = ElCLib::Parameter(gpl1,ptat11);
      Standard_Real par12 = ElCLib::Parameter(gpl1,ptat12);
      if (par_p1_attach > par11 && par_p1_attach > par12) {
	par_p1_attach = Max(par11,par12);
	myFAttach = ElCLib::Value(par_p1_attach,gpl1);
      }
      else if (par_p1_attach < par11 && par_p1_attach < par12) {
	par_p1_attach = Min(par11,par12);
	myFAttach = ElCLib::Value(par_p1_attach,gpl1);
      }
    }

    if (!isInfinite2) {
      Standard_Real par_p2_attach(ElCLib::Parameter(gpl2,mySAttach));
      Standard_Real par21 = ElCLib::Parameter(gpl2,ptat21);
      Standard_Real par22 = ElCLib::Parameter(gpl2,ptat22);
      if (par_p2_attach > par21 && par_p2_attach > par22) {
	par_p2_attach = Max(par21,par22);
	mySAttach = ElCLib::Value(par_p2_attach,gpl2);
      }
      else if (par_p2_attach < par21 && par_p2_attach < par22) {
	par_p2_attach = Min(par21,par22);
	mySAttach = ElCLib::Value(par_p2_attach,gpl2);
      }
    }
    if ( myVal < PI) curpos.SetXYZ(.5*(myFAttach.XYZ()+mySAttach.XYZ())); 
    else {
      curpos.SetXYZ(.5*(myFAttach.XYZ()+mySAttach.XYZ())); 
      gp_Vec transl(curpos, myCenter);
      transl*= 2;
      curpos.Translate(transl);
    }

    gp_Ax2 ax(myCenter,myFDir.Crossed(mySDir),myFDir);
    gp_Circ circle(ax,theLength);
    Standard_Real par = ElCLib::Parameter(circle,curpos);
    curpos = ElCLib::Value(par,circle);    

    // small offset like in LengthDimension
    gp_Vec transl(myCenter, curpos);
    transl*= 0.3;
    curpos.Translate(transl);
    
    if (myIsSetBndBox)
      curpos = AIS::TranslatePointToBound( curpos, gp_Dir( gp_Vec( myCenter, curpos ) ), myBndBox );

    myPosition = curpos;
    myAutomaticPosition = Standard_True;      
  }

  else {
  // point is projected on the plane
    gp_Pnt2d pointOnPln(ProjLib::Project(myPlane->Pln(),myPosition));
    myPosition = BRepAdaptor_Surface(BRepBuilderAPI_MakeFace(myPlane->Pln()).Face()).Value(pointOnPln.X(),pointOnPln.Y());
    curpos = myPosition;
    Standard_Real dist(curpos.Distance(myCenter));    
    if (dist<=Precision::Confusion()) {
      gp_XYZ delta(1.,1.,1.);
      curpos.SetXYZ(curpos.XYZ()+delta);
      dist = curpos.Distance(myCenter);
    }
    // To learn if it is necessary to take distance -dist or not
    // it is necessary to know if we are in the sector opposite to the angle
    // if not : we are in the opposite sector if the coordinates
    // of curpos in point (d1,d2) are negative
    gp_Ax2 ax(myCenter,myFDir.Crossed(mySDir),myFDir);
    gp_Circ circle(ax,dist);
#ifdef DEB
//    gp_Pnt p1(myCenter.Translated(gp_Vec(d1)*dist));
#endif
    gp_Pnt p2(myCenter.Translated(gp_Vec(d2)*dist));
    Standard_Real uc1 = 0;
    Standard_Real uc2 = ElCLib::Parameter(circle, p2 );
    Standard_Real uco = ElCLib::Parameter(circle, curpos );
    Standard_Real udeb = uc1;
    Standard_Real ufin = uc2;
    if (uco > ufin) {
      if (Abs(myVal)<PI) {
	// test if uco is in the opposite sector 
	if (uco > udeb+PI && uco < ufin+PI){
	  dist = -dist;
	}
      }
    }

     gp_Pnt p1_attach(myCenter.Translated(gp_Vec(d1)*dist));
     gp_Pnt p2_attach(myCenter.Translated(gp_Vec(d2)*dist));
    
     if (!isInfinite1) {
       Standard_Real par_p1_attach(ElCLib::Parameter(gpl1,p1_attach));
       Standard_Real par11 = ElCLib::Parameter(gpl1,ptat11);
       Standard_Real par12 = ElCLib::Parameter(gpl1,ptat12);
       if (par_p1_attach > par11 && par_p1_attach > par12) {
	 par_p1_attach = Max(par11,par12);
	 p1_attach = ElCLib::Value(par_p1_attach,gpl1);
       }
       else if (par_p1_attach < par11 && par_p1_attach < par12) {
	 par_p1_attach = Min(par11,par12);
	 p1_attach = ElCLib::Value(par_p1_attach,gpl1);
       }
     }
     myFAttach = p1_attach;
	    
     if (!isInfinite2) {
       Standard_Real par_p2_attach(ElCLib::Parameter(gpl2,p2_attach));
       Standard_Real par21 = ElCLib::Parameter(gpl2,ptat21);
       Standard_Real par22 = ElCLib::Parameter(gpl2,ptat22);
       if (par_p2_attach > par21 && par_p2_attach > par22) {
	 par_p2_attach = Max(par21,par22);
	 p2_attach = ElCLib::Value(par_p2_attach,gpl2);
       }
       else if (par_p2_attach < par21 && par_p2_attach < par22) {
	 par_p2_attach = Min(par21,par22);
	 p2_attach = ElCLib::Value(par_p2_attach,gpl2);
       }
     }
     mySAttach = p2_attach;
  }  
  myAxis = theaxis.Position();

  //--------------------------------------------------------
  //    Computation of the presentation
  //--------------------------------------------------------
  Handle(Prs3d_AngleAspect) la = myDrawer->AngleAspect();
  Handle(Prs3d_ArrowAspect) arr = la->ArrowAspect();

  arr->SetLength(myArrowSize);

  DsgPrs_AnglePresentation::Add(aPresentation,
				myDrawer,
				myVal,
				myText,
				myCenter,
				myFAttach,
				mySAttach,
				myFDir,
				mySDir,
				curpos,
				mySymbolPrs);
}



//=======================================================================
//function : ComputeTwoEdgesNullAngle
//purpose  : compute the presentation of a angle dimension if it's null.
//             -> the aim of the computation is to have a constant radius 
//                during the dimension moving : the radius is independant
//                of the cursor position, it's equal to a arbitrary value
//=======================================================================

void AIS_AngleDimension::ComputeTwoEdgesNullAngle(const Handle(Prs3d_Presentation)& aPresentation,
						  const Handle(Geom_Line)& l1,
						  const Handle(Geom_Line)& l2,
						  const gp_Pnt& ptat11,
						  const gp_Pnt& ptat12,
						  const gp_Pnt& ptat21,
						  const gp_Pnt& ptat22,
						  const Standard_Boolean isInfinite1,
						  const Standard_Boolean isInfinite2)
{
  // current face
  BRepBuilderAPI_MakeFace makeface(myPlane->Pln());
  TopoDS_Face face(makeface.Face());  
  BRepAdaptor_Surface adp(makeface.Face());
  // 2d lines => projection of 3d on current plane
  Handle(Geom2d_Curve) geoC1 = GeomAPI::To2d(l1,myPlane->Pln());
  Handle(Geom2d_Line) l1_2d = *((Handle(Geom2d_Line)*)& geoC1);
  Handle(Geom2d_Curve) geoC2 = GeomAPI::To2d(l2,myPlane->Pln());
  Handle(Geom2d_Line) l2_2d = *((Handle(Geom2d_Line)*)& geoC2);

  gp_Lin gpl1 = l1->Lin();
  gp_Lin gpl2 = l2->Lin();

  //------------------------------------------------------------
  //                Computation of myCenter
  // -> Point located on the median of 2 straight lines,
  //    is calculated as located between 2 closest points 
  //    of each straight line.
  //-----------------------------------------------------------
     //   theLength : radius of the future circle
  Standard_Real theLength = gpl1.Distance(gpl2.Location());
  // processing of the particular case when 2 straight lines are coincident
  Standard_Boolean SameLines(Standard_False);
  if ( theLength <= Precision::Confusion()) {
    SameLines = Standard_True;
    if (!isInfinite1) {
      if (!isInfinite2) theLength = 0.75 * Max( ptat11.Distance(ptat12), ptat21.Distance(ptat22));
      else theLength = 0.75*ptat11.Distance(ptat12);
    }
    else {
      if (!isInfinite2) theLength = 0.75*ptat21.Distance(ptat22);
      else theLength = 50.;
    }
  }
  else theLength = theLength*8/10;

  gp_Pnt pmin1 ,pmin2;
  if (!isInfinite1 && !isInfinite2) {
    pmin1 = ptat11; pmin2 = ptat21;
    Standard_Real dis = ptat11.Distance(ptat21);
    Standard_Real dis2 = ptat11.Distance(ptat22);
    if ( dis2 < dis)  {
      pmin1 = ptat11;
      pmin2 = ptat22;
      dis = dis2;
    }
    dis2 = ptat12.Distance(ptat22);
    if ( dis2 < dis)  {
      pmin1 = ptat12;
      pmin2 = ptat22;
      dis = dis2;
    }
    dis2 = ptat12.Distance(ptat21);
    if ( dis2 < dis)  {
      pmin1 = ptat12;
      pmin2 = ptat21;
      dis = dis2;
    }
    myCenter.SetXYZ( (pmin1.XYZ() + pmin2.XYZ()) / 2. );
  }
  else {
    gp_Pnt pntOnl1 = gpl1.Location();
    gp_Pnt pntOnl2 = ElCLib::Value(ElCLib::Parameter(gpl1,pntOnl1),gpl2);
    myCenter.SetXYZ( (pntOnl1.XYZ() + pntOnl2.XYZ()) / 2. );
  }

  
  // directions 
  gp_Dir d1,d2;
  if (!isInfinite1) {
    if (myCenter.SquareDistance(ptat11) > myCenter.SquareDistance(ptat12)) d1 = gp_Dir(gp_Vec(myCenter,ptat11));
    else d1 = gp_Dir(gp_Vec(myCenter,ptat12));  
  }
  else d1 = gpl1.Direction();
  
  if (!isInfinite2) {
    if (myCenter.SquareDistance(ptat21) > myCenter.SquareDistance(ptat22)) d2 = gp_Dir(gp_Vec(myCenter,ptat21));
    else d2 = gp_Dir(gp_Vec(myCenter,ptat22));
  }
  else d2 = gpl2.Direction();  

  gp_Dir theaxis;
  if ( SameLines ) theaxis = myPlane->Pln().Axis().Direction();
  else {
    theaxis = gp_Dir(d1^d2);
    gp_Vec V1(d1); gp_Vec V2(d2);
    if ( V1.CrossMagnitude(V2) < 0 ) theaxis.Reverse();
  }

  gp_Pnt curpos; // cursor position
  TColStd_Array1OfReal tabdist(1,4);
  gp_Pnt P1, P2; // points at intersection of the circle with 2 straight lines
 
  if (myAutomaticPosition) {
    if (!isInfinite1) {
      tabdist(1) = myCenter.Distance(ptat11);
      tabdist(2) = myCenter.Distance(ptat12);
    }
    else {
      tabdist(1) = tabdist(2) = 0.;
    }
    if (!isInfinite2) { 
      tabdist(3) = myCenter.Distance(ptat21);
      tabdist(4) = myCenter.Distance(ptat22);
    }
    else {
      tabdist(3) = tabdist(4) = 0.;
    }
    if ( SameLines ) {
      Standard_Real dist1(RealLast());
      if (!isInfinite1) dist1 = Max(tabdist(1),tabdist(2));
      Standard_Real dist2(RealLast());
      if (!isInfinite2) dist2 = Max(tabdist(3),tabdist(4));      
      
      myFAttach = myCenter;
      mySAttach = myCenter;
      P1 = myFAttach;
      P2 = mySAttach;

      myCenter.Translate(gp_Vec(d1)*theLength);

      // calculate attachments of the face 
      //  -> they are points of intersection if  
      //     intersection is outside of the edges
      Standard_Real pparam = ElCLib::Parameter(gpl1,myFAttach);
      Standard_Real pparam1 = ElCLib::Parameter(gpl1,ptat11);
      Standard_Real pparam2 = ElCLib::Parameter(gpl1,ptat12);
      if (!isInfinite1) {
	if ( pparam1 < pparam2 ) {
	  if ( pparam < pparam1 ) myFAttach = ptat11;
	  else if ( pparam > pparam2) myFAttach = ptat12;
	}
	else {
	  if ( pparam < pparam2) myFAttach = ptat12;
	  else if ( pparam > pparam1) myFAttach = ptat11;
	}
      }
      if (!isInfinite2) {
	pparam = ElCLib::Parameter(gpl2,myFAttach);
	pparam1 = ElCLib::Parameter(gpl2,ptat21);
	pparam2 = ElCLib::Parameter(gpl2,ptat22);
	if ( pparam1 < pparam2 ) {
	  if ( pparam < pparam1 ) mySAttach = ptat21;
	  else if ( pparam > pparam2) mySAttach = ptat22;
	}
	else {
	  if ( pparam < pparam2) mySAttach = ptat22;
	  else if ( pparam > pparam1) mySAttach = ptat21;
	}
      }
    }
    // Case of disconneted lines
    else {
      gp_Ax2 AX(myCenter,theaxis,d1);
      Handle(Geom_Circle)  circle = new Geom_Circle(AX,theLength);
      Handle(Geom2d_Curve) geoCurve = GeomAPI::To2d(circle,myPlane->Pln());
      Handle(Geom2d_Circle) c2d = *((Handle(Geom2d_Circle)*)& geoCurve);
      // calculate the intersection of circle with l1
      Standard_Real pparam; // parameter of the point of intersection on l1
      IntAna2d_AnaIntersection inter(l1_2d->Lin2d(),c2d->Circ2d());
      gp_Pnt2d pint1(inter.Point(1).Value());
      gp_Pnt2d pint2(inter.Point(2).Value());
      
      gp_Pnt Int1 = adp.Value(pint1.X(),pint1.Y());
      gp_Pnt Int2 = adp.Value(pint2.X(),pint2.Y());
      gp_Dir I1I2(gp_Vec(Int1,Int2));
      if ( d1*I1I2 > 0 ) {
	myFAttach = Int2;
	pparam = inter.Point(2).ParamOnFirst();
      }
      else {
	myFAttach = Int1;
	pparam = inter.Point(1).ParamOnFirst();
      }
      P1 = myFAttach;

      Standard_Real pparam1;
      Standard_Real pparam2; 
      if (!isInfinite1) {
	pparam1 = ElCLib::Parameter(gpl1,ptat11);
	pparam2 = ElCLib::Parameter(gpl1,ptat12);	
	if ( pparam1 < pparam2 ) {
	  if ( pparam < pparam1 ) myFAttach = ptat11;
	  else if ( pparam > pparam2) myFAttach = ptat12;
	}
	else {
	  if ( pparam < pparam2) myFAttach = ptat12;
	  else if ( pparam > pparam1) myFAttach = ptat11;
	}
      }
      pparam = ElCLib::Parameter(gpl2,P1);
      mySAttach = ElCLib::Value(pparam, gpl2);
      P2 = mySAttach;

      if (!isInfinite2) {
	pparam1 = ElCLib::Parameter(gpl2,ptat21);
	pparam2 = ElCLib::Parameter(gpl2,ptat22);
	if ( pparam1 < pparam2 ) {
	  if ( pparam < pparam1 ) mySAttach = ptat21;
	  else if ( pparam > pparam2) mySAttach = ptat22;
	}
	else {
	  if ( pparam < pparam2) mySAttach = ptat22;
	  else if ( pparam > pparam1) mySAttach = ptat21;
	}
      }
    }
    curpos.SetXYZ(.5*(P1.XYZ()+P2.XYZ()));

    gp_Ax2 ax(myCenter,theaxis,d1);
    gp_Circ circle(ax,theLength);
    Standard_Real par = ElCLib::Parameter(circle,curpos);
    curpos = ElCLib::Value(par,circle);

    if (myIsSetBndBox)
      curpos = AIS::TranslatePointToBound( curpos, gp_Dir( gp_Vec( myCenter, curpos ) ), myBndBox );
    myPosition =curpos;
    myAutomaticPosition = Standard_True;      
  }
  else {
    curpos = myPosition;
    gp_Lin Media(myCenter, gpl1.Direction());
    Standard_Real pcurpos = ElCLib::Parameter(Media, curpos);
    myCenter =  ElCLib::Value(pcurpos, Media);
    // the centre is translated to avoid a constant radius!
    myCenter.Translate(-theLength*gp_Vec(gpl1.Direction()));
    gp_Ax2 AX(myCenter,theaxis,gpl1.Direction());
    Handle(Geom_Circle)  circle = new Geom_Circle(AX,theLength);

    // re-update curpos
    pcurpos = ElCLib::Parameter(circle->Circ(), curpos);
    curpos = ElCLib::Value(pcurpos, circle->Circ());

    Handle(Geom2d_Curve) geoCurve = GeomAPI::To2d(circle,myPlane->Pln());
    Handle(Geom2d_Circle) c2d = *((Handle(Geom2d_Circle)*)& geoCurve);
    // calculate the point of intersection of circle with l1
    IntAna2d_AnaIntersection inter(l1_2d->Lin2d(),c2d->Circ2d());
    gp_Pnt2d pint1(inter.Point(1).Value());
    gp_Pnt2d pint2(inter.Point(2).Value());
    gp_Pnt Int1 = adp.Value(pint1.X(),pint1.Y());
    gp_Pnt Int2 = adp.Value(pint2.X(),pint2.Y());
    if ( curpos.SquareDistance(Int1) < curpos.SquareDistance(Int2)) myFAttach = Int1;
    else myFAttach = Int2;
    P1 = myFAttach;
    
    // calculate the point of intersection of circle with l2
    // -> this is the projection because the centre of circle
    //    is in the middle of l1 and l2
    Standard_Real pparam = ElCLib::Parameter(gpl2,myFAttach);
    mySAttach = ElCLib::Value(pparam, gpl2);

    P2 = mySAttach;

    Standard_Real par_attach(ElCLib::Parameter(gpl1,myFAttach));
    Standard_Real par1,par2; 
    if (!isInfinite1) {
      par1 = ElCLib::Parameter(gpl1,ptat11);
      par2 = ElCLib::Parameter(gpl1,ptat12);
      if (par1 < par2) {
	if ( par_attach < par1 ) myFAttach = ptat11;
	else if ( par_attach > par2) myFAttach = ptat12;
      }
      else {
	if ( par_attach < par2 ) myFAttach = ptat12;
	else if ( par_attach > par1) myFAttach = ptat11;
      }
    }
    par_attach = ElCLib::Parameter(gpl2,mySAttach);
    if (!isInfinite2) {
      par1 = ElCLib::Parameter(gpl2,ptat21);
      par2 = ElCLib::Parameter(gpl2,ptat22);
      if (par1 < par2) {
	if ( par_attach < par1 ) mySAttach = ptat21;
	else if ( par_attach > par2) mySAttach = ptat22;
      }
      else {
	if ( par_attach < par2 ) mySAttach = ptat22;
	else if ( par_attach > par1) mySAttach = ptat21;
      }
    }
  }

  myFDir = gp_Dir(gp_Vec(myCenter,P1));
  mySDir = gp_Dir(gp_Vec(myCenter,P2));

  //--------------------------------------------------------
  //    Computation of the presentation
  //--------------------------------------------------------
  Handle(Prs3d_AngleAspect) la = myDrawer->AngleAspect();
  Handle(Prs3d_ArrowAspect) arr = la->ArrowAspect();

  arr->SetLength(myArrowSize);

  if (SameLines)
    DsgPrs_AnglePresentation::Add(aPresentation,
				  myDrawer,
				  myVal,
				  myText,
				  myCenter,
				  myFAttach,
				  mySAttach,
				  myFDir,
				  mySDir,
				  theaxis,
				  Standard_True,
				  myAxis,
				  curpos,
				  DsgPrs_AS_NONE);
  else
    DsgPrs_AnglePresentation::Add(aPresentation,
				  myDrawer,
				  myVal,
				  myText,
				  myCenter,
				  myFAttach,
				  mySAttach,
				  myFDir,
				  mySDir,
				  curpos,
				  mySymbolPrs);
}


//=======================================================================
//function : Compute3DSelection
// purpose  : compute the zones of selection for an angle dimension
//            between 2 faces
//=======================================================================

void AIS_AngleDimension::Compute3DSelection( const Handle( SelectMgr_Selection )& aSelection )
{
  gp_Circ AngleCirc, AttachCirc;
  Standard_Real FirstParAngleCirc, LastParAngleCirc, FirstParAttachCirc, LastParAttachCirc;
  gp_Pnt EndOfArrow1, EndOfArrow2, ProjAttachPoint2;
  gp_Dir DirOfArrow1, DirOfArrow2;
  gp_Dir axisdir = (myVal <= Precision::Angular() || Abs( PI-myVal ) <= Precision::Angular())?
                    myPlane->Pln().Axis().Direction() : (myFDir ^ mySDir);
  Standard_Boolean isPlane = (myFirstSurfType == AIS_KOS_Plane)? Standard_True : Standard_False;

  Standard_Real ArrowLength = myDrawer->AngleAspect()->ArrowAspect()->Length();
  DsgPrs::ComputeFacesAnglePresentation( ArrowLength,
					 myVal,
					 myCenter,
					 myFAttach,
					 mySAttach,
					 myFDir,
					 mySDir,
					 axisdir,
					 isPlane,
					 myAxis,
					 myPosition,
					 AngleCirc,
					 FirstParAngleCirc,
					 LastParAngleCirc,
					 EndOfArrow1,
					 EndOfArrow2,
					 DirOfArrow1,
					 DirOfArrow2,
					 ProjAttachPoint2,
					 AttachCirc,
					 FirstParAttachCirc,
					 LastParAttachCirc );
  
  Handle( SelectMgr_EntityOwner ) own = new SelectMgr_EntityOwner( this, 7 );
  Handle( Select3D_SensitiveSegment ) seg;
  Handle( Geom_TrimmedCurve ) curve; 
  Handle( Select3D_SensitiveCurve ) SensCurve;

  // Angle's arc or line
  if (myVal > Precision::Angular() && Abs( PI-myVal ) > Precision::Angular())
    {
      curve = new Geom_TrimmedCurve( new Geom_Circle( AngleCirc ), FirstParAngleCirc, LastParAngleCirc );
      SensCurve = new Select3D_SensitiveCurve( own, curve );
      aSelection->Add( SensCurve );
    }
  else // angle's line
    {
      gp_Vec ArrowVec( DirOfArrow1 );
      ArrowVec *= ArrowLength;
      gp_Pnt FirstPoint, LastPoint;

      if (myPosition.Distance( EndOfArrow1 ) > ArrowLength)
	{
	  FirstPoint = myPosition;
	  LastPoint = EndOfArrow1.Translated( ArrowVec );
	  if (myPosition.SquareDistance( LastPoint ) < myPosition.SquareDistance( EndOfArrow1 ))
	    LastPoint = EndOfArrow1.Translated( -ArrowVec );
	}
      else
	{
	  FirstPoint = EndOfArrow1.Translated( ArrowVec );
	  LastPoint = EndOfArrow1.Translated( -ArrowVec );
	}
      seg = new Select3D_SensitiveSegment( own, FirstPoint, LastPoint );
      aSelection->Add( seg );
    }

  if (! myFAttach.IsEqual( EndOfArrow1, Precision::Confusion() ))
    {
      seg = new Select3D_SensitiveSegment( own, myFAttach, EndOfArrow1 );
      aSelection->Add( seg );
    }
  if (! ProjAttachPoint2.IsEqual( EndOfArrow2, Precision::Confusion() ))
    {
      seg = new Select3D_SensitiveSegment( own, ProjAttachPoint2, EndOfArrow2 );
      aSelection->Add( seg );
    }

  // Line or arc from mySAttach to its "projection"
  if (! mySAttach.IsEqual( ProjAttachPoint2, Precision::Confusion() ))
    {
      if (isPlane)
	{
	  seg = new Select3D_SensitiveSegment( own, mySAttach, ProjAttachPoint2 );
	  aSelection->Add( seg );
	}
      else
	{
	  curve = new Geom_TrimmedCurve( new Geom_Circle( AttachCirc ),
					 FirstParAttachCirc,
					 LastParAttachCirc );
	  SensCurve = new Select3D_SensitiveCurve( own, curve );
	  aSelection->Add( SensCurve );
	}
    }

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

//=======================================================================
//function : Compute2DSelection
//purpose  : compute zones of selection on a side of angle between 2 edges
//           Special processing of zero angles!
//=======================================================================

void AIS_AngleDimension::Compute2DSelection(const Handle(SelectMgr_Selection)& aSelection)
{
  BRepAdaptor_Curve cu1(TopoDS::Edge(myFShape));
  BRepAdaptor_Curve cu2(TopoDS::Edge(mySShape));

  gp_Lin l1(cu1.Line());
  gp_Lin l2(cu2.Line());

  // it is patch!
  if (Abs( myVal ) <= Precision::Angular() || Abs( PI - myVal ) <= Precision::Angular())
/*
  //---------------------------------------------------------
  //    Cas de droites paralleles ( <=> angle nul a PI pres)
  if ((Abs(l1.Angle(l2)) < Precision::Angular()) ||
      (Abs((l1.Angle(l2) - PI)) < Precision::Angular()) )
*/
    {
       
    Standard_Real distLL= l1.Distance(l2);
    if ( Abs(distLL) <= Precision::Confusion() ) {
      gp_Pnt ptat11 = cu1.Value(cu1.FirstParameter());
      gp_Pnt ptat12 = cu1.Value(cu1.LastParameter());
      gp_Pnt ptat21 = cu2.Value(cu2.FirstParameter());
      gp_Pnt ptat22 = cu2.Value(cu2.LastParameter());
      distLL =  0.75 * Max( ptat11.Distance(ptat12), ptat21.Distance(ptat22));
      ComputeNull2DSelection(aSelection, distLL);
    }
    else {
      ComputeNull2DSelection(aSelection, distLL*8/10);
    }
  }
  
  //----------------------------------------------------------
  //  Classic case  ( angle != 0 )
  else {

    if (myFDir.IsParallel(mySDir,Precision::Angular())) {
      Standard_Real distLL= l1.Distance(l2);
      if ( Abs(distLL) <= Precision::Confusion() ) {
	gp_Pnt ptat11 = cu1.Value(cu1.FirstParameter());
	gp_Pnt ptat12 = cu1.Value(cu1.LastParameter());
	gp_Pnt ptat21 = cu2.Value(cu2.FirstParameter());
	gp_Pnt ptat22 = cu2.Value(cu2.LastParameter());
	distLL =  0.75 * Max( ptat11.Distance(ptat12), ptat21.Distance(ptat22));
	ComputeNull2DSelection(aSelection, distLL*8/10);
      }
    }
    else {
      gp_Dir Norm = myFDir.Crossed(mySDir);
      
      gp_Ax2 ax(myCenter,Norm,myFDir);
      gp_Circ cer(ax,myCenter.Distance(myPosition));
      gp_Vec vec1(myFDir);
      
      Standard_Boolean nullrad(Standard_False);
      if (cer.Radius() == 0.) {
	cer.SetRadius(1.);
	nullrad = Standard_True;
      }
      vec1 *= cer.Radius();
      gp_Pnt p1 = myCenter.Translated(vec1);
      gp_Vec vec2(mySDir);
      vec2 *= cer.Radius();
      gp_Pnt p2 = myCenter.Translated(vec2);
      
      Standard_Real uc1 = 0.;
      Standard_Real uc2 = ElCLib::Parameter(cer,p2);
      Standard_Real uco;
      if (nullrad) uco = ElCLib::Parameter(cer,p1);
      else uco = ElCLib::Parameter(cer,myPosition);
      
      Standard_Real udeb = uc1;
      Standard_Real ufin = uc2;
      
      if (uco > ufin) {
	if (Abs(myVal)<PI) {
	  // test if uco is in the opposing sector 
	  if (uco > udeb+PI && uco < ufin+PI){
	    udeb = udeb + PI;
	  ufin = ufin + PI;
	    uc1  = udeb;
	    uc2  = ufin;
	  }
      }
      }  
      if (uco > ufin) {
	if ((uco-uc2) < (uc1-uco+(2*PI))) ufin = uco;
	else udeb = uco - 2*PI;
      }
      p1   = ElCLib::Value(udeb,cer);
      p2   = ElCLib::Value(ufin,cer);
      
      //Create 2 owners for each part of the arrow
      Handle(AIS_DimensionOwner) own1 = new AIS_DimensionOwner(this,7);
      Handle(AIS_DimensionOwner) own2 = new AIS_DimensionOwner(this,7);
      if (myExtShape != 0) {
	if (myExtShape == 1) {
	  own1->SetShape(mySShape);
	  own2->SetShape(mySShape);
	}
	else {
	  own1->SetShape(myFShape);
	  own2->SetShape(myFShape);
	}
      }
      else {
	own1->SetShape(myFShape);
	own2->SetShape(mySShape);
      }
      
      Handle(Geom_Circle) thecirc = new Geom_Circle(cer);
      
      Handle(Geom_TrimmedCurve) thecu1 = new Geom_TrimmedCurve(thecirc,udeb,(udeb+ufin)/2);
      Handle(Geom_TrimmedCurve) thecu2 = new Geom_TrimmedCurve(thecirc,(udeb+ufin)/2,ufin);
      
      Handle(Select3D_SensitiveCurve) scurv = new Select3D_SensitiveCurve(own1,thecu1);
      aSelection->Add(scurv);
      scurv = new Select3D_SensitiveCurve(own2,thecu2);
      aSelection->Add(scurv);
      
      Handle(Select3D_SensitiveSegment) seg;
      if (!myFAttach.IsEqual(p1,Precision::Confusion())) {
	seg = new Select3D_SensitiveSegment(own1,myFAttach,p1);
	aSelection->Add(seg);
      }
      if (!mySAttach.IsEqual(p2,Precision::Confusion())) {
	seg = new Select3D_SensitiveSegment(own2,mySAttach,p2);
	aSelection->Add(seg);
      }
    }
  }

}
//=======================================================================
//function : Compute2DNullSelection
//purpose  : for dimension of null angle
//=======================================================================

void AIS_AngleDimension::ComputeNull2DSelection(
			 const Handle(SelectMgr_Selection)& aSelection,
			 const Standard_Real distLL)
{
  gp_Dir Norm; 
  if ( myFDir.IsParallel(mySDir, Precision::Angular()) ) {
    Norm = myPlane->Pln().Axis().Direction();
  }
  else
    Norm = myFDir.Crossed(mySDir);

  gp_Ax2 ax(myCenter,Norm,myFDir);
  gp_Circ cer(ax,distLL);
  
  gp_Vec vec1(myFDir);
  vec1 *= cer.Radius();
  gp_Pnt p1 = myCenter.Translated(vec1);
  gp_Vec vec2(mySDir);
  vec2 *= cer.Radius();
  gp_Pnt p2 = myCenter.Translated(vec2);

  // calcul de parametres de debut et de fin des extremites de l'arc
  Standard_Real uc1 = 0.;
  Standard_Real uc2 = ElCLib::Parameter(cer,p2);
  Standard_Real uco = ElCLib::Parameter(cer,myPosition);

  Standard_Real udeb = uc1;
  Standard_Real ufin = uc2;

  if (uco > ufin) {
    if (Abs(myVal)<PI) {
      // test if uco is in the opposing sector 
      if (uco > udeb+PI && uco < ufin+PI){
	udeb = udeb + PI;
	ufin = ufin + PI;
	uc1  = udeb;
	uc2  = ufin;
      }
    }
  }

  if (uco > ufin) {
    if ((uco-uc2) < (uc1-uco+(2*PI))) {
      ufin = uco;
    }
    else {
      udeb = uco - 2*PI;
    }
  }

  //Create 2 owners for each part of the arrow
  Handle(AIS_DimensionOwner) own1 = new AIS_DimensionOwner(this,7);
  Handle(AIS_DimensionOwner) own2 = new AIS_DimensionOwner(this,7);
  if (myExtShape != 0) {
    if (myExtShape == 1) {
      own1->SetShape(mySShape);
      own2->SetShape(mySShape);
    }
    else {
      own1->SetShape(myFShape);
      own2->SetShape(myFShape);
    }
  }
  else {
    own1->SetShape(myFShape);
    own2->SetShape(mySShape);
  }
  
  Handle(Geom_Circle) thecirc = new Geom_Circle(cer);

  if ( udeb != ufin ) {
    Handle(Geom_TrimmedCurve) thecu1 = new Geom_TrimmedCurve(thecirc,udeb,(udeb+ufin)/2);
    Handle(Geom_TrimmedCurve) thecu2 = new Geom_TrimmedCurve(thecirc,(udeb+ufin)/2,ufin);

    Handle(Select3D_SensitiveCurve) scurv = new Select3D_SensitiveCurve(own1,thecu1);
    aSelection->Add(scurv);
    scurv = new Select3D_SensitiveCurve(own2,thecu2);
    aSelection->Add(scurv);
  }
  else {
    // find end of segment to allow selection
    gp_Vec VTrans(myFDir.Crossed(Norm));
    Handle(Select3D_SensitiveSegment) seg1;
    seg1 = new Select3D_SensitiveSegment(own1, 
					 p1, 
					 p1.Translated( VTrans*distLL/10 ) );
    aSelection->Add(seg1);
    seg1 = new Select3D_SensitiveSegment(own2, 
					 p2, 
					 p2.Translated(-VTrans*distLL/10 ) );
    aSelection->Add(seg1);
  }

  Handle(Select3D_SensitiveSegment) seg;
  if (!myFAttach.IsEqual(p1,Precision::Confusion())) {
    seg = new Select3D_SensitiveSegment(own1,myFAttach,p1);
    aSelection->Add(seg);
  }
    
  if (!mySAttach.IsEqual(p2,Precision::Confusion())) {
    seg = new Select3D_SensitiveSegment(own2,mySAttach,p2);
    aSelection->Add(seg);
  }
}


//=======================================================================
//function : ComputeConeAngleSelection
//purpose  : for cone angle
//=======================================================================
void AIS_AngleDimension::ComputeConeAngleSelection(const Handle(SelectMgr_Selection)& aSelection)
{
  if( myCone.IsNull() ) return;


  Handle( SelectMgr_EntityOwner ) owner = new SelectMgr_EntityOwner( this, 7 );
  Handle( Select3D_SensitiveSegment ) seg;
  
  gp_Pln aPln;
  gp_Cone aCone;
  gp_Circ myCircle;
  gp_Pnt Apex;
  Handle( Geom_Surface ) aSurf;         //a surface from the Face
  Handle( Geom_OffsetSurface ) aOffsetSurf; 
  Handle( Geom_ConicalSurface ) aConicalSurf;
  Handle( Geom_SurfaceOfRevolution ) aRevSurf; 
  Handle( Geom_Line ) aLine;
  BRepAdaptor_Surface tmpSurf(myCone);  
  TopoDS_Face aFace;
  AIS_KindOfSurface aSurfType;
  Standard_Real Offset = 0. ;
  Handle( Standard_Type ) aType;

  Standard_Real maxV = tmpSurf.FirstVParameter();
  Standard_Real minV = tmpSurf.LastVParameter();

  AIS::GetPlaneFromFace( myCone, aPln, aSurf, aSurfType, Offset );
  
  if ( aSurfType == AIS_KOS_Revolution ) {                                    //surface of revolution

    aRevSurf = Handle( Geom_SurfaceOfRevolution )::DownCast( aSurf ); 
    gp_Lin ln( aRevSurf->Axis() );
    Handle( Geom_Curve ) tmpCrv = aRevSurf->BasisCurve();
    if ( tmpCrv ->DynamicType() != STANDARD_TYPE(Geom_Line) )  return;        //Must be a part of line

    Standard_Real par;
    gp_Pnt fst = tmpSurf.Value(0., minV);
    gp_Pnt lst = tmpSurf.Value(0., maxV);
 
    gp_Vec vec1(fst, lst);
   
    par = ElCLib::Parameter( ln, fst );
    gp_Pnt fst2 = ElCLib::Value( par, ln );                         //projection fst on ln
    par = ElCLib::Parameter( ln, lst );
    gp_Pnt lst2 = ElCLib::Value( par, ln );                         //projection lst on ln

    gp_Vec vec2(fst2, lst2);

   // Check if two parts of revolution are parallel ( it's a cylinder ) or normal (it's a circle ) 
    if( vec1.IsParallel( vec2,Precision::Angular() ) || vec1.IsNormal( vec2,Precision::Angular() ) ) return; 
 
    gce_MakeCone mkCone(aRevSurf->Axis(), fst, lst);
    aCone =  mkCone.Value();
    Apex = aCone.Apex();
  }
  else {  
    aType = aSurf->DynamicType();
    if ( aType == STANDARD_TYPE(Geom_OffsetSurface) || Offset > 0.01 ) {            //offset surface
      aOffsetSurf = new Geom_OffsetSurface (aSurf, Offset);
      aSurf = aOffsetSurf->Surface();
      BRepBuilderAPI_MakeFace mkFace(aSurf);
      mkFace.Build();
      if( !mkFace.IsDone() ) return;
      tmpSurf.Initialize( mkFace.Face() );
    }  
 
    aCone = tmpSurf.Cone();
    aConicalSurf = Handle( Geom_ConicalSurface)::DownCast( aSurf );
    Apex =  aConicalSurf->Apex();
  }

  Handle(Geom_Curve) aCurve;                 //A circle where the angle is drawn
  
  if ( myAutomaticPosition ) {
    Standard_Real midV = ( minV + maxV ) / 2.5; 

    aCurve =   aSurf->VIso(midV);
    myCircle = Handle(Geom_Circle)::DownCast(aCurve)->Circ();

    myPosition = ElCLib::Value(Standard_PI / 2.0, myCircle);
    myAutomaticPosition = Standard_False;
  }
  else {
    Standard_Real U, V;
    ElSLib::Parameters(aCone, myPosition, U, V);
    aCurve = aSurf->VIso(V); 
    myCircle = Handle(Geom_Circle)::DownCast(aCurve)->Circ();
  }
 //__________________________________________________________________
  aCurve = aSurf->VIso(maxV);
  gp_Circ CircVmax = Handle(Geom_Circle)::DownCast(aCurve)->Circ();
  aCurve = aSurf->VIso(minV);
  gp_Circ CircVmin = Handle(Geom_Circle)::DownCast(aCurve)->Circ();
 //__________________________________________________________________

  if( CircVmax.Radius() < CircVmin.Radius() ) {
   gp_Circ tmpCirc = CircVmax;
   CircVmax = CircVmin;
   CircVmin = tmpCirc;
  }
 
  Standard_Boolean IsArrowOut = Standard_True;    //Is arrows inside or outside of the cone
  //Standard_Real PntOnMainAxis = 0;   //Is projection of aPosition inside of the cone = 0, above = 1, or below = -1
  Standard_Boolean IsConeTrimmed = Standard_False; 

  if( CircVmin.Radius() > 0.01 ) IsConeTrimmed = Standard_True;

  gp_Pnt AttachmentPnt;
  gp_Pnt OppositePnt;
  gp_Pnt aPnt, tmpPnt;
  Quantity_Length X,Y,Z;

  Standard_Real param = ElCLib::Parameter(myCircle, myPosition);

  aPnt = Apex;
  gp_Pnt P1 = ElCLib::Value(0., myCircle);
  gp_Pnt P2 = ElCLib::Value(Standard_PI, myCircle);

  gce_MakePln mkPln(P1, P2,  aPnt);   // create a plane whitch defines plane for projection aPosition on it

  aPnt =  AIS::ProjectPointOnPlane(myPosition, mkPln.Value()); 
  tmpPnt = aPnt;

  if( aPnt.Distance(P1) <  aPnt.Distance(P2) ){
    AttachmentPnt = P1; 
    OppositePnt = P2; 
  }
  else {
    AttachmentPnt = P2; 
    OppositePnt = P1;
  }
  
  aPnt = AttachmentPnt ;                          // Creating of circle whitch defines a plane for a dimension arc
  gp_Vec Vec(AttachmentPnt, Apex);                // Dimension arc is a part of the circle 
  Vec.Scale(2);
  aPnt.Translate(Vec);
  GC_MakeCircle mkCirc(AttachmentPnt, OppositePnt,  aPnt); 
  gp_Circ  aCircle2 = mkCirc.Value()->Circ();

  
  Standard_Integer i;
  Standard_Real AttParam = ElCLib::Parameter(aCircle2, AttachmentPnt);
  Standard_Real OppParam = ElCLib::Parameter(aCircle2, OppositePnt);
  
  while ( AttParam >= 2*Standard_PI ) AttParam -= 2*Standard_PI;
  while ( OppParam >= 2*Standard_PI ) OppParam -= 2*Standard_PI;

  if( myPosition.Distance( myCircle.Location() ) <= myCircle.Radius() )
    if( 2 * myCircle.Radius() > aCircle2.Radius() * 0.4 ) IsArrowOut = Standard_False;  //four times more than an arrow size
 
  Graphic3d_Array1OfVertex V(1, 12);
  
  Standard_Real angle;
  param = ElCLib::Parameter(aCircle2, tmpPnt);

  if(IsArrowOut) {
    angle = OppParam - AttParam + Standard_PI/6; //An angle between AttParam and OppParam + 30 degrees
    param = AttParam - Standard_PI/12;      //out parts of dimension line are 15 degrees
    
    while ( angle > 2*Standard_PI ) angle -= 2*Standard_PI;
    for( i = 0; i <= 11; i++ ) {       //calculating of arc             
      aPnt = ElCLib::Value(param + angle/11 * i, aCircle2);
      aPnt.Coord(X, Y, Z);
      V(i+1).SetCoord(X, Y, Z);    
    }
      
  }
  else {
    angle = OppParam - AttParam;
    param = AttParam;
    while ( angle > 2*Standard_PI ) angle -= 2*Standard_PI;
    for( i = 0; i <= 11; i++ ) {       //calculating of arc             
      aPnt = ElCLib::Value(param + angle/11 * i, aCircle2);
      aPnt.Coord(X, Y, Z);
      V(i+1).SetCoord(X, Y, Z);
    }
  }
  
  for(i = 1; i<=11; i++) {

    V(i).Coord(X, Y, Z);
    P1.SetCoord(X, Y, Z);
    V(i+1).Coord(X, Y, Z);
    P1.SetCoord(X, Y, Z);

    seg = new Select3D_SensitiveSegment(owner, P1, P2);
    aSelection->Add(seg);   
  }

  tmpPnt =  tmpPnt.Translated(gp_Vec(0, 0, -1)*2);

  Standard_Real size(Min(myVal/100.+1.e-6,myArrowSize+1.e-6));
  Handle( Select3D_SensitiveBox ) box = new Select3D_SensitiveBox( owner,
								   tmpPnt.X(),
								   tmpPnt.Y(),
								   tmpPnt.Z(),
								   tmpPnt.X() + size,
								   tmpPnt.Y() + size,
								   tmpPnt.Z() + size);
  aSelection->Add(box);
}



