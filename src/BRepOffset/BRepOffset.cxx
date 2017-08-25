// Created on: 1995-10-25
// Created by: Bruno DUMORTIER
// Copyright (c) 1995-1999 Matra Datavision
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


#include <BRepOffset.hxx>
#include <BRep_Tool.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <NCollection_LocalArray.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_ListOfShape.hxx>
#include <TopoDS_Vertex.hxx>

//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================
Handle(Geom_Surface) BRepOffset::Surface(const Handle(Geom_Surface)& Surface,
					                     const Standard_Real Offset,
                                         BRepOffset_Status& theStatus,
                                         Standard_Boolean allowC0)
{
  Standard_Real Tol = Precision::Confusion();

  theStatus = BRepOffset_Good;
  Handle(Geom_Surface) Result;
  
  Handle(Standard_Type) TheType = Surface->DynamicType();

  if (TheType == STANDARD_TYPE(Geom_Plane)) {
    Handle(Geom_Plane) P =
      Handle(Geom_Plane)::DownCast(Surface);
    gp_Vec T = P->Position().XDirection()^P->Position().YDirection();
    T *= Offset;
    Result = Handle(Geom_Plane)::DownCast(P->Translated(T));
  }
  else if (TheType == STANDARD_TYPE(Geom_CylindricalSurface)) {
    Handle(Geom_CylindricalSurface) C =
      Handle(Geom_CylindricalSurface)::DownCast(Surface);
    Standard_Real Radius = C->Radius();
    gp_Ax3 Axis = C->Position();
    if (Axis.Direct()) 
      Radius += Offset;
    else 
      Radius -= Offset;
    if ( Radius >= Tol ) {
      Result = new Geom_CylindricalSurface( Axis, Radius);
    }
    else if ( Radius <= -Tol ){
      Axis.Rotate(gp_Ax1(Axis.Location(),Axis.Direction()),M_PI);
      Result = new Geom_CylindricalSurface( Axis, Abs(Radius));
      theStatus = BRepOffset_Reversed;
    }
    else {
      theStatus = BRepOffset_Degenerated;
    }
  }
  else if (TheType == STANDARD_TYPE(Geom_ConicalSurface)) {
    Handle(Geom_ConicalSurface) C =
      Handle(Geom_ConicalSurface)::DownCast(Surface);
    Standard_Real Alpha = C->SemiAngle();
    Standard_Real Radius = C->RefRadius() + Offset * Cos(Alpha);
    gp_Ax3 Axis = C->Position();
    if ( Radius >= 0.) {
      gp_Vec Z( Axis.Direction());
      Z *= - Offset * Sin(Alpha);
      Axis.Translate(Z);
    }
    else {
      Radius = -Radius; 
      gp_Vec Z( Axis.Direction());
      Z *= - Offset * Sin(Alpha);
      Axis.Translate(Z);
      Axis.Rotate(gp_Ax1(Axis.Location(),Axis.Direction()),M_PI);
      Alpha = -Alpha; 
    }
    Result = new Geom_ConicalSurface(Axis, Alpha, Radius);
  }
  else if (TheType == STANDARD_TYPE(Geom_SphericalSurface)) {
    Handle(Geom_SphericalSurface) S = 
      Handle(Geom_SphericalSurface)::DownCast(Surface);
    Standard_Real Radius = S->Radius();
    gp_Ax3 Axis = S->Position();
    if (Axis.Direct()) 
      Radius += Offset;
    else 
      Radius -= Offset;
    if ( Radius >= Tol) {
      Result = new Geom_SphericalSurface(Axis, Radius);
    }
    else if ( Radius <= -Tol ) {
      Axis.Rotate(gp_Ax1(Axis.Location(),Axis.Direction()),M_PI);
      Axis.ZReverse();
      Result = new Geom_SphericalSurface(Axis, -Radius);
      theStatus = BRepOffset_Reversed;
    }
    else {
      theStatus = BRepOffset_Degenerated;
    }
  }
  else if (TheType == STANDARD_TYPE(Geom_ToroidalSurface)) {
    Handle(Geom_ToroidalSurface) S = 
      Handle(Geom_ToroidalSurface)::DownCast(Surface);
    Standard_Real MajorRadius = S->MajorRadius();
    Standard_Real MinorRadius = S->MinorRadius();
    gp_Ax3 Axis = S->Position();
    if (MinorRadius < MajorRadius) {  // A FINIR
      if (Axis.Direct())
	MinorRadius += Offset;
      else 
	MinorRadius -= Offset;
      if (MinorRadius >= Tol) {
	Result = new Geom_ToroidalSurface(Axis,MajorRadius,MinorRadius);
      }
      else if (MinorRadius <= -Tol) {
	theStatus = BRepOffset_Reversed;
      }
      else {
	theStatus = BRepOffset_Degenerated;
      }
    }
  }
  else if (TheType == STANDARD_TYPE(Geom_SurfaceOfRevolution)) {
  }
  else if (TheType == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)) {
  }
  else if (TheType == STANDARD_TYPE(Geom_BSplineSurface)) {
  }
  else if (TheType == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    Handle(Geom_RectangularTrimmedSurface) S = 
      Handle(Geom_RectangularTrimmedSurface)::DownCast(Surface);
    Standard_Real U1,U2,V1,V2;
    S->Bounds(U1,U2,V1,V2);
    Handle(Geom_Surface) Off = BRepOffset::Surface (S->BasisSurface(), Offset, theStatus, allowC0);
    Result = new Geom_RectangularTrimmedSurface (Off,U1,U2,V1,V2);
  }
  else if (TheType == STANDARD_TYPE(Geom_OffsetSurface)) {
  }

  if ( Result.IsNull()) {
    Result = new Geom_OffsetSurface( Surface, Offset, allowC0);
  }
  
  return Result;
}

