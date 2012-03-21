// Created on: 1998-01-22
// Created by: Sergey ZARITCHNY
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



#include <AIS_EllipseRadiusDimension.ixx>

#include <TCollection_ExtendedString.hxx>

#include <ElCLib.hxx>
#include <ElSLib.hxx>

#include <TopoDS.hxx>

#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve.hxx>

#include <Geom_Ellipse.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <Geom_Line.hxx>
#include <GeomAPI.hxx>

#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Lin.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <gp_Elips.hxx>

#include <AIS.hxx>

#include <Precision.hxx>

#include <TopExp_Explorer.hxx>

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
	  Standard_ConstructionError::Raise("AIS:: Not expected type of surface") ;
	    return;
	  }
      
// Offset  

      if(surf1.GetType() ==  GeomAbs_OffsetSurface)
	{
	  if(Offset <0.0 && Abs(Offset) > myEllipse.MinorRadius ())
	    {
	      Standard_ConstructionError::Raise("AIS:: Absolute value of negative offset is larger than MinorRadius");
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
      Standard_ConstructionError::Raise("AIS:: Curve is not an ellipsee or is Null") ;
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
