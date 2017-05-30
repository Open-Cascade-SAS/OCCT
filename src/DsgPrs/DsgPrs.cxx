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

#include <Aspect_TypeOfLine.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <DsgPrs.hxx>
#include <ElCLib.hxx>
#include <gce_MakeLin.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Surface.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <gp_Ax1.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Precision.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>
#include <Quantity_Color.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

void DsgPrs::ComputeSymbol (const Handle(Prs3d_Presentation)& aPresentation,
			    const Handle(Prs3d_DimensionAspect)& LA,
			    const gp_Pnt& pt1,
			    const gp_Pnt& pt2,
			    const gp_Dir& dir1,
			    const gp_Dir& dir2,
			    const DsgPrs_ArrowSide ArrowSide,
			    const Standard_Boolean drawFromCenter) 
{
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  Quantity_Color aColor = LA->LineAspect()->Aspect()->Color();
  Handle(Graphic3d_AspectMarker3d) aMarkerAsp = new Graphic3d_AspectMarker3d (Aspect_TOM_O, aColor, 1.0);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect (aMarkerAsp);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  switch(ArrowSide) {
  case DsgPrs_AS_NONE:
    {
      break;
    }
  case DsgPrs_AS_FIRSTAR:
    {
      Prs3d_Arrow::Draw(Prs3d_Root::CurrentGroup (aPresentation),
		    pt1,
		    dir1,
		    LA->ArrowAspect()->Angle(),
		    LA->ArrowAspect()->Length());  
      break;
    }
  case DsgPrs_AS_LASTAR:
    {

      Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation),
		    pt2,
		    dir2,
		    LA->ArrowAspect()->Angle(),
		    LA->ArrowAspect()->Length());  
      break;
    }

  case DsgPrs_AS_BOTHAR:
    {
      Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation),
		    pt1,
		    dir1,
		    LA->ArrowAspect()->Angle(),
		    LA->ArrowAspect()->Length());  
      Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation),
		    pt2,
		    dir2,
		    LA->ArrowAspect()->Angle(),
		    LA->ArrowAspect()->Length());  

      break;
    }

  case DsgPrs_AS_FIRSTPT:
    {
      if(drawFromCenter)
      {
        Handle(Graphic3d_ArrayOfPoints) anArrayOfPoints = new Graphic3d_ArrayOfPoints (1);
        anArrayOfPoints->AddVertex (pt1.X(), pt1.Y(), pt1.Z());
        Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray (anArrayOfPoints);
      }
      break;
    }

  case DsgPrs_AS_LASTPT:
    {
      // On dessine un rond 
      Handle(Graphic3d_ArrayOfPoints) anArrayOfPoints = new Graphic3d_ArrayOfPoints (1);
      anArrayOfPoints->AddVertex (pt2.X(), pt2.Y(), pt2.Z());
      Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray (anArrayOfPoints);
      break;
    }

  case DsgPrs_AS_BOTHPT:
    {
      if(drawFromCenter)
      {
        Handle(Graphic3d_ArrayOfPoints) anArrayOfPoints1 = new Graphic3d_ArrayOfPoints (1);
        anArrayOfPoints1->AddVertex (pt1.X(), pt1.Y(), pt1.Z());
        Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray (anArrayOfPoints1);
      }
      if(drawFromCenter)
      {
        Handle(Graphic3d_ArrayOfPoints) anArrayOfPoints2 = new Graphic3d_ArrayOfPoints (1);
        anArrayOfPoints2->AddVertex (pt2.X(), pt2.Y(), pt2.Z());
        Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray (anArrayOfPoints2);
      }
      break;
    }

  case DsgPrs_AS_FIRSTAR_LASTPT:
    {
      // an Arrow
      Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation),
                        pt1,
                        dir1,
                        LA->ArrowAspect()->Angle(),
                        LA->ArrowAspect()->Length());
      // a Round
      Handle(Graphic3d_ArrayOfPoints) anArrayOfPoints = new Graphic3d_ArrayOfPoints (1);
      anArrayOfPoints->AddVertex (pt2.X(), pt2.Y(), pt2.Z());
      Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray (anArrayOfPoints);
      break;
    }

  case DsgPrs_AS_FIRSTPT_LASTAR:
    {
      // a Round
      if(drawFromCenter)
      {
        Handle(Graphic3d_ArrayOfPoints) anArrayOfPoints = new Graphic3d_ArrayOfPoints (1);
        anArrayOfPoints->AddVertex (pt1.X(), pt1.Y(), pt1.Z());
        Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray (anArrayOfPoints);
      }
      // an Arrow
      Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation),
                        pt2,
                        dir2,
                        LA->ArrowAspect()->Angle(),
                        LA->ArrowAspect()->Length());
      break;
    }
  }
}


