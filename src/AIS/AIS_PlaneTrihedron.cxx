// Created on: 1996-12-13
// Created by: Jean-Pierre COMBE
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

// + X/YAxis() returns AIS_Line instead of AIS_Axis
// + (-1) selection mode token into account 
// (SAMTECH specific)

#include <AIS_InteractiveObject.hxx>
#include <AIS_Line.hxx>
#include <AIS_PlaneTrihedron.hxx>
#include <AIS_Point.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <DsgPrs_XYZAxisPresentation.hxx>
#include <Geom_Axis1Placement.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Transformation.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Graphic3d_Structure.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Projector.hxx>
#include <Quantity_Color.hxx>
#include <Select3D_SensitivePoint.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_Type.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TCollection_AsciiString.hxx>
#include <UnitsAPI.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_PlaneTrihedron,AIS_InteractiveObject)

void  ExtremityPoints(TColgp_Array1OfPnt& PP,const Handle(Geom_Plane)& myPlane,const Handle(Prs3d_Drawer)& myDrawer);

//=======================================================================
//function : AIS_PlaneTrihedron
//purpose  : 
//=======================================================================
AIS_PlaneTrihedron::AIS_PlaneTrihedron(const Handle(Geom_Plane)& aPlane)
:myPlane(aPlane)
{
  Handle (Prs3d_DatumAspect) DA = new Prs3d_DatumAspect();
//POP  Standard_Real aLength = UnitsAPI::CurrentFromLS (100. ,"LENGTH");
  Standard_Real aLength = UnitsAPI::AnyToLS (100. ,"mm");
  DA->SetAxisLength(aLength,aLength,aLength);
  Quantity_Color col (Quantity_NOC_ROYALBLUE1);
  DA->LineAspect(Prs3d_DP_XAxis)->SetColor(col);
  DA->LineAspect(Prs3d_DP_YAxis)->SetColor(col);
  DA->SetDrawDatumAxes(Prs3d_DA_XYAxis);
  myDrawer->SetDatumAspect(DA); // odl - specific is created because it is modified
  myShapes[0] = Position();
  myShapes[1] = XAxis();
  myShapes[2] = YAxis();

  myXLabel = TCollection_AsciiString( "X" );
  myYLabel = TCollection_AsciiString( "Y" );
}

//=======================================================================
//function : Component
//purpose  : 
//=======================================================================

 Handle(Geom_Plane) AIS_PlaneTrihedron::Component()
{
  return myPlane;
}


//=======================================================================
//function : SetComponent
//purpose  : 
//=======================================================================

 void AIS_PlaneTrihedron::SetComponent(const Handle(Geom_Plane)& aPlane)
{
  myPlane = aPlane;
}

//=======================================================================
//function : XAxis
//purpose  : 
//=======================================================================
Handle(AIS_Line) AIS_PlaneTrihedron::XAxis() const 
{
  Handle(Geom_Line) aGLine = new Geom_Line(myPlane->Pln().XAxis());
  Handle(AIS_Line) aLine = new AIS_Line (aGLine);
  aLine->SetColor(Quantity_NOC_ROYALBLUE1);
  return aLine;
}

//=======================================================================
//function : YAxis
//purpose  : 
//=======================================================================
Handle(AIS_Line) AIS_PlaneTrihedron::YAxis() const 
{
  Handle(Geom_Line) aGLine = new Geom_Line(myPlane->Pln().YAxis());
  Handle(AIS_Line) aLine = new AIS_Line (aGLine);
  aLine->SetColor(Quantity_NOC_ROYALBLUE1);
  return aLine;
}

//=======================================================================
//function : Position
//purpose  : 
//=======================================================================
Handle(AIS_Point) AIS_PlaneTrihedron::Position() const 
{
  gp_Pnt aPnt = myPlane->Pln().Location();
  Handle(Geom_Point) aPoint = new Geom_CartesianPoint(aPnt);
  Handle(AIS_Point) aPt = new AIS_Point (aPoint);
  return aPt;
}

void AIS_PlaneTrihedron::SetLength(const Standard_Real theLength) {
  myDrawer->DatumAspect()->SetAxisLength(theLength, theLength, theLength);
  SetToUpdate();
}

