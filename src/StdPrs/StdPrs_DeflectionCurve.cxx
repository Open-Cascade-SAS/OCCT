// Great zoom leads to non-coincidence of
// a point and non-infinite lines passing throught this point:
#define OCC64 

#include <StdPrs_DeflectionCurve.ixx>

#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <gp_Pnt.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <Prs3d.hxx>
#include <Bnd_Box.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <Precision.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>


//==================================================================
// function: GetDeflection
// purpose:
//==================================================================
static Standard_Real GetDeflection(const Adaptor3d_Curve&        aCurve,
				   const Standard_Real         U1, 
				   const Standard_Real         U2, 
				   const Handle(Prs3d_Drawer)& aDrawer) {
    Standard_Real TheDeflection;
    Aspect_TypeOfDeflection TOD = aDrawer->TypeOfDeflection();    
    if (TOD == Aspect_TOD_RELATIVE) {
// On calcule la fleche en fonction des min max globaux de la piece:
      Bnd_Box Total;
      BndLib_Add3dCurve::Add(aCurve, U1, U2, 0.,Total);
      Standard_Real m = RealFirst();
      Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
      Total.Get( aXmin, aYmin, aZmin, aXmax, aYmax, aZmax );
      m = RealLast();
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
static Standard_Boolean  FindLimits(const Adaptor3d_Curve& aCurve,
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
// function: DrawCurve
// purpose:
//==================================================================
static void DrawCurve (Adaptor3d_Curve&          aCurve,
                       const Handle(Graphic3d_Group) aGroup,
                       const Quantity_Length         TheDeflection,
		       const Standard_Real           anAngle,
                       const Standard_Real           U1,
                       const Standard_Real           U2,
		       TColgp_SequenceOfPnt&     Points,
		       const Standard_Boolean drawCurve)
{
  Standard_Boolean isPrimArrayEnabled = Graphic3d_ArrayOfPrimitives::IsEnable() && !drawCurve;

  switch (aCurve.GetType()) {
  case GeomAbs_Line:
    {
#ifdef OCC64
      Graphic3d_Array1OfVertex VertexArray(1, 3);
      gp_Pnt p = aCurve.Value(U1);
      Points.Append(p);
      VertexArray(1).SetCoord(p.X(), p.Y(), p.Z());
      p = aCurve.Value(0.5 * (U1 + U2));
      Points.Append(p);
      VertexArray(2).SetCoord(p.X(), p.Y(), p.Z());
      p = aCurve.Value(U2);
      Points.Append(p);
      VertexArray(3).SetCoord(p.X(), p.Y(), p.Z());
      if(!isPrimArrayEnabled)
	aGroup->Polyline(VertexArray);
#else
     static Graphic3d_Array1OfVertex VertexArray(1,2);
     gp_Pnt p = aCurve.Value(U1);
     Points.Append(p);
     VertexArray(1).SetCoord(p.X(), p.Y(), p.Z());
     p = aCurve.Value(U2);
     Points.Append(p);
     VertexArray(2).SetCoord(p.X(), p.Y(), p.Z());
     if(!isPrimArrayEnabled)
       aGroup->Polyline(VertexArray);
#endif
   }
    break;
  default:
    {
      Standard_Integer nbinter = aCurve.NbIntervals(GeomAbs_C1);
      Standard_Integer i, j;
      TColStd_Array1OfReal T(1, nbinter+1);
      aCurve.Intervals(T, GeomAbs_C1);
      Standard_Real theU1, theU2;
      Standard_Integer NumberOfPoints;
      TColgp_SequenceOfPnt SeqP;

      for (j = 1; j <= nbinter; j++) {
	theU1 = T(j); theU2 = T(j+1);
	if (theU2 > U1 && theU1 < U2) {
	  theU1 = Max(theU1, U1);
	  theU2 = Min(theU2, U2);
	  
	  GCPnts_TangentialDeflection Algo(aCurve, theU1, theU2, anAngle, TheDeflection);
	  NumberOfPoints = Algo.NbPoints();
	  
	  if (NumberOfPoints > 0) {
	    for (i=1;i<NumberOfPoints;i++) { 
	      SeqP.Append(Algo.Value(i)); 
	    }
	    if (j == nbinter) {
	      SeqP.Append(Algo.Value(NumberOfPoints)); 
	    }
	  }
	}
      }

      Graphic3d_Array1OfVertex VertexArray(1, SeqP.Length());
      
      Standard_Integer totalpoints = 1;
      for (i = 1; i <= SeqP.Length(); i++) { 
	const gp_Pnt& p = SeqP.Value(i);
	Points.Append(p);
	VertexArray(totalpoints++).SetCoord(p.X(), p.Y(), p.Z());
      }
      if(!isPrimArrayEnabled)
	aGroup->Polyline(VertexArray);
    }
  }
      
}


//==================================================================
// function: MatchCurve
// purpose:
//==================================================================
static Standard_Boolean MatchCurve (
		       const Quantity_Length X,
		       const Quantity_Length Y,
		       const Quantity_Length Z,
		       const Quantity_Length aDistance,
		       const Adaptor3d_Curve&  aCurve,
                       const Quantity_Length TheDeflection,
		       const Standard_Real   anAngle,
                       const Standard_Real   U1,
                       const Standard_Real   U2)
{
  Quantity_Length retdist;
  switch (aCurve.GetType()) {
  case GeomAbs_Line:
    {
     static Graphic3d_Array1OfVertex VertexArray(1,2);
     gp_Pnt p1 = aCurve.Value(U1);
     if ( Abs(X-p1.X()) + Abs(Y-p1.Y()) + Abs(Z-p1.Z()) <= aDistance)
       return Standard_True;
     gp_Pnt p2 = aCurve.Value(U2);
     if ( Abs(X-p2.X()) + Abs(Y-p2.Y()) + Abs(Z-p2.Z()) <= aDistance)
       return Standard_True;
     return Prs3d::MatchSegment(X,Y,Z,aDistance,p1,p2,retdist);
   }
    break;
  case GeomAbs_Circle:
    {
      Standard_Real Radius = aCurve.Circle().Radius();
      if (!Precision::IsInfinite(Radius)) {
	Standard_Real DU = Sqrt(8.0 * TheDeflection / Radius);
	Standard_Real Er = Abs( U2 - U1) / DU;
	Standard_Integer N = Max(2, (Standard_Integer)IntegerPart(Er));
	gp_Pnt p1,p2;
	if ( N > 0) {
	  Standard_Real U;
	  for (Standard_Integer Index = 1; Index <= N+1; Index++) {
	    U = U1 + (Index - 1) * DU;
	    p2 = aCurve.Value(U);
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
      return Standard_False;
   }
    break;
  default:
    {
      GCPnts_TangentialDeflection Algo(aCurve,U1, U2, anAngle, TheDeflection);
      gp_Pnt p1,p2;
      Standard_Integer NumberOfPoints = Algo.NbPoints();
      if (NumberOfPoints > 0) {
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
      return Standard_False;
    }
  }
  return Standard_False;
}


//==================================================================
// function: Add
// purpose:
//==================================================================
void StdPrs_DeflectionCurve::Add (const Handle (Prs3d_Presentation)& aPresentation,
				  Adaptor3d_Curve&                aCurve,
				 const Handle (Prs3d_Drawer)&        aDrawer,
				  const Standard_Boolean drawCurve) {

  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(aDrawer->LineAspect()->Aspect());

  Standard_Real V1, V2, angle = aDrawer->DeviationAngle();
  Standard_Boolean OK = FindLimits(aCurve, aDrawer->MaximalParameterValue(), V1, V2);

  TColgp_SequenceOfPnt Points;
  if (OK) {
    DrawCurve(aCurve,
	      Prs3d_Root::CurrentGroup(aPresentation),
	      GetDeflection(aCurve, V1, V2, aDrawer),
	      angle,
	      V1 , V2, Points, drawCurve);
    
    if (aDrawer->LineArrowDraw()) {
      gp_Pnt Location;
      gp_Vec Direction;
      aCurve.D1(V2, Location,Direction);
      Prs3d_Arrow::Draw (aPresentation,
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
				  Adaptor3d_Curve&               aCurve,
				 const Standard_Real                U1,
				 const Standard_Real                U2,
				 const Handle (Prs3d_Drawer)&       aDrawer,
				  const Standard_Boolean drawCurve) {

  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(aDrawer->LineAspect()->Aspect());

  Standard_Real V1 = U1;
  Standard_Real V2 = U2;  


  if (Precision::IsNegativeInfinite(V1)) V1 = -aDrawer->MaximalParameterValue();
  if (Precision::IsPositiveInfinite(V2)) V2 = aDrawer->MaximalParameterValue();

  Standard_Real angle = aDrawer->DeviationAngle();
  TColgp_SequenceOfPnt Points;
  DrawCurve(aCurve,
	    Prs3d_Root::CurrentGroup(aPresentation),
	    GetDeflection(aCurve, V1, V2, aDrawer),
	    angle,
	    V1 , V2, Points, drawCurve);

  if (aDrawer->LineArrowDraw()) {
    gp_Pnt Location;
    gp_Vec Direction;
    aCurve.D1(V2, Location,Direction);
    Prs3d_Arrow::Draw (aPresentation,
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
				  Adaptor3d_Curve&                aCurve,
				 const Standard_Real                 U1,
				 const Standard_Real                 U2,
				 const Standard_Real                 aDeflection,
				 TColgp_SequenceOfPnt&            Points,
				 const Standard_Real                 anAngle,
				  const Standard_Boolean drawCurve) 
{
  DrawCurve(aCurve, Prs3d_Root::CurrentGroup(aPresentation),
            aDeflection, anAngle, U1, U2, Points, drawCurve);
}


//==================================================================
// function: Add
// purpose:
//==================================================================
void StdPrs_DeflectionCurve::Add (const Handle (Prs3d_Presentation)& aPresentation,
				  Adaptor3d_Curve&                aCurve,
				 const Standard_Real                 aDeflection,
				 const Standard_Real                 aLimit,
				 const Standard_Real                 anAngle,
				  const Standard_Boolean drawCurve) 
{
  Standard_Real V1, V2;
  Standard_Boolean OK = FindLimits(aCurve, aLimit, V1, V2);

  TColgp_SequenceOfPnt Points;
  if (OK) DrawCurve(aCurve, Prs3d_Root::CurrentGroup(aPresentation),
		    aDeflection, anAngle, V1, V2, Points, drawCurve);
}


//================================================================================
// function: Add
// purpose:
//================================================================================
void StdPrs_DeflectionCurve::Add (const Handle (Prs3d_Presentation)& aPresentation,
				  Adaptor3d_Curve&                aCurve,
				  const Standard_Real             aDeflection,
				  const Handle(Prs3d_Drawer)&     aDrawer,
				  TColgp_SequenceOfPnt&           Points,
				  const Standard_Boolean drawCurve) 
{
  Standard_Real aLimit = aDrawer->MaximalParameterValue();
  Standard_Real V1, V2;
  Standard_Boolean OK = FindLimits(aCurve, aLimit, V1, V2);

  if (OK) DrawCurve(aCurve, Prs3d_Root::CurrentGroup(aPresentation),
		    aDeflection, aDrawer->DeviationAngle(), V1, V2, Points, drawCurve);
}


//==================================================================
// function: Match
// purpose:
//==================================================================
Standard_Boolean StdPrs_DeflectionCurve::Match 
		      (const Quantity_Length        X,
		       const Quantity_Length        Y,
		       const Quantity_Length        Z,
		       const Quantity_Length        aDistance,
		       const Adaptor3d_Curve&         aCurve,
		       const Handle (Prs3d_Drawer)& aDrawer) 
{
  Standard_Real V1, V2;
  Standard_Boolean OK = FindLimits(aCurve, aDrawer->MaximalParameterValue(), V1, V2);
  if (OK) {
    return MatchCurve(X,Y,Z,aDistance,aCurve,
		      GetDeflection(aCurve, V1, V2, aDrawer),
		      aDrawer->DeviationAngle(),
		      V1 , V2);
  }
  else {
    return Standard_False;
  }
}



//==================================================================
// function: Match
// purpose:
//==================================================================
Standard_Boolean StdPrs_DeflectionCurve::Match 
			(const Quantity_Length        X,
			 const Quantity_Length        Y,
			 const Quantity_Length        Z,
			 const Quantity_Length        aDistance,
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
		    aDrawer->DeviationAngle(), V1 , V2);
}


//==================================================================
// function: Match
// purpose:
//==================================================================
Standard_Boolean StdPrs_DeflectionCurve::Match 
		        (const Quantity_Length X,
			 const Quantity_Length Y,
			 const Quantity_Length Z,
			 const Quantity_Length aDistance,
			 const Adaptor3d_Curve&  aCurve,
			 const Standard_Real   U1,
			 const Standard_Real   U2,
			 const Standard_Real   aDeflection,
			 const Standard_Real   anAngle) {
  

  return MatchCurve(X,Y,Z,aDistance,aCurve,aDeflection,anAngle,U1,U2);
  
}

//==================================================================
// function: Match
// purpose:
//==================================================================
Standard_Boolean StdPrs_DeflectionCurve::Match 
			  (const Quantity_Length X,
			   const Quantity_Length Y,
			   const Quantity_Length Z,
			   const Quantity_Length aDistance,
			   const Adaptor3d_Curve&  aCurve,
			   const Standard_Real   aDeflection,
			   const Standard_Real   aLimit,
			   const Standard_Real   anAngle) {

  Standard_Real V1, V2;
  Standard_Boolean OK = FindLimits(aCurve, aLimit, V1, V2);

  if (OK) {
    return MatchCurve(X,Y,Z,aDistance,aCurve,
		      aDeflection,
		      anAngle, 
		      V1 , V2);
  }
  else {
    return Standard_False;
  }
  
}





