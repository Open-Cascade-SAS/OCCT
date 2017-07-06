// Created on: 1998-09-07
// Created by: Denis PASCAL
// Copyright (c) 1998-1999 Matra Datavision
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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <DBRep.hxx>
#include <DBRep_DrawableShape.hxx>
#include <DDataStd.hxx>
#include <DDataStd_DrawDriver.hxx>
#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Axis3D.hxx>
#include <Draw_Drawable3D.hxx>
#include <Draw_Marker3D.hxx>
#include <DrawDim_Angle.hxx>
#include <DrawDim_Distance.hxx>
#include <DrawDim_PlanarAngle.hxx>
#include <DrawDim_PlanarDiameter.hxx>
#include <DrawDim_PlanarDistance.hxx>
#include <DrawDim_PlanarRadius.hxx>
#include <DrawDim_Radius.hxx>
#include <DrawTrSurf_Surface.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Plane.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pln.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDataStd.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDataXtd_Axis.hxx>
#include <TDataXtd_Constraint.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDataXtd_GeometryEnum.hxx>
#include <TDataXtd_Plane.hxx>
#include <TDataXtd_Point.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelList.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_Tool.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DDataStd_DrawDriver,Standard_Transient)

// attribut affichable
// drawable object
static Standard_Integer DISCRET    = 100;
static Standard_Integer NBISOS     = 10;
static Standard_Real    THESIZE       = 1000.;

static Handle(DDataStd_DrawDriver) DrawDriver;


Handle(DDataStd_DrawDriver) DDataStd_DrawDriver::Get () {return DrawDriver; }
void DDataStd_DrawDriver::Set (const Handle(DDataStd_DrawDriver)& DD) {DrawDriver =  DD; }

//=======================================================================
//function : Displayer
//purpose  : 
//=======================================================================

DDataStd_DrawDriver::DDataStd_DrawDriver () {}


//=======================================================================
//function : Geometry
//purpose  : 
//=======================================================================

static TopoDS_Shape Geometry (const Handle(TDataXtd_Constraint)& A, 
			      const Standard_Integer i, TopAbs_ShapeEnum T) 
{
  TopoDS_Shape S = TNaming_Tool::GetShape(A->GetGeometry(i));
  if (!S.IsNull()) {
    if (T != TopAbs_SHAPE && T != S.ShapeType())
      S.Nullify();
  }
  return S;
}

//=======================================================================
//function : Drawable
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D) DDataStd_DrawDriver::Drawable (const TDF_Label& L) const
{ 
  // CONSTRAINT

  Handle(TDataXtd_Constraint) CTR;
  if (L.FindAttribute(TDataXtd_Constraint::GetID(),CTR)) {
    return DrawableConstraint(CTR);
  }
  
  // OBJECT
  
  TopoDS_Shape s;

  //Handle(TDataStd_Object) OBJ;  
  //if (L.FindAttribute(TDataStd_Object::GetID(),OBJ)) { 
  //  return DrawableShape (L,Draw_vert);
  //}  

  // DATUM


  Handle(TDataXtd_Point) POINT;
  if (L.FindAttribute(TDataXtd_Point::GetID(),POINT)) {  
    return DrawableShape (L,Draw_magenta,Standard_False);
  }
  
  Handle(TDataXtd_Axis) AXIS;
  if (L.FindAttribute(TDataXtd_Axis::GetID(),AXIS)) {  
    return DrawableShape (L,Draw_magenta,Standard_False);
  }

  Handle(TDataXtd_Plane) PLANE;
  if (L.FindAttribute(TDataXtd_Plane::GetID(),PLANE)) {  
    return DrawableShape(L,Draw_magenta,Standard_False);
  }

  // Standard GEOMETRY
  

  Handle(TDataXtd_Geometry) STD_GEOM;
  if (L.FindAttribute(TDataXtd_Geometry::GetID(),STD_GEOM)) {  
    switch (STD_GEOM->GetType()) {
    case TDataXtd_POINT :
      {  
	return DrawableShape(L,Draw_jaune,Standard_False);
      }
    case TDataXtd_LINE :
    case TDataXtd_CIRCLE :
    case TDataXtd_ELLIPSE : 
    case TDataXtd_SPLINE :
      {  
	return DrawableShape(L,Draw_cyan,Standard_False);
      }  
    case TDataXtd_ANY_GEOM :
      { 
	break;
      }
      default :
	{
	  break;
	}
    }
  }

  // PURE SHAPE

  Handle(TNaming_NamedShape) NS;
  if (L.FindAttribute(TNaming_NamedShape::GetID(),NS)) {   
    return DrawableShape (NS->Label(),Draw_jaune);
  }

  Handle(Draw_Drawable3D) D3D;
  return D3D;
}




