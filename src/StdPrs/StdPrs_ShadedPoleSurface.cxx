// Created on: 1995-08-01
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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

// If the surface is closed, the faces from back-side are not traced:


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