//=======================================================================
//function : ComputePlanarFacesLengthPresentation
//purpose  : 
//=======================================================================

void DsgPrs::ComputePlanarFacesLengthPresentation( const Standard_Real FirstArrowLength,
						   const Standard_Real SecondArrowLength,
						   const gp_Pnt& AttachmentPoint1,
						   const gp_Pnt& AttachmentPoint2,
						   const gp_Dir& DirAttach,
						   const gp_Pnt& OffsetPoint,
						   const gp_Pln& PlaneOfFaces,
						   gp_Pnt &        EndOfArrow1,
						   gp_Pnt &        EndOfArrow2,
						   gp_Dir &        DirOfArrow1 )
{
  gp_Lin FirstLin( AttachmentPoint1, DirAttach );  
  gp_Lin SecondLin( AttachmentPoint2, DirAttach );  
  
  EndOfArrow1 = ElCLib::Value( ElCLib::Parameter( FirstLin, OffsetPoint ), FirstLin );
  EndOfArrow2 = ElCLib::Value( ElCLib::Parameter( SecondLin, OffsetPoint ), SecondLin );
 
  if (EndOfArrow1.SquareDistance( EndOfArrow2 ) > Precision::SquareConfusion()) // not null length
    {
      gp_Dir LengthDir( gp_Vec( EndOfArrow1, EndOfArrow2 ) );
      if ((FirstArrowLength + SecondArrowLength)*(FirstArrowLength + SecondArrowLength) < 
	  EndOfArrow1.SquareDistance( EndOfArrow2 ))
	DirOfArrow1 = -LengthDir;
      else
	DirOfArrow1 = LengthDir;
    }
  else // null length
    DirOfArrow1 = PlaneOfFaces.Axis().Direction();
}

//=======================================================================
//function : ComputeCurvilinearFacesLengthPresentation
//purpose  : 
//=======================================================================

