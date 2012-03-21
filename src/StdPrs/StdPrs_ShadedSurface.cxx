// Created on: 1995-07-27
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

#include <StdPrs_ShadedSurface.ixx>

#include <Graphic3d_Group.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_VertexN.hxx>
#include <Graphic3d_Array1OfVertexN.hxx>
#include <gp_Vec.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Precision.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <gp_Pnt.hxx>
#ifdef G005
#include <Graphic3d_ArrayOfTriangleStrips.hxx>
#endif

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void StdPrs_ShadedSurface::Add(const Handle(Prs3d_Presentation)& aPresentation,
			      const Adaptor3d_Surface&            aSurface,
			      const Handle(Prs3d_Drawer)&       aDrawer)
{
  Standard_Integer N1 = aDrawer->UIsoAspect()->Number();
  Standard_Integer N2 = aDrawer->VIsoAspect()->Number();
  
  N1 = N1 < 3 ? 3 : N1;
  N2 = N2 < 3 ? 3 : N2;
  
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

  Standard_Integer NBUintv = aSurface.NbUIntervals(GeomAbs_C1);
  Standard_Integer NBVintv = aSurface.NbVIntervals(GeomAbs_C1);
  TColStd_Array1OfReal InterU(1, NBUintv+1);
  TColStd_Array1OfReal InterV(1, NBVintv+1);
  
  aSurface.UIntervals(InterU, GeomAbs_C1);
  aSurface.VIntervals(InterV, GeomAbs_C1);
  
  Standard_Real  U1, U2, V1, V2, DU, DV;
  
  Standard_Integer i,j;
  
  gp_Pnt P1,P2;
  gp_Vec D1U,D1V,D1,D2;

#ifdef G005
  if( Graphic3d_ArrayOfPrimitives::IsEnable() ) {
    Prs3d_Root::CurrentGroup(aPresentation)->BeginPrimitives(); 
    for (Standard_Integer NU = 1; NU <= NBUintv; NU++) {
      for (Standard_Integer NV = 1; NV <= NBVintv; NV++) {
        U1 = InterU(NU); U2 = InterU(NU+1);
        V1 = InterV(NV); V2 = InterV(NV+1);

        U1 = (Precision::IsNegativeInfinite(U1)) ? - aDrawer->MaximalParameterValue() : U1;
        U2 = (Precision::IsPositiveInfinite(U2)) ?   aDrawer->MaximalParameterValue() : U2;
      
        V1 = (Precision::IsNegativeInfinite(V1)) ? - aDrawer->MaximalParameterValue() : V1;
        V2 = (Precision::IsPositiveInfinite(V2)) ?   aDrawer->MaximalParameterValue() : V2;
      
        DU = (U2-U1)/ N1;
        DV = (V2-V1)/ N2;
     
	Handle(Graphic3d_ArrayOfTriangleStrips) parray = new
		Graphic3d_ArrayOfTriangleStrips(2*(N1+1)*(N2+1),N1+1,
		Standard_True,Standard_False,Standard_False,Standard_False); 
        for ( i = 1; i<= N1+1; i++) {
	  parray->AddBound(N2+1);
	  for (j = 1; j <= N2+1; j++) {
	    aSurface.D1(U1 + DU * (i-1), V1 + DV * (j-1),P2,D1U,D1V);
	    D1 = D1U^D1V;
	    D1.Normalize();
	    aSurface.D1(U1 + DU * i, V1 + DV * (j-1),P2,D1U,D1V);
	    D2 = D1U^D1V;
	    D2.Normalize();
	    parray->AddVertex(P1,D1);
	    parray->AddVertex(P2,D2);
	  }
        }
	Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(parray);
      }
      Prs3d_Root::CurrentGroup(aPresentation)->EndPrimitives(); 
    }
    return;
  }
#endif 
  gp_Pnt P;
  gp_Vec Normale;
  Quantity_Length x,y,z;
  Graphic3d_Array1OfVertexN  A1 ( 1 , N2+1);
  Graphic3d_Array1OfVertexN  A2 ( 1 , N2+1);
  Graphic3d_Array1OfVertexN  TriangleStrip ( 1, 2*(N2+1));
  
  Prs3d_Root::CurrentGroup(aPresentation)->BeginPrimitives(); 
  for (Standard_Integer NU = 1; NU <= NBUintv; NU++) {
    for (Standard_Integer NV = 1; NV <= NBVintv; NV++) {
      U1 = InterU(NU); U2 = InterU(NU+1);
      V1 = InterV(NV); V2 = InterV(NV+1);

      U1 = (Precision::IsNegativeInfinite(U1)) ? - aDrawer->MaximalParameterValue() : U1;
      U2 = (Precision::IsPositiveInfinite(U2)) ?   aDrawer->MaximalParameterValue() : U2;
      
      V1 = (Precision::IsNegativeInfinite(V1)) ? - aDrawer->MaximalParameterValue() : V1;
      V2 = (Precision::IsPositiveInfinite(V2)) ?   aDrawer->MaximalParameterValue() : V2;
      
      DU = (U2-U1)/ N1;
      DV = (V2-V1)/ N2;
      
      // Calculation of the first line;
      
      for ( i = 1; i<= N2+1; i++) {
	aSurface.D1(U1 , V1 + DV * (i-1),P,D1U,D1V);
	P.Coord(x,y,z);
	A1(i).SetCoord (x,y,z);
	Normale = D1U^D1V;
	Normale.Normalize();
	Normale.Coord(x,y,z);
	A1(i).SetNormal(x,y,z);
      }
      
      for ( i = 2; i<= N1+1; i++) {
	for (j = 1; j <= N2+1; j++) {
	  aSurface.D1(U1 + DU * (i-1), V1 + DV * (j-1),
		      P,D1U,D1V);
	  
	  P.Coord(x,y,z);
	  A2(j).SetCoord (x,y,z);
	  Normale = D1U^D1V;
	  Normale.Normalize();
	  Normale.Coord(x,y,z);
	  A2(j).SetNormal(x,y,z);
	  TriangleStrip (2*(j-1) + 1) = A1(j);
	  TriangleStrip (2*(j-1) + 2) = A2(j);
	}
	Prs3d_Root::CurrentGroup(aPresentation)->TriangleMesh(TriangleStrip);
	for ( j = 1; j <= N2 + 1; j++) { A1(j) = A2(j);}
	
      }
    }
  }
  Prs3d_Root::CurrentGroup(aPresentation)->EndPrimitives(); 
}
