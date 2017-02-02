// Created on: 1997-03-04
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
#include <AIS_OffsetDimension.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <DsgPrs_OffsetPresentation.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <gce_MakeLin.hxx>
#include <Geom_Transformation.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Graphic3d_Structure.hxx>
#include <Precision.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Projector.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>
#include <StdPrs_WFShape.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_OffsetDimension,AIS_Relation)

//=======================================================================
//function : AIS_OffsetDimension
//purpose  : 
//=======================================================================
AIS_OffsetDimension::AIS_OffsetDimension(const TopoDS_Shape& FistShape, 
					 const TopoDS_Shape& SecondShape,
					 const Standard_Real aVal,
					 const TCollection_ExtendedString& aText)
:AIS_Relation(),
myFAttach(0.,0.,0.),
mySAttach(0.,0.,0.)
{
  myFShape = FistShape;
  mySShape = SecondShape;
  mySymbolPrs = DsgPrs_AS_BOTHAR;
  myVal = aVal;
  myText = aText;
  //myArrowSize = fabs (myVal/5.);
  myArrowSize = fabs (myVal/10.0);
  if (myArrowSize > 30.) myArrowSize = 30.;
  if (myArrowSize < 15.) myArrowSize = 15.;
  //cout<<"AIS_OffsetDimension::AIS_OffsetDimension " <<  myArrowSize << " myArrowSize"<<endl;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void AIS_OffsetDimension::Compute(const Handle(PrsMgr_PresentationManager3d)&,
				  const Handle(Prs3d_Presentation)& aprs,
				  const Standard_Integer)
{
  gp_Trsf aInvertTrsf = myRelativePos;
  //myArrowSize = fabs (myVal/5.);
  myArrowSize = fabs (myVal/10.0);
  if (myArrowSize > 30.) myArrowSize = 30.;
  if (myArrowSize < 15.) myArrowSize = 15.;
  //cout<<"AIS_OffsetDimension::AIS_OffsetDimension " <<  myArrowSize << " myArrowSize"<<endl;
  
  BRepAdaptor_Surface surf1(TopoDS::Face(myFShape));
  BRepAdaptor_Surface surf2(TopoDS::Face(mySShape));
  
  if (surf1.GetType() == GeomAbs_Cylinder || 
      surf1.GetType() == GeomAbs_Cone     || 
      surf1.GetType() == GeomAbs_Torus) {
    if (surf2.GetType() == GeomAbs_Cylinder ||
	surf2.GetType() == GeomAbs_Cone     ||
	surf2.GetType() == GeomAbs_Torus) {
      ComputeTwoAxesOffset(aprs, aInvertTrsf);
    } else {
      ComputeAxeFaceOffset(aprs, aInvertTrsf);
    }
  }
  else {
    //myDirAttach : oriente de myFShape vers mySShape
    gp_Pln aPln = surf1.Plane();
    gp_Pnt aPnt = aPln.Location();
    
    gp_Pln bPln = surf2.Plane();
    
    Standard_Real uPnt, vPnt;
    ElSLib::Parameters (bPln , aPnt , uPnt, vPnt);
    gp_Pnt bPnt = ElSLib::Value (uPnt, vPnt, bPln);
    if (aPnt.IsEqual(bPnt,Precision::Confusion())) {
      gp_Ax1 aAx1 = aPln.Axis();
      myDirAttach = aAx1.Direction();
    } else {
      gp_Vec aVec (aPnt,bPnt);
      myDirAttach.SetCoord(aVec.X(),aVec.Y(),aVec.Z());
    }
    ComputeTwoFacesOffset(aprs, aInvertTrsf);
  }  
}


//=======================================================================
//function : Compute
//purpose  : to avoid warning at compilation (SUN)
//=======================================================================
void AIS_OffsetDimension::Compute(const Handle(Prs3d_Projector)& /*aProjector*/,
				  const Handle(Prs3d_Presentation)& /*aPresentation*/)
{
// throw Standard_NotImplemented("AIS_OffsetDimension::Compute(const Handle(Prs3d_Projector)& aProjector,const Handle(Prs3d_Presentation)& aPresentation)");
// PrsMgr_PresentableObject::Compute( aProjector , aPresentation ) ;
}