void DsgPrs::ComputeCurvilinearFacesLengthPresentation( const Standard_Real FirstArrowLength,
							const Standard_Real SecondArrowLength,
							const Handle( Geom_Surface )& SecondSurf,
							const gp_Pnt& AttachmentPoint1,
							const gp_Pnt& AttachmentPoint2,
							const gp_Dir& DirAttach,
							gp_Pnt &        EndOfArrow2,
							gp_Dir &        DirOfArrow1,
							Handle( Geom_Curve )& VCurve,
							Handle( Geom_Curve )& UCurve,
							Standard_Real & FirstU,
							Standard_Real & deltaU,
							Standard_Real & FirstV,
							Standard_Real & deltaV )
{
  GeomAPI_ProjectPointOnSurf ProjectorOnSurface;
  GeomAPI_ProjectPointOnCurve ProjectorOnCurve;
  Standard_Real U1, V1, U2, V2;
  Standard_Real LastU, LastV;
  Standard_Real SquareTolerance = Precision::SquareConfusion();

  ProjectorOnSurface.Init( AttachmentPoint1, SecondSurf );
  Standard_Integer Index(1);
  Standard_Real MinDist = RealLast();
  Standard_Real LocalU, LocalV;
  gp_Vec D1U, D1V;
  gp_Dir LocalDir;
  for (Standard_Integer i = 1; i <= ProjectorOnSurface.NbPoints(); i++)
    {
      ProjectorOnSurface.Parameters( i, LocalU, LocalV );

      SecondSurf->D1( LocalU, LocalV, EndOfArrow2, D1U, D1V );
      if (D1U.SquareMagnitude() <= SquareTolerance || D1V.SquareMagnitude() <= SquareTolerance)
	LocalDir = gp_Dir( gp_Vec( AttachmentPoint1, ProjectorOnSurface.Point( i ) ) );
      else
	LocalDir = gp_Dir( D1U ^ D1V );
      if (DirAttach.IsParallel( LocalDir, Precision::Angular() ) && ProjectorOnSurface.Distance( i ) < MinDist)
	{
	  Index = i;
	  MinDist = ProjectorOnSurface.Distance( i );
	}
    }
  EndOfArrow2 = ProjectorOnSurface.Point( Index );
  ProjectorOnSurface.Parameters( Index, U1, V1 );
  
  if ((FirstArrowLength + SecondArrowLength)*(FirstArrowLength + SecondArrowLength) <
      AttachmentPoint1.SquareDistance( EndOfArrow2 ))
    DirOfArrow1 = -DirAttach;
  else
    DirOfArrow1 = DirAttach;

  if (EndOfArrow2.SquareDistance( AttachmentPoint2 ) > Precision::SquareConfusion())
    {
      VCurve = SecondSurf->VIso( V1 );
      ProjectorOnCurve.Init( EndOfArrow2, VCurve );
      FirstU = ProjectorOnCurve.LowerDistanceParameter();

      ProjectorOnSurface.Init( AttachmentPoint2, SecondSurf );
      ProjectorOnSurface.LowerDistanceParameters( U2, V2 );
      UCurve = SecondSurf->UIso( U2 );
      
      ProjectorOnCurve.Init( AttachmentPoint2, UCurve );
      LastV = ProjectorOnCurve.LowerDistanceParameter();

      gp_Pnt Intersection = SecondSurf->Value( U2, V1 );
      ProjectorOnCurve.Init( Intersection, VCurve );
      LastU = ProjectorOnCurve.LowerDistanceParameter();
      ProjectorOnCurve.Init( Intersection, UCurve );
      FirstV = ProjectorOnCurve.LowerDistanceParameter();

      deltaU = LastU - FirstU;
      deltaV = LastV - FirstV;

      if (VCurve->IsPeriodic() && Abs( deltaU ) > VCurve->Period()/2)
	{
	  Standard_Real Sign = (deltaU > 0.0)? -1.0 : 1.0;
	  deltaU = VCurve->Period() - Abs( deltaU );
	  deltaU *= Sign;
	}
      if (UCurve->IsPeriodic() && Abs( deltaV ) > UCurve->Period()/2)
	{
	  Standard_Real Sign = (deltaV > 0.0)? -1.0 : 1.0;
	  deltaV = UCurve->Period() - Abs( deltaV );
	  deltaV *= Sign;
	}
    }
}


//=======================================================================
//function : ComputeFacesAnglePresentation
//purpose  : 
//=======================================================================

