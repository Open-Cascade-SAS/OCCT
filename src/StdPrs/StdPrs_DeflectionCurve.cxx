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

// Great zoom leads to non-coincidence of
// a point and non-infinite lines passing throught this point:

#include <Adaptor3d_Curve.hxx>
#include <Bnd_Box.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_Group.hxx>
#include <Precision.hxx>
#include <Prs3d.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <StdPrs_DeflectionCurve.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>

//==================================================================
// function: GetDeflection
// purpose:
//==================================================================
static Standard_Real GetDeflection(const Adaptor3d_Curve&        aCurve,
                                   const Standard_Real         U1, 
                                   const Standard_Real         U2, 
                                   const Handle(Prs3d_Drawer)& aDrawer)
{
  Standard_Real TheDeflection;

  if (aDrawer->TypeOfDeflection() == Aspect_TOD_RELATIVE)
  {
    // On calcule la fleche en fonction des min max globaux de la piece:
    Bnd_Box Total;
    BndLib_Add3dCurve::Add(aCurve, U1, U2, 0.,Total);
    Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    Total.Get( aXmin, aYmin, aZmin, aXmax, aYmax, aZmax );
    Standard_Real m = RealLast();
    if ( ! (Total.IsOpenXmin() || Total.IsOpenXmax() ))
      m = Abs (aXmax-aXmin);
    if ( ! (Total.IsOpenYmin() || Total.IsOpenYmax() ))
      m = Max ( m , Abs (aYmax-aYmin));
    if ( ! (Total.IsOpenZmin() || Total.IsOpenZmax() ))
      m = Max ( m , Abs (aZmax-aZmin));
      
    m = Min ( m , aDrawer->MaximalParameterValue());
    m = Max(m, Precision::Confusion());
      
    TheDeflection = m * aDrawer->DeviationCoefficient();
  }
  else
    TheDeflection = aDrawer->MaximalChordialDeviation();

  return TheDeflection;
}

//==================================================================
// function: FindLimits
// purpose:
//==================================================================
static Standard_Boolean FindLimits(const Adaptor3d_Curve& aCurve,
                                   const Standard_Real  aLimit,
                                   Standard_Real&       First,
                                   Standard_Real&       Last)
{
  First = aCurve.FirstParameter();
  Last  = aCurve.LastParameter();
  Standard_Boolean firstInf = Precision::IsNegativeInfinite(First);
  Standard_Boolean lastInf  = Precision::IsPositiveInfinite(Last);

  if (firstInf || lastInf) {
    gp_Pnt P1,P2;
    Standard_Real delta = 1;
    Standard_Integer count = 0;
    if (firstInf && lastInf) {
      do {
        if (count++ == 100000) return Standard_False;
          delta *= 2;
          First = - delta;
          Last  =   delta;
          aCurve.D0(First,P1);
          aCurve.D0(Last,P2);
      } while (P1.Distance(P2) < aLimit);
    }
    else if (firstInf) {
      aCurve.D0(Last,P2);
      do {
        if (count++ == 100000) return Standard_False;
          delta *= 2;
          First = Last - delta;
          aCurve.D0(First,P1);
      } while (P1.Distance(P2) < aLimit);
    }
    else if (lastInf) {
      aCurve.D0(First,P1);
      do {
        if (count++ == 100000) return Standard_False;
          delta *= 2;
          Last = First + delta;
          aCurve.D0(Last,P2);
      } while (P1.Distance(P2) < aLimit);
    }
  }    
  return Standard_True;
}


