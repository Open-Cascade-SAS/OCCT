// Created on: 1999-09-30
// Created by: Sergey RUIN
// Copyright (c) 1999 Matra Datavision
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


#include <AIS_Circle.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Line.hxx>
#include <AIS_Point.hxx>
#include <AIS_Shape.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Type.hxx>
#include <TDataXtd.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDF_Label.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TPrsStd_GeometryDriver.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TPrsStd_GeometryDriver,TPrsStd_Driver)

//=======================================================================
//function :
//purpose  : 
//=======================================================================
TPrsStd_GeometryDriver::TPrsStd_GeometryDriver()
{
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean TPrsStd_GeometryDriver::Update(const TDF_Label& aLabel,
					       Handle(AIS_InteractiveObject)& anAISObject) 
{
  Handle(TDataXtd_Geometry) aGeom;
  Handle(TNaming_NamedShape) NS;
  TDataXtd_GeometryEnum GeomType;

  if (!aLabel.FindAttribute(TDataXtd_Geometry::GetID(), aGeom)) {
    if(aLabel.FindAttribute(TNaming_NamedShape::GetID(), NS) ) {
      GeomType = TDataXtd_Geometry::Type(aLabel);
    }
    else {
      return Standard_False; 
    }
  }
  else {
    GeomType = aGeom->GetType();
  }
  
  switch (GeomType)  {
  case  TDataXtd_POINT   :
    {
      gp_Pnt pt; 
      if (!TDataXtd_Geometry::Point(aLabel,pt)) return Standard_False;
      Handle(Geom_Point) apt = new Geom_CartesianPoint(pt);
      Handle(AIS_Point) ais1;
      if( anAISObject.IsNull() ) ais1 = new AIS_Point(apt);
      else {	
	ais1 = Handle(AIS_Point)::DownCast(anAISObject);
	if (ais1.IsNull()) 
	  ais1 = new AIS_Point(apt);
	else {
	  ais1->SetComponent(apt);
	  ais1->ResetTransformation();
	  ais1->SetToUpdate();
	  ais1->UpdateSelection();
	}
	
      }
      anAISObject = ais1;
      anAISObject->SetColor(Quantity_NOC_RED);
    }
  break;
  case  TDataXtd_LINE   :
    {
      gp_Lin ln; 
      if (!TDataXtd_Geometry::Line(aLabel,ln)) return Standard_False;
      Handle(Geom_Line) aln = new Geom_Line(ln);
      Handle(AIS_Line) ais2;
      if( anAISObject.IsNull() ) ais2 = new AIS_Line(aln);
      else {
	ais2 = Handle(AIS_Line)::DownCast(anAISObject);
	if (ais2.IsNull()) 
	  ais2 = new AIS_Line(aln);
	else {
	  ais2->SetLine(aln);
	  ais2->ResetTransformation();
	  ais2->SetToUpdate();
	  ais2->UpdateSelection();
	}
      }
      anAISObject = ais2;
      anAISObject->SetColor(Quantity_NOC_RED);
      anAISObject->SetInfiniteState(Standard_True);
      break;
    }
  case  TDataXtd_CIRCLE   :
    {
      Handle(AIS_Line) ais2;
      gp_Circ cir; 
      if (!TDataXtd_Geometry::Circle(aLabel,cir)) return Standard_False; 
      Handle(Geom_Circle) acir = new Geom_Circle(cir);
      Handle(AIS_Circle) ais3;
      if (anAISObject.IsNull()) ais3 = new AIS_Circle(acir);
      else {
	ais3 = Handle(AIS_Circle)::DownCast(anAISObject);
	if (ais3.IsNull()) 
	  ais3 = new AIS_Circle(acir);
	else {
	  ais3->SetCircle(acir);
	  ais3->ResetTransformation();
	  ais3->SetToUpdate();
	  ais3->UpdateSelection();
	}
      }
      anAISObject = ais3;
      anAISObject->SetColor(Quantity_NOC_RED);
      break;
    }
  case  TDataXtd_ELLIPSE   :
    {
      gp_Elips elp; 
      if (!TDataXtd_Geometry::Ellipse(aLabel, elp)) return Standard_False;
      BRepBuilderAPI_MakeEdge mkEdge(elp);
      if( !mkEdge.IsDone() ) return Standard_False;
      Handle(AIS_Shape) ais;
      if (anAISObject.IsNull()) ais = new AIS_Shape(mkEdge);
      else {
          ais = Handle(AIS_Shape)::DownCast(anAISObject);
	  if (ais.IsNull()) 
	    ais = new AIS_Shape(mkEdge);
	  else {
	    ais->ResetTransformation();
	    ais->Set(mkEdge);
	    ais->SetToUpdate();
	    ais->UpdateSelection();
	  }
      }	  
      anAISObject = ais;
      anAISObject->SetColor(Quantity_NOC_RED);
      break;
    }
  default:
    return Standard_False;
  }
  
  return Standard_True;
}

