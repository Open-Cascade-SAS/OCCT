// File:	DsgPrs_EllipseRadiusPresentation.cxx
// Created:	Mon Jan 26 09:59:22 1998
// Author:	Sergey ZARITCHNY
//		<szy@androx.nnov.matra-dtv.fr>


#include <DsgPrs_EllipseRadiusPresentation.ixx>

#include <gp_Lin.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <ElCLib.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Prs3d_LengthAspect.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Text.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Aspect_AspectMarker.hxx>
#include <Quantity_Color.hxx>
#include <DsgPrs.hxx>
#include <Precision.hxx>

#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <Geom_OffsetCurve.hxx>
//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void DsgPrs_EllipseRadiusPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
					    const Handle(Prs3d_Drawer)& aDrawer,
					    const Standard_Real theval,
					    const TCollection_ExtendedString & aText,
//					    const gp_Elips & anEllipse,
					    const gp_Pnt & aPosition,
					    const gp_Pnt & anEndOfArrow,
					    const gp_Pnt & aCenter,
					    const Standard_Boolean IsMaxRadius,
					    const DsgPrs_ArrowSide ArrowPrs)
{

  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
 
  Standard_Boolean inside  = Standard_False;
//  gp_Pnt EndPoint, EndOfArrow;
  gp_Pnt EndPoint;
  Standard_Real dist    = aCenter.Distance( aPosition );
  if( dist > theval ) EndPoint = aPosition;
  else {
    EndPoint = anEndOfArrow;
    inside   = Standard_True;
  }
  Graphic3d_Array1OfVertex V(1,2);
  Quantity_Length X,Y,Z;
  aCenter.Coord(X,Y,Z);
  V(1).SetCoord(X,Y,Z);
  EndPoint.Coord(X,Y,Z);
  V(2).SetCoord(X,Y,Z);
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
  
   // value
  TCollection_ExtendedString Text;
  if(IsMaxRadius)
    Text = TCollection_ExtendedString("a = ");
  else
    Text = TCollection_ExtendedString("b = ");
  Text +=  aText;
  Prs3d_Text::Draw(aPresentation, LA->TextAspect(), Text, aPosition );

   // arrows
  gp_Dir arrdir( gp_Vec( aCenter, anEndOfArrow));
  if (!inside) arrdir.Reverse();

  DsgPrs::ComputeSymbol(aPresentation, LA,  anEndOfArrow,  anEndOfArrow, arrdir, arrdir, ArrowPrs );

}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void DsgPrs_EllipseRadiusPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
					    const Handle(Prs3d_Drawer)& aDrawer,
					    const Standard_Real theval,
					    const TCollection_ExtendedString & aText,
					    const gp_Elips & anEllipse,
					    const gp_Pnt & aPosition,
					    const gp_Pnt & anEndOfArrow,
					    const gp_Pnt & aCenter,
					    const Standard_Real uFirst,
					    const Standard_Boolean IsInDomain,
					    const Standard_Boolean IsMaxRadius,
					    const DsgPrs_ArrowSide ArrowPrs)
{

  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  if(!IsInDomain)
    {
      Standard_Real parFirst;
      Standard_Real uLast = ElCLib::Parameter ( anEllipse, anEndOfArrow );
      Standard_Real Alpha = DsgPrs::DistanceFromApex(anEllipse, anEndOfArrow, uFirst);//length of ellipse arc
      gp_Vec Vapex(aCenter, ElCLib::Value( uLast, anEllipse )) ;
      gp_Vec Vpnt(aCenter,  ElCLib::Value( uFirst, anEllipse )) ;
      gp_Dir dir(Vpnt ^ Vapex);
      if(anEllipse.Position().Direction().IsOpposite( dir, Precision::Angular()))
	parFirst = uLast;
      else
	parFirst = uFirst;
      Standard_Integer NodeNumber = Max (4 , Standard_Integer (50. * Alpha / PI));
      Graphic3d_Array1OfVertex ApproxArc( 0, NodeNumber-1 );
      Standard_Real delta = Alpha / ( NodeNumber - 1 );
      gp_Pnt CurPnt;
      for (Standard_Integer i = 0 ; i < NodeNumber; i++)
	{
	  CurPnt =  ElCLib::Value( parFirst, anEllipse );
	  ApproxArc(i).SetCoord( CurPnt.X(), CurPnt.Y(), CurPnt.Z() );
	  parFirst += delta ;
	}
      Prs3d_Root::CurrentGroup( aPresentation )->Polyline( ApproxArc );
    }
  DsgPrs_EllipseRadiusPresentation::Add(aPresentation, aDrawer, theval, aText,
					aPosition, anEndOfArrow, aCenter,  IsMaxRadius, ArrowPrs);
  
}



//=======================================================================
//function : Add
//purpose  : // for offset curve
//=======================================================================

void DsgPrs_EllipseRadiusPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
					    const Handle(Prs3d_Drawer)& aDrawer,
					    const Standard_Real theval,
					    const TCollection_ExtendedString & aText,
					    const Handle(Geom_OffsetCurve) & aCurve,
					    const gp_Pnt & aPosition,
					    const gp_Pnt & anEndOfArrow,
					    const gp_Pnt & aCenter,
					    const Standard_Real uFirst,
					    const Standard_Boolean IsInDomain,
					    const Standard_Boolean IsMaxRadius,
					    const DsgPrs_ArrowSide ArrowPrs)
{

  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  if(!IsInDomain)
    {
      Standard_Real parFirst;
      if(!aCurve->IsCN(1)) return ;
      gp_Elips aBEllipse = Handle(Geom_Ellipse)::DownCast(aCurve->BasisCurve ())->Elips();
      Standard_Real Offset = aCurve->Offset();
      aBEllipse.SetMajorRadius(aBEllipse.MajorRadius() + Offset);
      aBEllipse.SetMinorRadius(aBEllipse.MinorRadius() + Offset);
      Standard_Real uLast = ElCLib::Parameter ( aBEllipse, anEndOfArrow );
      Standard_Real Alpha = DsgPrs::DistanceFromApex(aBEllipse, anEndOfArrow, uFirst);//length of ellipse arc
      gp_Pnt p1;
      aCurve->D0(uFirst, p1);
      gp_Vec Vapex(aCenter,  anEndOfArrow) ;
      gp_Vec Vpnt (aCenter,   p1) ;
      gp_Dir dir(Vpnt ^ Vapex);
      if(aCurve->Direction().IsOpposite( dir, Precision::Angular()))
	parFirst = uLast;
      else
	parFirst = uFirst;
      Standard_Integer NodeNumber = Max (4 , Standard_Integer (50. * Alpha / PI));
      Graphic3d_Array1OfVertex ApproxArc( 0, NodeNumber-1 );
      Standard_Real delta = Alpha / ( NodeNumber - 1 );
      gp_Pnt CurPnt;
      for (Standard_Integer i = 0 ; i < NodeNumber; i++)
	{
	  aCurve->D0( parFirst, CurPnt );
	  ApproxArc(i).SetCoord( CurPnt.X(), CurPnt.Y(), CurPnt.Z() );
	  parFirst += delta ;
	}
      Prs3d_Root::CurrentGroup( aPresentation )->Polyline( ApproxArc );
    }
  DsgPrs_EllipseRadiusPresentation::Add(aPresentation, aDrawer, theval, aText,
					aPosition, anEndOfArrow, aCenter,  IsMaxRadius, ArrowPrs);

}