//==================================================================
// function: drawCurve
// purpose:
//==================================================================
static void drawCurve (Adaptor3d_Curve&              aCurve,
                       const Handle(Graphic3d_Group)& aGroup,
                       const Standard_Real           TheDeflection,
                       const Standard_Real           anAngle,
                       const Standard_Real           U1,
                       const Standard_Real           U2,
                       TColgp_SequenceOfPnt&         Points)
{
  switch (aCurve.GetType())
  {
    case GeomAbs_Line:
    {
      gp_Pnt p1 = aCurve.Value(U1);
      gp_Pnt p2 = aCurve.Value(U2);
      Points.Append(p1);
      Points.Append(p2);
      if (!aGroup.IsNull())
      {
        Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
        aPrims->AddVertex(p1);
        aPrims->AddVertex(p2);
        aGroup->AddPrimitiveArray(aPrims);
      }
      break;
    }
    default:
    {
      const Standard_Integer nbinter = aCurve.NbIntervals(GeomAbs_C1);
      TColStd_Array1OfReal T(1, nbinter+1);
      aCurve.Intervals(T, GeomAbs_C1);

      Standard_Real theU1, theU2;
      Standard_Integer NumberOfPoints, i, j;
      TColgp_SequenceOfPnt SeqP;

      for (j = 1; j <= nbinter; j++) {
        theU1 = T(j); theU2 = T(j+1);
        if (theU2 > U1 && theU1 < U2) {
          theU1 = Max(theU1, U1);
          theU2 = Min(theU2, U2);
	  
          GCPnts_TangentialDeflection Algo(aCurve, theU1, theU2, anAngle, TheDeflection);
          NumberOfPoints = Algo.NbPoints();

          if (NumberOfPoints > 0) {
            for (i = 1; i <= NumberOfPoints; i++)
              SeqP.Append(Algo.Value(i)); 
          }
        }
      }

      Handle(Graphic3d_ArrayOfPolylines) aPrims;
      if (!aGroup.IsNull())
        aPrims = new Graphic3d_ArrayOfPolylines(SeqP.Length());
      
      for (i = 1; i <= SeqP.Length(); i++) { 
        const gp_Pnt& p = SeqP.Value(i);
        Points.Append(p);
        if (!aGroup.IsNull())
        {
          aPrims->AddVertex(p);
        }
      }
      if (!aGroup.IsNull())
      {
        aGroup->AddPrimitiveArray (aPrims);
      }
    }
  }
}


//==================================================================
// function: MatchCurve
// purpose:
//==================================================================
static Standard_Boolean MatchCurve (
		       const Standard_Real X,
		       const Standard_Real Y,
		       const Standard_Real Z,
		       const Standard_Real aDistance,
		       const Adaptor3d_Curve&  aCurve,
		       const Standard_Real TheDeflection,
		       const Standard_Real   anAngle,
		       const Standard_Real   U1,
		       const Standard_Real   U2)
{
  Standard_Real retdist;
  switch (aCurve.GetType())
  {
    case GeomAbs_Line:
    {
      gp_Pnt p1 = aCurve.Value(U1);
      if ( Abs(X-p1.X()) + Abs(Y-p1.Y()) + Abs(Z-p1.Z()) <= aDistance)
        return Standard_True;
      gp_Pnt p2 = aCurve.Value(U2);
      if ( Abs(X-p2.X()) + Abs(Y-p2.Y()) + Abs(Z-p2.Z()) <= aDistance)
        return Standard_True;
      return Prs3d::MatchSegment(X,Y,Z,aDistance,p1,p2,retdist);
    }
    case GeomAbs_Circle:
    {
      const Standard_Real Radius = aCurve.Circle().Radius();
      if (!Precision::IsInfinite(Radius)) {
        const Standard_Real DU = Sqrt(8.0 * TheDeflection / Radius);
        const Standard_Real Er = Abs( U2 - U1) / DU;
        const Standard_Integer N = Max(2, (Standard_Integer)IntegerPart(Er));
        if ( N > 0) {
          gp_Pnt p1,p2;
          for (Standard_Integer Index = 1; Index <= N+1; Index++) {
            p2 = aCurve.Value(U1 + (Index - 1) * DU);
            if ( Abs(X-p2.X()) + Abs(Y-p2.Y()) + Abs(Z-p2.Z()) <= aDistance)
              return Standard_True;

            if (Index>1) { 
              if (Prs3d::MatchSegment(X,Y,Z,aDistance,p1,p2,retdist)) 
                return Standard_True;
            }
            p1=p2;
          }
        }
      }
      break;
    }
    default:
    {
      GCPnts_TangentialDeflection Algo(aCurve,U1, U2, anAngle, TheDeflection);
      const Standard_Integer NumberOfPoints = Algo.NbPoints();
      if (NumberOfPoints > 0) {
        gp_Pnt p1,p2;
        for (Standard_Integer i=1;i<=NumberOfPoints;i++) { 
          p2 = Algo.Value(i);
          if ( Abs(X-p2.X()) + Abs(Y-p2.Y()) + Abs(Z-p2.Z()) <= aDistance)
            return Standard_True;
          if (i>1) { 
            if (Prs3d::MatchSegment(X,Y,Z,aDistance,p1,p2,retdist)) 
              return Standard_True;
          }
          p1=p2;
        }
      }
    }
  }
  return Standard_False;
}