//=======================================================================
//function : CollapseSingularities
//purpose  : 
//=======================================================================
Handle(Geom_Surface) BRepOffset::CollapseSingularities (const Handle(Geom_Surface)& theSurface,
                                                        const TopoDS_Face& theFace,
                                                        Standard_Real thePrecision)
{
  // check surface type to see if it can be processed
  Handle(Standard_Type) aType = theSurface->DynamicType();
  if (aType != STANDARD_TYPE(Geom_BSplineSurface))
  {
    // for the moment, only bspline surfaces are treated;
    // in the future, bezier surfaces and surfaces of revolution can be also handled
    return theSurface;
  }

  // find singularities (vertices of degenerated edges)
  NCollection_List<gp_Pnt> aDegenPnt;
  NCollection_List<Standard_Real> aDegenTol;
  for (TopExp_Explorer anExp (theFace, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    TopoDS_Edge anEdge = TopoDS::Edge (anExp.Current());
    if (! BRep_Tool::Degenerated (anEdge))
    {
      continue;
    }
    TopoDS_Vertex aV1, aV2;
    TopExp::Vertices (anEdge, aV1, aV2);
    if (! aV1.IsSame (aV2))
    {
      continue;
    }

    aDegenPnt.Append (BRep_Tool::Pnt (aV1));
    aDegenTol.Append (BRep_Tool::Tolerance (aV1));
  }

  // iterate by sides of the surface
  if (aType == STANDARD_TYPE(Geom_BSplineSurface))
  {
    Handle(Geom_BSplineSurface) aBSpline = Handle(Geom_BSplineSurface)::DownCast (theSurface);
    const TColgp_Array2OfPnt& aPoles = aBSpline->Poles();
    
    Handle(Geom_BSplineSurface) aCopy;

    // iterate by sides: {U=0; V=0; U=1; V=1}
    Standard_Integer RowStart[4] = {aPoles.LowerRow(), aPoles.LowerRow(), aPoles.UpperRow(), aPoles.LowerRow()};
    Standard_Integer ColStart[4] = {aPoles.LowerCol(), aPoles.LowerCol(), aPoles.LowerCol(), aPoles.UpperCol()};
    Standard_Integer RowStep[4]  = {0, 1, 0, 1};
    Standard_Integer ColStep[4]  = {1, 0, 1, 0};
    Standard_Integer NbSteps[4]  = {aPoles.RowLength(), aPoles.ColLength(), aPoles.RowLength(), aPoles.ColLength()};
    for (Standard_Integer iSide = 0; iSide < 4; iSide++)
    {
      // compute center of gravity of side poles
      gp_XYZ aSum;
      for (int iPole = 0; iPole < NbSteps[iSide]; iPole++)
      {
        aSum += aPoles (RowStart[iSide] + iPole * RowStep[iSide], ColStart[iSide] + iPole * ColStep[iSide]).XYZ();
      }
      gp_Pnt aCenter (aSum / NbSteps[iSide]);

      // determine if all poles of the side fit into:
      Standard_Boolean isCollapsed = Standard_True; // aCenter precisely (with gp::Resolution())
      Standard_Boolean isSingular = Standard_True;  // aCenter with thePrecision
      NCollection_LocalArray<Standard_Boolean,4> isDegenerated (aDegenPnt.Extent()); // degenerated vertex
      for (size_t iDegen = 0; iDegen < isDegenerated.Size(); ++iDegen) isDegenerated[iDegen] = Standard_True;
      for (int iPole = 0; iPole < NbSteps[iSide]; iPole++)
      {
        const gp_Pnt& aPole = aPoles (RowStart[iSide] + iPole * RowStep[iSide], ColStart[iSide] + iPole * ColStep[iSide]);

        // distance from CG
        Standard_Real aDistCG = aCenter.Distance (aPole);
        if (aDistCG > gp::Resolution())
          isCollapsed = Standard_False;
        if (aDistCG > thePrecision)
          isSingular = Standard_False;

        // distances from degenerated points
        NCollection_List<gp_Pnt>::Iterator aDegPntIt (aDegenPnt);
        NCollection_List<Standard_Real>::Iterator aDegTolIt(aDegenTol);
        for (size_t iDegen = 0; iDegen < isDegenerated.Size(); aDegPntIt.Next(), aDegTolIt.Next(), ++iDegen)
        {
          if (isDegenerated[iDegen] && aDegPntIt.Value().Distance (aPole) >= aDegTolIt.Value())
          {
            isDegenerated[iDegen] = Standard_False;
          }
        }
      }
      if (isCollapsed)
      {
        continue; // already Ok, nothing to be done
      }

      // decide to collapse the side: either if it is singular with thePrecision,
      // or if it fits into one (and only one) degenerated point
      if (! isSingular)
      {
        Standard_Integer aNbFit = 0;
        NCollection_List<gp_Pnt>::Iterator aDegPntIt (aDegenPnt);
        NCollection_List<Standard_Real>::Iterator aDegTolIt(aDegenTol);
        for (size_t iDegen = 0; iDegen < isDegenerated.Size(); ++iDegen)
        {
          if (isDegenerated[iDegen])
          {
            // remove degenerated point as soon as it fits at least one side, to prevent total collapse
            aDegenPnt.Remove (aDegPntIt);
            aDegenTol.Remove (aDegTolIt);
            aNbFit++;
          }
          else
          {
            aDegPntIt.Next();
            aDegTolIt.Next();
          }
        }

        // if side fits more than one degenerated vertex, do not collapse it
        // to be on the safe side
        isSingular = (aNbFit == 1);
      }
      
      // do collapse
      if (isSingular)
      {
        if (aCopy.IsNull())
        {
          aCopy = Handle(Geom_BSplineSurface)::DownCast (theSurface->Copy());
        }
        for (int iPole = 0; iPole < NbSteps[iSide]; iPole++)
        {
          aCopy->SetPole (RowStart[iSide] + iPole * RowStep[iSide], ColStart[iSide] + iPole * ColStep[iSide], aCenter);
        }
      }
    }

    if (! aCopy.IsNull())
      return aCopy;
  }

  return theSurface;
}
