// File:        AIS_LengthDimension.cxx
// Created:     Tue Dec  5 15:09:04 1996
// Author:      Arnaud BOUZY/Odile Olivier
//              <ODL>

#define BUC60915        //GG 05/06/01 Enable to compute the requested arrow size
//                      if any in all dimensions.

#include <Standard_NotImplemented.hxx>

#include <AIS_LengthDimension.ixx>

#include <AIS.hxx>
#include <AIS_DimensionOwner.hxx>
#include <AIS_Drawer.hxx>

#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRep_Tool.hxx>

#include <DsgPrs.hxx>
#include <DsgPrs_LengthPresentation.hxx>

#include <ElCLib.hxx>
#include <ElSLib.hxx>

#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_OffsetSurface.hxx>

#include <Precision.hxx>

#include <ProjLib.hxx>

#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LengthAspect.hxx>
#include <Prs3d_LineAspect.hxx>

#include <Select3D_SensitiveBox.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <SelectMgr_EntityOwner.hxx>

#include <Standard_DomainError.hxx>

#include <StdPrs_WFDeflectionShape.hxx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#include <gce_MakeDir.hxx>
#include <gce_MakeLin.hxx>

#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

#include <Prs3d_AngleAspect.hxx>

#include <BRepGProp_Face.hxx>


//=======================================================================
//function : Constructor
//purpose  : TwoFacesLength dimension
//=======================================================================

AIS_LengthDimension::AIS_LengthDimension (const TopoDS_Face&  aFirstFace, 
                                          const TopoDS_Face&  aSecondFace,
                                          const Standard_Real aVal,
                                          const TCollection_ExtendedString& aText)
:AIS_Relation(),
 myNbShape(2)
{
  SetFirstShape( aFirstFace );
  SetSecondShape( aSecondFace );
  myVal = aVal;

  myText = aText;
  mySymbolPrs = DsgPrs_AS_BOTHAR;
  myAutomaticPosition = Standard_True;

  myArrowSize = myVal / 10.;
}

//=======================================================================
//function : Constructor
//purpose  : TwoFacesLength dimension  (with position and text)
//=======================================================================

AIS_LengthDimension::AIS_LengthDimension(const TopoDS_Face& aFirstFace,
                                         const TopoDS_Face& aSecondFace,
                                         const Standard_Real aVal, 
                                         const TCollection_ExtendedString& aText,
                                         const gp_Pnt& aPosition,
                                         const DsgPrs_ArrowSide aSymbolPrs,
                                         const Standard_Real anArrowSize)
