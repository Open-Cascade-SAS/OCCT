// Created by: GG
// Copyright (c) 1991-1999 Matra Datavision
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



//-Version

//-Design

//-Warning

//-References

//-Language     C++ 2.1

#include <V3d.hxx>
#include <V3d_Plane.ixx>
#include <Viewer_BadValue.hxx>

#include <Graphic3d_Group.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <gp_Pln.hxx>

//-Constructors

V3d_Plane::V3d_Plane(const Standard_Real A, const Standard_Real B, const Standard_Real C, const Standard_Real D) {

  Viewer_BadValue_Raise_if( sqrt(A*A + B*B + C*C) <= 0., "V3d_Plane::V3d_Plane, bad plane coefficients");

  MyPlane = new Visual3d_ClipPlane(A,B,C,D) ;
}

//-Methods, in order

void V3d_Plane::SetPlane( const Standard_Real A, const Standard_Real B, const Standard_Real C, const Standard_Real D) {

  Viewer_BadValue_Raise_if( sqrt(A*A + B*B + C*C) <= 0., "V3d_Plane::SetPlane, bad plane coefficients");

  MyPlane->SetPlane(A,B,C,D) ;

  if( IsDisplayed() ) {
    Update();
  }
}

void V3d_Plane::Display(const Handle(V3d_View)& aView,
			const Quantity_Color& aColor) {
    Handle(V3d_Viewer) theViewer = aView->Viewer();
    if (!MyGraphicStructure.IsNull()) {
      MyGraphicStructure->Clear();
    }
    Standard_Real size = theViewer->DefaultViewSize();
    Standard_Real offset = size/10000.;
    MyGraphicStructure = new Graphic3d_Structure(theViewer->Viewer());
    Handle(Graphic3d_Group) group = new Graphic3d_Group(MyGraphicStructure);
    Handle(Graphic3d_AspectFillArea3d) aspect =
				new Graphic3d_AspectFillArea3d();
    Graphic3d_MaterialAspect plastic(Graphic3d_NOM_PLASTIC);
    plastic.SetColor(aColor);
    plastic.SetTransparency(0.5);
    aView->SetTransparency(Standard_True);
    aspect->SetFrontMaterial(plastic);
//    aspect->SetInteriorStyle (Aspect_IS_SOLID);
    aspect->SetInteriorStyle (Aspect_IS_HATCH);
    aspect->SetHatchStyle (Aspect_HS_GRID_DIAGONAL_WIDE);
    MyGraphicStructure->SetPrimitivesAspect(aspect);
    Graphic3d_Array1OfVertex p(1,4);
    p(1).SetCoord(-size/2.,-size/2.,offset);
    p(2).SetCoord(-size/2., size/2.,offset);
    p(3).SetCoord( size/2., size/2.,offset);
    p(4).SetCoord( size/2.,-size/2.,offset);
    group->Polygon(p);
    MyGraphicStructure->Display(0);
    Update();
}

void V3d_Plane::Erase() {
  if (!MyGraphicStructure.IsNull()) MyGraphicStructure->Erase();
}

void V3d_Plane::Plane(Standard_Real& A, Standard_Real& B, Standard_Real& C, Standard_Real& D)const  {

  MyPlane->Plane(A,B,C,D) ;
}

Handle(Visual3d_ClipPlane) V3d_Plane::Plane()const  {

  return MyPlane ;
}

Standard_Boolean V3d_Plane::IsDisplayed() const {

  if( MyGraphicStructure.IsNull() ) return Standard_False;
  return MyGraphicStructure->IsDisplayed();
}

void V3d_Plane::Update() {

  if( !MyGraphicStructure.IsNull() ) {
    TColStd_Array2OfReal matrix(1,4,1,4);
    Standard_Real A,B,C,D;
    MyPlane->Plane(A,B,C,D) ;
    gp_Pln plan(A,B,C,D);
    gp_Trsf trsf;
    trsf.SetTransformation(plan.Position());
    trsf.Invert();
    for (Standard_Integer i=1; i<=3; i++){
      for (Standard_Integer j=1; j<=4; j++){
        matrix.SetValue(i,j,trsf.Value(i,j));
      }
    }
    matrix.SetValue(4,1,0.);
    matrix.SetValue(4,2,0.);
    matrix.SetValue(4,3,0.);
    matrix.SetValue(4,4,1.);
    MyGraphicStructure->SetTransform(matrix,Graphic3d_TOC_REPLACE);
  }
}