//=======================================================================
//function : DrawableConstraint
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D) DDataStd_DrawDriver::DrawableConstraint (const Handle(TDataXtd_Constraint)& A) const
{
  Handle(DrawDim_Dimension) D;
  
  switch (A->GetType()) {
    
  case TDataXtd_RADIUS:
    {
      if (A->IsPlanar()) {
        D = new DrawDim_PlanarRadius (TNaming_Tool::GetShape(A->GetGeometry(1)));
      }
      else {
        TopoDS_Shape aLocalShape = Geometry(A,1,TopAbs_FACE);
        TopoDS_Face F1 = TopoDS::Face(aLocalShape);
        // TopoDS_Face F1 = TopoDS::Face(Geometry(A,1,TopAbs_FACE));
        if (!F1.IsNull()) D = new DrawDim_Radius(F1);
      }
    }
    break;

  case TDataXtd_DIAMETER:
    if (A->IsPlanar()) {
      D = new DrawDim_PlanarDiameter (TNaming_Tool::GetShape(A->GetGeometry(1)));
    }
    break;

  case TDataXtd_MINOR_RADIUS:
    break;

  case TDataXtd_MAJOR_RADIUS:
    break;

  case TDataXtd_TANGENT:
    break;

  case TDataXtd_PARALLEL:
    break;

  case TDataXtd_PERPENDICULAR:
    break;

  case TDataXtd_CONCENTRIC:
    break;

  case TDataXtd_COINCIDENT:
    break;

  case TDataXtd_DISTANCE:
    {
      if (A->IsPlanar()) {
        D = new DrawDim_PlanarDistance (TNaming_Tool::GetShape(A->GetGeometry(1)),
                                        TNaming_Tool::GetShape(A->GetGeometry(2)));
      }
      break;
    }
  case TDataXtd_ANGLE:
    {     
      if (A->IsPlanar()) {
	Handle(DrawDim_PlanarAngle) DAng = new 
	  DrawDim_PlanarAngle (TNaming_Tool::GetShape(A->GetGeometry(1)),
			       TNaming_Tool::GetShape(A->GetGeometry(2)));
	DAng->Sector(A->Reversed(),A->Inverted());
	TopoDS_Shape aLocalShape = TNaming_Tool::GetShape(A->GetPlane());
	DAng->SetPlane(TopoDS::Face(aLocalShape));
//	DAng->SetPlane(TopoDS::Face(TNaming_Tool::GetShape(A->GetPlane())));
	D = DAng;
      }
      else {
	TopoDS_Shape aLocalShape = Geometry(A,1,TopAbs_FACE);
	TopoDS_Face F1 = TopoDS::Face(aLocalShape);
	aLocalShape = Geometry(A,2,TopAbs_FACE);
	TopoDS_Face F2 = TopoDS::Face(aLocalShape);
//	TopoDS_Face F1 = TopoDS::Face(Geometry(A,1,TopAbs_FACE));
//	TopoDS_Face F2 = TopoDS::Face(Geometry(A,2,TopAbs_FACE));
	if (!F1.IsNull() && !F2.IsNull()) D = new DrawDim_Angle(F1,F2);
      }
    }
    break;

  case TDataXtd_EQUAL_RADIUS:
    {

    }
    
    break;

  case TDataXtd_SYMMETRY:
    break;

  case TDataXtd_MIDPOINT:
    break;

  case TDataXtd_EQUAL_DISTANCE:
    break;

  case TDataXtd_FIX:
    break;

  case TDataXtd_RIGID:
    break;

  case TDataXtd_FROM:
    break;

  case TDataXtd_AXIS:
    break;

  case TDataXtd_MATE:
    {
      TopoDS_Shape aLocalShape = Geometry(A,1,TopAbs_FACE);
      TopoDS_Face F1 = TopoDS::Face(aLocalShape);
      aLocalShape = Geometry(A,2,TopAbs_FACE);
      TopoDS_Face F2 = TopoDS::Face(aLocalShape);
//      TopoDS_Face F1 = TopoDS::Face(Geometry(A,1,TopAbs_FACE));
//      TopoDS_Face F2 = TopoDS::Face(Geometry(A,2,TopAbs_FACE));
      if (!F1.IsNull() && !F2.IsNull())
	D = new DrawDim_Distance(F1,F2);
    }
    break;

  case TDataXtd_ALIGN_FACES:
    {
      TopoDS_Shape aLocalShape = Geometry(A,1,TopAbs_FACE) ;
      TopoDS_Face F1 = TopoDS::Face(aLocalShape);
      aLocalShape = Geometry(A,2,TopAbs_FACE) ;
      TopoDS_Face F2 = TopoDS::Face(aLocalShape);
//      TopoDS_Face F1 = TopoDS::Face(Geometry(A,1,TopAbs_FACE));
//      TopoDS_Face F2 = TopoDS::Face(Geometry(A,2,TopAbs_FACE));
      if (!F1.IsNull() && !F2.IsNull())
	D = new DrawDim_Distance(F1,F2);
    }
    break;

  case TDataXtd_ALIGN_AXES:
    break;

  case TDataXtd_AXES_ANGLE:
    break;

  case TDataXtd_FACES_ANGLE:
    break;

  case TDataXtd_ROUND:
    break;

  case TDataXtd_OFFSET          :
    break;

  }

  if (!D.IsNull()) {
    if (!A->GetValue().IsNull()) {
      Standard_Real val = A->GetValue()->Get();
      Standard_DISABLE_DEPRECATION_WARNINGS
      if (A->GetValue()->GetDimension() == TDataStd_ANGULAR) val = (180. * val) / M_PI;
      Standard_ENABLE_DEPRECATION_WARNINGS
      D->SetValue(val);
    }
    // unverified constraints are red (default is white)
    if (!A->Verified()) D->TextColor(Draw_rouge);  
  }
  return D;
}

//=======================================================================
//function : DrawableShape
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D) DDataStd_DrawDriver::DrawableShape (const TDF_Label& L,
							      const Draw_ColorKind color,
							      const Standard_Boolean current) const
{  
  Handle(Draw_Drawable3D) DS;
  Handle(TNaming_NamedShape) NS;
  if (L.FindAttribute(TNaming_NamedShape::GetID(),NS)) {
    TopoDS_Shape S;
    if (current) S = TNaming_Tool::CurrentShape (NS);
    else S =  TNaming_Tool::GetShape (NS);
    DS = DrawableShape (S,color);
  }
  return DS;
}

//=======================================================================
//function : DrawableShape
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D) DDataStd_DrawDriver::DrawableShape (const TopoDS_Shape& s,
								 const Draw_ColorKind color)
{  
  Handle(DBRep_DrawableShape) DS;
  DS = new DBRep_DrawableShape (s,color,color,color,Draw_bleu,THESIZE,NBISOS,DISCRET);
  return DS;
}