void DsgPrs::ComputeFacesAnglePresentation( const Standard_Real ArrowLength,
					    const Standard_Real Value,
					    const gp_Pnt& CenterPoint,
					    const gp_Pnt& AttachmentPoint1,
					    const gp_Pnt& AttachmentPoint2,
					    const gp_Dir& dir1,
					    const gp_Dir& dir2,
					    const gp_Dir& axisdir,
					    const Standard_Boolean isPlane,
					    const gp_Ax1& AxisOfSurf,
					    const gp_Pnt& OffsetPoint,
					    gp_Circ &       AngleCirc,
					    Standard_Real & FirstParAngleCirc,
					    Standard_Real & LastParAngleCirc,
					    gp_Pnt &        EndOfArrow1,
					    gp_Pnt &        EndOfArrow2,
					    gp_Dir &        DirOfArrow1,
					    gp_Dir &        DirOfArrow2,
					    gp_Pnt &        ProjAttachPoint2,
					    gp_Circ &       AttachCirc,
					    Standard_Real & FirstParAttachCirc,
					    Standard_Real & LastParAttachCirc )
{
  if (Value > Precision::Angular() && Abs( M_PI-Value ) > Precision::Angular())
    {
      // Computing presentation of angle's arc
      gp_Ax2 ax( CenterPoint, axisdir, dir1 );
      AngleCirc.SetPosition( ax );
      AngleCirc.SetRadius( CenterPoint.Distance( OffsetPoint ) );
      gp_Vec vec1( dir1 );
      vec1 *= AngleCirc.Radius();
      gp_Pnt p1 = CenterPoint.Translated( vec1 );
      gp_Vec vec2( dir2 );
      vec2 *= AngleCirc.Radius();
      gp_Pnt p2 = CenterPoint.Translated( vec2 );
      
      Standard_Real Par1 = 0.;
      Standard_Real Par2 = ElCLib::Parameter( AngleCirc, p2 );
      Standard_Real Par0 = ElCLib::Parameter( AngleCirc, OffsetPoint );
      
      gp_Vec PosVec( CenterPoint, OffsetPoint );
      gp_Vec NormalOfPlane = vec1 ^ vec2;
      
      gp_Vec Normal1 = NormalOfPlane ^ vec1;
      gp_Vec Normal2 = NormalOfPlane ^ vec2;
      Standard_Integer Sign1 = (PosVec * Normal1 >= 0)? 1 : -1;
      Standard_Integer Sign2 = (PosVec * Normal2 >= 0)? 1 : -1;
      if (Sign1 == 1 && Sign2 == -1)
	{
	  FirstParAngleCirc = Par1;
	  LastParAngleCirc  = Par2;
	}
      else if (Sign1 == 1 && Sign2 == 1)
	{ 
	  FirstParAngleCirc = Par1;
	  LastParAngleCirc  = Par0;
	}
      else if (Sign1 == -1 && Sign2 == 1)
	{
	  Par1 += M_PI;
	  Par2 += M_PI;
	  FirstParAngleCirc = Par1;
	  LastParAngleCirc  = Par2;
	}
      else //Sign1 == -1 && Sign2 == -1
	{
	  AngleCirc.SetPosition( gp_Ax2( CenterPoint, axisdir, gp_Dir( PosVec ) ) );
	  Par0 = 0.;
	  Par1 = ElCLib::Parameter( AngleCirc, p1 );
	  Par2 = ElCLib::Parameter( AngleCirc, p2 );
	  FirstParAngleCirc = Par0;
	  LastParAngleCirc  = Par2;
	}

      // Computing presentation of arrows
      EndOfArrow1 = ElCLib::Value( Par1, AngleCirc );
      EndOfArrow2  = ElCLib::Value( Par2, AngleCirc );
      Standard_Real beta = 0.;
      if (AngleCirc.Radius() > Precision::Confusion())
	beta = ArrowLength / AngleCirc.Radius();
      gp_Pnt OriginOfArrow1 = ElCLib::Value( Par1 + beta, AngleCirc );
      gp_Pnt OriginOfArrow2 = ElCLib::Value( Par2 - beta, AngleCirc );
      DirOfArrow1 = gp_Dir( gp_Vec( OriginOfArrow1, EndOfArrow1 ) );
      DirOfArrow2 = gp_Dir( gp_Vec( OriginOfArrow2, EndOfArrow2 ) );
      if (EndOfArrow1.SquareDistance( EndOfArrow2 ) <= (ArrowLength + ArrowLength)*(ArrowLength + ArrowLength))
	{
	  DirOfArrow1.Reverse();
	  DirOfArrow2.Reverse();
	}
    }
  else // dir1 and dir2 are parallel
    {
      gp_Dir ArrowDir = axisdir ^ dir1;
      DirOfArrow1 = ArrowDir;
      DirOfArrow2 = -ArrowDir;
      gp_Lin DirLine( AttachmentPoint1, dir1 );
      EndOfArrow1 = ElCLib::Value( ElCLib::Parameter( DirLine, OffsetPoint ), DirLine );
      EndOfArrow2 = EndOfArrow1;
    }

  // Line or arc from AttachmentPoint2 to its "projection"
  gp_Lin SecondLin( CenterPoint, dir2 );
  if (SecondLin.Contains( AttachmentPoint2, Precision::Confusion() ))
    ProjAttachPoint2 = AttachmentPoint2;
  else
    {
      if (isPlane)
	ProjAttachPoint2 = ElCLib::Value( ElCLib::Parameter( SecondLin, AttachmentPoint2 ), SecondLin );
      else
	{
	  gp_Lin LineOfAxis( AxisOfSurf );
	  gp_Pnt CenterOfArc = ElCLib::Value( ElCLib::Parameter( LineOfAxis, AttachmentPoint2 ),
					      LineOfAxis );
	  
	  gp_Ax2 Ax2( CenterOfArc,
		      AxisOfSurf.Direction(),
		      gp_Dir( gp_Vec( CenterOfArc, AttachmentPoint2 ) ) );
	  AttachCirc.SetPosition( Ax2 );
	  AttachCirc.SetRadius( CenterOfArc.Distance( AttachmentPoint2 ) );
	  
	  GeomAPI_ExtremaCurveCurve Intersection( new Geom_Circle( AttachCirc ),
						  new Geom_Line( SecondLin ) );
	  Intersection.NearestPoints( ProjAttachPoint2, ProjAttachPoint2 );
	  
	  Standard_Real U2 = ElCLib::Parameter( AttachCirc, ProjAttachPoint2 );
	  if (U2 <= M_PI)
	    {
	      FirstParAttachCirc = 0;
	      LastParAttachCirc  = U2;
	    }
	  else
	    {
	      FirstParAttachCirc = U2;
	      LastParAttachCirc  = 2*M_PI;
	    }
	}
    }
}



