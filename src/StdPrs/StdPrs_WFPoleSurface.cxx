// File:	StdPrs_WFPoleSurface.cxx
// Created:	Mon Jul 24 16:52:35 1995
// Author:	Modelistation
//		<model@metrox>


#include <StdPrs_WFPoleSurface.ixx>
#include <Graphic3d_Group.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Graphic3d_Array1OfVertex.hxx>

#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>

static void AddPoles(const Handle (Prs3d_Presentation)& aPresentation,
		     const TColgp_Array2OfPnt&          A,
		     const Handle (Prs3d_Drawer)&       aDrawer)
{
  Standard_Integer i,j;
  Standard_Real x,y,z;
  Standard_Integer n = A.ColLength();
  Standard_Integer m = A.RowLength();
  
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(aDrawer->UIsoAspect()->Aspect());    
  
  Graphic3d_Array1OfVertex VertexArray1(1,m);
  
  for (i=1; i<=n; i++){
    for (j=1; j<=m; j++) {
      A(i,j).Coord(x,y,z);
      VertexArray1(j).SetCoord(x,y,z);
    }
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(VertexArray1);
  }
  
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(aDrawer->VIsoAspect()->Aspect());    
  
  Graphic3d_Array1OfVertex VertexArray2(1,n);
  for (j=1; j<=m; j++){
    for (i=1; i<=n; i++) {
      A(i,j).Coord(x,y,z);
      VertexArray2(i).SetCoord(x,y,z);
    }
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(VertexArray2);  
  }
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void StdPrs_WFPoleSurface::Add (const Handle (Prs3d_Presentation)& aPresentation,
				const Adaptor3d_Surface&             aSurface,
				const Handle (Prs3d_Drawer)&       aDrawer)
{

  GeomAbs_SurfaceType SType = aSurface.GetType();
  if (SType == GeomAbs_BezierSurface || SType == GeomAbs_BSplineSurface) {
    Standard_Integer n , m;
    if (SType == GeomAbs_BezierSurface) {
      Handle(Geom_BezierSurface) B = aSurface.Bezier();
      n = aSurface.NbUPoles();
      m = aSurface.NbVPoles();
      TColgp_Array2OfPnt A(1,n,1,m);
      (aSurface.Bezier())->Poles(A);
      AddPoles(aPresentation, A, aDrawer);
    }
    else if (SType == GeomAbs_BSplineSurface) {
      Handle(Geom_BSplineSurface) B = aSurface.BSpline();
      n = (aSurface.BSpline())->NbUPoles();
      m = (aSurface.BSpline())->NbVPoles();
      TColgp_Array2OfPnt A(1,n,1,m);
      (aSurface.BSpline())->Poles(A);
      AddPoles(aPresentation, A, aDrawer);
    }

  }
}

