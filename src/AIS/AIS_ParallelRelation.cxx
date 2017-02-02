// Created on: 1996-12-05
// Created by: Jean-Pierre COMBE/Odile Olivier
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
#include <AIS_ParallelRelation.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <DsgPrs_LengthPresentation.hxx>
#include <ElCLib.hxx>
#include <gce_MakeLin.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Transformation.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Projector.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_ParallelRelation,AIS_Relation)

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
AIS_ParallelRelation::AIS_ParallelRelation(const TopoDS_Shape& aFShape, 
					   const TopoDS_Shape& aSShape, 
					   const Handle(Geom_Plane)& aPlane)
{
  myFShape = aFShape;
  mySShape = aSShape;
  myPlane = aPlane;
  myAutomaticPosition = Standard_True;
  myArrowSize = 0.01;
  mySymbolPrs = DsgPrs_AS_BOTHAR;
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
AIS_ParallelRelation::AIS_ParallelRelation(const TopoDS_Shape& aFShape, 
					   const TopoDS_Shape& aSShape, 
					   const Handle(Geom_Plane)& aPlane, 
					   const gp_Pnt& aPosition,
					   const DsgPrs_ArrowSide aSymbolPrs, 
					   const Standard_Real anArrowSize)
{
  myFShape = aFShape;
  mySShape = aSShape;
  myPlane = aPlane;
  myAutomaticPosition = Standard_False;
  SetArrowSize( anArrowSize );
  myPosition = aPosition;
  mySymbolPrs = aSymbolPrs;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void AIS_ParallelRelation::Compute(const Handle(PrsMgr_PresentationManager3d)&, 
				   const Handle(Prs3d_Presentation)& aPresentation, 
				   const Standard_Integer)
{
  switch (myFShape.ShapeType())
    {
    case TopAbs_FACE :
      {
	// cas longueur entre deux faces
	ComputeTwoFacesParallel(aPresentation);
      }
      break;
    case TopAbs_EDGE :
      {
	// cas longueur entre deux edges
	ComputeTwoEdgesParallel(aPresentation);
      }
      break;
    default:
      break;
    }
}

//=======================================================================
//function : Compute
//purpose  : to avoid warning
//=======================================================================
void AIS_ParallelRelation::Compute(const Handle(Prs3d_Projector)& aProjector,
				   const Handle(Prs3d_Presentation)& aPresentation)
{
// throw Standard_NotImplemented("AIS_ParallelRelation::Compute(const Handle(Prs3d_Projector)&,const Handle(Prs3d_Presentation)&)");
  PrsMgr_PresentableObject::Compute( aProjector , aPresentation ) ;
}

void AIS_ParallelRelation::Compute(const Handle(Prs3d_Projector)& aProjector, const Handle(Geom_Transformation)& aTransformation, const Handle(Prs3d_Presentation)& aPresentation)
{
// throw Standard_NotImplemented("AIS_ParallelRelation::Compute(const Handle(Prs3d_Projector)&, const Handle(Geom_Transformation)&, const Handle(Prs3d_Presentation)&)");
  PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation ) ;
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================
void AIS_ParallelRelation::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection, 
					    const Standard_Integer)
{
  gp_Lin L1 (myFAttach,myDirAttach);
  gp_Lin L2 (mySAttach,myDirAttach);
  gp_Pnt Proj1 = ElCLib::Value(ElCLib::Parameter(L1,myPosition),L1);
  gp_Pnt Proj2 = ElCLib::Value(ElCLib::Parameter(L2,myPosition),L2);
  
  gp_Lin L3;
  Handle(SelectMgr_EntityOwner) own = new SelectMgr_EntityOwner(this,7);

  if (!Proj1.IsEqual(Proj2,Precision::Confusion()))
    {
      L3 = gce_MakeLin(Proj1,Proj2);
    }
  else
    {
      L3 = gce_MakeLin(Proj1,myDirAttach);
      Standard_Real size(Min(myVal/100.+1.e-6,myArrowSize+1.e-6));
      Handle(Select3D_SensitiveBox) box =
	new Select3D_SensitiveBox(own,
				  myPosition.X(),
				  myPosition.Y(),
				  myPosition.Z(),
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
  
  if (!PointMin.IsEqual(PointMax,Precision::Confusion()))
    {
      seg = new Select3D_SensitiveSegment(own,
					  PointMin,
					  PointMax);
      aSelection->Add(seg);
    }
  if (!myFAttach.IsEqual(Proj1,Precision::Confusion()))
    {
      seg = new Select3D_SensitiveSegment(own, myFAttach, Proj1);
      aSelection->Add(seg);
    }
  if (!mySAttach.IsEqual(Proj2,Precision::Confusion()))
    {
      seg = new Select3D_SensitiveSegment(own, mySAttach, Proj2);
      aSelection->Add(seg);
    }
}

//=======================================================================
//function : ComputeTwoFacesParallel
//purpose  : 
//=======================================================================
void AIS_ParallelRelation::ComputeTwoFacesParallel(const Handle(Prs3d_Presentation)&)
{
  throw Standard_NotImplemented("AIS_ParallelRelation::ComputeTwoFacesParallel not implemented");
}

//=======================================================================
//function : ComputeTwoEdgesParallel
//purpose  : 
//=======================================================================
void AIS_ParallelRelation::ComputeTwoEdgesParallel(const Handle(Prs3d_Presentation)& aPresentation)
{
  TopoDS_Edge E1 = TopoDS::Edge(myFShape);
  TopoDS_Edge E2 = TopoDS::Edge(mySShape);

  gp_Pnt ptat11,ptat12,ptat21,ptat22;//,pint3d;
  Handle(Geom_Curve) geom1,geom2;
  Standard_Boolean isInfinite1,isInfinite2;
  Handle(Geom_Curve) extCurv;
  if (!AIS::ComputeGeometry(E1,E2,myExtShape,
			    geom1,geom2,
			    ptat11,ptat12,ptat21,ptat22,
			    extCurv,
			    isInfinite1,isInfinite2,
			    myPlane))
    {
      return;
    }

  aPresentation->SetInfiniteState((isInfinite1 || isInfinite2) && (myExtShape != 0));

  gp_Lin l1;
  gp_Lin l2;
  Standard_Boolean isEl1 = Standard_False, isEl2 = Standard_False;

  if (geom1->IsInstance(STANDARD_TYPE(Geom_Ellipse)))
    {
      Handle(Geom_Ellipse) geom_el1 (Handle(Geom_Ellipse)::DownCast (geom1));
      // construct lines through focuses
      gp_Ax1 elAx = geom_el1->XAxis();
      l1 = gp_Lin(elAx);
      Standard_Real focex = geom_el1->MajorRadius() - geom_el1->Focal()/2.0;
      gp_Vec transvec = gp_Vec(elAx.Direction())*focex;
      ptat11 = geom_el1->Focus1().Translated(transvec);
      ptat12 = geom_el1->Focus2().Translated(-transvec);
      isEl1 = Standard_True;
    }
  else if (geom1->IsInstance(STANDARD_TYPE(Geom_Line)))
    {
      Handle(Geom_Line) geom_lin1 (Handle(Geom_Line)::DownCast (geom1));
      l1 = geom_lin1->Lin();
    }
  else return;

  if (geom2->IsInstance(STANDARD_TYPE(Geom_Ellipse)))
    {
      Handle(Geom_Ellipse) geom_el2 (Handle(Geom_Ellipse)::DownCast (geom2));
      // construct lines through focuses
      gp_Ax1 elAx = geom_el2->XAxis();
      l2 = gp_Lin(elAx);
      Standard_Real focex = geom_el2->MajorRadius() - geom_el2->Focal()/2.0;
      gp_Vec transvec = gp_Vec(elAx.Direction())*focex;
      ptat21 = geom_el2->Focus1().Translated(transvec);
      ptat22 = geom_el2->Focus2().Translated(-transvec);
      isEl2 = Standard_True;
    }
  else if (geom2->IsInstance(STANDARD_TYPE(Geom_Line)))
    {
      Handle(Geom_Line) geom_lin2 (Handle(Geom_Line)::DownCast (geom2));
      l2 = geom_lin2->Lin();
    }
  else return;

  const Handle(Geom_Line)& geom_lin1 = new Geom_Line(l1);
  const Handle(Geom_Line)& geom_lin2 = new Geom_Line(l2);

  myDirAttach = l1.Direction();
  // size
  if( !myArrowSizeIsDefined ) {
    Standard_Real arrSize1 (myArrowSize), arrSize2 (myArrowSize);
    if (!isInfinite1) arrSize1 = ptat11.Distance(ptat12)/50.;
    if (!isInfinite2) arrSize2 = ptat21.Distance(ptat22)/50.;
    myArrowSize = Max(myArrowSize,Max(arrSize1,arrSize2));
//  myArrowSize = Min(myArrowSize,Min(arrSize1,arrSize2));
  }

  if ( myAutomaticPosition )
    {    
      gp_Pnt curpos;
      if ( !isInfinite1 )
	{
	  gp_Pnt p2 = ElCLib::Value(ElCLib::Parameter(l2,ptat11),l2);
	  curpos.SetXYZ((ptat11.XYZ() + p2.XYZ())/2.);
	}
      else if ( !isInfinite2 )
	{
	  gp_Pnt p2 = ElCLib::Value(ElCLib::Parameter(l1,ptat21),l1);
	  curpos.SetXYZ((ptat21.XYZ()+p2.XYZ())/2.);
	}
      else
	{
	  curpos.SetXYZ((l1.Location().XYZ()+l2.Location().XYZ())/2.);
	}
      // offset pour eviter confusion Edge et Dimension
      gp_Vec offset (myDirAttach);
      offset = offset*myArrowSize*(-10.);
      curpos.Translate(offset);
      myPosition = curpos;
    }

  // recherche points attache
  if (!isInfinite1)
    {
      if ( isEl1 )
	{
	  if (myPosition.Distance(ptat11) < myPosition.Distance(ptat12)) myFAttach = ptat12;
	  else myFAttach = ptat11;
	}
      else
	{
	  if (myPosition.Distance(ptat11) > myPosition.Distance(ptat12)) myFAttach = ptat12;
	  else myFAttach = ptat11;
	}
    }
  else
    {
      myFAttach = ElCLib::Value(ElCLib::Parameter(l1,myPosition),l1);
    }
  
  if (!isInfinite2)
    {
      if ( isEl2 )
	{
	  if (myPosition.Distance(ptat21) < myPosition.Distance(ptat22)) mySAttach = ptat22;
	  else mySAttach = ptat21;
	}
      else
	{
	  if (myPosition.Distance(ptat21) > myPosition.Distance(ptat22)) mySAttach = ptat22;
	  else mySAttach = ptat21;
	}
    }
  else
    {
      mySAttach = ElCLib::Value(ElCLib::Parameter(l2,myPosition),l2);
    }
  TCollection_ExtendedString aText (" //");
  
  if (l1.Distance(l2) <= Precision::Confusion())
    {
      myArrowSize = 0.;
    }
  Handle(Prs3d_DimensionAspect) la = myDrawer->DimensionAspect();
  Handle(Prs3d_ArrowAspect) arr = la->ArrowAspect();
  arr->SetLength(myArrowSize);
  arr = la->ArrowAspect();
  arr->SetLength(myArrowSize);
  if ( myExtShape == 1)
    mySymbolPrs = DsgPrs_AS_FIRSTPT_LASTAR;
  else if ( myExtShape == 2)
    mySymbolPrs = DsgPrs_AS_FIRSTAR_LASTPT;

  DsgPrs_LengthPresentation::Add(aPresentation,
				 myDrawer,
				 aText,
				 myFAttach,
				 mySAttach,
				 myDirAttach,
				 myPosition,
				 mySymbolPrs);
  if ( (myExtShape != 0) &&  !extCurv.IsNull())
    {
      gp_Pnt pf, pl;
      if ( myExtShape == 1 )
	{
	  if (!isInfinite1)
	    {
	      pf = ptat11; 
	      pl = ptat12;
	    }
	  ComputeProjEdgePresentation(aPresentation,E1,geom_lin1,pf,pl);
	}
      else
	{
	  if (!isInfinite2)
	    {
	      pf = ptat21; 
	      pl = ptat22;
	    }
	  ComputeProjEdgePresentation(aPresentation,E2,geom_lin2,pf,pl);
	}
    }
}