//==================================================================
// function: Add
// purpose:
//==================================================================
void StdPrs_DeflectionCurve::Add (const Handle (Prs3d_Presentation)& aPresentation,
                                  Adaptor3d_Curve&                   aCurve,
                                  const Handle (Prs3d_Drawer)&       aDrawer,
                                  const Standard_Boolean theToDrawCurve)
{
  Handle(Graphic3d_Group) aGroup;
  if (theToDrawCurve)
  {
    aGroup = Prs3d_Root::CurrentGroup(aPresentation);
    aGroup->SetPrimitivesAspect (aDrawer->LineAspect()->Aspect());
  }

  Standard_Real V1, V2;
  if (FindLimits(aCurve, aDrawer->MaximalParameterValue(), V1, V2))
  {
    TColgp_SequenceOfPnt Points;
    drawCurve(aCurve,
              aGroup,
              GetDeflection(aCurve, V1, V2, aDrawer),
              aDrawer->DeviationAngle(),
              V1, V2, Points);

    if (aDrawer->LineArrowDraw()
    && !aGroup.IsNull())
    {
      gp_Pnt Location;
      gp_Vec Direction;
      aCurve.D1(V2, Location,Direction);
      Prs3d_Arrow::Draw (aGroup,
                         Location,
                         gp_Dir(Direction),
                         aDrawer->ArrowAspect()->Angle(),
                         aDrawer->ArrowAspect()->Length());
    }
  }
}


//==================================================================
// function: Add
// purpose:
//==================================================================
void StdPrs_DeflectionCurve::Add (const Handle (Prs3d_Presentation)& aPresentation,
                                  Adaptor3d_Curve&                   aCurve,
                                  const Standard_Real                U1,
                                  const Standard_Real                U2,
                                  const Handle (Prs3d_Drawer)&       aDrawer,
                                  const Standard_Boolean theToDrawCurve)
{
  Handle(Graphic3d_Group) aGroup;
  if (theToDrawCurve)
  {
    aGroup = Prs3d_Root::CurrentGroup (aPresentation);
    aGroup->SetPrimitivesAspect(aDrawer->LineAspect()->Aspect());
  }

  Standard_Real V1 = U1;
  Standard_Real V2 = U2;  

  if (Precision::IsNegativeInfinite(V1)) V1 = -aDrawer->MaximalParameterValue();
  if (Precision::IsPositiveInfinite(V2)) V2 = aDrawer->MaximalParameterValue();

  TColgp_SequenceOfPnt Points;
  drawCurve(aCurve,
            aGroup,
            GetDeflection(aCurve, V1, V2, aDrawer),
            aDrawer->DeviationAngle(),
            V1 , V2, Points);

  if (aDrawer->LineArrowDraw()
  && !aGroup.IsNull())
  {
    gp_Pnt Location;
    gp_Vec Direction;
    aCurve.D1(V2, Location,Direction);
    Prs3d_Arrow::Draw (aGroup,
                       Location,
                       gp_Dir(Direction),
                       aDrawer->ArrowAspect()->Angle(),
                       aDrawer->ArrowAspect()->Length());
  }
}

//==================================================================
// function: Add
// purpose:
//==================================================================
void StdPrs_DeflectionCurve::Add (const Handle (Prs3d_Presentation)& aPresentation,
                                  Adaptor3d_Curve&                   aCurve,
                                  const Standard_Real                U1,
                                  const Standard_Real                U2,
                                  const Standard_Real                aDeflection,
                                  TColgp_SequenceOfPnt&              Points,
                                  const Standard_Real                anAngle,
                                  const Standard_Boolean theToDrawCurve)
{
  Handle(Graphic3d_Group) aGroup;
  if (theToDrawCurve)
  {
    aGroup = Prs3d_Root::CurrentGroup (aPresentation);
  }

  drawCurve (aCurve, aGroup, aDeflection, anAngle, U1, U2, Points);
}

//==================================================================
// function: Add
// purpose:
//==================================================================
void StdPrs_DeflectionCurve::Add (const Handle (Prs3d_Presentation)& aPresentation,
                                  Adaptor3d_Curve&                   aCurve,
                                  const Standard_Real                aDeflection,
                                  const Standard_Real                aLimit,
                                  const Standard_Real                anAngle,
                                  const Standard_Boolean theToDrawCurve)
{
  Standard_Real V1, V2;
  if (!FindLimits(aCurve, aLimit, V1, V2))
  {
    return;
  }

  Handle(Graphic3d_Group) aGroup;
  if (theToDrawCurve)
  {
    aGroup = Prs3d_Root::CurrentGroup (aPresentation);
  }

  TColgp_SequenceOfPnt Points;
  drawCurve (aCurve, aGroup, aDeflection, anAngle, V1, V2, Points);
}