void DsgPrs::ComputeFilletRadiusPresentation( const Standard_Real /*ArrowLength*/,
					      const Standard_Real Value,
					      const gp_Pnt &      Position,
					      const gp_Dir &      NormalDir,
					      const gp_Pnt &      FirstPoint,
					      const gp_Pnt &      SecondPoint,
					      const gp_Pnt &      Center,
					      const gp_Pnt &      BasePnt,
					      const Standard_Boolean drawRevers,
					      Standard_Boolean &  SpecCase,
					      gp_Circ &           FilletCirc,
					      Standard_Real &     FirstParCirc,
					      Standard_Real &     LastParCirc,
					      gp_Pnt &            EndOfArrow,
					      gp_Dir &            DirOfArrow,
					      gp_Pnt &            DrawPosition)
{
  gp_Dir dir1(gp_Vec(Center, FirstPoint));
  gp_Dir dir2(gp_Vec(Center, SecondPoint));
  Standard_Real Angle = dir1.Angle(dir2);
  if(Angle <= Precision::Angular() || ( M_PI - Angle ) <= Precision::Angular() ||
     Value <= Precision::Confusion()) SpecCase = Standard_True;
  else SpecCase = Standard_False;
  if ( !SpecCase )
    {
       // Computing presentation of fillet's arc
      gp_Ax2 ax( Center, NormalDir, dir1 );
      FilletCirc.SetPosition( ax );
      FilletCirc.SetRadius( Center.Distance( FirstPoint ) ); //***
      gp_Vec vec1( dir1 );
      vec1 *= FilletCirc.Radius();
      gp_Vec vec2( dir2 );
      vec2 *= FilletCirc.Radius();
      gp_Vec PosVec;
      if(! Center.IsEqual( Position, Precision::Confusion() ))
	PosVec.SetXYZ( gp_Vec(Center, Position).XYZ() );
      else
	PosVec.SetXYZ( (vec1.Added(vec2)).XYZ() );
      gp_Vec NormalOfPlane = vec1 ^ vec2;      
      gp_Vec Normal1 = NormalOfPlane ^ vec1;
      gp_Vec Normal2 = NormalOfPlane ^ vec2;
      Standard_Integer Sign1 = (PosVec * Normal1 >= 0)? 1 : -1;
      Standard_Integer Sign2 = (PosVec * Normal2 >= 0)? 1 : -1;
      gp_Lin L1( Center, dir1 );
      gp_Lin L2( Center, dir2 );
      if ( Sign1 != Sign2 )
	{
	  DrawPosition = Position; //***
	  gp_Dir direction(PosVec) ;
	  Standard_Real angle = dir1.Angle(direction) ;
          if (( dir1 ^ direction) * NormalDir < 0.0e0)   angle = -angle ;
	  if(Sign1 == -1) angle += M_PI;
	  EndOfArrow = ElCLib::Value(angle, FilletCirc); //***
	  	   
	}
      else
	{
	  if(L1.Distance(Position) < L2.Distance(Position))
	      {
		EndOfArrow = FirstPoint; //***
		DrawPosition =	ElCLib::Value(ElCLib::Parameter(L1, Position), L1);	
	      }
	  else
	    {
	      EndOfArrow = SecondPoint; //***
	      DrawPosition = ElCLib::Value(ElCLib::Parameter(L2, Position), L2);
	    }
	}
      if((dir1^dir2).IsOpposite(NormalDir, Precision::Angular()))
	{
	  gp_Dir newdir = NormalDir.Reversed() ;
	  gp_Ax2 axnew( Center, newdir, dir1 );
	  FilletCirc.SetPosition( axnew );
	}
      FirstParCirc = ElCLib::Parameter( FilletCirc, FirstPoint );
      LastParCirc  = ElCLib::Parameter( FilletCirc, SecondPoint );
    }
  else //Angle equal 0 or PI or R = 0
    {
      DrawPosition = Position;
      EndOfArrow   = BasePnt;
    }

  if(drawRevers)
    {
      gp_Vec Vd(DrawPosition, EndOfArrow);
      DrawPosition.Translate(Vd *2);
    }
  DirOfArrow.SetXYZ(gp_Dir(gp_Vec(DrawPosition, EndOfArrow)).XYZ());      
}

