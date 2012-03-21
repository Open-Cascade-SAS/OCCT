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

#include <TopoDSToStep_FacetedTool.ixx>

#include <BRep_Tool.hxx>

#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>

#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>

// ============================================================================
// Method  :
// Purpose :
// ============================================================================

TopoDSToStep_FacetedError TopoDSToStep_FacetedTool::CheckTopoDSShape
(const TopoDS_Shape& aShape)
{
  TopExp_Explorer FaceExp, EdgeExp;
  FaceExp.Init(aShape,TopAbs_FACE);
  while (FaceExp.More()) {
    const TopoDS_Face aFace = TopoDS::Face(FaceExp.Current());
    FaceExp.Next();
    
    Handle(Geom_Surface) Su = BRep_Tool::Surface(aFace);
    
    if (Su->IsKind(STANDARD_TYPE(Geom_Plane))) {
      // OK -> no further check
    }
    else if (Su->IsKind(STANDARD_TYPE(Geom_BSplineSurface))) {
      Handle(Geom_BSplineSurface) aBsplS =
	Handle(Geom_BSplineSurface)::DownCast(Su);
      Standard_Integer uDeg, vDeg, nUPol, nVPol;
      uDeg = aBsplS->UDegree();
      if (uDeg == 1) {
	vDeg = aBsplS->VDegree();
	if (vDeg == 1) {
	  nUPol = aBsplS->NbUPoles();
	  nVPol = aBsplS->NbVPoles();
	  if (nUPol != 2  ||  nVPol != 2) {
	    return TopoDSToStep_SurfaceNotPlane;
	  }
	}
	else {
	  // Degree in v != 1
	  return TopoDSToStep_SurfaceNotPlane;
	}
      }
      else {
	// Degree in u != 1
	return TopoDSToStep_SurfaceNotPlane;
      }
    }
    else if (Su->IsKind(STANDARD_TYPE(Geom_BezierSurface))) {
      Handle(Geom_BezierSurface) aBzS = 
	Handle(Geom_BezierSurface)::DownCast(Su);
      Standard_Integer uDeg, vDeg, nUPol, nVPol;
      uDeg = aBzS->UDegree();
      if (uDeg == 1) {
	vDeg = aBzS->VDegree();
	if (vDeg == 1) {
	  nUPol = aBzS->NbUPoles();
	  nVPol = aBzS->NbVPoles();
	  if (nUPol != 2  ||  nVPol != 2) {
	    return TopoDSToStep_SurfaceNotPlane;
	  }
	}
	else {
	  // Degree in v != 1
	  return TopoDSToStep_SurfaceNotPlane;
	}
      }
      else {
	// Degree in u != 1
	return TopoDSToStep_SurfaceNotPlane;
      }
    }
    else {
      // the surface is neither a Plane nor a flat BSpline or Beziersurface
      return  TopoDSToStep_SurfaceNotPlane;
    }
    
    // surface is flat; now check, if the PCurves are linear
    
    EdgeExp.Init(aFace,TopAbs_EDGE);
    while (EdgeExp.More()) {
      const TopoDS_Edge anEdge = TopoDS::Edge(EdgeExp.Current());
      EdgeExp.Next();
      
      Standard_Real cf, cl;
      Handle(Geom2d_Curve) C2d = 
	BRep_Tool::CurveOnSurface(anEdge, aFace, cf, cl);
      
      if (C2d->IsKind(STANDARD_TYPE(Geom2d_Line))) {
	return  TopoDSToStep_FacetedDone;
      }
      else if (C2d->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve))) {
	Handle(Geom2d_BSplineCurve) aBspl2d = 
	  Handle(Geom2d_BSplineCurve)::DownCast(C2d);
	if ((aBspl2d->Degree() != 1) || (aBspl2d->NbPoles() != 2)) {
	  return  TopoDSToStep_PCurveNotLinear;
	}
      }
      else if (C2d->IsKind(STANDARD_TYPE(Geom2d_BezierCurve))) {
	Handle(Geom2d_BezierCurve) aBzC2d = 
	  Handle(Geom2d_BezierCurve)::DownCast(C2d);
	if ((aBzC2d->Degree() != 1) || (aBzC2d->NbPoles() != 2)) {
	  return  TopoDSToStep_PCurveNotLinear;
	}
      }
      else {
	return  TopoDSToStep_PCurveNotLinear;
      }
    }  // end while (EdgeExp.More())
  }  // end while (FaceExp.More())
 
  return  TopoDSToStep_FacetedDone;
}
