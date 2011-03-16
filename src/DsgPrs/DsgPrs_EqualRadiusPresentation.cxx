// File:	DsgPrs_EqualRadiusPresentation.cxx
// Created:	Tue Jan 20 20:51:19 1998
// Author:	Julia GERASIMOVA
//		<jgv@orthodox.nnov.matra-dtv.fr>


#include <DsgPrs_EqualRadiusPresentation.ixx>

#include <DsgPrs.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_LengthAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Prs3d_Root.hxx>
#include <gp_Dir.hxx>
#include <gce_MakeDir.hxx>
#include <gp_Vec.hxx>
#include <gp_Pln.hxx>
#include <Precision.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Prs3d_Text.hxx>

void DsgPrs_EqualRadiusPresentation::Add( const Handle( Prs3d_Presentation )& aPresentation,
					  const Handle( Prs3d_Drawer )& aDrawer,
					  const gp_Pnt& FirstCenter,
					  const gp_Pnt& SecondCenter,
					  const gp_Pnt& FirstPoint,
					  const gp_Pnt& SecondPoint,
					  const Handle( Geom_Plane )& Plane )
{
  Handle( Prs3d_LengthAspect ) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup( aPresentation )->SetPrimitivesAspect( LA->LineAspect()->Aspect() );

  Graphic3d_Array1OfVertex VertexArray( 1, 2 );
  Quantity_Length X,Y,Z;

  // Radius lines
  FirstCenter.Coord( X, Y, Z );
  VertexArray( 1 ).SetCoord( X, Y, Z );
  FirstPoint.Coord( X, Y, Z );
  VertexArray( 2 ).SetCoord( X, Y, Z );
  Prs3d_Root::CurrentGroup( aPresentation )->Polyline( VertexArray );

  SecondCenter.Coord( X, Y, Z );
  VertexArray( 1 ).SetCoord( X, Y, Z );
  SecondPoint.Coord( X, Y, Z );
  VertexArray( 2 ).SetCoord( X, Y, Z );
  Prs3d_Root::CurrentGroup( aPresentation )->Polyline( VertexArray );

  // Add presentation of arrows
  gp_Dir FirstDir = gce_MakeDir( FirstCenter, FirstPoint ), SecondDir = gce_MakeDir( SecondCenter, SecondPoint );
  DsgPrs::ComputeSymbol( aPresentation, LA,
			 FirstCenter, FirstPoint,
			 FirstDir.Reversed(), FirstDir,
			 DsgPrs_AS_FIRSTPT_LASTAR );
  DsgPrs::ComputeSymbol( aPresentation, LA,
			 SecondCenter, SecondPoint,
			 SecondDir.Reversed(), SecondDir,
			 DsgPrs_AS_FIRSTPT_LASTAR );

  // Line between two centers
  FirstCenter.Coord( X, Y, Z );
  VertexArray( 2 ).SetCoord( X, Y, Z );
  Prs3d_Root::CurrentGroup( aPresentation )->Polyline( VertexArray );
  
//ota === beging ===

  gp_Pnt Middle( (FirstCenter.XYZ() + SecondCenter.XYZ()) *0.5 ), aTextPos;
  Standard_Real SmallDist;
  //Mark of constraint
  TCollection_ExtendedString aText("==");
  
  Standard_Real Dist = FirstCenter.Distance( SecondCenter );
  if (Dist > Precision::Confusion())
    {
      SmallDist = Dist * 0.05; // take 1/20 part of length;
      if (SmallDist <= Precision::Confusion())
	SmallDist = Dist;
      gp_Dir LineDir = gce_MakeDir( FirstCenter, SecondCenter );
      gp_Dir OrtDir  = Plane->Pln().Axis().Direction() ^ LineDir;

      gp_Vec OrtVec  = gp_Vec( OrtDir ) * SmallDist;
      
      //Compute the text position
      aTextPos = Middle.Translated(OrtVec);
    }
  else
    {
      Standard_Real Rad = Max(FirstCenter.Distance( FirstPoint ),
			      SecondCenter.Distance(SecondPoint));
      
      SmallDist = Rad *0.05; // take 1/20 part of length;
      if (SmallDist <= Precision::Confusion())
	SmallDist = Rad;

      gp_Vec aVec(SmallDist, SmallDist, SmallDist);
      
      //Compute the text position
      aTextPos = FirstCenter.Translated(aVec);
    }
  
  //Draw the text
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText, aTextPos);
//ota === end ===     
}
