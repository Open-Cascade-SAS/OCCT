// Copyright (c) 1995-1999 Matra Datavision
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

// Modified:     22/03/04 ; SAN : OCC4895 High-level interface for controlling polygon offsets

#define IMP020200       //GG Add Transformation() method

#include <Prs3d_Presentation.ixx>
#include <TColStd_Array2OfReal.hxx>
#include <Graphic3d_Structure.hxx>
#include <Aspect_TypeOfHighlightMethod.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax1.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Standard_Real.hxx>
#include <Aspect_InteriorStyle.hxx>
#include <Aspect_TypeOfLine.hxx>
// OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets 
#include <Aspect_PolygonOffsetMode.hxx>
// OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets
#include <Graphic3d_NameOfMaterial.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>


static void MakeGraphicTrsf (const Handle(Geom_Transformation)& aGeomTrsf,
                             TColStd_Array2OfReal& Array){
  for (Standard_Integer i=1; i<=3; i++){
    for (Standard_Integer j=1; j<=4; j++){
      Array.SetValue(i,j,aGeomTrsf->Value(i,j));
    }
  }
  Array.SetValue(4,1,0.);
  Array.SetValue(4,2,0.);
  Array.SetValue(4,3,0.);
  Array.SetValue(4,4,1.);
}

//=======================================================================
//function : Prs3d_Presentation
//purpose  : 
//=======================================================================
Prs3d_Presentation::Prs3d_Presentation 
  (const Handle(Graphic3d_StructureManager)& aViewer,
   const Standard_Boolean                    Init):
  Graphic3d_Structure(aViewer) 
{
  if (Init) {
    Graphic3d_MaterialAspect aMat (Graphic3d_NOM_BRASS);
    Quantity_Color Col;
    // Ceci permet de recuperer la couleur associee
    // au materiau defini par defaut.
//POP pour K4L
    Col = aMat.AmbientColor ();
//    Col = aMat.Color ();

    // OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets 
    // It is necessary to set default polygon offsets for a new presentation
    Handle(Graphic3d_AspectFillArea3d) aDefAspect = 
      new Graphic3d_AspectFillArea3d (Aspect_IS_SOLID,
                                      Col,
                                      Col,
                                      Aspect_TOL_SOLID,
                                      1.0,
                                      Graphic3d_NOM_BRASS,
                                      Graphic3d_NOM_BRASS);
    aDefAspect->SetPolygonOffsets( Aspect_POM_Fill, 1., 0. );
    SetPrimitivesAspect( aDefAspect );
    // OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets 
  }

//  myStruct = Handle(Graphic3d_Structure)::DownCast(This ());
//  myCurrentGroup = new Graphic3d_Group(myStruct);
}

//=======================================================================
//function : Highlight
//purpose  : 
//=======================================================================
void Prs3d_Presentation::Highlight()
{
  SetHighlightColor(Quantity_Color(Quantity_NOC_GRAY99));
  Aspect_TypeOfHighlightMethod Method = Aspect_TOHM_COLOR;
  Graphic3d_Structure::Highlight(Method);
}

//=======================================================================
//function : Color
//purpose  : 
//=======================================================================
void Prs3d_Presentation::Color(const Quantity_NameOfColor aColor) 
{
  SetHighlightColor(Quantity_Color(aColor));
  Graphic3d_Structure::Highlight(Aspect_TOHM_COLOR);
}

//=======================================================================
//function : BoundBox
//purpose  : 
//=======================================================================
void Prs3d_Presentation::BoundBox()
{ 
  SetHighlightColor(Quantity_Color(Quantity_NOC_GRAY99));
  Graphic3d_Structure::Highlight(Aspect_TOHM_BOUNDBOX);
}


//=======================================================================
//function : SetShadingAspect
//purpose  : 
//=======================================================================
void Prs3d_Presentation::SetShadingAspect(const Handle(Prs3d_ShadingAspect)& aShadingAspect) 
{ 
  SetPrimitivesAspect(aShadingAspect->Aspect());
}

//=======================================================================
//function : IsPickable
//purpose  : 
//=======================================================================
Standard_Boolean Prs3d_Presentation::IsPickable () const 
{
  return Graphic3d_Structure::IsSelectable();
}

//=======================================================================
//function : SetPickable
//purpose  : 
//=======================================================================

void Prs3d_Presentation::SetPickable() 
{
  SetPick(Standard_True);
}

//=======================================================================
//function : SetUnPickable
//purpose  : 
//=======================================================================
void Prs3d_Presentation::SetUnPickable() 
{
  SetPick(Standard_False);
}

//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Prs3d_Presentation::Transform(const Handle(Geom_Transformation)& aTransformation) 
{
  TColStd_Array2OfReal Array (1,4,1,4);
  MakeGraphicTrsf(aTransformation, Array);
  SetTransform(Array, Graphic3d_TOC_REPLACE);
}

#ifdef IMP020200
//=======================================================================
//function : Transformation
//purpose  : 
//=======================================================================

