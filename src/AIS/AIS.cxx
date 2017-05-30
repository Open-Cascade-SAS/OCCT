// Created on: 1996-12-11
// Created by: Robert COUBLANC
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


#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <AIS.hxx>
#include <Bnd_Box.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepTools.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <gce_MakeDir.hxx>
#include <gce_MakeLin.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <GeomAPI_IntSS.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomLib.hxx>
#include <GeomProjLib.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <StdPrs_Point.hxx>
#include <StdPrs_WFShape.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

const Standard_Real SquareTolerance = Precision::SquareConfusion();

//=======================================================================
//function : Nearest
//purpose  :
//=======================================================================
gp_Pnt AIS::Nearest(const TopoDS_Shape& ashape, const gp_Pnt& apoint)
{
  Standard_Real dist2 = RealLast();
  Standard_Real curdist2;
  gp_Pnt result(0.0,0.0,0.0);
  gp_Pnt curpnt(0.0,0.0,0.0);
  TopExp_Explorer explo(ashape,TopAbs_VERTEX);
  while (explo.More())
    {
      curpnt = BRep_Tool::Pnt(TopoDS::Vertex(explo.Current()));
      curdist2 = apoint.SquareDistance(curpnt);
      if (curdist2 < dist2)
        {
          result = curpnt;
          dist2 = curdist2;
        }
      explo.Next();
    }
  return result;
}

//=======================================================================
//function : Nearest
//purpose  : For <thePoint> finds the nearest point on <theLine>.
//=======================================================================
gp_Pnt AIS::Nearest (const gp_Lin& theLine, const gp_Pnt& thePoint)
{
  Handle(Geom_Line) aLine = new Geom_Line (theLine);

  GeomAPI_ProjectPointOnCurve aPointProj (thePoint, aLine);
  return aPointProj.Point (1);
}

