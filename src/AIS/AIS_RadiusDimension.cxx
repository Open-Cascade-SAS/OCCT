// Created on: 1996-12-05
// Created by: Jean-Pierre COMBE/Odile Olivier/Serguei Zaritchny
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

// modified     <SZY> 20-feb-98
//              <VBU> myFShape could be a wire.

#define BUC60915        //GG 05/06/01 Enable to compute the requested arrow size
//                      if any in all dimensions.

#include <Standard_NotImplemented.hxx>

#include <AIS_RadiusDimension.ixx>

#include <AIS_DimensionOwner.hxx>
#include <DsgPrs_RadiusPresentation.hxx>
#include <DsgPrs.hxx>

#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>

#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <ElCLib.hxx>
#include <GC_MakeCircle.hxx>
#include <gce_MakeLin.hxx>
#include <gce_MakeCirc.hxx>

#include <SelectMgr_EntityOwner.hxx>
#include <Select3D_SensitiveSegment.hxx>

#include <Precision.hxx>

#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopLoc_Location.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <Adaptor3d_HCurve.hxx>

#include <Prs3d_AngleAspect.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LengthAspect.hxx>
#include <Prs3d_Drawer.hxx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>


#include <AIS.hxx>
#include <AIS_Drawer.hxx>
#include <gce_MakeDir.hxx>
#include <Select3D_SensitiveBox.hxx>



//======================================================================
//function : CircleFromPlanarFace
//purpose  : if possible gets circle from planar face
//=======================================================================
static Standard_Boolean CircleFromPlanarFace (const TopoDS_Shape& aFace,
					      Handle(Geom_Curve)& aCurve, 
					      gp_Pnt & ptfirst,
					      gp_Pnt & ptend)
{
  TopExp_Explorer ExploEd (aFace, TopAbs_EDGE);
  for ( ; ExploEd.More(); ExploEd.Next())
    {
      TopoDS_Edge curedge =  TopoDS::Edge (ExploEd.Current());
      if (AIS::ComputeGeometry (curedge, aCurve, ptfirst, ptend))
	if (aCurve->IsInstance (STANDARD_TYPE(Geom_Circle)) && 
	    !Handle(Geom_Circle)::DownCast(aCurve).IsNull())
	  return Standard_True;
    }
  return Standard_False;
}


//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
AIS_RadiusDimension::AIS_RadiusDimension(const TopoDS_Shape& aShape, 
					 const Standard_Real aVal, 
					 const TCollection_ExtendedString& aText)
:mydrawFromCenter(Standard_True)
{
  myFShape = aShape;
  myVal = aVal;
  myText = aText;
  mySymbolPrs = DsgPrs_AS_FIRSTPT_LASTAR;
  myAutomaticPosition = Standard_True;

  myArrowSize = myVal / 100.;
  InitFirstShape();
}

//=======================================================================
//function : Constructor
//purpose  :(avec position et texte)
//=======================================================================
AIS_RadiusDimension::AIS_RadiusDimension( const TopoDS_Shape& aShape, 
					  const Standard_Real aVal, 
					  const TCollection_ExtendedString& aText, 
					  const gp_Pnt& aPosition,
					  const DsgPrs_ArrowSide aSymbolPrs, 
					  const Standard_Real anArrowSize )
:mydrawFromCenter(Standard_True)
{
  myFShape = aShape;
  myVal = aVal;
  myText = aText;
  myPosition = aPosition;
  mySymbolPrs = aSymbolPrs;
#ifdef BUC60915
  SetArrowSize( anArrowSize );
#else
  myArrowSize = anArrowSize;
#endif
  myAutomaticPosition = Standard_False;
  InitFirstShape();
}

