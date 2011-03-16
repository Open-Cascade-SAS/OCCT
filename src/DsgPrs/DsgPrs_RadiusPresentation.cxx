// File:	DsgPrs_RadiusPresentation.cxx
// Created:	Wed Mar  1 15:50:43 1995
// Author:	Arnaud BOUZY
//		<adn>
//modified      20-feb-98 by <SZY>
//              Sergei Zaritchny

#include <DsgPrs_RadiusPresentation.ixx>
#include <gp_Lin.hxx>
#include <gp_Dir.hxx>
#include <gp_Circ.hxx>
#include <ElCLib.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_LengthAspect.hxx>
#include <Prs3d_Text.hxx>

#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Aspect_AspectMarker.hxx>
#include <Quantity_Color.hxx>
#include <DsgPrs.hxx>
#include <Precision.hxx>
#include <gce_MakeLin.hxx>
#include <gce_MakeDir.hxx>


static Standard_Boolean DsgPrs_InDomain(const Standard_Real fpar,
					const Standard_Real lpar,
					const Standard_Real para) 
{
  if (fpar >= 0.) {
    return ((para >= fpar) && (para <= lpar));
  }
  if (para >= (fpar+2*PI)) return Standard_True;
  if (para <= lpar) return Standard_True;
  return Standard_False;
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void DsgPrs_RadiusPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
				     const Handle(Prs3d_Drawer)& aDrawer,
				     const TCollection_ExtendedString& aText,
				     const gp_Pnt& AttachmentPoint,
				     const gp_Circ& aCircle,
				     const Standard_Real firstparam,
				     const Standard_Real lastparam,
				     const Standard_Boolean drawFromCenter,
				     const Standard_Boolean reverseArrow) 
{
  Standard_Real fpara = firstparam;
  Standard_Real lpara = lastparam;
  while (lpara > 2*PI) {
    fpara -= 2*PI;
    lpara -= 2*PI;
  }
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Standard_Real parat = ElCLib::Parameter(aCircle,AttachmentPoint);
  gp_Pnt attpoint = AttachmentPoint;
  Standard_Boolean otherside = Standard_False;
  if ( !DsgPrs_InDomain(fpara,lpara,parat)) {
    Standard_Real otherpar = parat + PI;
    if (otherpar > 2*PI) otherpar -= 2*PI;
    if (DsgPrs_InDomain(fpara,lpara,otherpar)) {
      parat = otherpar;
      otherside = Standard_True;
    }
    else {
      Standard_Real ecartpar = Min(Abs(fpara-parat),
				   Abs(lpara-parat));
      Standard_Real ecartoth = Min(Abs(fpara-otherpar),
				   Abs(lpara-otherpar));
      if (ecartpar <= ecartoth) {
	if (parat < fpara) {
	  parat = fpara;
	}
	else {
	  parat = lpara;
	}
      }
      else {
	otherside = Standard_True;
	if (otherpar < fpara) {
	  parat = fpara;
	}
	else {
	  parat = lpara;
	}
      }
      gp_Pnt ptdir = ElCLib::Value(parat,aCircle);
      gp_Lin lsup(aCircle.Location(),
		  gp_Dir(ptdir.XYZ()-aCircle.Location().XYZ()));
      Standard_Real parpos = ElCLib::Parameter(lsup,AttachmentPoint);
      attpoint = ElCLib::Value(parpos,lsup);
    }
  }
  gp_Pnt ptoncirc = ElCLib::Value(parat,aCircle);
  gp_Lin L (aCircle.Location(),gp_Dir(attpoint.XYZ()-aCircle.Location().XYZ()));
  gp_Pnt firstpoint = attpoint;
  gp_Pnt drawtopoint = ptoncirc;
  if (drawFromCenter && !otherside) {
    Standard_Real uatt = ElCLib::Parameter(L,attpoint);
    Standard_Real uptc = ElCLib::Parameter(L,ptoncirc);
    if (Abs(uatt) > Abs(uptc)) {
      drawtopoint = aCircle.Location();
    }
    else {
      firstpoint = aCircle.Location();
    }
  }

  Graphic3d_Array1OfVertex V(1,2);

  Quantity_Length X,Y,Z;

  firstpoint.Coord(X,Y,Z);
  V(1).SetCoord(X,Y,Z);

  drawtopoint.Coord(X,Y,Z);
  V(2).SetCoord(X,Y,Z);

  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

  gp_Dir arrdir = L.Direction();
  if (reverseArrow) {
    arrdir.Reverse();
  }
  // fleche
  Prs3d_Arrow::Draw(aPresentation,ptoncirc,arrdir,
		    LA->Arrow1Aspect()->Angle(),
		    LA->Arrow1Aspect()->Length());

  // texte
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,attpoint);
  
}

//=======================================================================
//function : DsgPrs_RadiusPresentation::Add
//purpose  : SZY 20-february-98 
//         : adds radius representation according drawFromCenter value
//=======================================================================

void DsgPrs_RadiusPresentation::Add( const Handle(Prs3d_Presentation)& aPresentation,
				     const Handle(Prs3d_Drawer)& aDrawer,
				     const TCollection_ExtendedString& aText,
				     const gp_Pnt& AttachmentPoint,
				     const gp_Pnt& Center,
				     const gp_Pnt& EndOfArrow,
				     const DsgPrs_ArrowSide ArrowPrs,
				     const Standard_Boolean drawFromCenter,
				     const Standard_Boolean reverseArrow)
{
  Handle( Prs3d_LengthAspect ) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup( aPresentation )->SetPrimitivesAspect( LA->LineAspect()->Aspect() );

  Graphic3d_Array1OfVertex VertexArray( 1, 2 );
  gp_Pnt  LineOrigin, LineEnd;
  Quantity_Length X,Y,Z;

  DsgPrs::ComputeRadiusLine( Center, EndOfArrow, AttachmentPoint, drawFromCenter,
			    LineOrigin, LineEnd);
//
  LineOrigin.Coord( X, Y, Z );
  VertexArray(1).SetCoord( X, Y, Z );

  LineEnd.Coord( X, Y, Z );
  VertexArray(2).SetCoord( X, Y, Z );

  Prs3d_Root::CurrentGroup( aPresentation )->Polyline( VertexArray );
  // text
  Prs3d_Text::Draw( aPresentation, LA->TextAspect(), aText, AttachmentPoint );

  gp_Dir ArrowDir = gce_MakeDir( LineOrigin , LineEnd );
  if (reverseArrow)
    ArrowDir.Reverse();
  DsgPrs::ComputeSymbol( aPresentation, LA, Center, EndOfArrow, ArrowDir.Reversed(), ArrowDir,
			ArrowPrs, drawFromCenter );
}
  