Standard_Real AIS_PlaneTrihedron::GetLength() const {
  return myDrawer->DatumAspect()->AxisLength(Prs3d_DP_XAxis);
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void AIS_PlaneTrihedron::Compute(const Handle(PrsMgr_PresentationManager3d)&,
				    const Handle(Prs3d_Presentation)& aPresentation, 
				    const Standard_Integer)
{
  aPresentation->SetDisplayPriority(5);
  // drawing axis in X direction
  gp_Pnt first, last;
  Standard_Real value = myDrawer->DatumAspect()->AxisLength(Prs3d_DP_XAxis);
  gp_Dir xDir = myPlane->Position().Ax2().XDirection();

  gp_Pnt orig = myPlane->Position().Ax2().Location();
  Standard_Real xo,yo,zo,x,y,z;
  orig.Coord( xo, yo, zo );
  xDir.Coord( x, y, z );
  first.SetCoord( xo, yo, zo );
  last.SetCoord( xo + x * value, yo + y * value, zo + z * value );
  
  DsgPrs_XYZAxisPresentation::Add( aPresentation, myDrawer->DatumAspect()->LineAspect(Prs3d_DP_XAxis), myDrawer->ArrowAspect(), myDrawer->TextAspect(), xDir, value, myXLabel.ToCString(), first, last );
  
  // drawing axis in Y direction
  value = myDrawer->DatumAspect()->AxisLength(Prs3d_DP_YAxis);
  gp_Dir yDir = myPlane->Position().Ax2().YDirection();

  yDir.Coord( x, y, z );
  last.SetCoord( xo + x * value, yo + y * value, zo + z * value );
  DsgPrs_XYZAxisPresentation::Add( aPresentation, myDrawer->DatumAspect()->LineAspect(Prs3d_DP_XAxis), myDrawer->ArrowAspect(), myDrawer->TextAspect(), yDir, value, myYLabel.ToCString(), first, last );

  aPresentation->SetInfiniteState (Standard_True);
}

void AIS_PlaneTrihedron::Compute(const Handle(Prs3d_Projector)& aProjector, const Handle(Geom_Transformation)& aTransformation, const Handle(Prs3d_Presentation)& aPresentation)
{
// throw Standard_NotImplemented("AIS_PlaneTrihedron::Compute(const Handle(Prs3d_Projector)&, const Handle(Geom_Transformation)&, const Handle(Prs3d_Presentation)&)");
 PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation) ;
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================

void AIS_PlaneTrihedron::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
					     const Standard_Integer aMode)
{
  Standard_Integer Prior;
  Handle(SelectMgr_EntityOwner) eown;
  TColgp_Array1OfPnt PP(1,4),PO(1,4);
//  ExtremityPoints(PP);
  ExtremityPoints(PP,myPlane,myDrawer);
  switch (aMode) {
  case 0:
    {   // triedre complet
      Prior = 5;
//      gp_Ax2 theax = gp_Ax2(myPlane->Position().Ax2());
//      gp_Pnt p1 = theax.Location();
      
      eown = new SelectMgr_EntityOwner(this,Prior);
      for (Standard_Integer i=1; i<=2;i++)
	aSelection->Add(new Select3D_SensitiveSegment(eown,PP(1),PP(i+1)));
      
      break;
    }
  case 1:
    {  //origine
      Prior = 8;
      const Handle(SelectMgr_SelectableObject)& anObj = myShapes[0]; // to avoid ambiguity
      eown= new SelectMgr_EntityOwner(anObj,Prior);
      aSelection->Add(new Select3D_SensitivePoint(eown,myPlane->Location()));

      break;
    }
  case 2:
    { //axes ... priorite 7
      Prior = 7;
      for (Standard_Integer i=1; i<=2;i++){
        const Handle(SelectMgr_SelectableObject)& anObj = myShapes[i]; // to avoid ambiguity
	eown= new SelectMgr_EntityOwner(anObj,Prior);
	aSelection->Add(new Select3D_SensitiveSegment(eown,PP(1),PP(i+1)));

      }
      break;
    }
  case -1:
    {
      Prior = 5;
      aSelection->Clear();
      break;
    }
  }
}

void AIS_PlaneTrihedron::SetColor(const Quantity_Color &aCol)
{
  hasOwnColor=Standard_True;
  myDrawer->SetColor (aCol);
  myDrawer->DatumAspect()->LineAspect(Prs3d_DP_XAxis)->SetColor(aCol);
  myDrawer->DatumAspect()->LineAspect(Prs3d_DP_YAxis)->SetColor(aCol);
}


void AIS_PlaneTrihedron::Compute(const Handle(Prs3d_Projector)&, 
			       const Handle(Prs3d_Presentation)&)
{
}


//=======================================================================
//function : ExtremityPoints
//purpose  : to avoid warning
//=======================================================================
//void  AIS_Trihedron::ExtremityPoints(TColgp_Array1OfPnt& PP) const 
void  ExtremityPoints(TColgp_Array1OfPnt& PP,const Handle(Geom_Plane)& myPlane,const Handle(Prs3d_Drawer)& myDrawer )
{
//  gp_Ax2 theax(myPlane->Ax2());
  gp_Ax2 theax(myPlane->Position().Ax2());
  PP(1) = theax.Location();

  Standard_Real len = myDrawer->DatumAspect()->AxisLength(Prs3d_DP_XAxis);
  gp_Vec vec = theax.XDirection();
  vec *= len;
  PP(2) = PP(1).Translated(vec);
  
  len = myDrawer->DatumAspect()->AxisLength(Prs3d_DP_YAxis);
  vec = theax.YDirection();
  vec *= len;
  PP(3) = PP(1).Translated(vec);

}

//=======================================================================
//function : AcceptDisplayMode
//purpose  : 
//=======================================================================
Standard_Boolean  AIS_PlaneTrihedron::AcceptDisplayMode(const Standard_Integer aMode) const
{return aMode == 0;}