void AIS_OffsetDimension::Compute(const Handle(Prs3d_Projector)& aProjector, 
				  const Handle(Geom_Transformation)& aTransformation,
				  const Handle(Prs3d_Presentation)& aPresentation)
{
// throw Standard_NotImplemented("AIS_OffsetDimension::Compute(const Handle(Prs3d_Projector)&,const Handle(Geom_Transformation)&,const Handle(Prs3d_Presentation)&)");
 PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation ) ;
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================
void AIS_OffsetDimension::ComputeSelection(const Handle(SelectMgr_Selection)& aSel, 
					   const Standard_Integer)
{
  //myArrowSize = fabs (myVal/5.);
  myArrowSize = fabs (myVal/10.0);
  if (myArrowSize > 30.) myArrowSize = 30.;
  if (myArrowSize < 15.) myArrowSize = 15.;
  //cout<<"AIS_OffsetDimension::AIS_OffsetDimension " <<  myArrowSize << " myArrowSize"<<endl;
  gp_Pnt myTFAttach = myFAttach.Transformed (myRelativePos);
  gp_Pnt myTSAttach = mySAttach.Transformed (myRelativePos);
  gp_Dir myTDirAttach = myDirAttach.Transformed (myRelativePos);
  gp_Dir myTDirAttach2 = myDirAttach2.Transformed (myRelativePos);
  gp_Pnt Tcurpos = myPosition.Transformed (myRelativePos);;

  gp_Lin L1 (myTFAttach,myTDirAttach);
  gp_Lin L2 (myTSAttach,myTDirAttach2);
  gp_Pnt Proj1 = ElCLib::Value(ElCLib::Parameter(L1,Tcurpos),L1);
  gp_Pnt Proj2 = ElCLib::Value(ElCLib::Parameter(L2,Tcurpos),L2);
  gp_Lin L3;

  Handle(SelectMgr_EntityOwner) own = new SelectMgr_EntityOwner(this,7);

  if (!Proj1.IsEqual(Proj2,Precision::Confusion())) {
    L3 = gce_MakeLin(Proj1,Proj2);
  }
  else {    // cas ou la dimension est nulle
    if (!Proj1.IsEqual(Tcurpos,Precision::Confusion())) {
      gp_Vec v3 (Proj1,Tcurpos);
      gp_Dir d3 (v3);
      L3 = gce_MakeLin(Proj1,d3);
    } else {
      L3 = gce_MakeLin(Proj1,myTDirAttach);
    }

  // Text
    Standard_Real size(Min(myVal/100.+1.e-6,myArrowSize+1.e-6));
    Handle( Select3D_SensitiveBox ) box = new Select3D_SensitiveBox( own,
								     Tcurpos.X(),
								     Tcurpos.Y(),
								     Tcurpos.Z(),
								     Tcurpos.X() + size,
								     Tcurpos.Y() + size,
								     Tcurpos.Z() + size);
    aSel->Add(box);
  }

  Standard_Real parmin,parmax,parcur;
  parmin = ElCLib::Parameter(L3,Proj1);
  parmax = parmin;

  parcur = ElCLib::Parameter(L3,Proj2);
  parmin = Min(parmin,parcur);
  parmax = Max(parmax,parcur);

  parcur = ElCLib::Parameter(L3,Tcurpos);
  parmin = Min(parmin,parcur);
  parmax = Max(parmax,parcur);

  gp_Pnt PointMin = ElCLib::Value(parmin,L3);
  gp_Pnt PointMax = ElCLib::Value(parmax,L3);

  Handle(Select3D_SensitiveSegment) seg;
  if (!PointMin.IsEqual(PointMax,Precision::Confusion())) {
    seg = new Select3D_SensitiveSegment(own,
					PointMin,
					PointMax);
    aSel->Add(seg);
  }
  
  if (!myTFAttach.IsEqual(Proj1,Precision::Confusion())) {
    seg = new Select3D_SensitiveSegment(own,
					myTFAttach,
					Proj1);
    aSel->Add(seg);
  }
  if (!myTSAttach.IsEqual(Proj2,Precision::Confusion())) {
    seg = new Select3D_SensitiveSegment(own,
					myTSAttach,
					Proj2);
    aSel->Add(seg);
  }
}