//================================================================================
// function: Add
// purpose:
//================================================================================
void StdPrs_DeflectionCurve::Add (const Handle (Prs3d_Presentation)& aPresentation,
                                  Adaptor3d_Curve&                   aCurve,
                                  const Standard_Real                aDeflection,
                                  const Handle(Prs3d_Drawer)&        aDrawer,
                                  TColgp_SequenceOfPnt&              Points,
                                  const Standard_Boolean theToDrawCurve)
{
  Standard_Real V1, V2;
  if (!FindLimits(aCurve, aDrawer->MaximalParameterValue(), V1, V2))
  {
    return;
  }

  Handle(Graphic3d_Group) aGroup;
  if (theToDrawCurve)
  {
    aGroup = Prs3d_Root::CurrentGroup (aPresentation);
  }
  drawCurve (aCurve, aGroup, aDeflection, aDrawer->DeviationAngle(), V1, V2, Points);
}


//==================================================================
// function: Match
// purpose:
//==================================================================
Standard_Boolean StdPrs_DeflectionCurve::Match 
		      (const Standard_Real        X,
		       const Standard_Real        Y,
		       const Standard_Real        Z,
		       const Standard_Real        aDistance,
		       const Adaptor3d_Curve&         aCurve,
		       const Handle (Prs3d_Drawer)& aDrawer) 
{
  Standard_Real V1, V2;
  if (FindLimits(aCurve, aDrawer->MaximalParameterValue(), V1, V2))
  {
    return MatchCurve(X,Y,Z,aDistance,aCurve,
                      GetDeflection(aCurve, V1, V2, aDrawer),
                      aDrawer->DeviationAngle(),
                      V1, V2);
  }
  return Standard_False;
}

//==================================================================
// function: Match
// purpose:
//==================================================================
Standard_Boolean StdPrs_DeflectionCurve::Match 
			(const Standard_Real        X,
			 const Standard_Real        Y,
			 const Standard_Real        Z,
			 const Standard_Real        aDistance,
			 const Adaptor3d_Curve&         aCurve,
			 const Standard_Real          U1,
			 const Standard_Real          U2,
			 const Handle (Prs3d_Drawer)& aDrawer) 
{
  Standard_Real V1 = U1;
  Standard_Real V2 = U2;

  if (Precision::IsNegativeInfinite(V1)) V1 = -aDrawer->MaximalParameterValue();
  if (Precision::IsPositiveInfinite(V2)) V2 = aDrawer->MaximalParameterValue();

  return MatchCurve(X,Y,Z,aDistance,aCurve,
                    GetDeflection(aCurve, V1, V2, aDrawer),
                    aDrawer->DeviationAngle(), V1, V2);
}

//==================================================================
// function: Match
// purpose:
//==================================================================
Standard_Boolean StdPrs_DeflectionCurve::Match 
		        (const Standard_Real X,
                 const Standard_Real Y,
                 const Standard_Real Z,
                 const Standard_Real aDistance,
                 const Adaptor3d_Curve&  aCurve,
                 const Standard_Real   U1,
                 const Standard_Real   U2,
                 const Standard_Real   aDeflection,
                 const Standard_Real   anAngle)
{
  return MatchCurve(X,Y,Z,aDistance,aCurve,aDeflection,anAngle,U1,U2);
}

//==================================================================
// function: Match
// purpose:
//==================================================================
Standard_Boolean StdPrs_DeflectionCurve::Match 
			  (const Standard_Real X,
			   const Standard_Real Y,
			   const Standard_Real Z,
			   const Standard_Real aDistance,
			   const Adaptor3d_Curve&  aCurve,
			   const Standard_Real   aDeflection,
			   const Standard_Real   aLimit,
			   const Standard_Real   anAngle)
{
  Standard_Real V1, V2;
  if (FindLimits(aCurve, aLimit, V1, V2))
  {
    return MatchCurve(X,Y,Z,aDistance,aCurve,aDeflection,anAngle,V1,V2);
  }
  return Standard_False;
}