:AIS_Relation(),
 myNbShape(2)
{
  SetFirstShape( aFirstFace );
  SetSecondShape( aSecondFace );
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
//function : AIS_LengthDimension
//purpose  : Distance Face - Edge for chamfer 3D
//=======================================================================

AIS_LengthDimension::AIS_LengthDimension (const TopoDS_Face& Face,const TopoDS_Edge& Edge,const Standard_Real Val,const TCollection_ExtendedString& Text)

:AIS_Relation (),
 myNbShape    (2)
{
  SetFirstShape  (Face);
  SetSecondShape (Edge);
  myText = Text;
  myVal = Val;
  mySymbolPrs = DsgPrs_AS_BOTHAR;
  myAutomaticPosition = Standard_True;
  myArrowSize = myVal/10.0;
}



//=======================================================================
//function : Constructor
//purpose  : TwoEdgesLength dimension or OneEdgeOneVertexLength dimension or TwoVerticesLength dimension
//=======================================================================

AIS_LengthDimension::AIS_LengthDimension(const TopoDS_Shape& aFShape,
                                         const TopoDS_Shape& aSShape,
                                         const Handle(Geom_Plane)& aPlane,
                                         const Standard_Real aVal,
                                         const TCollection_ExtendedString& aText)
:AIS_Relation(),
 myNbShape(2),
 myTypeDist(AIS_TOD_Unknown)
{
  myFShape = aFShape;
  mySShape = aSShape;
  myVal = aVal;
  myText = aText;
  mySymbolPrs = DsgPrs_AS_BOTHAR;
  myAutomaticPosition = Standard_True;
  myPlane =aPlane;
  myArrowSize = myVal / 10.;
}

//=======================================================================
//function : Constructor
//purpose  : TwoEdgesLength dimension or OneEdgeOneVertexLength dimension or TwoVerticesLength dimension
//=======================================================================

AIS_LengthDimension::AIS_LengthDimension(const TopoDS_Shape& aFShape, 
                                         const TopoDS_Shape& aSShape, 
                                         const Handle(Geom_Plane)& aPlane, 
                                         const Standard_Real aVal, 
                                         const TCollection_ExtendedString& aText, 
                                         const gp_Pnt& aPosition,
                                         const DsgPrs_ArrowSide aSymbolPrs,
                                         const AIS_TypeOfDist aTypeDist, 
                                         const Standard_Real anArrowSize)
:AIS_Relation(),
 myNbShape(2),
 myTypeDist(aTypeDist)
{
  myFShape = aFShape;
  mySShape = aSShape;
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
//function : Compute
//purpose  : 
//=======================================================================

void AIS_LengthDimension::Compute(const Handle(PrsMgr_PresentationManager3d)&,
                                  const Handle(Prs3d_Presentation)& aPresentation,
                                  const Standard_Integer)
{
  aPresentation->Clear();

  if (myNbShape == 1) {
    switch (myFShape.ShapeType()) {
    case TopAbs_FACE:
      {
        // case length on a face
        ComputeOneFaceLength(aPresentation);
      }
      break;
    case TopAbs_EDGE:
      {
        // case length of an edge
        ComputeOneEdgeLength(aPresentation);
      }
      break;
    default:
      break;
    }
  }
  else if (myNbShape == 2) {
    switch (myFShape.ShapeType()) {
    case TopAbs_FACE:
      {
	if (mySShape.ShapeType () == TopAbs_FACE) {
	  // case length between two faces
	  ComputeTwoFacesLength(aPresentation);
	}
	else if (mySShape.ShapeType () == TopAbs_EDGE) {
	  ComputeEdgeFaceLength (aPresentation);
	}
      }
      break;
    case TopAbs_EDGE:
      {
        if (mySShape.ShapeType() == TopAbs_VERTEX) {      
#ifdef BUC60915
	  if( !myArrowSizeIsDefined ) 
#endif
	    myArrowSize = Abs(myVal)/100.;
          ComputeOneEdgeOneVertexLength( aPresentation,
					myDrawer,
					myText,
					myArrowSize,
					myFShape,
					mySShape,
					myPlane,
					myAutomaticPosition,
					myIsSetBndBox,
					myBndBox,
					myExtShape,
					myVal,
					myDirAttach,
					myPosition,
					myFAttach,
					mySAttach,
					mySymbolPrs );
        }
        else if (mySShape.ShapeType() == TopAbs_EDGE) {
          // case length between two edges
#ifdef BUC60915
	  if( !myArrowSizeIsDefined ) 
#endif
	    myArrowSize = Abs(myVal)/100.;
          ComputeTwoEdgesLength( aPresentation,
				myDrawer,
				myText,
				myArrowSize,
				TopoDS::Edge( myFShape ),
				TopoDS::Edge( mySShape ),
				myPlane,
				myAutomaticPosition,
				myIsSetBndBox,
				myBndBox,
				myExtShape,
				myVal,
				myDirAttach,
				myPosition,
				myFAttach,
				mySAttach,
				mySymbolPrs );
        }
      }
      break;

    case TopAbs_VERTEX:
      {
        if (mySShape.ShapeType() == TopAbs_VERTEX) {
#ifdef BUC60915
	  if( !myArrowSizeIsDefined ) 
#endif
	    myArrowSize = Abs(myVal)/100.;
          ComputeTwoVerticesLength( aPresentation,
				   myDrawer,
				   myText,
				   myArrowSize,
				   TopoDS::Vertex( myFShape ),
				   TopoDS::Vertex( mySShape ),
				   myPlane,
				   myAutomaticPosition,
				   myIsSetBndBox,
				   myBndBox,
				   myTypeDist,
				   myExtShape,
				   myVal,
				   myDirAttach,
				   myPosition,
				   myFAttach,
				   mySAttach,
				   mySymbolPrs );
        }
        else if (mySShape.ShapeType() == TopAbs_EDGE) {
#ifdef BUC60915
	  if( !myArrowSizeIsDefined ) 
#endif
	    myArrowSize = Abs(myVal)/100.;
          ComputeOneEdgeOneVertexLength( aPresentation,
					myDrawer,
					myText,
					myArrowSize,
					myFShape,
					mySShape,
					myPlane,
					myAutomaticPosition,
					myIsSetBndBox,
					myBndBox,
					myExtShape,
					myVal,
					myDirAttach,
					myPosition,
					myFAttach,
					mySAttach,
					mySymbolPrs );
        }
      }
      break;
    default:
      break;
    }
  }

}

//=======================================================================
//function : Compute
//purpose  : to avoid warning
//=======================================================================

void AIS_LengthDimension::Compute(const Handle(Prs3d_Projector)& aProjector, 
                                  const Handle(Prs3d_Presentation)& aPresentation)
{
// Standard_NotImplemented::Raise("AIS_LengthDimension::Compute(const Handle(Prs3d_Projector)&,const Handle(Prs3d_Presentation)&)");
 PrsMgr_PresentableObject::Compute( aProjector , aPresentation ) ;
}

//=======================================================================
//function : Compute
//purpose  : to avoid warning
//=======================================================================

void AIS_LengthDimension::Compute(const Handle(PrsMgr_PresentationManager2d)& aPresentationManager2d,
                                  const Handle(Graphic2d_GraphicObject)& aGraphicObject,
                                  const Standard_Integer anInteger)
{
// Standard_NotImplemented::Raise("AIS_LengthDimension::Compute(const Handle(PrsMgr_PresentationManager2d)&,const Handle(Graphic2d_GraphicObject)&,const Standard_Integer)");
 PrsMgr_PresentableObject::Compute( aPresentationManager2d ,aGraphicObject,anInteger) ;
}

void AIS_LengthDimension::Compute(const Handle_Prs3d_Projector& aProjector, const Handle_Geom_Transformation& aTransformation, const Handle_Prs3d_Presentation& aPresentation)
{
// Standard_NotImplemented::Raise("AIS_LengthDimension::Compute(const Handle_Prs3d_Projector&, const Handle_Geom_Transformation&, const Handle_Prs3d_Presentation&)");
 PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation ) ;
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================

void AIS_LengthDimension::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                           const Standard_Integer)
{
  if (myFShape.IsNull() && mySShape.IsNull()) return;
  if (myFShape.ShapeType() == TopAbs_FACE) ComputeFaceSelection(aSelection);
  else ComputeEdgeVertexSelection(aSelection);

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
//function : ComputeOneFaceLength
//purpose  : 
//=======================================================================

void AIS_LengthDimension::ComputeOneFaceLength(const Handle(Prs3d_Presentation)&)
{
  Standard_DomainError::Raise("AIS_LengthDimension::ComputeOneFaceLength : Not implemented");
}

//=======================================================================
//function : ComputeOneOneEdgeLength
//purpose  : 
//=======================================================================

void AIS_LengthDimension::ComputeOneEdgeLength(const Handle(Prs3d_Presentation)&)
{
  Standard_DomainError::Raise("AIS_LengthDimension::ComputeOneEdgeLength : Not implemented");
}

//=======================================================================
//function : ComputeTwoFacesLength
//purpose  : 
//=======================================================================

void AIS_LengthDimension::ComputeTwoFacesLength( const Handle( Prs3d_Presentation )& aPresentation )
{
  if (myFShape.ShapeType() == TopAbs_COMPOUND)  {                //rob debug from vbu?
    TopExp_Explorer E (myFShape,TopAbs_FACE);
    if (E.More())  SetFirstShape(E.Current());
  }
  if (mySShape.ShapeType() == TopAbs_COMPOUND)  {
    TopExp_Explorer E (mySShape,TopAbs_FACE);
    if (E.More())  SetSecondShape(E.Current()); // rob debug from vbu?
  }

  if (myFirstSurfType == AIS_KOS_Plane)
    {
      AIS::ComputeLengthBetweenPlanarFaces( TopoDS::Face(myFShape),
                                            TopoDS::Face(mySShape),
                                            myFirstPlane,
                                            mySecondPlane,
                                            myVal,
                                            myFAttach,
                                            mySAttach,
                                            myDirAttach,
                                            myAutomaticPosition,
                                            myPosition );
      if (myAutomaticPosition && myIsSetBndBox)
	myPosition = AIS::TranslatePointToBound( myPosition, myDirAttach, myBndBox );

      myDrawer->LengthAspect()->Arrow1Aspect()->SetLength( myArrowSize );
      myDrawer->LengthAspect()->Arrow2Aspect()->SetLength( myArrowSize );

      // Find text of the face

      DsgPrs_LengthPresentation::Add( aPresentation,
				      myDrawer,
				      myText,
				      myFAttach,
				      mySAttach,
				      myFirstPlane,
				      myDirAttach,
				      myPosition,
				      mySymbolPrs );
    }
  else
    {
      AIS::ComputeLengthBetweenCurvilinearFaces( TopoDS::Face(myFShape),
                                                 TopoDS::Face(mySShape),
                                                 myFirstBasisSurf,
                                                 mySecondBasisSurf,
                                                 myAutomaticPosition,
                                                 myVal,
                                                 myPosition,
                                                 myFAttach,
                                                 mySAttach,
                                                 myDirAttach );
      if (myAutomaticPosition && myIsSetBndBox)
	myPosition = AIS::TranslatePointToBound( myPosition, myDirAttach, myBndBox );

      DsgPrs_LengthPresentation::Add( aPresentation,
                                      myDrawer,
                                      myText,
                                      mySecondBasisSurf,
                                      myFAttach,
                                      mySAttach,
                                      myDirAttach,
                                      myPosition,
                                      mySymbolPrs );
    }
}


//=======================================================================
//function : ComputeEdgeFaceLength
//purpose  : Jean-Claude Vauthier 17/06/98
//           A quick implementation in order to edit the constraint of 
//           distance for a chamfer 3D.   
//=======================================================================

void AIS_LengthDimension::ComputeEdgeFaceLength (const Handle(Prs3d_Presentation )& aPresentation )
{

  //The first attachment point is P1 from the reference Edge
  //Find the second attachment point which belongs to the reference face
  //Iterate over the edges of the face and find the point FP1...
  //....it is the closest point according to P1

  const TopoDS_Edge& E = TopoDS::Edge (mySShape); //The reference edge
  const TopoDS_Face& F = TopoDS::Face (myFShape); //The reference face
  TopoDS_Vertex V1, V2;
  TopExp::Vertices (E, V1, V2);
  myFAttach = BRep_Tool::Pnt (V1);
  gp_Pnt P  = BRep_Tool::Pnt (V2);

  TopExp_Explorer It (F, TopAbs_EDGE);
  gp_Pnt2d FP1uv, FP2uv;
  Standard_Real Dist1 = RealLast ();
  Standard_Real Dist2 = RealLast ();
  for (; It.More (); It.Next ()) {
    const TopoDS_Edge FE = TopoDS::Edge(It.Current ());
    TopExp::Vertices (FE, V1, V2);
    gp_Pnt FP1c = BRep_Tool::Pnt (V1);
    gp_Pnt FP2c = BRep_Tool::Pnt (V2);
    Standard_Real Dc1 = myFAttach.SquareDistance (FP1c);
    Standard_Real Dc2 = myFAttach.SquareDistance (FP2c);
    if (Dc1 <= Dc2) {
      if (Dc1 <= Dist1) {
	Dc2 = P.SquareDistance (FP2c);
	if (Dc2 <= Dist2) {
	  mySAttach  = FP1c;
	  Dist1 = Dc1;
	  Dist2 = Dc2;
	  BRep_Tool::UVPoints (FE, F, FP1uv, FP2uv); 
	}
      }
    }
    else {
      if (Dc2 <= Dist1) {
	Dc1 = P.SquareDistance (FP1c);
	if (Dc1 <= Dist2) {
	  mySAttach = FP2c;
	  Dist1 = Dc2;
	  Dist2 = Dc1;
	  BRep_Tool::UVPoints (FE, F, FP2uv, FP1uv); 
	}
      }
    }
  }

  gp_Vec OffsetDirection (0.0, 0.0, 0.0);

  //The offset direction is the normal to the face at the point FP1 
  BRepGProp_Face GF;
  GF.Load (F);
  GF.Normal (FP1uv.X(), FP1uv.Y(), P, OffsetDirection);

  if (OffsetDirection.Magnitude () > Precision::Confusion ()) {
    myDirAttach = gp_Dir (OffsetDirection);
  }
  else myDirAttach = gp::DZ ();


  gp_Vec Vt (myDirAttach);
  Vt.Multiply (1.5 * myVal); 
  myPosition = mySAttach.Translated (Vt);

  DsgPrs_LengthPresentation::Add(aPresentation,
                                 myDrawer,
                                 myText,        
				 myFAttach,
                                 mySAttach,
                                 myDirAttach,
                                 myPosition,
                                 mySymbolPrs);

}


//=======================================================================
//function : ComputeTwoEdgesLength
//purpose  : 
//=======================================================================

void AIS_LengthDimension::ComputeTwoEdgesLength( const Handle( Prs3d_Presentation )& aPresentation,
						 const Handle( AIS_Drawer )& aDrawer,
						 const TCollection_ExtendedString & aText,
						 const Standard_Real ArrowSize,
						 const TopoDS_Edge & FirstEdge,
						 const TopoDS_Edge & SecondEdge,
						 const Handle( Geom_Plane )& Plane,
						 const Standard_Boolean AutomaticPos,
						 const Standard_Boolean IsSetBndBox,
						 const Bnd_Box & BndBox,
						 Standard_Integer & ExtShape,
						 Standard_Real & Val,
						 gp_Dir & DirAttach,
						 gp_Pnt & Position,
						 gp_Pnt & FirstAttach,
						 gp_Pnt & SecondAttach,
						 DsgPrs_ArrowSide & SymbolPrs )
{
  BRepAdaptor_Curve cu1( FirstEdge );
  if (cu1.GetType() != GeomAbs_Line) return;
  BRepAdaptor_Curve cu2( SecondEdge );
  if (cu2.GetType() != GeomAbs_Line) return;
  
  // 3d lines
  Handle(Geom_Curve) geom1,geom2;
  gp_Pnt ptat11,ptat12,ptat21,ptat22;//,pint3d;
  
  Standard_Boolean isInfinite1(Standard_False),isInfinite2(Standard_False);
  Standard_Integer ext(ExtShape);
  Handle(Geom_Curve) extCurv;
  if (!AIS::ComputeGeometry(FirstEdge,
			    SecondEdge,
			    ExtShape,
			    geom1,
			    geom2,
			    ptat11,
			    ptat12,
			    ptat21,
			    ptat22,
			    extCurv,
			    isInfinite1,
			    isInfinite2,
			    Plane)) 
    {
      return;
    }
  ExtShape = ext;
  aPresentation->SetInfiniteState(isInfinite1 || isInfinite2);
  const Handle(Geom_Line)& geom_lin1 = (Handle(Geom_Line)&) geom1;
  const Handle(Geom_Line)& geom_lin2 = (Handle(Geom_Line)&) geom2;
  const gp_Lin& l1 = geom_lin1->Lin();
  const gp_Lin& l2 = geom_lin2->Lin();
  
  // New: computation of myVal
  Val = l1.Distance( l2 );

  DirAttach = l1.Direction();

  // size
  Standard_Real arrsize = ArrowSize;
 
  if (AutomaticPos) {
    gp_Pnt curpos;
    if ( !isInfinite1 ) {
      gp_Pnt p2 = ElCLib::Value(ElCLib::Parameter(l2,ptat11),l2);
      curpos.SetXYZ((ptat11.XYZ()+p2.XYZ())/2.);
    }
    else if (!isInfinite2) {
      gp_Pnt p2 = ElCLib::Value(ElCLib::Parameter(l1,ptat21),l1);
      curpos.SetXYZ((ptat21.XYZ()+p2.XYZ())/2.);
    }
    else {
      curpos.SetXYZ((l1.Location().XYZ()+l2.Location().XYZ())/2.);
    }
    // offset to avoid confusion Edge and Dimension
    gp_Vec offset(DirAttach);
    offset = offset*ArrowSize*(-10.);
    curpos.Translate(offset);
    Position = curpos;
  }
  else {    // the point is projected in the plane
    // it is patch!
    Position = AIS::ProjectPointOnPlane( Position, Plane->Pln() );
  }

  // find attachment points 
  if (!isInfinite1) {
    if (Position.Distance(ptat11) > Position.Distance(ptat12)) FirstAttach = ptat12;
    else FirstAttach = ptat11;
  }
  else {
    FirstAttach = ElCLib::Value(ElCLib::Parameter(l1,Position),l1);
  }
  
  if (!isInfinite2) {
    if (Position.Distance(ptat21) > Position.Distance(ptat22)) SecondAttach = ptat22;
    else SecondAttach = ptat21;
  }
  else {
    SecondAttach = ElCLib::Value(ElCLib::Parameter(l2,Position),l2);
  }
  
  Standard_Real confusion(Precision::Confusion());
  if (arrsize < confusion) arrsize = Val/10.;
  if (Abs(Val) <= confusion) {arrsize = 0.;}

  Handle(Prs3d_LengthAspect) la = aDrawer->LengthAspect();
  Handle(Prs3d_ArrowAspect) arr = la->Arrow1Aspect();  
  arr->SetLength(arrsize);
  arr = la->Arrow2Aspect();
  arr->SetLength(arrsize);

  if ( ExtShape == 1)
    SymbolPrs = DsgPrs_AS_FIRSTPT_LASTAR;
  else if ( ExtShape == 2)
    SymbolPrs = DsgPrs_AS_FIRSTAR_LASTPT;

  if (AutomaticPos && IsSetBndBox)
    Position = AIS::TranslatePointToBound( Position, DirAttach, BndBox );
  
  DsgPrs_LengthPresentation::Add(aPresentation,
                                 aDrawer,
                                 aText,        
				 FirstAttach,
                                 SecondAttach,
                                 DirAttach,
                                 Position,
                                 SymbolPrs);

  if ( (ExtShape != 0) &&  !extCurv.IsNull()) {
    gp_Pnt pf, pl;
    if ( ExtShape == 1 ) {
      if (!isInfinite1) {
        pf = ptat11; 
        pl = ptat12;
      }
      AIS::ComputeProjEdgePresentation( aPresentation, aDrawer, FirstEdge, geom_lin1, pf, pl );
    }
    else {
      if (!isInfinite2) {
        pf = ptat21; 
        pl = ptat22;
      }
      AIS::ComputeProjEdgePresentation( aPresentation, aDrawer, SecondEdge, geom_lin2, pf, pl );
    }
  }
}

//=======================================================================
//function : ComputeOneEdgeOneVertexLength
//purpose  : 
//=======================================================================

void AIS_LengthDimension::ComputeOneEdgeOneVertexLength( const Handle( Prs3d_Presentation )& aPresentation,
							const Handle( AIS_Drawer )& aDrawer,
							const TCollection_ExtendedString & aText,
							const Standard_Real ArrowSize,
							const TopoDS_Shape & FirstShape,
							const TopoDS_Shape & SecondShape,
							const Handle( Geom_Plane )& Plane,
							const Standard_Boolean AutomaticPos,
							const Standard_Boolean IsSetBndBox,
							const Bnd_Box & BndBox,
							Standard_Integer & ExtShape,
							Standard_Real & Val,
							gp_Dir & DirAttach,
							gp_Pnt & Position,
							gp_Pnt & FirstAttach,
							gp_Pnt & SecondAttach,
							DsgPrs_ArrowSide & SymbolPrs )
{
  TopoDS_Vertex thevertex;
  TopoDS_Edge theedge;
  Standard_Integer numedge;
  
  if (FirstShape.ShapeType() == TopAbs_VERTEX) {
    thevertex = TopoDS::Vertex(FirstShape);
    theedge   = TopoDS::Edge(SecondShape);
    numedge = 2;// edge = 2nd shape
  }
  else {
    thevertex = TopoDS::Vertex(SecondShape);
    theedge   = TopoDS::Edge(FirstShape);
    numedge = 1;  // edge = 1st shape
  }

  gp_Pnt ptonedge1,ptonedge2;
  Handle(Geom_Curve) aCurve;
  Handle(Geom_Curve) extCurv;
  Standard_Boolean isInfinite;
  Standard_Boolean isOnPlanEdge, isOnPlanVertex;
  if (!AIS::ComputeGeometry(theedge,aCurve,ptonedge1,ptonedge2,extCurv,isInfinite,isOnPlanEdge,Plane))
    return;
  aPresentation->SetInfiniteState(isInfinite);
  AIS::ComputeGeometry(thevertex, FirstAttach, Plane, isOnPlanVertex);

  // take into consideration that only the curve can be projected 
  if (!isOnPlanEdge && !isOnPlanVertex)
    return;

  if (!isOnPlanEdge) {
    if (numedge == 1) ExtShape = 1;
    else ExtShape = 2;
  }
  else if (!isOnPlanVertex) {
    if (numedge == 1) ExtShape = 2;
    else ExtShape = 1;
  }
  

  const Handle(Geom_Line)& geom_lin = (Handle(Geom_Line)&) aCurve;
  const gp_Lin& l = geom_lin->Lin();

  // New: computation of Val
  Val = l.Distance( FirstAttach );

  DirAttach = l.Direction();
  // size
  Standard_Real arrsize = ArrowSize;
  if (Abs(Val) <= Precision::Confusion()) {arrsize = 0.;}

  if (AutomaticPos) {
    gp_Pnt p = ElCLib::Value(ElCLib::Parameter(l,FirstAttach),l);
    gp_Pnt curpos((FirstAttach.XYZ()+p.XYZ())/2.);
    // offset to avoid confusion Edge and Dimension
    gp_Vec offset(DirAttach);
    offset = offset*ArrowSize*(-10.);
    curpos.Translate(offset);
    Position = curpos;
  }
  else {    // the point is projected in the plane
    // it is patch!
    Position = AIS::ProjectPointOnPlane( Position, Plane->Pln() );

    /*
    Standard_Real u,v;
    ElSLib::Parameters(Plane->Pln() , Position, u, v);
    Position = ElSLib::Value(u,v,Plane->Pln());
    */
  }
  
  if (!isInfinite) {
    if (Position.Distance(ptonedge1) > Position.Distance(ptonedge2)) SecondAttach = ptonedge2;
    else SecondAttach = ptonedge1;
  }
  else {
    SecondAttach = ElCLib::Value(ElCLib::Parameter(l,Position),l);
  }
  
  Handle(Prs3d_LengthAspect) la = aDrawer->LengthAspect();
  Handle(Prs3d_ArrowAspect) arr = la->Arrow1Aspect();  
  arr->SetLength(arrsize);
  arr = la->Arrow2Aspect();
  arr->SetLength(arrsize);

  if (AutomaticPos && IsSetBndBox)
    Position = AIS::TranslatePointToBound( Position, DirAttach, BndBox );

  DsgPrs_LengthPresentation::Add(aPresentation,
                                 aDrawer,
                                 aText,
                                 FirstAttach,
                                 SecondAttach,
                                 DirAttach,
                                 Position,
                                 SymbolPrs);  

  //Display des morceaux de raccordement vers la curve si elle
  // n'est pas dans le WP
 if (ExtShape != 0) {
   
   if (!extCurv.IsNull()) { // c'est l'edge qui n'est pas dans le WP
     AIS::ComputeProjEdgePresentation(aPresentation,aDrawer,theedge,geom_lin,ptonedge1,ptonedge2);
   }
   else { //  c'est le point qui n'est pas dans le WP
     AIS::ComputeProjVertexPresentation(aPresentation,aDrawer,thevertex,FirstAttach);
   }
 }
}

//=======================================================================
//function : ComputeTwoVerticesLength
//purpose  : 
//=======================================================================

void AIS_LengthDimension::ComputeTwoVerticesLength( const Handle( Prs3d_Presentation )& aPresentation,
						   const Handle( AIS_Drawer )& aDrawer,
						   const TCollection_ExtendedString& aText,
						   const Standard_Real ArrowSize,
						   const TopoDS_Vertex& FirstVertex,
						   const TopoDS_Vertex& SecondVertex,
						   const Handle( Geom_Plane )& Plane,
						   const Standard_Boolean AutomaticPos,
						   const Standard_Boolean IsSetBndBox,
						   const Bnd_Box& BndBox,
						   const AIS_TypeOfDist TypeDist,
						   Standard_Integer& ExtShape,
						   Standard_Real& Val,
						   gp_Dir& DirAttach,
						   gp_Pnt& Position,
						   gp_Pnt& FirstAttach,
						   gp_Pnt& SecondAttach,
						   DsgPrs_ArrowSide& SymbolPrs )
{
  Standard_Boolean isOnPlane1, isOnPlane2;

  AIS::ComputeGeometry( FirstVertex, FirstAttach, Plane, isOnPlane1);
  AIS::ComputeGeometry( SecondVertex, SecondAttach, Plane, isOnPlane2);

  // New: computation of Val
  Val = FirstAttach.Distance( SecondAttach );

  if (ExtShape == 0) {
    if (isOnPlane1 && isOnPlane2)
      ExtShape = 0;
    else if ( isOnPlane1 && !isOnPlane2)
      ExtShape = 2;
    else if (!isOnPlane1 && isOnPlane2)
      ExtShape = 1;
    else
      return ;
  }
  Standard_Real confusion(Precision::Confusion());
  Standard_Boolean samePoint(FirstAttach.IsEqual(SecondAttach,confusion));

  if (TypeDist == AIS_TOD_Vertical) DirAttach =  Plane->Pln().XAxis().Direction();
  else if (TypeDist == AIS_TOD_Horizontal) DirAttach =  Plane->Pln().YAxis().Direction();
  else {
    if (!samePoint) {
      DirAttach = gce_MakeDir(FirstAttach,SecondAttach);
      DirAttach.Rotate(Plane->Pln().Axis(),PI/2.);
    }
  }
  
  // size
  //Standard_Real arrsize = ArrowSize;
  //if (Abs(Val) <= confusion) arrsize =0.;

  if (AutomaticPos) {
   if (!samePoint) {
     gp_Pnt curpos((FirstAttach.XYZ()+SecondAttach.XYZ())/2.);
     // offset to avoid confusion Edge and Dimension
     gp_Vec offset(DirAttach);
     offset = offset*ArrowSize*(-10.);
     curpos.Translate(offset);
     Position = curpos;
   }
   else {     
     Position = gp_Pnt(FirstAttach.XYZ()+gp_XYZ(1.,1.,1.));
     // it is patch!
     Position = AIS::ProjectPointOnPlane( Position, Plane->Pln() );
     DirAttach = gce_MakeDir(FirstAttach,Position);
   }
  }
  else {   
    // it is patch!
    Position = AIS::ProjectPointOnPlane( Position, Plane->Pln() );
  }

 
  Handle(Prs3d_LengthAspect) la = aDrawer->LengthAspect();
  Handle(Prs3d_ArrowAspect) arr = la->Arrow1Aspect();  
  arr->SetLength(ArrowSize);
  arr = la->Arrow2Aspect();
  arr->SetLength(ArrowSize);

  // Type of arrows
  if ( ExtShape == 1) SymbolPrs = DsgPrs_AS_FIRSTPT_LASTAR;
  else if (ExtShape == 2) SymbolPrs = DsgPrs_AS_FIRSTAR_LASTPT;

  if (AutomaticPos && IsSetBndBox)
    Position = AIS::TranslatePointToBound( Position, DirAttach, BndBox );

  DsgPrs_LengthPresentation::Add(aPresentation,
                                 aDrawer,
                                 aText,
                                 FirstAttach,
                                 SecondAttach,
                                 DirAttach,
                                 Position,
                                 SymbolPrs);

  // Calculate the projection of the vertex
  if ( ExtShape == 1)
    AIS::ComputeProjVertexPresentation(aPresentation, aDrawer, FirstVertex, FirstAttach);
  else if ( ExtShape == 2)
    AIS::ComputeProjVertexPresentation(aPresentation, aDrawer, SecondVertex, SecondAttach);
}

//=======================================================================
//function : ComputeTwoFaceSelection
//purpose  : 
//=======================================================================

void AIS_LengthDimension::ComputeFaceSelection( const Handle( SelectMgr_Selection )& aSelection )
{
  Handle( SelectMgr_EntityOwner ) own = new SelectMgr_EntityOwner( this, 7 );
  Handle( Select3D_SensitiveSegment ) seg;
  Handle( Geom_TrimmedCurve ) curve; 
  Handle( Select3D_SensitiveCurve ) SensCurve;

  Standard_Real ArrowLength = myDrawer->AngleAspect()->ArrowAspect()->Length();

  gp_Pnt EndOfArrow1, EndOfArrow2;
  gp_Dir DirOfArrow1;

  if (myFirstSurfType == AIS_KOS_Plane)
    {
      DsgPrs::ComputePlanarFacesLengthPresentation( ArrowLength,
						    ArrowLength,
						    myFAttach,
						    mySAttach,
						    myDirAttach,
						    myPosition,
						    myFirstPlane,
						    EndOfArrow1,
						    EndOfArrow2,
						    DirOfArrow1 );
      //Add attach lines
      seg = new Select3D_SensitiveSegment( own, myFAttach, EndOfArrow1 );
      aSelection->Add( seg );
      
      seg = new Select3D_SensitiveSegment( own, mySAttach, EndOfArrow2 );
      aSelection->Add( seg );
    }
  else // curvilinear case
    {
      if(mySecondBasisSurf.IsNull())
	return;
      
      Handle( Geom_Curve ) VCurve, UCurve;
      Standard_Real FirstU, deltaU = 0.0e0, FirstV, deltaV = 0.0e0;
      
      EndOfArrow1 = myFAttach;
      DsgPrs::ComputeCurvilinearFacesLengthPresentation( ArrowLength,
                                                         ArrowLength,
                                                         mySecondBasisSurf,
                                                         myFAttach,
                                                         mySAttach,
                                                         myDirAttach,
                                                         EndOfArrow2,
                                                         DirOfArrow1,
                                                         VCurve,
                                                         UCurve,
                                                         FirstU, deltaU,
                                                         FirstV, deltaV );
      // Add attach curves
      if (Abs( deltaU ) > Precision::PConfusion())
        {
          if (deltaU > 0.0)
            curve = new Geom_TrimmedCurve( VCurve, FirstU, FirstU + deltaU );
          else
            curve = new Geom_TrimmedCurve( VCurve, FirstU + deltaU, FirstU );
          SensCurve = new Select3D_SensitiveCurve( own, curve );
          aSelection->Add( SensCurve );
        }
      if (Abs( deltaV ) > Precision::PConfusion())
        {
          if (deltaV > 0.0)
            curve = new Geom_TrimmedCurve( UCurve, FirstV, FirstV + deltaV );
          else
            curve = new Geom_TrimmedCurve( UCurve, FirstV + deltaV, FirstV );
          SensCurve = new Select3D_SensitiveCurve( own, curve );
          aSelection->Add( SensCurve );
        }
    }

  // Add the length's line
  gp_Pnt FirstPoint, LastPoint; // ends of length's line
  gp_Vec ArrowVec( DirOfArrow1 );
  ArrowVec *= ArrowLength;
  if (myVal <= Precision::Confusion())
    {
      if (myPosition.SquareDistance( EndOfArrow1 ) > ArrowLength*ArrowLength)
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
    }
  else
    {
      gp_Lin LengthLine( myPosition, DirOfArrow1 );
      Standard_Real Par1 = ElCLib::Parameter( LengthLine, EndOfArrow1 );
      Standard_Real Par2 = ElCLib::Parameter( LengthLine, EndOfArrow2 );
      if (Par1 > 0.0 && Par2 > 0.0 || Par1 < 0.0 && Par2 < 0.0)
        {
          FirstPoint = myPosition;
          LastPoint  = (Abs( Par1 ) > Abs( Par2 ))? EndOfArrow1 : EndOfArrow2;
          LastPoint.Translate( ((Abs( Par1 ) > Abs( Par2 ))? -ArrowVec : ArrowVec) );
        }
      else
        {
          FirstPoint = EndOfArrow1;
          LastPoint  = EndOfArrow2;
        }
    }
  seg = new Select3D_SensitiveSegment( own, FirstPoint, LastPoint );
  aSelection->Add( seg );
}

//=======================================================================
//function : ComputeEdgeVertexSelection
//purpose  : 
//=======================================================================

void AIS_LengthDimension::ComputeEdgeVertexSelection(const Handle(SelectMgr_Selection)& aSelection)
{
  //  ********** NB ->
  // in the case of a constraint relatively to the border of a face 
  // only the shape of this contour is valid

  // Create 2 owner for edition of constraints  
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
  

  gp_Lin L1 (myFAttach,myDirAttach);
  gp_Lin L2 (mySAttach,myDirAttach);
  gp_Pnt Proj1 = ElCLib::Value(ElCLib::Parameter(L1,myPosition),L1);
  gp_Pnt Proj2 = ElCLib::Value(ElCLib::Parameter(L2,myPosition),L2);
  gp_Lin L3;

  Standard_Real confusion(Precision::Confusion());  

  if (!Proj1.IsEqual(Proj2,confusion)) L3 = gce_MakeLin(Proj1,Proj2);
  else { 
    // cas of zero dimension
    // own1 is chosen by default
    L3 = gce_MakeLin(Proj1,myDirAttach);
    Standard_Real size(Min(myVal/100.+1.e-6,myArrowSize+1.e-6));
    Handle(Select3D_SensitiveBox) box = new Select3D_SensitiveBox(own1,myPosition.X(),myPosition.Y(),myPosition.Z(),
								  myPosition.X()+size,
								  myPosition.Y()+size,
								  myPosition.Z()+size);
    aSelection->Add(box);
  }

  Standard_Real parmin,parmax,parcur;
  parmin = ElCLib::Parameter(L3,Proj1);
  parmax = parmin;
  
  parcur = ElCLib::Parameter(L3,Proj2);
  parmin = Min(parmin,parcur);
  parmax = Max(parmax,parcur);

  parcur = ElCLib::Parameter(L3,myPosition);
  parmin = Min(parmin,parcur);
  parmax = Max(parmax,parcur);

  gp_Pnt PointMin = ElCLib::Value(parmin,L3);
  gp_Pnt PointMax = ElCLib::Value(parmax,L3);

  Handle(Select3D_SensitiveSegment) seg;

  if (myFAttach.IsEqual(mySAttach,confusion) && !myPosition.IsEqual(mySAttach,confusion)) { 
      seg = new Select3D_SensitiveSegment(own1,mySAttach,myPosition);
      aSelection->Add(seg);
  }
  if (!PointMin.IsEqual(PointMax,confusion)) {    
    gp_Pnt MiddlePoint( (PointMin.XYZ() + PointMax.XYZ())/2);
    seg = new Select3D_SensitiveSegment(own1,PointMin,MiddlePoint);
    aSelection->Add(seg);
    seg = new Select3D_SensitiveSegment(own2,MiddlePoint, PointMax);
    aSelection->Add(seg);
  }

  if (!myFAttach.IsEqual(Proj1,confusion)) {
    seg = new Select3D_SensitiveSegment(own1,myFAttach,Proj1);
    aSelection->Add(seg);
  }
  if (!mySAttach.IsEqual(Proj2,confusion)) {
    seg = new Select3D_SensitiveSegment(own2,mySAttach,Proj2);
    aSelection->Add(seg);
  }
}




//=======================================================================
//function : KindOfDimension
//purpose  : 
//=======================================================================
 AIS_KindOfDimension AIS_LengthDimension::KindOfDimension() const 
{
  return AIS_KOD_LENGTH;
}

//=======================================================================
//function : IsMovable
//purpose  : 
//=======================================================================
 Standard_Boolean AIS_LengthDimension::IsMovable() const 
{
  return Standard_True;
}

//=======================================================================
//function : SetFirstShape
//purpose  : 
//=======================================================================
void AIS_LengthDimension::SetFirstShape( const TopoDS_Shape& aFShape )
{
  myFShape = aFShape;

  if (myFShape.ShapeType() == TopAbs_FACE)
    AIS::InitFaceLength( TopoDS::Face( myFShape ), myFirstPlane, myFirstBasisSurf, myFirstSurfType, myFirstOffset );
}

//=======================================================================
//function : SetSecondShape
//purpose  : 
//=======================================================================
void AIS_LengthDimension::SetSecondShape( const TopoDS_Shape& aSShape )
{
  mySShape = aSShape;

  if (mySShape.ShapeType() == TopAbs_FACE)
    AIS::InitFaceLength( TopoDS::Face( mySShape ), mySecondPlane, mySecondBasisSurf, mySecondSurfType, mySecondOffset );
}