//=======================================================================
//function : ComputeRadiusLine
//purpose  : 
//=======================================================================

void DsgPrs::ComputeRadiusLine(const gp_Pnt & aCenter,
			       const gp_Pnt & anEndOfArrow,
			       const gp_Pnt & aPosition,
			       const Standard_Boolean drawFromCenter,
			             gp_Pnt & aRadLineOrign,
			             gp_Pnt & aRadLineEnd)
{
  if(drawFromCenter)
    {
      gp_Lin RadiusLine = gce_MakeLin( aCenter, anEndOfArrow );
      Standard_Real PosParOnLine = ElCLib::Parameter( RadiusLine, aPosition );
      Standard_Real EndOfArrowPar     = ElCLib::Parameter( RadiusLine, anEndOfArrow );
      if (PosParOnLine < 0.0)
	{
	  aRadLineOrign = aPosition;
	  aRadLineEnd   = anEndOfArrow;
	}
      else if (PosParOnLine > EndOfArrowPar)
	{
	  aRadLineOrign = aPosition;
	  aRadLineEnd   = aCenter;
	}
      else
	{
	  aRadLineOrign = aCenter;
	  aRadLineEnd   = anEndOfArrow;
	}
    }
  else
    {
      aRadLineOrign = aPosition;
      aRadLineEnd   = anEndOfArrow;
    }
}


//=======================================================================
//function : DistanceFromApex
//purpose  : 
//=======================================================================

Standard_Real DsgPrs::DistanceFromApex(const gp_Elips & elips,
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
	  if(par <= parApex + M_PI && par > parApex )
	    dist = par - parApex;
	  else 
	    { 
	      if(par >  parApex + M_PI)
		dist = 2*M_PI - par + parApex;
	      else
		dist = parApex - par; // 0 < par < M_PI/2
	    }
	}
      else //neg Apex == 3/2 PI
	{
	  if(par <= parApex && par >= M_PI/2) 
	    dist = parApex - par;
	  else
	    {
	      if(par >  parApex) 
		dist = par - parApex;
	      else
		dist = par + M_PI/2; // 0 < par < PI/2
	    }
	}
    }
  return dist;
}