//=======================================================================
//function : InitFirstShape
//purpose  : 
//=======================================================================
void AIS_RadiusDimension::InitFirstShape()
{
  if (myFShape.ShapeType() == TopAbs_FACE)
    {
      BRepAdaptor_Surface surfAlgo( TopoDS::Face( myFShape ) );
      Standard_Real uFirst, uLast, vFirst, vLast;
      uFirst = surfAlgo.FirstUParameter();
      uLast = surfAlgo.LastUParameter();
      vFirst = surfAlgo.FirstVParameter();
      vLast = surfAlgo.LastVParameter();
      Standard_Real uMoy = (uFirst + uLast)/2;
      Standard_Real vMoy = (vFirst + vLast)/2;
      Handle( Geom_Surface ) surf = surfAlgo.Surface().Surface();
      surf = Handle( Geom_Surface )::DownCast( surf->Transformed( surfAlgo.Trsf() ) );
      Handle( Geom_Curve ) aCurve;

      if (surf->DynamicType() == STANDARD_TYPE(Geom_ToroidalSurface))
	{
	  aCurve = surf->UIso( uMoy );
	  uFirst = vFirst;
	  uLast = vLast;
	}
      else if (surf->DynamicType() == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)) 
	{
	  Handle( Adaptor3d_HCurve ) BasisCurve = surfAlgo.BasisCurve();
	  if ( BasisCurve->GetType() == GeomAbs_Circle )
	    aCurve = surf->VIso( vMoy );
	  else return;
	}
      else if (surf->DynamicType() == STANDARD_TYPE(Geom_Plane)) 
	{
	  gp_Pnt FirstPnt, LastPnt;
	  if (CircleFromPlanarFace (TopoDS::Face(myFShape), aCurve, FirstPnt, LastPnt))
//	  if (CircleFromPlanarFace (myFShape, aCurve, FirstPnt, LastPnt))
	    {
	      uFirst = ElCLib::Parameter ((Handle(Geom_Circle)::DownCast( aCurve ))->Circ(), FirstPnt);
	      uLast  = ElCLib::Parameter ((Handle(Geom_Circle)::DownCast( aCurve ))->Circ(), LastPnt);
	    }
	  else return;
	}
      else
	aCurve = surf->VIso( vMoy );
      
      if (aCurve->DynamicType() == STANDARD_TYPE(Geom_Circle))
	myCircle = (Handle( Geom_Circle )::DownCast( aCurve ))->Circ();
      else if (aCurve->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve)) {
	Handle(Geom_TrimmedCurve) tCurve = Handle(Geom_TrimmedCurve)::DownCast(aCurve); 
	aCurve = tCurve->BasisCurve();
	uFirst = tCurve->FirstParameter();
	uLast  = tCurve->LastParameter();
	if (aCurve->DynamicType() == STANDARD_TYPE(Geom_Circle))
	  myCircle = Handle(Geom_Circle)::DownCast(aCurve)->Circ();//gp_Circ
	else return;
      }
      else if (aCurve->DynamicType() == STANDARD_TYPE(Geom_BSplineCurve)) {
	gp_Pnt P1,P2,P3;
	aCurve->D0(uFirst,P1);
	aCurve->D0((uFirst+uLast)/2,P2);
	aCurve->D0(uLast,P3);
	gce_MakeCirc MakeCirc = gce_MakeCirc(P1,P2,P3);
	myCircle = MakeCirc.Value();
	uFirst = ElCLib::Parameter(myCircle,P1);
	uLast = ElCLib::Parameter(myCircle,P2);
      }

      myFirstPar = uFirst;
      myLastPar  = uLast;
    } // TopAbs_FACE

  else // it is edge or a wire
    {
      TopoDS_Edge anEdge;
      if (myFShape.ShapeType() == TopAbs_WIRE)  {
	TopExp_Explorer exp (myFShape,TopAbs_EDGE);
	if (exp.More())  anEdge = TopoDS::Edge (exp.Current());
      }
      else {
	if ( myFShape.ShapeType() == TopAbs_EDGE) { 
	  anEdge = TopoDS::Edge (myFShape);
	  BRepAdaptor_Curve AdaptedCurve (anEdge);
	  if (!AdaptedCurve.GetType() == GeomAbs_Circle) return;
	  
	  myCircle = AdaptedCurve.Circle();
	  myFirstPar = AdaptedCurve.FirstParameter();
	  myLastPar = AdaptedCurve.LastParameter();
	}
#ifdef DEB
	else {
	  cout << "AIS_RadiusDimension::InitFirstShape ==> myFShape.ShapeType() == " << myFShape.ShapeType()  << endl;
	}
#endif
      }
    } 

  myCenter = myCircle.Location();
  myCircle.SetRadius(myVal);
  while (myFirstPar > 2*M_PI) myFirstPar -= 2*M_PI;
  while (myFirstPar < 0.0 ) myFirstPar += 2*M_PI;
  while (myLastPar  > 2*M_PI) myLastPar  -= 2*M_PI;
  while (myLastPar  < 0.0 ) myLastPar  += 2*M_PI;
  myPlane = new Geom_Plane(gp_Pln(gp_Ax3(myCircle.Position())));
  myFirstLine = gce_MakeLin( myCenter, ElCLib::Value( myFirstPar, myCircle ) );
  myLastLine  = gce_MakeLin( myCenter, ElCLib::Value( myLastPar, myCircle ) );
}

