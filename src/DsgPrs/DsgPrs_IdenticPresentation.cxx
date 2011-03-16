// File:	DsgPrs_IdenticPresentation.cxx
// Created:	Fri Jan  3 18:05:10 1997
// Author:      Flore Lantheaume
//		<fla@chariox.paris1.matra-dtv.fr>


#include <DsgPrs_IdenticPresentation.ixx>

#include <Graphic3d_Group.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>

#include <Prs3d_LineAspect.hxx>
#include <Prs3d_LengthAspect.hxx>
#include <Prs3d_Text.hxx>

#include <TCollection_AsciiString.hxx>

#include <gp_Vec.hxx>
#include <gp_Dir.hxx>

#include <ElCLib.hxx>

#include <Precision.hxx>
#include <gp_Elips.hxx>

void DsgPrs_IdenticPresentation::Add( const Handle(Prs3d_Presentation)& aPresentation,
				      const Handle(Prs3d_Drawer)& aDrawer,
				      const TCollection_ExtendedString& aText,
				      const gp_Pnt& aPntAttach,
				      const gp_Pnt& aPntOffset)
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  Graphic3d_Array1OfVertex V(1,2);
  V(1).SetCoord(aPntAttach.X(), aPntAttach.Y(), aPntAttach.Z());
  V(2).SetCoord(aPntOffset.X(), aPntOffset.Y(), aPntOffset.Z());

  // trait de cote 
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

  // On ajoute un rond au point d'attache
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Handle(Graphic3d_AspectMarker3d) MarkerAsp = new Graphic3d_AspectMarker3d();
  MarkerAsp->SetType(Aspect_TOM_BALL);
  MarkerAsp->SetScale(0.8);
  Quantity_Color acolor;
  Aspect_TypeOfLine atype;
  Standard_Real awidth;
  LA->LineAspect()->Aspect()->Values(acolor, atype, awidth);
  MarkerAsp->SetColor(acolor);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(MarkerAsp);
  Graphic3d_Vertex V3d(aPntAttach.X(), aPntAttach.Y(), aPntAttach.Z());
  Prs3d_Root::CurrentGroup(aPresentation)->Marker(V3d);

  // texte 
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,aPntOffset);
}



void DsgPrs_IdenticPresentation::Add( const Handle(Prs3d_Presentation)& aPresentation,
				      const Handle(Prs3d_Drawer)& aDrawer,
				      const TCollection_ExtendedString& aText,
				      const gp_Pnt& aFAttach,
				      const gp_Pnt& aSAttach,
				      const gp_Pnt& aPntOffset)
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  
  Graphic3d_Array1OfVertex V(1,2);
  V(1).SetCoord(aFAttach.X(), aFAttach.Y(), aFAttach.Z());
  V(2).SetCoord(aSAttach.X(), aSAttach.Y(), aSAttach.Z());

  // trait de cote 
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

  // trait joignant aPntOffset
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  gp_Vec v1(aFAttach, aSAttach);
  gp_Vec v2(aSAttach, aPntOffset);
  V(1).SetCoord(aPntOffset.X(), aPntOffset.Y(), aPntOffset.Z());
  if ( !v1.IsParallel(v2, Precision::Angular())) {
    // on joint aPntOffset a son projete
    gp_Lin ll(aFAttach, gp_Dir(v1));
    gp_Pnt ProjPntOffset = ElCLib::Value(ElCLib::Parameter(ll,aPntOffset ), ll);
    V(2).SetCoord(ProjPntOffset.X(), ProjPntOffset.Y(), ProjPntOffset.Z());
  }
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V); 

  // texte 
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,aPntOffset);
}



void DsgPrs_IdenticPresentation::Add(const Handle(Prs3d_Presentation)& aPresentation,
				     const Handle(Prs3d_Drawer)& aDrawer,
				     const TCollection_ExtendedString& aText,
				     const gp_Ax2& theAxe,
				     const gp_Pnt& aCenter,
				     const gp_Pnt& aFAttach,
				     const gp_Pnt& aSAttach,
				     const gp_Pnt& aPntOffset)
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  gp_Ax2 ax = theAxe;
  ax.SetLocation(aCenter);
  Standard_Real rad = aCenter.Distance(aFAttach);
  gp_Circ CC(ax,rad );
  Standard_Real pFAttach =  ElCLib::Parameter(CC, aFAttach);
  Standard_Real pSAttach =  ElCLib::Parameter(CC, aSAttach);
  Standard_Real alpha = pSAttach - pFAttach;
  if ( alpha < 0 ) alpha += 2*Standard_PI;
  Standard_Integer nb = (Standard_Integer )( 50. * alpha / PI);
  Standard_Integer nbp = Max (4 , nb);
  Graphic3d_Array1OfVertex V(1,nbp);
  Standard_Real dteta = alpha/(nbp-1);
  gp_Pnt ptcur;
  for (Standard_Integer i = 1; i<=nbp; i++)
    {
      ptcur =  ElCLib::Value(pFAttach + dteta*(i-1),CC);
      V(i).SetCoord(ptcur.X(),ptcur.Y(),ptcur.Z());
    }

  // trait de cote 
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
  
  // trait joignant aPntOffset
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Graphic3d_Array1OfVertex V2(1,2);
  if ( Abs((aPntOffset.Distance(aCenter) - rad )) >= Precision::Confusion() ) {
    gp_Pnt ProjPntOffset = ElCLib::Value(ElCLib::Parameter(CC,aPntOffset ), CC);
    
    V2(1).SetCoord(aPntOffset.X(), aPntOffset.Y(), aPntOffset.Z());
    V2(2).SetCoord(ProjPntOffset.X(), ProjPntOffset.Y(), ProjPntOffset.Z());
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V2); 
  }

  // texte 
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,aPntOffset);
}