//=======================================================================
//function : ComputeTwoAxesOffset
//purpose  : 
//=======================================================================
void AIS_OffsetDimension::ComputeTwoAxesOffset(const Handle(Prs3d_Presentation)& aprs, 
						const gp_Trsf& aTrsf)
{
  BRepAdaptor_Surface surf1(TopoDS::Face(myFShape));
  BRepAdaptor_Surface surf2(TopoDS::Face(mySShape));

  gp_Ax1 Ax1Surf1, Ax1Surf2;

  if (surf1.GetType() == GeomAbs_Cylinder) {
    gp_Cylinder aCyl= surf1.Cylinder();
    Ax1Surf1 = aCyl.Axis();    
  } else if (surf1.GetType() == GeomAbs_Cone) {
    gp_Cone aCone= surf1.Cone();
    Ax1Surf1 = aCone.Axis();    
  } else if (surf1.GetType() == GeomAbs_Torus) {
    gp_Torus aTore= surf1.Torus();
    Ax1Surf1 = aTore.Axis();    
  }
  Standard_Real FirstUParam = surf1.FirstUParameter();
  Standard_Real FirstVParam = surf1.FirstVParameter();
  Standard_Real LastVParam  = surf1.LastVParameter();
  gp_Pnt P1First = surf1.Value(FirstUParam,FirstVParam);
  gp_Pnt P1Last  = surf1.Value(FirstUParam,LastVParam);


  if (surf2.GetType() == GeomAbs_Cylinder) {
    gp_Cylinder aCyl= surf2.Cylinder();
    Ax1Surf2 = aCyl.Axis();    
  } else if (surf2.GetType() == GeomAbs_Cone) {
    gp_Cone aCone= surf2.Cone();
    Ax1Surf2 = aCone.Axis();    
  } else if (surf2.GetType() == GeomAbs_Torus) {
    gp_Torus aTore= surf2.Torus();
    Ax1Surf2 = aTore.Axis();    
  }
  FirstUParam = surf2.FirstUParameter();
  FirstVParam = surf2.FirstVParameter();
  LastVParam  = surf2.LastVParameter();
  gp_Pnt P2First = surf2.Value(FirstUParam,FirstVParam);
  gp_Pnt P2Last  = surf2.Value(FirstUParam,LastVParam);

  
  
  myFAttach = Ax1Surf1.Location();
  mySAttach = Ax1Surf2.Location();
  myDirAttach = Ax1Surf1.Direction();
  myDirAttach2 = myDirAttach;
  gp_Pnt curpos;
  gp_Lin aProjLine  = gce_MakeLin(myFAttach,myDirAttach);

  if (myAutomaticPosition) {
    curpos.SetX ( (myFAttach.X() +  mySAttach.X()) /2. + 0.01);
    curpos.SetY ( (myFAttach.Y() +  mySAttach.Y()) /2. + 0.01);
    curpos.SetZ ( (myFAttach.Z() +  mySAttach.Z()) /2. + 0.01);
    // + 0.01 pour eviter un raise de ComputeSelection...

    myPosition = curpos;
  } 
  else {
    curpos = myPosition;
  }
  
  curpos = ElCLib::Value(ElCLib::Parameter(aProjLine,curpos),aProjLine);
  // on projette pour la presentation

  gp_Pnt P1FirstProj  = ElCLib::Value(ElCLib::Parameter(aProjLine,P1First),aProjLine);
  gp_Pnt P1LastProj   = ElCLib::Value(ElCLib::Parameter(aProjLine,P1Last),aProjLine);
  if (P1FirstProj.Distance(curpos) > P1LastProj.Distance(curpos))
    myFAttach = P1FirstProj;
  else
    myFAttach = P1LastProj;
  
  gp_Pnt P2FirstProj  = ElCLib::Value(ElCLib::Parameter(aProjLine,P2First),aProjLine);
  gp_Pnt P2LastProj   = ElCLib::Value(ElCLib::Parameter(aProjLine,P2Last),aProjLine);
  if (P2FirstProj.Distance(curpos) > P2LastProj.Distance(curpos))
    mySAttach = P2FirstProj;
  else
    mySAttach = P2LastProj;


  Handle(Prs3d_DimensionAspect) la = myDrawer->DimensionAspect();
  Handle(Prs3d_ArrowAspect) arr = la->ArrowAspect();
  //cout<<"AIS_OffsetDimension::AIS_OffsetDimension " <<  myArrowSize << " myArrowSize"<<endl;
  arr->SetLength(myArrowSize);
  arr = la->ArrowAspect();
  arr->SetLength(myArrowSize);

  gp_Pnt myTFAttach = myFAttach.Transformed (aTrsf);
  gp_Pnt myTSAttach = mySAttach.Transformed (aTrsf);
  gp_Dir myTDirAttach = myDirAttach.Transformed (aTrsf);
  gp_Dir myTDirAttach2 = myTDirAttach;
  gp_Pnt Tcurpos = curpos.Transformed (aTrsf);
 
  if (myIsSetBndBox)
    Tcurpos = AIS::TranslatePointToBound( Tcurpos, myDirAttach, myBndBox );

  DsgPrs_OffsetPresentation::AddAxes(aprs,
				     myDrawer,
				     myText,
				     myTFAttach,
				     myTSAttach,
				     myTDirAttach,
				     myTDirAttach2,
				     Tcurpos);
  
  BRepBuilderAPI_Transform transform1 (myFShape, aTrsf, Standard_True);
  TopoDS_Shape myTFShape = transform1.Shape();
  BRepBuilderAPI_Transform transform2 (mySShape, aTrsf, Standard_True);
  TopoDS_Shape myTSShape = transform2.Shape();

  StdPrs_WFShape::Add (aprs, myTFShape, myDrawer);
  StdPrs_WFShape::Add (aprs, myTSShape, myDrawer);
}