//=======================================================================
//function : SetFirstShape
//purpose  : 
//=======================================================================
void AIS_RadiusDimension::SetFirstShape( const TopoDS_Shape& aFShape )
{
  myFShape = aFShape;
  InitFirstShape();
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void AIS_RadiusDimension::Compute(const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
				  const Handle(Prs3d_Presentation)& aPresentation, 
				  const Standard_Integer /*aMode*/)
{
  aPresentation->Clear();

  ComputeRadius( aPresentation );
}

//=======================================================================
//function : Compute
//purpose  :  to avoid warning
//=======================================================================
void AIS_RadiusDimension::Compute(const Handle(Prs3d_Projector)& aProjector, 
				  const Handle(Prs3d_Presentation)& aPresentation)
{
// Standard_NotImplemented::Raise("AIS_RadiusDimension::Compute(const Handle(Prs3d_Projector)& aProjector,const Handle(Prs3d_Presentation)& aPresentation)");
 PrsMgr_PresentableObject::Compute( aProjector , aPresentation ) ;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_RadiusDimension::Compute(const Handle_Prs3d_Projector& aProjector,
                                  const Handle_Geom_Transformation& aTransformation,
                                  const Handle_Prs3d_Presentation& aPresentation)
{
// Standard_NotImplemented::Raise("AIS_RadiusDimension::Compute(const Handle_Prs3d_Projector&, const Handle_Geom_Transformation&, const Handle_Prs3d_Presentation&)");
 PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation ) ;
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================
void AIS_RadiusDimension::ComputeSelection( const Handle(SelectMgr_Selection)& aSelection, 
					    const Standard_Integer )
{
  gp_Pnt LineOrigin, LineEnd;
  DsgPrs::ComputeRadiusLine( myCenter, myEndOfArrow, myPosition, mydrawFromCenter,
			    LineOrigin,LineEnd); 
  Handle(AIS_DimensionOwner) own = new AIS_DimensionOwner(this,7);
  own->SetShape(myFShape);
  Handle( Select3D_SensitiveSegment ) seg = new Select3D_SensitiveSegment( own, LineOrigin, LineEnd );
  aSelection->Add( seg );

  // Text
  Standard_Real size(Min(myVal/100.+1.e-6,myArrowSize+1.e-6));
  Handle( Select3D_SensitiveBox ) box = new Select3D_SensitiveBox( own,
                                                                   myPosition.X(),
                                                                   myPosition.Y(),
                                                                   myPosition.Z(),
                                                                   myPosition.X() + size,
                                                                   myPosition.Y() + size,
                                                                   myPosition.Z() + size );
  aSelection->Add(box);
}

//=======================================================================
//function : ComputeRadius
//purpose  : 
//=======================================================================

void AIS_RadiusDimension::ComputeRadius( const Handle( Prs3d_Presentation )& aPresentation )
{
   if (myAutomaticPosition)
     {
       InitFirstShape();
       myEndOfArrow = ElCLib::Value( (myFirstPar + myLastPar)/2, myCircle );
       if(mydrawFromCenter) {
	 myPosition = myCenter;
       }
       else 
	 {
	   gp_Vec v1(myCenter, myEndOfArrow);
	   myPosition = myCenter.Translated(v1 * 1.2);
	 }
       myAutomaticPosition = Standard_True;
       
       if (myIsSetBndBox)
	 myPosition = AIS::TranslatePointToBound(myPosition, gce_MakeDir( myCenter, myEndOfArrow ),
						 myBndBox );       
     }
   else
     {
       //!Automaticposition
       myPosition = AIS::ProjectPointOnPlane( myPosition, myPlane->Pln() );
       Standard_Real PosPar = ElCLib::Parameter( myCircle, myPosition );
       if (!AIS::InDomain(myFirstPar, myLastPar, PosPar))
	 { // not in domain
	   Standard_Real otherpar = PosPar + M_PI;
	   if (otherpar > 2*M_PI) otherpar -= 2*M_PI;
	   if (AIS::InDomain(myFirstPar, myLastPar, otherpar)){
	     PosPar = otherpar;// parameter on circle
	     myEndOfArrow = ElCLib::Value( PosPar, myCircle );
	   }
	   else {
	     Standard_Real Teta1 = Abs( PosPar - myFirstPar ), Teta2 = Abs( PosPar - myLastPar );
	     if (Teta1 > M_PI)
	       Teta1 = 2.0*M_PI - Teta1; 
	     if (Teta2 > M_PI)
	       Teta2 = 2.0*M_PI - Teta2; 
	     if (Teta1 < Teta2) 
	       {
		 if(myFirstLine.Contains(myPosition,Precision::Confusion()))
		   PosPar = myFirstPar;
		 else
		   PosPar = myLastPar;
	       }
	     else
	       {
		 if(myLastLine.Contains(myPosition,Precision::Confusion()))
		   PosPar = myLastPar; 
		 else 
		   PosPar = myFirstPar;
	       }
	     myEndOfArrow = ElCLib::Value( PosPar, myCircle );
	     gp_Lin RadiusLine = gce_MakeLin( myCenter, myEndOfArrow );
	     // project on radius line
	     myPosition = ElCLib::Value( ElCLib::Parameter( RadiusLine, myPosition ), RadiusLine );
	   }
	 }
       else
	 myEndOfArrow = ElCLib::Value( PosPar, myCircle );
     }
   
   Handle(Prs3d_LengthAspect) la = myDrawer->LengthAspect();
   Handle(Prs3d_ArrowAspect) arr = la->Arrow1Aspect();
   // size
#ifdef BUC60915
   if( !myArrowSizeIsDefined ) {
     myArrowSize = Min(myArrowSize,myVal/5.);
   }
   arr->SetLength(myArrowSize);
#else
   if (myVal/5. > myArrowSize) 
     arr->SetLength(myArrowSize);
   else 
     arr->SetLength(myVal/5.);
#endif
   
   DsgPrs_RadiusPresentation::Add( aPresentation, myDrawer, myText, myPosition,
				  myCenter, myEndOfArrow, mySymbolPrs, mydrawFromCenter );
 }
