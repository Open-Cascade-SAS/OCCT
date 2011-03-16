// File:	StdPrs_ShadedPoleSurface.cxx
// Created:	Tue Aug  1 08:38:50 1995
// Author:	Modelistation
//		<model@metrox>

#define G005    //ATS,GG 04/01/01 Use PrimitiveArray instead Sets of primitives
//              for performance improvment

#include <StdPrs_ShadedPoleSurface.ixx>

#include <Graphic3d_Group.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_Array2OfVertex.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <gp_Pnt.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>

#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#ifdef G005
#include <Graphic3d_ArrayOfQuadrangleStrips.hxx>
#endif

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void StdPrs_ShadedPoleSurface::Add(
			const Handle(Prs3d_Presentation)& aPresentation,
			const Adaptor3d_Surface&            aSurface,
			const Handle (Prs3d_Drawer)&      aDrawer)
{
  if ( ! aDrawer->ShadingAspectGlobal() ) {

// Si la surface est fermee, on ne tracera pas les faces tournant le dos:


    Handle(Graphic3d_AspectFillArea3d) Asp = aDrawer->ShadingAspect()->Aspect();
    if(aSurface.IsUClosed() && aSurface.IsVClosed()) {
      Asp->SuppressBackFace();
    } else {
      Asp->AllowBackFace();
    }
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(Asp);
  }

  Standard_Integer i,j,n = aSurface.NbUPoles(), m = aSurface.NbVPoles();

  if (aSurface.GetType() == GeomAbs_BezierSurface) {
    Handle(Geom_BezierSurface) surface = aSurface.Bezier();
#ifdef G005
    if( Graphic3d_ArrayOfPrimitives::IsEnable() ) {
      Handle(Graphic3d_ArrayOfQuadrangleStrips) parray =
		new Graphic3d_ArrayOfQuadrangleStrips(n*m,n,
		Standard_False,Standard_False,Standard_False,Standard_False);
      for ( i=1; i<=n; i++) {
	parray->AddBound(m);
        for ( j=1; j<=m; j++) {
	  parray->AddVertex(surface->Pole(i,j));
        }
      }
      Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(parray);
      return;
    }
#endif    
    TColgp_Array2OfPnt A(1,n,1,m);
    surface->Poles(A);
    Graphic3d_Array2OfVertex V(1,n,1,m);
    Standard_Real x,y,z;
    for ( i=1; i<=n; i++) {
      for ( j=1; j<=m; j++) {
	A(i,j).Coord(x,y,z);
	V(i,j).SetCoord(x,y,z);
      }
    }
    Prs3d_Root::CurrentGroup(aPresentation)->QuadrangleMesh(V);
  }
  else if (aSurface.GetType() == GeomAbs_BSplineSurface) {
      Handle(Geom_BSplineSurface) surface = aSurface.BSpline();
#ifdef G005
    if( Graphic3d_ArrayOfPrimitives::IsEnable() ) {
      Handle(Graphic3d_ArrayOfQuadrangleStrips) parray =
		new Graphic3d_ArrayOfQuadrangleStrips(n*m,n,
		Standard_False,Standard_False,Standard_False,Standard_False);
      for ( i=1; i<=n; i++) {
	parray->AddBound(m);
        for ( j=1; j<=m; j++) {
	  parray->AddVertex(surface->Pole(i,j));
        }
      }
      Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(parray);
      return;
    }
#endif    
    TColgp_Array2OfPnt A(1,n,1,m);
    surface->Poles(A);
    Graphic3d_Array2OfVertex V(1,n,1,m);
    Standard_Real x,y,z;
    for ( i=1; i<=n; i++) {
      for ( j=1; j<=m; j++) {
	A(i,j).Coord(x,y,z);
	V(i,j).SetCoord(x,y,z);
      }
    }
    Prs3d_Root::CurrentGroup(aPresentation)->QuadrangleMesh(V);
  }
}