// jfa 16/10/2000
void DsgPrs_IdenticPresentation::Add(const Handle(Prs3d_Presentation)& aPresentation,
				     const Handle(Prs3d_Drawer)& aDrawer,
				     const TCollection_ExtendedString& aText,
				     const gp_Ax2& theAxe,
				     const gp_Pnt& aCenter,
				     const gp_Pnt& aFAttach,
				     const gp_Pnt& aSAttach,
				     const gp_Pnt& aPntOffset,
				     const gp_Pnt& aPntOnCirc)
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  gp_Ax2 ax = theAxe;
  ax.SetLocation(aCenter);
  Standard_Real rad = aCenter.Distance(aFAttach);
  gp_Circ CC(ax,rad );
  Standard_Real pFAttach =  ElCLib::Parameter(CC, aFAttach);
  Standard_Real pSAttach =  ElCLib::Parameter(CC, aSAttach);
  Standard_Real alpha = pSAttach - pFAttach;
  if ( alpha < 0 ) alpha += 2*Standard_PI;
  Standard_Integer nb = (Standard_Integer )( 50. * alpha / PI);
  Standard_Integer nbp = Max (4 , nb);
  Graphic3d_Array1OfVertex V(1,nbp);
  Standard_Real dteta = alpha/(nbp-1);
  gp_Pnt ptcur;
  for (Standard_Integer i = 1; i<=nbp; i++)
    {
      ptcur =  ElCLib::Value(pFAttach + dteta*(i-1),CC);
      V(i).SetCoord(ptcur.X(),ptcur.Y(),ptcur.Z());
    }

  // trait de cote 
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
  
  // trait joignant aPntOffset
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Graphic3d_Array1OfVertex V2(1,2);
  if ( aPntOffset.Distance(aPntOnCirc) >= Precision::Confusion() )
    {
      V2(1).SetCoord(aPntOffset.X(), aPntOffset.Y(), aPntOffset.Z());
      V2(2).SetCoord(aPntOnCirc.X(), aPntOnCirc.Y(), aPntOnCirc.Z());
      Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V2); 
    }

  // texte 
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,aPntOffset);
}
// jfa 16/10/2000 end

// jfa 10/10/2000
void DsgPrs_IdenticPresentation::Add(const Handle(Prs3d_Presentation)& aPresentation,
				     const Handle(Prs3d_Drawer)& aDrawer,
				     const TCollection_ExtendedString& aText,
				     const gp_Elips& anEllipse,
				     const gp_Pnt& aFAttach,
				     const gp_Pnt& aSAttach,
				     const gp_Pnt& aPntOffset,
				     const gp_Pnt& aPntOnElli)
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  Standard_Real pFAttach =  ElCLib::Parameter(anEllipse, aFAttach);
  Standard_Real pSAttach =  ElCLib::Parameter(anEllipse, aSAttach);
  Standard_Real alpha = pSAttach - pFAttach;
  if ( alpha < 0 ) alpha += 2*Standard_PI;
  Standard_Integer nb = (Standard_Integer)(50.0*alpha/PI);
  Standard_Integer nbp = Max (4 , nb);
  Graphic3d_Array1OfVertex V(1,nbp);
  Standard_Real dteta = alpha/(nbp-1);
  gp_Pnt ptcur;
  for (Standard_Integer i = 1; i<=nbp; i++)
    {
      ptcur =  ElCLib::Value(pFAttach + dteta*(i-1),anEllipse);
      V(i).SetCoord(ptcur.X(),ptcur.Y(),ptcur.Z());
    }

  // trait de cote 
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
  
  // trait joignant aPntOffset
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Graphic3d_Array1OfVertex V2(1,2);

  if ( ! aPntOnElli.IsEqual(aPntOffset, Precision::Confusion()) )
    {
      V2(1).SetCoord(aPntOffset.X(), aPntOffset.Y(), aPntOffset.Z());
      V2(2).SetCoord(aPntOnElli.X(), aPntOnElli.Y(), aPntOnElli.Z());
      Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V2); 
    }

  // texte 
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,aPntOffset);
}
// jfa 10/10/2000 end