//=======================================================================
//function : Nearest
//purpose  : For the given point finds nearest point on the curve,
//           return TRUE if found point is belongs to curve
//              and FALSE otherwise.
//=======================================================================
Standard_Boolean AIS::Nearest (const Handle(Geom_Curve)& theCurve,
                               const gp_Pnt& thePoint,
                               const gp_Pnt& theFirstPoint,
                               const gp_Pnt& theLastPoint,
                               gp_Pnt& theNearestPoint)
{
  GeomAPI_ProjectPointOnCurve aPointProj (thePoint, theCurve);
  theNearestPoint = theCurve->Value (aPointProj.LowerDistanceParameter());

  Standard_Real aLength = theFirstPoint.Distance (theLastPoint);

  if (theNearestPoint.Distance (theFirstPoint) > aLength
      || theNearestPoint.Distance (theLastPoint) >aLength)
  {
    return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
//function : Farest
//purpose  :
//=======================================================================
gp_Pnt AIS::Farest( const TopoDS_Shape& aShape, const gp_Pnt& aPoint )
{
  Standard_Real MaxDist2 = 0.0e0, curdist2;
  gp_Pnt Result(0.0,0.0,0.0);
  gp_Pnt curpnt(0.0,0.0,0.0);
  TopExp_Explorer Explo( aShape, TopAbs_VERTEX );
  for (; Explo.More(); Explo.Next())
    {
      curpnt = BRep_Tool::Pnt( TopoDS::Vertex( Explo.Current() ) );
      curdist2 = aPoint.SquareDistance( curpnt );
      if (curdist2 > MaxDist2)
        {
          MaxDist2 = curdist2;
          Result = curpnt;
        }
    }
  return Result;
}


//=======================================================================
//function : ComputeGeometry
//purpose  : for line, circle, ellipse.
//=======================================================================
Standard_Boolean AIS::ComputeGeometry (const TopoDS_Edge&  theEdge,
                                       Handle(Geom_Curve)& theCurve,
                                       gp_Pnt&             theFirstPnt,
                                       gp_Pnt&             theLastPnt)
{
  TopLoc_Location anEdgeLoc;
  Standard_Real aFirst, aLast;
  theCurve = BRep_Tool::Curve (theEdge, anEdgeLoc, aFirst, aLast);
  if (theCurve.IsNull())
  {
    return Standard_False;
  }

  if (!anEdgeLoc.IsIdentity())
  {
    Handle(Geom_Geometry) aGeometry = theCurve->Transformed (anEdgeLoc.Transformation());
    theCurve = Handle(Geom_Curve)::DownCast (aGeometry);
  }

  if (theCurve->IsInstance (STANDARD_TYPE (Geom_TrimmedCurve)))
  {
    theCurve = Handle(Geom_TrimmedCurve)::DownCast (theCurve)->BasisCurve();
  }

  if (theCurve->IsInstance (STANDARD_TYPE (Geom_Line)))
  {
    Handle(Geom_Line) aLine = Handle(Geom_Line)::DownCast (theCurve);
    theFirstPnt = ElCLib::Value (aFirst, aLine->Lin());
    theLastPnt = ElCLib::Value (aLast, aLine->Lin());
  }
  else if (theCurve->IsInstance (STANDARD_TYPE (Geom_Circle)))
  {
    Handle(Geom_Circle) aCirc = Handle(Geom_Circle)::DownCast (theCurve);

    theFirstPnt = ElCLib::Value (aFirst, aCirc->Circ());
    theLastPnt = ElCLib::Value (aLast, aCirc->Circ());
  }
  else if (theCurve->IsInstance (STANDARD_TYPE (Geom_Ellipse)))
  {
    Handle(Geom_Ellipse) anEllipse = Handle(Geom_Ellipse)::DownCast (theCurve);
    theFirstPnt = ElCLib::Value (aFirst, anEllipse->Elips());
    theLastPnt = ElCLib::Value (aLast, anEllipse->Elips());
  }
  else
  {
    return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
//function : ComputeGeometry
//purpose  : for line, circle, ellipse.
//=======================================================================
Standard_Boolean AIS::ComputeGeometry (const TopoDS_Edge& theEdge,
                                       Handle(Geom_Curve)& theCurve,
                                       gp_Pnt& theFirstPnt,
                                       gp_Pnt& theLastPnt,
                                       Standard_Boolean& theIsInfinite)
{
  Standard_Real aFirst, aLast;

  BRepAdaptor_Curve anAdaptor (theEdge);

  theCurve = Handle(Geom_Curve)::DownCast
    (anAdaptor.Curve().Curve()->Transformed (anAdaptor.Trsf()));

  if (theCurve.IsNull())
  {
    return Standard_False;
  }

  aFirst = anAdaptor.FirstParameter();
  aLast = anAdaptor.LastParameter();

  theIsInfinite = (Precision::IsInfinite (aFirst) || Precision::IsInfinite (aLast));

  if (theCurve->IsInstance (STANDARD_TYPE (Geom_TrimmedCurve)))
  {
    theCurve = Handle(Geom_TrimmedCurve)::DownCast (theCurve)->BasisCurve();
  }

  if (!theIsInfinite)
  {
    theFirstPnt = theCurve->Value (aFirst);
    theLastPnt  = theCurve->Value (aLast);
  }
  else
  {
    theFirstPnt = gp::Origin();
    theLastPnt  = gp::Origin();
  }

  return Standard_True;
}

//=======================================================================
//function : ComputeGeometry
//purpose  :
//=======================================================================

Standard_Boolean AIS::ComputeGeometry (const TopoDS_Edge& theEdge,
                                       Handle(Geom_Curve)& theCurve,
                                       gp_Pnt& theFirstPnt,
                                       gp_Pnt& theLastPnt,
                                       Handle(Geom_Curve)& theExtCurve,
                                       Standard_Boolean& theIsInfinite,
                                       Standard_Boolean& theIsOnPlane,
                                       const Handle(Geom_Plane)& thePlane)
{
  if (thePlane.IsNull())
  {
    return Standard_False;
  }

  Standard_Real aFirst, aLast;
  BRepAdaptor_Curve aCurveAdaptor (theEdge);
  theCurve = Handle(Geom_Curve)::DownCast (aCurveAdaptor.Curve().Curve()->Transformed (aCurveAdaptor.Trsf()));
  aFirst = aCurveAdaptor.FirstParameter();
  aLast = aCurveAdaptor.LastParameter();

  if (theCurve.IsNull())
  {
    return Standard_False;
  }
  
  theExtCurve = theCurve;
  theIsInfinite = (Precision::IsInfinite (aFirst) || Precision::IsInfinite (aLast));

  // Checks that the projected curve is not in the plane.
  theIsOnPlane = Standard_True;
  if (theExtCurve->IsInstance (STANDARD_TYPE (Geom_TrimmedCurve)))
  {
    theExtCurve = Handle(Geom_TrimmedCurve)::DownCast (theExtCurve)->BasisCurve();
  }

  if (theExtCurve->IsInstance (STANDARD_TYPE (Geom_Line)))
  {
    Handle(Geom_Line) aLine = Handle(Geom_Line)::DownCast (theExtCurve);
    theIsOnPlane = thePlane->Pln().Contains (aLine->Lin(),
                                             Precision::Confusion(),
                                             Precision::Angular());
  }
  else if (theExtCurve->IsInstance (STANDARD_TYPE (Geom_Circle)))
  {
    Handle(Geom_Circle) aCircle = Handle(Geom_Circle)::DownCast (theExtCurve);

    gp_Ax3 aCircPos (aCircle->Position());
    theIsOnPlane = aCircPos.IsCoplanar (thePlane->Pln().Position(),
                                                Precision::Confusion(),
                                                Precision::Angular());
  }

  if (theIsOnPlane)
  {
    theExtCurve.Nullify();
  }

  theCurve = GeomProjLib::ProjectOnPlane (theCurve, thePlane,
                                          thePlane->Pln().Axis().Direction(),
                                          Standard_False);

  if (theCurve->IsInstance (STANDARD_TYPE (Geom_Line)))
  {
    Handle(Geom_Line) aLine = Handle(Geom_Line)::DownCast (theCurve);
    if (!theIsInfinite)
    {
      theFirstPnt = ElCLib::Value (aFirst, aLine->Lin());
      theLastPnt = ElCLib::Value (aLast, aLine->Lin());
    }
  }
  else if (theCurve->IsInstance (STANDARD_TYPE (Geom_Circle)))
  {
    Handle(Geom_Circle) aCirc = Handle(Geom_Circle)::DownCast (theCurve);

    theFirstPnt = ElCLib::Value (aFirst, aCirc->Circ());
    theLastPnt = ElCLib::Value (aLast, aCirc->Circ());
  }
  else if (theCurve->IsInstance (STANDARD_TYPE (Geom_Ellipse)))
    {
      Handle(Geom_Ellipse) anEllipse = Handle(Geom_Ellipse)::DownCast (theCurve);

      theFirstPnt = ElCLib::Value (aFirst, anEllipse->Elips());
      theLastPnt = ElCLib::Value (aLast, anEllipse->Elips());
    }
  else
  {
    return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
//function : ComputeGeometry
//purpose  :
//=======================================================================
Standard_Boolean AIS::ComputeGeometry (const TopoDS_Edge& theFirstEdge,
                                      const TopoDS_Edge& theSecondEdge,
                                      Handle(Geom_Curve)& theFirstCurve,
                                      Handle(Geom_Curve)& theSecondCurve,
                                      gp_Pnt& theFirstPnt1,
                                      gp_Pnt& theLastPnt1,
                                      gp_Pnt& theFirstPnt2,
                                      gp_Pnt& theLastPnt2,
                                      const Handle(Geom_Plane)& thePlane)
{
  if (thePlane.IsNull())
  {
    return Standard_False;
  }

  TopLoc_Location aFirstEdgeLoc, aSecondEdgeLoc;
  Standard_Real aFirst1, aLast1, aFirst2, aLast2;
  
  theFirstCurve = BRep_Tool::Curve (theFirstEdge, aFirstEdgeLoc, aFirst1, aLast1);
  theSecondCurve = BRep_Tool::Curve (theSecondEdge, aSecondEdgeLoc, aFirst2, aLast2);

  if (theFirstCurve.IsNull())
  {
    return Standard_False;
  }
    
  if (theSecondCurve.IsNull())
  {
    return Standard_False;
  }
  
  if (!aFirstEdgeLoc.IsIdentity())
  {
    Handle(Geom_Geometry) aGeomGeometry = theFirstCurve->Transformed (aFirstEdgeLoc.Transformation());
    theFirstCurve = Handle(Geom_Curve)::DownCast (aGeomGeometry);
  }
    
  if (!aSecondEdgeLoc.IsIdentity())
  {
    Handle(Geom_Geometry) aGeomGeometry = theSecondCurve->Transformed (aSecondEdgeLoc.Transformation());
    theSecondCurve = Handle(Geom_Curve)::DownCast (aGeomGeometry);
  }

  theFirstCurve = GeomProjLib::ProjectOnPlane (theFirstCurve, thePlane,
                                               thePlane->Pln().Axis().Direction(),
                                               Standard_False);


  theSecondCurve = GeomProjLib::ProjectOnPlane (theSecondCurve, thePlane,
                                                thePlane->Pln().Axis().Direction(),
                                                Standard_False);


  if (theFirstCurve->IsInstance (STANDARD_TYPE(Geom_TrimmedCurve)))
  {
    theFirstCurve = Handle(Geom_TrimmedCurve)::DownCast (theFirstCurve)->BasisCurve();
  }
    
  if (theSecondCurve->IsInstance (STANDARD_TYPE (Geom_TrimmedCurve)))
  {
    theSecondCurve = Handle(Geom_TrimmedCurve)::DownCast (theSecondCurve)->BasisCurve();
  }

  if (theFirstCurve->IsInstance(STANDARD_TYPE(Geom_Line)))
  {
    Handle(Geom_Line) aLine = Handle(Geom_Line)::DownCast (theFirstCurve);

    theFirstPnt1 = ElCLib::Value (aFirst1, aLine->Lin());
    theLastPnt1 = ElCLib::Value (aLast1, aLine->Lin());
  }
  else if (theFirstCurve->IsInstance(STANDARD_TYPE(Geom_Circle)))
  {
    Handle(Geom_Circle) aCirc = Handle(Geom_Circle)::DownCast (theFirstCurve);
      
    theFirstPnt1 = ElCLib::Value (aFirst1, aCirc->Circ());
    theLastPnt1 = ElCLib::Value (aLast1, aCirc->Circ());
  }
  else
  {
    return Standard_False;
  }

  if (theSecondCurve->IsInstance (STANDARD_TYPE (Geom_Line)))
{
    Handle(Geom_Line) aLine = Handle(Geom_Line)::DownCast (theSecondCurve);

    theFirstPnt2 = ElCLib::Value (aFirst2, aLine->Lin());
    theLastPnt2 = ElCLib::Value (aLast2, aLine->Lin());
  }
  else if (theSecondCurve->IsInstance (STANDARD_TYPE (Geom_Circle)))
  {
    Handle(Geom_Circle) aCirc = Handle(Geom_Circle)::DownCast (theSecondCurve);

    theFirstPnt2 = ElCLib::Value (aFirst2, aCirc->Circ());
    theLastPnt2 = ElCLib::Value (aLast2, aCirc->Circ());
  }
  else
  {
    return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
//function : ComputeGeometry
//purpose  : Computes the geometry of the 2 edges.
//=======================================================================
Standard_Boolean AIS::ComputeGeometry (const TopoDS_Edge& theFirstEdge,
                                       const TopoDS_Edge& theSecondEdge,
                                       Handle(Geom_Curve)& theFirstCurve,
                                       Handle(Geom_Curve)& theSecondCurve,
                                       gp_Pnt& theFirstPnt1,
                                       gp_Pnt& theLastPnt1,
                                       gp_Pnt& theFirstPnt2,
                                       gp_Pnt& theLastPnt2,
                                       Standard_Boolean& theIsInfinite1,
                                       Standard_Boolean& theIsInfinite2)
{
    theIsInfinite1 = theIsInfinite2 = Standard_False;

   if (!AIS::ComputeGeometry (theFirstEdge, theFirstCurve,theFirstPnt1, theLastPnt1, theIsInfinite1))
   {
     return Standard_False;
   }

   if (!AIS::ComputeGeometry (theSecondEdge, theSecondCurve,theFirstPnt2, theLastPnt2, theIsInfinite2))
   {
     return Standard_False;
   }

  if (theIsInfinite1 || theIsInfinite2)
  {
      if (theFirstCurve->DynamicType() == theSecondCurve->DynamicType())
      {
          gp_Lin aLin1 = Handle(Geom_Line)::DownCast (theFirstCurve)->Lin();
          gp_Lin aLin2 = Handle(Geom_Line)::DownCast (theSecondCurve)->Lin();

          if (theIsInfinite1)
          {
              theFirstPnt1 = ElCLib::Value (ElCLib::Parameter (aLin2, theFirstPnt2), aLin1);
              theLastPnt1 = ElCLib::Value (ElCLib::Parameter (aLin2, theLastPnt2), aLin1);
          }
          else if (theIsInfinite2)
          {
              theFirstPnt2 = ElCLib::Value (ElCLib::Parameter (aLin1, theFirstPnt1), aLin2);
              theLastPnt2 = ElCLib::Value (ElCLib::Parameter (aLin1, theLastPnt1), aLin2);
          }
      }
      else
      {
        if (theIsInfinite1 && !theIsInfinite2)
        {
          GeomAPI_ProjectPointOnCurve aProjector (theFirstPnt2, theFirstCurve);
          theFirstPnt1 = theFirstCurve->Value (aProjector.LowerDistanceParameter ());

          aProjector.Init (theLastPnt2, theFirstCurve);
          theLastPnt1 = theFirstCurve->Value (aProjector.LowerDistanceParameter ());
        }
        else if (!theIsInfinite1 && theIsInfinite2)
        {
          GeomAPI_ProjectPointOnCurve aProjector (theFirstPnt1, theSecondCurve);
          theFirstPnt2 = theSecondCurve->Value (aProjector.LowerDistanceParameter ());

          aProjector.Init (theLastPnt1, theSecondCurve);
          theLastPnt2 = theSecondCurve->Value (aProjector.LowerDistanceParameter ());
        }
        else
        {
          return Standard_False;
        }
      }
  }

    return Standard_True;
}

//=======================================================================
//function : ComputeGeometry
//purpose  : Computes the geometry of the 2 edges in the current wp
//           and the 'right' geometry of the edges if one doesn't
//           belong to the current working plane.
//           There may be only one curve that can't belong to the
//           current working plane ( attachement constraint)
//           if the 2 edges belong to the current WP, <WhatProj> = 0
//
//           indexExt = 0 2 edges are in the current wp
//           indexExt = 1 first edge is not in the current wp
//           indexExt = 2 second edge is not in the current wp
//           if none of the two edges is in the current wp ,
//           it returns Standard_False
//=======================================================================
Standard_Boolean AIS::ComputeGeometry (const TopoDS_Edge& theFirstEdge,
                                       const TopoDS_Edge& theSecondEdge,
                                       Standard_Integer& theExtIndex,
                                       Handle(Geom_Curve)& theFirstCurve,
                                       Handle(Geom_Curve)& theSecondCurve,
                                       gp_Pnt& theFirstPnt1,
                                       gp_Pnt& theLastPnt1,
                                       gp_Pnt& theFirstPnt2,
                                       gp_Pnt& theLastPnt2,
                                       Handle(Geom_Curve)& theExtCurve,
                                       Standard_Boolean& theIsInfinite1,
                                       Standard_Boolean& theIsInfinite2,
                                       const Handle(Geom_Plane)& thePlane)
{
  if (thePlane.IsNull())
  {
    return Standard_False;
  }

  theExtCurve.Nullify();
  theExtIndex = 0;

  Standard_Real aFirst1, aLast1, aFirst2, aLast2;
  theIsInfinite1 = theIsInfinite2 = Standard_False;

  BRepAdaptor_Curve aFirstAdaptor (theFirstEdge);
  BRepAdaptor_Curve aSecondAdaptor (theSecondEdge);

  theFirstCurve = Handle(Geom_Curve)::DownCast
                  (aFirstAdaptor.Curve().Curve()->Transformed (aFirstAdaptor.Trsf()));
  theSecondCurve = Handle(Geom_Curve)::DownCast
                  (aSecondAdaptor.Curve().Curve()->Transformed (aSecondAdaptor.Trsf()));

  if (theFirstCurve->IsInstance (STANDARD_TYPE (Geom_TrimmedCurve)))
  {
    theFirstCurve = Handle(Geom_TrimmedCurve)::DownCast (theFirstCurve)->BasisCurve();
  }
  if (theSecondCurve->IsInstance (STANDARD_TYPE (Geom_TrimmedCurve)))
  {
    theSecondCurve = Handle(Geom_TrimmedCurve)::DownCast (theSecondCurve)->BasisCurve();
  }

  aFirst1 = aFirstAdaptor.FirstParameter();
  aLast1 = aFirstAdaptor.LastParameter();

  aFirst2 = aSecondAdaptor.FirstParameter();
  aLast2 = aSecondAdaptor.LastParameter();

  if (theFirstCurve.IsNull() || theSecondCurve.IsNull())
  {
    return Standard_False;
  }

  Handle(Geom_Curve) aFirstSaved = theFirstCurve;
  Handle(Geom_Curve) aSecondSaved = theSecondCurve;

  // Checks that the projected curve is not in the plane
  Standard_Boolean isFirstOnPlane,isSecondOnPlane;

  if ((!ComputeGeomCurve (theFirstCurve, aFirst1, aLast1, theFirstPnt1, theLastPnt1, thePlane, isFirstOnPlane))
      || (!ComputeGeomCurve( theSecondCurve, aFirst2, aLast2, theFirstPnt2, theLastPnt2, thePlane,isSecondOnPlane)))
  {
    return Standard_False;
  }

  if (Precision::IsInfinite (aFirst1) || Precision::IsInfinite (aLast1))
  {
    theIsInfinite1 = Standard_True;
    theExtIndex = 1;
  }
  if (Precision::IsInfinite (aFirst2) || Precision::IsInfinite (aLast2))
  {
    theIsInfinite2 = Standard_True;
    theExtIndex = 2;
  }
  if (theIsInfinite1 && theIsInfinite2)
  {
    theExtIndex = 0;
  }

  if (theIsInfinite1 || theIsInfinite2)
  {
    if (theFirstCurve->DynamicType() == theSecondCurve->DynamicType())
    {
      gp_Lin aLin1 = Handle(Geom_Line)::DownCast (theFirstCurve)->Lin();
      gp_Lin aLin2 = Handle(Geom_Line)::DownCast (theSecondCurve)->Lin();

      if (theExtIndex == 1)
      {
        theFirstPnt1 = ElCLib::Value (ElCLib::Parameter (aLin2, theFirstPnt2), aLin1);
        theLastPnt1 = ElCLib::Value (ElCLib::Parameter (aLin2, theLastPnt2), aLin1);
      }
      else if (theExtIndex == 2)
      {
        theFirstPnt2 = ElCLib::Value (ElCLib::Parameter (aLin1, theFirstPnt1), aLin2);
        theLastPnt2 = ElCLib::Value (ElCLib::Parameter (aLin1, theLastPnt1), aLin2);
      }
    }
  }

  if (isFirstOnPlane && isSecondOnPlane)
  {
    return Standard_True;
  }

  if (!isFirstOnPlane && isSecondOnPlane)
  {// curve 2 only in the plane
    theExtIndex = 1;
    theExtCurve = aFirstSaved;
  }
  else if (isFirstOnPlane && !isSecondOnPlane)
  {// curve 1 only in the plane
    theExtIndex = 2;
    theExtCurve = aSecondSaved;
  }
  else
  {
    return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
//function : ComputeGeomCurve
//purpose  : Checks if aCurve belongs to aPlane; if not, projects aCurve in aPlane
//           and returns aCurveproj;
//           Return TRUE if ok
//=======================================================================
Standard_Boolean AIS::ComputeGeomCurve (Handle(Geom_Curve)& aCurve,
                                        const Standard_Real first1,
                                        const Standard_Real last1,
                                        gp_Pnt& FirstPnt1,
                                        gp_Pnt& LastPnt1,
                                        const Handle(Geom_Plane)& aPlane,
                                        Standard_Boolean& isOnPlane)
{
  isOnPlane = Standard_True;
  const Standard_Integer NodeNumber = 20;
  Standard_Real Delta = (last1 - first1) / (NodeNumber - 1);
  if (Delta <= Precision::PConfusion())
  {
    Delta = last1 - first1;
  }

  gp_Pnt CurPnt(0.0, 0.0, 0.0);
  Standard_Real CurPar = first1;
  for (Standard_Integer i = 1; i <= NodeNumber; i++)
  {
    CurPnt = aCurve->Value( CurPar );
    if (aPlane->Pln().SquareDistance( CurPnt ) > SquareTolerance)
    {
      isOnPlane = Standard_False;
      break;
    }
    CurPar += Delta;
  }

  if (!Precision::IsInfinite(first1) && !Precision::IsInfinite(last1))
  {
    FirstPnt1 = aCurve->Value (first1);
    LastPnt1  = aCurve->Value (last1);
  }

  if (!isOnPlane)
  {
    Handle(Geom_Curve) aGeomCurve = GeomProjLib::ProjectOnPlane (aCurve,
                                                                aPlane,
                                                                aPlane->Pln().Axis().Direction(),
                                                                Standard_False);
    aCurve = aGeomCurve;
    if (aCurve->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
    {
      aCurve = Handle(Geom_TrimmedCurve)::DownCast (aCurve)->BasisCurve();
    }
    if (! Precision::IsInfinite(first1) && ! Precision::IsInfinite(last1))
    {
      FirstPnt1 = AIS::ProjectPointOnPlane( FirstPnt1, aPlane->Pln() );
      LastPnt1 = AIS::ProjectPointOnPlane( LastPnt1, aPlane->Pln() );
    }
  }
  return Standard_True;
}

//=======================================================================
//function : ComputeGeometry
//purpose  : computes the point corresponding to the vertex <aVertex>
//           in the plane <aPlane>. If the vertex is already in the plane
//           <isOnPlane>, <isOnPlane> = true.
//           <point> is the projected vertex in the plane.
//=======================================================================
Standard_Boolean  AIS::ComputeGeometry(const TopoDS_Vertex& aVertex,
                                       gp_Pnt& point,
                                       const Handle(Geom_Plane)& aPlane,
                                       Standard_Boolean& isOnPlane)
{
  point = BRep_Tool::Pnt(aVertex);
  isOnPlane = aPlane->Pln().Contains(point,  Precision::Confusion());
  if ( !isOnPlane) {
    point = AIS::ProjectPointOnPlane( point, aPlane->Pln() );
  }
  return Standard_True;
}

//=======================================================================
//function : GetPlaneFromFace
//purpose  :
//           Returns type of surface which can be Plane or OtherSurface
//=======================================================================
Standard_Boolean AIS::GetPlaneFromFace(const TopoDS_Face&            aFace,
                                        gp_Pln &                     aPlane,
                                        Handle( Geom_Surface )&      aSurf,
                                        AIS_KindOfSurface &          aSurfType,
                                        Standard_Real &              Offset)

{
  Standard_Boolean Result = Standard_False;
  BRepAdaptor_Surface surf1( aFace );
  Handle( Adaptor3d_HSurface ) surf2;
  Standard_Boolean isOffset = Standard_False;

  if (surf1.GetType() == GeomAbs_OffsetSurface)
  {
    // Extracting Basis Surface
    surf2 = surf1.BasisSurface();
    isOffset = Standard_True;
  }
  else
    surf2 = new BRepAdaptor_HSurface( surf1 );

  aSurf = surf1.Surface().Surface();
  //  aSurf->Transform(surf1.Trsf()) ;
  aSurf = Handle( Geom_Surface )::DownCast( aSurf->Transformed( surf1.Trsf() ) );

  if (surf2->GetType() == GeomAbs_Plane)
  {
    aPlane = surf2->Plane();
    aSurfType = AIS_KOS_Plane;
    Offset = 0.;
    Result = Standard_True;
  }

  else if (surf2->GetType() == GeomAbs_SurfaceOfExtrusion)
  {
    Handle( Adaptor3d_HCurve ) BasisCurve = surf2->BasisCurve();
    gp_Dir ExtrusionDir = surf2->Direction();
    if (BasisCurve->GetType() == GeomAbs_Line)
    {
      gp_Lin BasisLine = BasisCurve->Line();
      gp_Dir LineDir = BasisLine.Direction();
      gp_Pnt LinePos = BasisLine.Location();
      gp_Pln thePlane( LinePos, LineDir ^ ExtrusionDir);
      aPlane = thePlane;
      aSurfType = AIS_KOS_Plane;
      Offset = 0.;
      Result = Standard_True;
    }
  }

  if (Result == Standard_True && isOffset)
  {
    aSurf = (Handle( Geom_OffsetSurface )::DownCast( aSurf ))->Surface();
    aPlane = (Handle( Geom_Plane )::DownCast( aSurf ))->Pln();
    Offset = 0.0e0;
  }
  if (Result == Standard_False)
  {
    if (isOffset)
    {
      Handle( Standard_Type ) TheType = aSurf->DynamicType();
      if (TheType == STANDARD_TYPE(Geom_CylindricalSurface) ||
        TheType == STANDARD_TYPE(Geom_ConicalSurface)     ||
        TheType == STANDARD_TYPE(Geom_SphericalSurface)   ||
        TheType == STANDARD_TYPE(Geom_ToroidalSurface))
      {
        aSurf = (Handle( Geom_OffsetSurface )::DownCast( aSurf ))->Surface();
        Offset = 0.0e0;
      }
      else
      {
        Offset = (Handle( Geom_OffsetSurface )::DownCast( aSurf ))->Offset();
        aSurf =  (Handle( Geom_OffsetSurface )::DownCast( aSurf ))->BasisSurface();
      }
    }
    Handle( Standard_Type ) TheType = aSurf->DynamicType();
    if (TheType == STANDARD_TYPE(Geom_CylindricalSurface))
      aSurfType = AIS_KOS_Cylinder;
    else if (TheType == STANDARD_TYPE(Geom_ConicalSurface))
      aSurfType = AIS_KOS_Cone;
    else if (TheType == STANDARD_TYPE(Geom_SphericalSurface))
      aSurfType = AIS_KOS_Sphere;
    else if (TheType == STANDARD_TYPE(Geom_ToroidalSurface))
      aSurfType = AIS_KOS_Torus;
    else if (TheType == STANDARD_TYPE(Geom_SurfaceOfRevolution))
      aSurfType = AIS_KOS_Revolution;
    else if (TheType == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))
      aSurfType = AIS_KOS_Extrusion;
    else
      aSurfType = AIS_KOS_OtherSurface;
  }
  return Result;
}


//=======================================================================
//function : ProjectPointOnPlane
//purpose  :
//=======================================================================

gp_Pnt AIS::ProjectPointOnPlane( const gp_Pnt & aPoint, const gp_Pln & aPlane )
{
  gp_Vec aVec( aPlane.Location(), aPoint );
  gp_Vec Normal = aPlane.Axis().Direction();
  Normal = (aVec * Normal) * Normal;

  return ( aPoint.Translated( -Normal ) );
}

//=======================================================================
//function : ProjectPointOnLine
//purpose  :
//=======================================================================

gp_Pnt AIS::ProjectPointOnLine( const gp_Pnt & aPoint, const gp_Lin & aLine )
{
  gp_XYZ LinLoc = aLine.Location().XYZ();
  gp_XYZ LinDir = aLine.Direction().XYZ();
  Standard_Real Parameter = (aPoint.XYZ() - LinLoc) * LinDir;
  gp_Pnt Result( LinLoc + Parameter * LinDir );
  return Result;
}

//=======================================================================
//function : InitFaceLength
//purpose  : 
//=======================================================================
void AIS::InitFaceLength (const TopoDS_Face& aFace,
                          gp_Pln               & aPlane,
                          Handle(Geom_Surface) & aSurface,
                          AIS_KindOfSurface    & aSurfaceType,
                          Standard_Real        & anOffset)
{
  AIS::GetPlaneFromFace( aFace, aPlane, aSurface, aSurfaceType, anOffset );
  
  if (Abs( anOffset ) > Precision::Confusion())
    {
      aSurface = new Geom_OffsetSurface( aSurface, anOffset );
      anOffset = 0.0e0;
    }
  
}

//=======================================================================
//function : InitAngleBetweenPlanarFaces
//purpose  :
//=======================================================================
Standard_Boolean AIS::InitAngleBetweenPlanarFaces (const TopoDS_Face&          theFirstFace,
                                                   const TopoDS_Face&          theSecondFace,
                                                   gp_Pnt &                    theCenter,
                                                   gp_Pnt &                    theFirstAttach,
                                                   gp_Pnt &                    theSecondAttach,
                                                   const Standard_Boolean      theIsFirstPointSet)
{
  Handle(Geom_Plane) aFirstPlane = Handle(Geom_Plane)::DownCast (BRep_Tool::Surface (theFirstFace));
  Handle(Geom_Plane) aSecondPlane = Handle(Geom_Plane)::DownCast (BRep_Tool::Surface (theSecondFace));

  GeomAPI_IntSS aPlaneIntersector (aFirstPlane, aSecondPlane, Precision::Confusion());

  // Fails if two planes haven't only one intersection line.
  if (!aPlaneIntersector.IsDone())
  {
    return Standard_False;
  }

  if (aPlaneIntersector.NbLines() != 1)
  {
    return Standard_False;
  }

  // Get intersect line.
  Handle(Geom_Curve) anIntersectCurve = aPlaneIntersector.Line (1);

  Handle(Geom_Line) anIntersectLine = Handle(Geom_Line)::DownCast (anIntersectCurve);

  if (anIntersectLine.IsNull())
  {
    return Standard_False;
  }

  gp_Lin anIntersectLin = anIntersectLine->Lin();

  gp_Pnt aFirstCenter, aSecondCenter;
  Standard_Real anU1Min, anU1Max, aV1Min, aV1Max;
  Standard_Real anU2Min, anU2Max, aV2Min, aV2Max;

  BRepTools::UVBounds (theFirstFace, anU1Min, anU1Max, aV1Min, aV1Max);
  BRepTools::UVBounds (theSecondFace, anU2Min, anU2Max, aV2Min, aV2Max);

  // Get first and second attach.
  if (theIsFirstPointSet)
  {
    GeomAPI_ProjectPointOnSurf aProjector (theFirstAttach, aFirstPlane);
    if (!aProjector.IsDone())
    {
      return Standard_False;
    }

    aFirstCenter = aProjector.Point (1);
  }
  else
  {
    aFirstCenter = aFirstPlane->Value ((anU1Min + anU1Max) * 0.5, (aV1Min + aV1Max) * 0.5);
  }

  aSecondCenter = aSecondPlane->Value ((anU2Min + anU2Max) * 0.5, (aV2Min + aV2Max) * 0.5);

  GeomAPI_ProjectPointOnCurve aProj (aFirstCenter, anIntersectCurve);
  theCenter = aProj.NearestPoint();

  gp_Vec aFirstNormal = anIntersectLin.Direction() ^ aFirstPlane->Pln().Axis().Direction();
  if (aFirstNormal * gp_Vec (theCenter, aFirstCenter) < 0.0)
  {
    aFirstNormal.Reverse();
  }
  theFirstAttach = theCenter.Translated (aFirstNormal);

  gp_Vec aSecondNormal = anIntersectLin.Direction() ^ aSecondPlane->Pln().Axis().Direction();
  if (aSecondNormal * gp_Vec (theCenter, aSecondCenter) < 0.0)
  {
    aSecondNormal.Reverse();
  }
  theSecondAttach = theCenter.Translated (aSecondNormal);

  return Standard_True;
}

//=======================================================================
//function : InitAngleBetweenCurvilinearFaces
//purpose  :
//=======================================================================
Standard_Boolean AIS::InitAngleBetweenCurvilinearFaces (const TopoDS_Face&          theFirstFace,
                                                        const TopoDS_Face&          theSecondFace,
                                                        const AIS_KindOfSurface     theFirstSurfType,
                                                        const AIS_KindOfSurface     theSecondSurfType,
                                                        gp_Pnt&                     theCenter,
                                                        gp_Pnt&                     theFirstAttach,
                                                        gp_Pnt&                     theSecondAttach,
                                                        const Standard_Boolean      theIsFirstPointSet)
{
  Handle(Geom_Surface) aFirstSurf = BRep_Tool::Surface (theFirstFace);
  Handle(Geom_Surface) aSecondSurf = BRep_Tool::Surface (theSecondFace);

  // Find intersection curve between two surfaces.
  GeomAPI_IntSS aSurfaceIntersector (aFirstSurf, aSecondSurf, Precision::Confusion());

  // Fails if two planes haven't only one intersection line.
  if (!aSurfaceIntersector.IsDone())
  {
    return Standard_False;
  }

  if (aSurfaceIntersector.NbLines() != 1)
  {
    return Standard_False;
  }

  // Get intersect line.
  Handle(Geom_Curve) anIntersectCurve = aSurfaceIntersector.Line (1);

  Handle(Geom_Line) aFirstLine, aSecondLine;
  Standard_Real aFirstU = 0.0;
  Standard_Real aFirstV = 0.0;

  if (theIsFirstPointSet)
  {
    GeomAPI_ProjectPointOnSurf aProjector (theFirstAttach, aFirstSurf);
    if (!aProjector.IsDone())
    {
      return Standard_False;
    }

    theFirstAttach = aProjector.Point (1);
  }
  else
  {
    theFirstAttach = aFirstSurf->Value (aFirstU, aFirstV);
  }

  aFirstLine = Handle(Geom_Line)::DownCast (aFirstSurf->UIso (aFirstU));

  if (theSecondSurfType == AIS_KOS_Cylinder)
  {
    Handle(Geom_CylindricalSurface) aCylinder = Handle(Geom_CylindricalSurface)::DownCast (aSecondSurf);

    Standard_Real aSecondU = aCylinder->Cylinder().XAxis().Direction().Angle(
                               gce_MakeDir (ProjectPointOnLine (theFirstAttach,
                                                                gp_Lin (aCylinder->Cylinder().Axis())),
                                            theFirstAttach));

    aSecondLine = Handle(Geom_Line)::DownCast (aCylinder->UIso (aSecondU));
  }
  else if (theSecondSurfType == AIS_KOS_Cone)
  {
    Handle(Geom_ConicalSurface) aCone = Handle(Geom_ConicalSurface)::DownCast (aSecondSurf);

    gp_Dir anXdirection = aCone->Cone().XAxis().Direction();

    gp_Dir aToFirstAttach = gce_MakeDir (ProjectPointOnLine (theFirstAttach,
                                                             gp_Lin (aCone->Cone().Axis())),
                                         theFirstAttach);

    Standard_Real aSecondU = anXdirection.Angle (aToFirstAttach);

    // Check sign
    if (!anXdirection.IsEqual (aToFirstAttach, Precision::Angular()) &&
        !anXdirection.IsOpposite (aToFirstAttach, Precision::Angular()) &&
        (anXdirection ^ aToFirstAttach) * aCone->Cone().Axis().Direction() < 0.0)
    {
      aSecondU = 2*M_PI - aSecondU;
    }
    aSecondLine = Handle( Geom_Line )::DownCast (aCone->UIso(aSecondU));
  }
  else
  {
    return Standard_False;
  }

  // If angle can be computed between two lines.
  if (!(aFirstLine->Lin().Direction().IsEqual (aSecondLine->Lin().Direction(), Precision::Angular() )) &&
      !(aFirstLine->Lin().Direction().IsOpposite (aSecondLine->Lin().Direction(), Precision::Angular())))
  {
    GeomAPI_ExtremaCurveCurve anIntersector (aFirstLine, aSecondLine);
    anIntersector.Points (1, theCenter, theCenter);

    // Move theFirstAttach on aFirstLine if it is on theCenter.
    if (theCenter.SquareDistance(theFirstAttach ) <= SquareTolerance)
    {
      gp_Vec aDir (aFirstLine->Lin().Direction());
      theFirstAttach = theCenter.Translated (aDir);

      // theFirstAttach should be on theFirstSurf.
      Standard_Real anU, aV;
      if (theFirstSurfType == AIS_KOS_Cylinder)
      {
        ElSLib::Parameters ((Handle(Geom_CylindricalSurface)::DownCast (aFirstSurf))->Cylinder(),
                            theFirstAttach, anU, aV);

        theFirstAttach = ElSLib::Value (aFirstU, aV,
                                        (Handle( Geom_CylindricalSurface )::DownCast (aFirstSurf))->Cylinder() );
      }
      else if (theFirstSurfType == AIS_KOS_Cone)
      {
        ElSLib::Parameters ((Handle(Geom_ConicalSurface)::DownCast (aFirstSurf))->Cone(),
                             theFirstAttach, anU, aV);
        theFirstAttach = ElSLib::Value (aFirstU, aV,
                                       (Handle(Geom_ConicalSurface)::DownCast (aFirstSurf))->Cone());
      }
      else
      {
        return Standard_False;
      }
    }

    // Find theSecondAttach
    GeomAPI_ProjectPointOnSurf aProjector (theFirstAttach, aSecondSurf);
    if (!aProjector.IsDone())
    {
      return Standard_False;
    }
    Standard_Real anU, aV;
    aProjector.LowerDistanceParameters (anU, aV);
    theSecondAttach = aSecondSurf->Value (anU, aV);
  }
  else // aFirstLine and aSecondLine are coincident
  {
      gp_Vec aDir (aFirstLine->Lin().Direction());
      theFirstAttach = theCenter.Translated (aDir);
      theSecondAttach = theCenter.Translated (-aDir);
  }
  return Standard_True;
}

//=======================================================================
//function : ComputeLengthBetweenCurvilinearFaces
//purpose  : 
//=======================================================================
void AIS::InitLengthBetweenCurvilinearFaces (const TopoDS_Face&    theFirstFace,
                                             const TopoDS_Face&    theSecondFace,
                                             Handle(Geom_Surface)& theFirstSurf,
                                             Handle(Geom_Surface)& theSecondSurf,
                                             gp_Pnt&               theFirstAttach,
                                             gp_Pnt&               theSecondAttach,
                                             gp_Dir&               theDirOnPlane)
{
  GeomAPI_ProjectPointOnSurf aProjector;
  Standard_Real aPU, aPV;

  TopExp_Explorer anExplorer (theFirstFace, TopAbs_VERTEX);

  theFirstAttach = BRep_Tool::Pnt (TopoDS::Vertex (anExplorer.Current()));
  aProjector.Init (theFirstAttach, theFirstSurf);

  theFirstAttach = aProjector.NearestPoint();
  aProjector.LowerDistanceParameters (aPU, aPV);

  gp_Vec aD1U, aD1V;
  theFirstSurf->D1 (aPU, aPV, theFirstAttach, aD1U, aD1V);

  if (aD1U.SquareMagnitude() <= SquareTolerance || aD1V.SquareMagnitude() <= SquareTolerance)
  {
    theFirstAttach = AIS::Farest (theFirstFace, theFirstAttach);
    aProjector.Init (theFirstAttach, theFirstSurf);
    aProjector.LowerDistanceParameters (aPU, aPV);
    theFirstSurf->D1 (aPU, aPV, theFirstAttach, aD1U, aD1V);
  }

  aD1U.Normalize();
  aD1V.Normalize();

  theDirOnPlane = gp_Dir (aD1U);

  gp_Dir aFirstSurfN = gp_Dir (aD1U ^ aD1V);

  aProjector.Init (theFirstAttach, theSecondSurf);

  Standard_Integer aBestPointIndex = 0;
  Standard_Real aMinDist = RealLast();
  gp_Dir aLocalDir;

  for (Standard_Integer aPointIt = 1; aPointIt <= aProjector.NbPoints(); aPointIt++)
  {
    aProjector.Parameters (aPointIt, aPU, aPV);

    theSecondSurf->D1 (aPU, aPV, theSecondAttach, aD1U, aD1V);

    aLocalDir = aD1U.SquareMagnitude() <= SquareTolerance || aD1V.SquareMagnitude() <= SquareTolerance
              ? gp_Dir (gp_Vec (theFirstAttach, aProjector.Point (aPointIt)))
              : gp_Dir (aD1U ^ aD1V);

    if (aFirstSurfN.IsParallel (aLocalDir, Precision::Angular()) && aProjector.Distance (aPointIt) < aMinDist)
    {
      aBestPointIndex = aPointIt;
      aMinDist = aProjector.Distance (aPointIt);
    }
  }

  if (aBestPointIndex == 0)
  {
    theSecondAttach = theFirstAttach;
  }
  else
  {
    theSecondAttach = aProjector.Point (aBestPointIndex);
    aProjector.Parameters (aBestPointIndex, aPU, aPV);

    // Now there is projection of FirstAttach onto SecondSurf in aProjector
    BRepTopAdaptor_FClass2d aClassifier (theSecondFace, Precision::Confusion());

    TopAbs_State aState = 
      aClassifier.Perform (gp_Pnt2d (aPU, aPV), theSecondSurf->IsUPeriodic() || theSecondSurf->IsVPeriodic());

    if (aState == TopAbs_OUT || aState == TopAbs_UNKNOWN)
    {
      theSecondAttach = AIS::Nearest (theSecondFace, theSecondAttach);
    }
  }
}

gp_Pnt AIS::TranslatePointToBound( const gp_Pnt & aPoint, const gp_Dir & aDir, const Bnd_Box & aBndBox )
{
  if (aBndBox.IsOut( aPoint ))
    return aPoint;
  else
    {
      gp_Pnt Result(0.0,0.0,0.0);
      TColStd_Array2OfReal Bound( 1, 3, 1, 2 );
      TColStd_Array1OfReal Origin( 1, 3 );
      TColStd_Array1OfReal Dir( 1, 3 );
      Standard_Real t;
      
      aBndBox.Get( Bound(1,1), Bound(2,1), Bound(3,1), Bound(1,2),  Bound(2,2), Bound(3,2) );
      aPoint.Coord( Origin(1), Origin(2), Origin(3) );
      aDir.Coord( Dir(1), Dir(2), Dir(3) );

      Bnd_Box EnlargedBox = aBndBox;
      EnlargedBox.Enlarge( aBndBox.GetGap() + Precision::Confusion() );

      Standard_Boolean IsFound = Standard_False;
      for (Standard_Integer i = 1; i <= 3; i++)
	{
	  if (Abs( Dir( i ) ) <= gp::Resolution())
	    continue;
	  for (Standard_Integer j = 1; j <= 2; j++)
	    {
	      t = (Bound( i, j ) - Origin( i )) / Dir( i );
	      if (t < 0.0e0)
		continue;
	      Result = aPoint.Translated( gp_Vec( aDir ) * t );
	      if (! EnlargedBox.IsOut( Result ))
		{
		  IsFound = Standard_True;
		  break;
		}
	    }
	  if (IsFound) break;
	}
      return Result;
    }
}


//=======================================================================
//function : InDomain
//purpose  : 
//=======================================================================

Standard_Boolean AIS::InDomain(const Standard_Real fpar,
			       const Standard_Real lpar,
			       const Standard_Real para) 
{
  if (fpar >= 0.) {
    if(lpar > fpar)
      return ((para >= fpar) && (para <= lpar));
    else { // fpar > lpar
      Standard_Real delta = 2*M_PI-fpar;
      Standard_Real lp, par, fp;
      lp = lpar + delta;
      par = para + delta;
      while(lp > 2*M_PI) lp-=2*M_PI;
      while(par > 2*M_PI) par-=2*M_PI;
      fp = 0.;
      return ((par >= fp) && (par <= lp));
    }
      
  }
  if (para >= (fpar+2*M_PI)) return Standard_True;
  if (para <= lpar) return Standard_True;
  return Standard_False;
}

//=======================================================================
//function : DistanceFromApex
//purpose  : calculates parametric length arc of ellipse
//=======================================================================

Standard_Real AIS::DistanceFromApex(const gp_Elips & elips,
				    const gp_Pnt   & Apex,
				    const Standard_Real par)
{
  Standard_Real dist;
  Standard_Real parApex = ElCLib::Parameter ( elips, Apex );
  if(parApex == 0.0 || parApex == M_PI) 
    {//Major case
      if(parApex == 0.0) //pos Apex
	dist = (par < M_PI) ? par : (2*M_PI - par);
      else //neg Apex
	dist = (par < M_PI) ? ( M_PI - par) : ( par - M_PI );
    }
  else 
    {// Minor case
      if(parApex == M_PI / 2) //pos Apex
	{
	  if(par <= parApex + M_PI && par > parApex) // 3/2*M_PI < par < M_PI/2
	    dist = par - parApex;
	  else 
	    { 
	      if(par >  parApex + M_PI) // 3/2*M_PI < par < 2*M_PI
		dist = 2*M_PI - par + parApex;
	      else
		dist = parApex - par; 
	    }
	  }
      else //neg Apex == 3/2*M_PI
	{
	  if(par <= parApex && par >= M_PI/2) // M_PI/2 < par < 3/2*M_PI
	    dist = parApex - par;
	  else
	    {
	      if(par >  parApex) // 3/2*M_PI < par < 2*M_PI
		dist = par - parApex;
	      else
		dist = par + M_PI/2; // 0 < par < M_PI/2
	    }
	}
    }
  return dist;
}

//=======================================================================
//function : NearestApex
//purpose  : 
//=======================================================================

gp_Pnt AIS::NearestApex(const gp_Elips & elips,
			const gp_Pnt   & pApex,
			const gp_Pnt   & nApex,
			const Standard_Real fpara,
			const Standard_Real lpara,
			      Standard_Boolean & IsInDomain)
{
  Standard_Real parP, parN;
  gp_Pnt EndOfArrow(0.0,0.0,0.0);
  IsInDomain = Standard_True;
  parP = ElCLib::Parameter ( elips, pApex );
  if(InDomain(fpara, lpara, parP)) EndOfArrow = pApex;
  else 
    {
      parN = ElCLib::Parameter ( elips, nApex );
      if(InDomain(fpara, lpara, parN)) EndOfArrow = nApex;
      else {
	IsInDomain = Standard_False;
	Standard_Real posd = Min(DistanceFromApex (elips,pApex, fpara), 
				 DistanceFromApex (elips,pApex, lpara));
	Standard_Real negd = Min(DistanceFromApex (elips,nApex, fpara), 
				 DistanceFromApex (elips,nApex, lpara));
	if( posd < negd ) 
	  EndOfArrow = pApex;
	else
	  EndOfArrow = nApex;
      }
    }
  return EndOfArrow;
}

//=======================================================================
//function : ComputeProjEdgePresentation
//purpose  : 
//=======================================================================

void AIS::ComputeProjEdgePresentation (const Handle(Prs3d_Presentation)& aPresentation,
                                       const Handle(Prs3d_Drawer)& aDrawer,
                                       const TopoDS_Edge& anEdge,
                                       const Handle(Geom_Curve)& ProjCurve,
                                       const gp_Pnt& FirstP,
                                       const gp_Pnt& LastP,
                                       const Quantity_NameOfColor aColor,
                                       const Standard_Real aWidth,
                                       const Aspect_TypeOfLine aProjTOL,
                                       const Aspect_TypeOfLine aCallTOL)
{
  if (!aDrawer->HasOwnWireAspect()){
    aDrawer->SetWireAspect(new Prs3d_LineAspect(aColor,aProjTOL,2.));}
  else {
    // CLE
    // const Handle(Prs3d_LineAspect)& li = aDrawer->WireAspect();
    Handle(Prs3d_LineAspect) li = aDrawer->WireAspect();
    // ENDCLE
    li->SetColor(aColor);
    li->SetTypeOfLine(aProjTOL);
    li->SetWidth(aWidth);
  }

  Standard_Real pf, pl;
  TopLoc_Location loc;
  Handle(Geom_Curve) curve;
  Standard_Boolean isInfinite;
  curve = BRep_Tool::Curve(anEdge,loc,pf,pl);
  isInfinite = (Precision::IsInfinite(pf) || Precision::IsInfinite(pl));

  TopoDS_Edge E;

  // Calculate  presentation of the edge
  if (ProjCurve->IsInstance(STANDARD_TYPE(Geom_Line)) ) {
    // CLE
    // Handle(Geom_Line) gl (Handle(Geom_Line)::DownCast (ProjCurve));
    Handle(Geom_Line) gl = Handle(Geom_Line)::DownCast (ProjCurve);
    // ENDCLE
    if ( !isInfinite) {
      pf = ElCLib::Parameter(gl->Lin(),FirstP);
      pl = ElCLib::Parameter(gl->Lin(),LastP);
      BRepBuilderAPI_MakeEdge MakEd(gl->Lin(), pf, pl);
      E = MakEd.Edge();
    }
    else {
      BRepBuilderAPI_MakeEdge MakEd(gl->Lin());
      E = MakEd.Edge();
    }
  }
  else if (ProjCurve->IsInstance(STANDARD_TYPE(Geom_Circle)) ) {
    // CLE
    // Handle(Geom_Circle) gc (Handle(Geom_Circle)::DownCast (ProjCurve));
    Handle(Geom_Circle) gc = Handle(Geom_Circle)::DownCast (ProjCurve);
    // ENDCLE
    pf = ElCLib::Parameter(gc->Circ(),FirstP);
    pl = ElCLib::Parameter(gc->Circ(),LastP);
    BRepBuilderAPI_MakeEdge MakEd(gc->Circ(),pf, pl);
    E = MakEd.Edge();
  }
  StdPrs_WFShape::Add (aPresentation, E, aDrawer);

  //Calculate the presentation of line connections
  aDrawer->WireAspect()->SetTypeOfLine(aCallTOL);
  if (!isInfinite) {
    gp_Pnt ppf(0.0,0.0,0.0), ppl(0.0,0.0,0.0);
    ppf = BRep_Tool::Pnt( TopExp::FirstVertex(TopoDS::Edge(anEdge)));
    ppl = BRep_Tool::Pnt( TopExp::LastVertex(TopoDS::Edge(anEdge)));

    // it is patch!
    if (FirstP.SquareDistance (ppf) > SquareTolerance)
    {
      BRepBuilderAPI_MakeEdge MakEd1 (FirstP, ppf);
      StdPrs_WFShape::Add (aPresentation, MakEd1.Edge(), aDrawer);
    }
    else
    {
      BRepBuilderAPI_MakeVertex MakVert1 (FirstP);
      StdPrs_WFShape::Add (aPresentation, MakVert1.Vertex(), aDrawer);
    }
    if (LastP.SquareDistance (ppl) > SquareTolerance)
    {
      BRepBuilderAPI_MakeEdge MakEd2 (LastP, ppl);
      StdPrs_WFShape::Add (aPresentation, MakEd2.Edge(), aDrawer);
    }
    else
    {
      BRepBuilderAPI_MakeVertex MakVert2 (LastP);
      StdPrs_WFShape::Add (aPresentation, MakVert2.Vertex(), aDrawer);
    }
/*
    BRepBuilderAPI_MakeEdge MakEd1 (FirstP, ppf);
    StdPrs_WFShape::Add (aPresentation, MakEd1.Edge(), aDrawer);
    BRepBuilderAPI_MakeEdge MakEd2 (LastP, ppl);
    StdPrs_WFShape::Add (aPresentation, MakEd2.Edge(), aDrawer);
*/
  }
}

//=======================================================================
//function : ComputeProjVertexPresentation
//purpose  : 
//=======================================================================

void AIS::ComputeProjVertexPresentation (const Handle( Prs3d_Presentation )& aPresentation,
                                         const Handle( Prs3d_Drawer )& aDrawer,
                                         const TopoDS_Vertex& aVertex,
                                         const gp_Pnt& ProjPoint,
                                         const Quantity_NameOfColor aColor,
                                         const Standard_Real aWidth,
                                         const Aspect_TypeOfMarker aProjTOM,
                                         const Aspect_TypeOfLine aCallTOL)
{
  if (!aDrawer->HasOwnPointAspect()){
    aDrawer->SetPointAspect(new Prs3d_PointAspect(aProjTOM, aColor,1));}
  else {
    // CLE
    // const Handle(Prs3d_PointAspect)& pa = aDrawer->PointAspect();
    Handle(Prs3d_PointAspect) pa = aDrawer->PointAspect();
    // ENDCLE
    pa->SetColor(aColor);
    pa->SetTypeOfMarker(aProjTOM);
  }
  
  // calculate the projection
  StdPrs_Point::Add(aPresentation, new Geom_CartesianPoint(ProjPoint), aDrawer);

  if (!aDrawer->HasOwnWireAspect()){
    aDrawer->SetWireAspect(new Prs3d_LineAspect(aColor,aCallTOL,2.));}
  else {
    // CLE
    // const Handle(Prs3d_LineAspect)& li = aDrawer->WireAspect();
    Handle(Prs3d_LineAspect) li = aDrawer->WireAspect();
    // ENDCLE
    li->SetColor(aColor);
    li->SetTypeOfLine(aCallTOL);
    li->SetWidth(aWidth);
  }
  
  // If the points are not mixed...
  if (!ProjPoint.IsEqual (BRep_Tool::Pnt (aVertex), Precision::Confusion()))
  {
    // calculate the lines of recall
    BRepBuilderAPI_MakeEdge MakEd (ProjPoint, BRep_Tool::Pnt (aVertex));
    StdPrs_WFShape::Add (aPresentation, MakEd.Edge(), aDrawer);
  }
}
