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

#include <Aspect_InteriorStyle.hxx>
#include <Aspect_PolygonOffsetMode.hxx>
#include <Aspect_TypeOfHighlightMethod.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Geom_Transformation.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_DataStructureManager.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Standard_Real.hxx>
#include <Standard_Type.hxx>
#include <TColStd_Array2OfReal.hxx>

// OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets 
// OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets
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
Prs3d_Presentation::Prs3d_Presentation (const Handle(Graphic3d_StructureManager)& theViewer,
                                        const Standard_Boolean                    theToInit)
: Graphic3d_Structure (theViewer)
{
  if (!theToInit)
  {
    return;
  }

  Graphic3d_MaterialAspect aMat (Graphic3d_NOM_BRASS);
  Quantity_Color aColor = aMat.AmbientColor();
  // It is necessary to set default polygon offsets for a new presentation
  Handle(Graphic3d_AspectFillArea3d) aDefAspect =
    new Graphic3d_AspectFillArea3d (Aspect_IS_SOLID,
                                    aColor,
                                    aColor,
                                    Aspect_TOL_SOLID,
                                    1.0,
                                    Graphic3d_NOM_BRASS,
                                    Graphic3d_NOM_BRASS);
  aDefAspect->SetPolygonOffsets (Aspect_POM_Fill, 1.0f, 0.0f);
  SetPrimitivesAspect (aDefAspect);
}

//=======================================================================
//function : Prs3d_Presentation
//purpose  :
//=======================================================================
Prs3d_Presentation::Prs3d_Presentation (const Handle(Graphic3d_StructureManager)& theViewer,
                                        const Handle(Prs3d_Presentation)&         thePrs)
: Graphic3d_Structure (theViewer, thePrs)
{
  Graphic3d_MaterialAspect aMat (Graphic3d_NOM_BRASS);
  Quantity_Color aColor = aMat.AmbientColor();
  // It is necessary to set default polygon offsets for a new presentation
  Handle(Graphic3d_AspectFillArea3d) aDefAspect =
    new Graphic3d_AspectFillArea3d (Aspect_IS_SOLID,
                                    aColor,
                                    aColor,
                                    Aspect_TOL_SOLID,
                                    1.0,
                                    Graphic3d_NOM_BRASS,
                                    Graphic3d_NOM_BRASS);
  aDefAspect->SetPolygonOffsets (Aspect_POM_Fill, 1.0f, 0.0f);
  SetPrimitivesAspect (aDefAspect);
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
//function : Transform
//purpose  : 
//=======================================================================

void Prs3d_Presentation::Transform(const Handle(Geom_Transformation)& aTransformation) 
{
  TColStd_Array2OfReal Array (1,4,1,4);
  MakeGraphicTrsf(aTransformation, Array);
  SetTransform(Array, Graphic3d_TOC_REPLACE);
}

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
        matrix.Value(3,1),matrix.Value(3,2),matrix.Value(3,3),matrix.Value(3,4));  
  Handle(Geom_Transformation) gtrsf = new Geom_Transformation(trsf); 

  return gtrsf;
}

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
  if (Groups().IsEmpty())
  {
    return const_cast<Prs3d_Presentation* >(this)->NewGroup();
  }
  return Groups().Last();
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

void Prs3d_Presentation::Compute(const Handle(Graphic3d_DataStructureManager)& aDataStruct, 
                                 Handle(Graphic3d_Structure)& aStruct)
{
 Graphic3d_Structure::Compute(aDataStruct,aStruct );
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

Handle(Graphic3d_Structure) Prs3d_Presentation::Compute(const Handle(Graphic3d_DataStructureManager)& aDataStruc, 
                                                       const TColStd_Array2OfReal& anArray)
{
 return Graphic3d_Structure::Compute(aDataStruc,anArray);
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void Prs3d_Presentation::Compute(const Handle(Graphic3d_DataStructureManager)& aDataStruc,
                                 const TColStd_Array2OfReal& anArray,
                                 Handle(Graphic3d_Structure)& aStruc)
{
 Graphic3d_Structure::Compute(aDataStruc,anArray,aStruc);
}