Handle(Geom_Transformation) Prs3d_Presentation::Transformation() const {
TColStd_Array2OfReal matrix(1,4,1,4);

  Graphic3d_Structure::Transform(matrix);

  gp_Trsf trsf;
  trsf.SetValues(
        matrix.Value(1,1),matrix.Value(1,2),matrix.Value(1,3),matrix.Value(1,4),
        matrix.Value(2,1),matrix.Value(2,2),matrix.Value(2,3),matrix.Value(2,4),
        matrix.Value(3,1),matrix.Value(3,2),matrix.Value(3,3),matrix.Value(3,4),
        0.001,0.0001);  
  Handle(Geom_Transformation) gtrsf = new Geom_Transformation(trsf); 

  return gtrsf;
}
#endif

//=======================================================================
//function : Place
//purpose  : 
//=======================================================================
void Prs3d_Presentation::Place (const Quantity_Length X,
                                const Quantity_Length Y,
                                const Quantity_Length Z) 
{
  Handle(Geom_Transformation) aTransformation = new Geom_Transformation;
  aTransformation->SetTranslation(gp_Vec(X,Y,Z));
  TColStd_Array2OfReal Array (1,4,1,4);
  MakeGraphicTrsf(aTransformation, Array);
  SetTransform(Array, Graphic3d_TOC_REPLACE);
}

//=======================================================================
//function : Multiply
//purpose  : 
//=======================================================================
void Prs3d_Presentation::Multiply(const Handle(Geom_Transformation)& aTransformation) 
{
  TColStd_Array2OfReal Array (1,4,1,4);
  MakeGraphicTrsf(aTransformation, Array);
  SetTransform(Array, Graphic3d_TOC_POSTCONCATENATE);
}

//=======================================================================
//function : Move
//purpose  : 
//=======================================================================
void Prs3d_Presentation::Move  (const Quantity_Length X,
                                const Quantity_Length Y,
                                const Quantity_Length Z) 
{
  Handle(Geom_Transformation) aTransformation = new Geom_Transformation;
  aTransformation->SetTranslation(gp_Vec(X,Y,Z));
  TColStd_Array2OfReal Array (1,4,1,4);
  MakeGraphicTrsf(aTransformation, Array);
  SetTransform(Array, Graphic3d_TOC_POSTCONCATENATE);
}


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void Prs3d_Presentation::Clear(const Standard_Boolean WithDestruction)
{
  Graphic3d_Structure::Clear(WithDestruction);
  // myCurrentGroup.Nullify();
  myCurrentGroup = NULL;

}


//=======================================================================
//function : Connect
//purpose  : 
//=======================================================================
void Prs3d_Presentation::Connect
  ( const Handle(Prs3d_Presentation)& aPresentation) 
{
  Graphic3d_Structure::Connect(aPresentation, Graphic3d_TOC_DESCENDANT);
}


//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================
void Prs3d_Presentation::Remove (const Handle(Prs3d_Presentation)& aPresentation) 
{
  Disconnect(aPresentation);
}

//=======================================================================
//function : RemoveAll
//purpose  : 
//=======================================================================
void Prs3d_Presentation::RemoveAll () 
{
  DisconnectAll(Graphic3d_TOC_DESCENDANT);
}


//=======================================================================
//function : CurrentGroup
//purpose  : 
//=======================================================================
Handle(Graphic3d_Group) Prs3d_Presentation::CurrentGroup () const 
{
  if(myCurrentGroup.IsNull()){
    void *ptr = (void*) this;
    Prs3d_Presentation* p = (Prs3d_Presentation *)ptr;
    p->NewGroup();
  }
  return myCurrentGroup;
}


//=======================================================================
//function : NewGroup
//purpose  : 
//=======================================================================
Handle(Graphic3d_Group) Prs3d_Presentation::NewGroup ()
{
  myCurrentGroup = new Graphic3d_Group(this);
  return myCurrentGroup;
}

//=======================================================================
//function : Display
//purpose  : 
//=======================================================================
void Prs3d_Presentation::Display ()
{
  Graphic3d_Structure::Display();
}


//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

Handle(Graphic3d_Structure) Prs3d_Presentation::
       Compute(const Handle(Graphic3d_DataStructureManager)& /*aProjector*/) 
{
  return this;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void Prs3d_Presentation::Compute(const Handle_Graphic3d_DataStructureManager& aDataStruct, 
                                 Handle_Graphic3d_Structure& aStruct)
{
 Graphic3d_Structure::Compute(aDataStruct,aStruct );
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

Handle_Graphic3d_Structure Prs3d_Presentation::Compute(const Handle_Graphic3d_DataStructureManager& aDataStruc, 
                                                       const TColStd_Array2OfReal& anArray)
{
 return Graphic3d_Structure::Compute(aDataStruc,anArray);
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void Prs3d_Presentation::Compute(const Handle_Graphic3d_DataStructureManager& aDataStruc,
                                 const TColStd_Array2OfReal& anArray,
                                 Handle_Graphic3d_Structure& aStruc)
{
 Graphic3d_Structure::Compute(aDataStruc,anArray,aStruc);
}