//=======================================================================
//function : ComputeTwoFacesOffset
//purpose  : 
//=======================================================================
void AIS_OffsetDimension::ComputeTwoFacesOffset(const Handle(Prs3d_Presentation)& aprs, 
						const gp_Trsf& aTrsf)
{
  gp_Dir norm1 = myDirAttach;
  gp_Pnt curpos;
  gp_Ax2 myax2;
  if (myAutomaticPosition && ! myIsSetBndBox) {
    TopExp_Explorer explo(myFShape,TopAbs_VERTEX);
    if (explo.More()) {
      TopoDS_Vertex vertref = TopoDS::Vertex(explo.Current());
      myFAttach = BRep_Tool::Pnt(vertref);
      gp_Vec trans = norm1.XYZ()*fabs(myVal/2);
      gp_Ax2 ax2(myFAttach,norm1);
      myDirAttach = ax2.XDirection();
      curpos = myFAttach.Translated(trans);
      if (myVal <= Precision::Confusion()) {
	gp_Vec vecnorm1 = norm1.XYZ()*.001;
	curpos.Translate(vecnorm1);
      }
      myPosition = curpos;
      myax2 = ax2;
    }
  }
  else {
    if (myAutomaticPosition && myIsSetBndBox)
      {
	Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
	myBndBox.Get( aXmin, aYmin, aZmin, aXmax, aYmax, aZmax );
	myPosition.SetCoord( aXmax, aYmax, aZmax );
      }
     
    curpos = myPosition;
    myFAttach = AIS::Nearest(myFShape,curpos);
    if (myFAttach.Distance(curpos) <= Precision::Confusion()) {
      gp_Ax2 ax2(myFAttach,norm1);
      myDirAttach = ax2.XDirection();
      myax2 = ax2;
    }
    else {
      gp_Dir orient(myFAttach.XYZ()-curpos.XYZ());
      gp_Ax2 ax2(myFAttach,norm1);
      if (orient.Angle(norm1) <= Precision::Angular()) {
	myDirAttach = ax2.XDirection();
      }
      else {
	gp_Dir adir = norm1 ^ orient;
	myDirAttach = adir ^ norm1;
      }
      myax2 = ax2;
    }
  }
  // en attendant mieux
  mySAttach = AIS::Nearest(mySShape,curpos);
  gp_Ax3 anax3 (myax2);
  gp_Pln apln (anax3);
  
  //gp_Pnt proj2;
  Standard_Real u2,v2, uatt, vatt;
  ElSLib::Parameters (apln , mySAttach, uatt,vatt);
  ElSLib::Parameters (apln , curpos   , u2,v2);
  
  if (uatt== u2 && vatt == v2) {
    myDirAttach2 = myDirAttach;
  } else {
    gp_Vec avec (ElSLib::Value (uatt,vatt, apln) , ElSLib::Value (u2,v2, apln));
    myDirAttach2.SetCoord (avec.X(),avec.Y(),avec.Z());
  }
  
  Handle(Prs3d_DimensionAspect) la = myDrawer->DimensionAspect();
  Handle(Prs3d_ArrowAspect) arr = la->ArrowAspect();
  //cout<<"AIS_OffsetDimension::AIS_OffsetDimension " <<  myArrowSize << " myArrowSize"<<endl;
  arr->SetLength(myArrowSize);
  arr = la->ArrowAspect();
  arr->SetLength(myArrowSize);

  gp_Pnt myTFAttach = myFAttach.Transformed (aTrsf);
  gp_Pnt myTSAttach = mySAttach.Transformed (aTrsf);
  gp_Dir myTDirAttach = myDirAttach.Transformed (aTrsf);
  gp_Dir myTDirAttach2 = myDirAttach2.Transformed (aTrsf);
  gp_Pnt Tcurpos = curpos.Transformed (aTrsf);

/*
  if (myIsSetBndBox)
    {
      BRepAdaptor_Surface surf1(TopoDS::Face(myFShape));
      Tcurpos = AIS::TranslatePointToBound( Tcurpos, surf1.Plane().XAxis().Direction(), myBndBox );
    }
*/
  DsgPrs_OffsetPresentation::Add(aprs,
				 myDrawer,
				 myText,
				 myTFAttach,
				 myTSAttach,
				 myTDirAttach,
				 myTDirAttach2,
				 Tcurpos);
  

  BRepBuilderAPI_Transform transform1 (myFShape, aTrsf, Standard_True);
  TopoDS_Shape myTFShape = transform1.Shape();
  BRepBuilderAPI_Transform transform2 (mySShape, aTrsf, Standard_True);
  TopoDS_Shape myTSShape = transform2.Shape();

  StdPrs_WFShape::Add (aprs, myTFShape, myDrawer);
  StdPrs_WFShape::Add (aprs, myTSShape, myDrawer);
}

//=======================================================================
//function : ComputeAxeFaceOffset
//purpose  : 
//=======================================================================
void AIS_OffsetDimension::ComputeAxeFaceOffset(const Handle(Prs3d_Presentation)& aprs, 
					       const gp_Trsf& aTrsf)
{
  BRepBuilderAPI_Transform transform1 (myFShape, aTrsf, Standard_True);
  TopoDS_Shape myTFShape = transform1.Shape();
  BRepBuilderAPI_Transform transform2 (mySShape, aTrsf, Standard_True);
  TopoDS_Shape myTSShape = transform2.Shape();

  StdPrs_WFShape::Add (aprs, myTFShape, myDrawer);
  StdPrs_WFShape::Add (aprs, myTSShape, myDrawer);
}

