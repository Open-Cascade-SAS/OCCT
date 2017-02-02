// Created on: 1998-01-22
// Created by: Sergey ZARITCHNY
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


#include <AIS.hxx>
#include <AIS_EllipseRadiusDimension.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_EllipseRadiusDimension,AIS_Relation)

//=======================================================================
//function : AIS_EllipseRadiusDimension
//purpose  : 
//=======================================================================
AIS_EllipseRadiusDimension::AIS_EllipseRadiusDimension(const TopoDS_Shape& aShape, 
						       const TCollection_ExtendedString& aText)
:AIS_Relation()
{
  myFShape = aShape;
  myText = aText;
//  ComputeGeometry( );
}

//=======================================================================
//function : ComputeGeometry
//purpose  : 
//=======================================================================

void AIS_EllipseRadiusDimension::ComputeGeometry()
{

 switch (myFShape.ShapeType()) {
  case TopAbs_FACE :
    {
      // compute one face case
      ComputeFaceGeometry ();
      break;
    }
  case TopAbs_EDGE:
    {
      ComputeEdgeGeometry ();
      break;
    }
  default:
    break;
  }
 while (myFirstPar > 2*M_PI) myFirstPar -= 2*M_PI;
 while (myLastPar > 2*M_PI)  myLastPar  -= 2*M_PI;
 while (myFirstPar < 0.0)  myFirstPar += 2*M_PI;
 while (myLastPar  < 0.0)  myLastPar  += 2*M_PI;
}

//=======================================================================
//function : ComputeFaceGeometry
//purpose  : 
//=======================================================================

void AIS_EllipseRadiusDimension::ComputeFaceGeometry()
{

  gp_Pln aPln;
  Handle( Geom_Surface ) aBasisSurf;
  AIS_KindOfSurface aSurfType;
  Standard_Real Offset;
  AIS::GetPlaneFromFace( TopoDS::Face(  myFShape),
					aPln,
					aBasisSurf,
				        aSurfType,
					Offset ) ;

  if ( aSurfType == AIS_KOS_Plane )
    ComputePlanarFaceGeometry( );
  else 
    ComputeCylFaceGeometry( aSurfType, aBasisSurf, Offset );

}

//=======================================================================
//function : ComputeCylFaceGeometry
//purpose  : defines Ellipse and plane of dimension
//=======================================================================

void AIS_EllipseRadiusDimension::ComputeCylFaceGeometry(const AIS_KindOfSurface  aSurfType,
							const Handle( Geom_Surface )&  aBasisSurf,
							const Standard_Real Offset)
{

  BRepAdaptor_Surface surf1(TopoDS::Face(myFShape));
  Standard_Real vFirst, vLast;
  vFirst = surf1.FirstVParameter();
  vLast  = surf1.LastVParameter();
  Standard_Real vMid = (vFirst + vLast)*0.5;
  gp_Pln aPlane;
  gp_Ax1 Axis;
//  Standard_Real Param;
  if (aSurfType == AIS_KOS_Extrusion)
    {
      Axis.SetDirection((Handle( Geom_SurfaceOfLinearExtrusion )::DownCast( aBasisSurf ))
			->Direction() );
      Axis.SetLocation( gp_Pnt((Handle( Geom_SurfaceOfLinearExtrusion )::DownCast( aBasisSurf ))
			       ->Direction().XYZ() ) );
      
      aPlane.SetAxis(Axis);
      aPlane.SetLocation(myEllipse.Location());
      myPlane = new Geom_Plane(aPlane);
      
      Handle(Geom_Curve) aCurve;
      aCurve =   aBasisSurf->VIso(vMid);
      if (aCurve->DynamicType() == STANDARD_TYPE(Geom_Ellipse)) 
	{
	  myEllipse = Handle(Geom_Ellipse)::DownCast(aCurve)-> Elips();//gp_Elips
	  myIsAnArc = Standard_False;
	}
      else if (aCurve->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve)) {
	Handle(Geom_TrimmedCurve) tCurve = Handle(Geom_TrimmedCurve)::DownCast(aCurve); 
	aCurve = tCurve->BasisCurve();
	myFirstPar = tCurve->FirstParameter();
	myLastPar  = tCurve->LastParameter();
	myIsAnArc = Standard_True;
	if (aCurve->DynamicType() == STANDARD_TYPE(Geom_Ellipse)) 
	  {
	    myEllipse = Handle(Geom_Ellipse)::DownCast(aCurve)->Elips();//gp_Elips
	  }
      }
      else 
	{
	  throw Standard_ConstructionError("AIS:: Not expected type of surface") ;
	    return;
	  }
      
// Offset  

      if(surf1.GetType() ==  GeomAbs_OffsetSurface)
	{
	  if(Offset <0.0 && Abs(Offset) > myEllipse.MinorRadius ())
	    {
	      throw Standard_ConstructionError("AIS:: Absolute value of negative offset is larger than MinorRadius");
		return;
	      }
	  
	  myOffsetCurve = new Geom_OffsetCurve(new Geom_Ellipse(myEllipse), Offset, 
					       myPlane->Pln().Axis().Direction());
	  myOffset = Offset;
	  myIsOffset = Standard_True;
	  gp_Elips elips = myEllipse;
	  Standard_Real Val = Offset + elips.MajorRadius ();//simulation
	  myEllipse.SetMajorRadius (Val);
	  Val = Offset + elips.MinorRadius ();
	  myEllipse.SetMinorRadius (Val);
	}
      else 
	myIsOffset = Standard_False;
    }
}


//=======================================================================
//function : ComputePlanarFaceGeometry
//purpose  : 
//=======================================================================

void AIS_EllipseRadiusDimension::ComputePlanarFaceGeometry()
{

  Standard_Boolean find = Standard_False;
  gp_Pnt ptfirst,ptend;
  TopExp_Explorer ExploEd( TopoDS::Face(myFShape), TopAbs_EDGE );
  for ( ; ExploEd.More(); ExploEd.Next())
    {
      TopoDS_Edge curedge =  TopoDS::Edge( ExploEd.Current() );
      Handle(Geom_Curve) curv;
      Handle(Geom_Ellipse) ellips;
      if (AIS::ComputeGeometry(curedge,curv,ptfirst,ptend)) 
	{ 
	  if (curv->DynamicType() == STANDARD_TYPE(Geom_Ellipse))
	    {
	      ellips = Handle(Geom_Ellipse)::DownCast(curv);
	      if ( !ellips.IsNull() ) {
		myEllipse = ellips->Elips();
		find = Standard_True;
		break;
	      }
	    }
	}
    }
  if( !find )
    {
      throw Standard_ConstructionError("AIS:: Curve is not an ellipsee or is Null") ;
	return;
      }
  
  if ( !ptfirst.IsEqual(ptend, Precision::Confusion()) )
    {
      myIsAnArc = Standard_True;
      myFirstPar = ElCLib::Parameter(myEllipse, ptfirst);
      myLastPar  = ElCLib::Parameter(myEllipse, ptend); 
    }
  else
    myIsAnArc = Standard_False;

  BRepAdaptor_Surface surfAlgo (TopoDS::Face(myFShape));
  myPlane  = new Geom_Plane( surfAlgo.Plane() );
  
}

 

//=======================================================================
//function : ComputeEdgeGeometry
//purpose  : 
//=======================================================================

void AIS_EllipseRadiusDimension::ComputeEdgeGeometry()
{
  gp_Pnt ptfirst,ptend;
  Handle(Geom_Curve) curv;
  if (!AIS::ComputeGeometry(TopoDS::Edge(myFShape),curv,ptfirst,ptend)) return;
  
  Handle(Geom_Ellipse) elips = Handle(Geom_Ellipse)::DownCast(curv);
  if ( elips.IsNull()) return;
  
  myEllipse =  elips->Elips();
  gp_Pln aPlane;
  aPlane.SetPosition(gp_Ax3(myEllipse.Position()));
  myPlane  = new Geom_Plane(aPlane);
  
  
  if ( ptfirst.IsEqual(ptend, Precision::Confusion()) ) {
    myIsAnArc = Standard_False;
  }
  else {
    myIsAnArc = Standard_True;
    myFirstPar = ElCLib::Parameter(myEllipse, ptfirst);
    myLastPar  = ElCLib::Parameter(myEllipse, ptend); 
  }
}
//=======================================================================
//function : KindOfDimension
//purpose  : 
//=======================================================================
 AIS_KindOfDimension AIS_EllipseRadiusDimension::KindOfDimension() const 
{
  return AIS_KOD_ELLIPSERADIUS;
}

//=======================================================================
//function : IsMovable
//purpose  : 
//=======================================================================
 Standard_Boolean AIS_EllipseRadiusDimension::IsMovable() const 
{
  return Standard_True;
}
