// Created on: 2016-10-11
// Created by: Ilya SEVRIKOV
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _V3d_Trihedron_HeaderFile
#define _V3d_Trihedron_HeaderFile

#include <Graphic3d_Group.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_TransformPers.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_TextAspect.hxx>

class V3d_View;

//! Class for presentation of zbuffer trihedron object.
class V3d_Trihedron : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(V3d_Trihedron, Standard_Transient)
public:

  //! Creates a default trihedron.
  Standard_EXPORT V3d_Trihedron();

  //! Destructor.
  Standard_EXPORT virtual ~V3d_Trihedron();

  //! Switch wireframe / shaded trihedron.
  void SetWireframe (const Standard_Boolean theAsWireframe) { myIsWireframe = theAsWireframe; }

  //! Setup the corner to draw the trihedron.
  Standard_EXPORT void SetPosition (const Aspect_TypeOfTriedronPosition thePosition);

  //! Setup the scale factor.
  Standard_EXPORT void SetScale (const Standard_Real theScale);

  //! Setup the size ratio factor.
  Standard_EXPORT void SetSizeRatio (const Standard_Real theRatio);

  //! Setup the arrow diameter.
  Standard_EXPORT void SetArrowDiameter (const Standard_Real theDiam);

  //! Setup the number of facets for tessellation.
  Standard_EXPORT void SetNbFacets (const Standard_Integer theNbFacets);

  //! Setup color of text labels.
  Standard_EXPORT void SetLabelsColor (const Quantity_Color& theColor);

  //! Setup colors of arrows.
  Standard_EXPORT void SetArrowsColor (const Quantity_Color& theXColor,
                                       const Quantity_Color& theYColor,
                                       const Quantity_Color& theZColor);

  //! Display trihedron.
  Standard_EXPORT void Display (const V3d_View& theView);

  //! Erase trihedron.
  Standard_EXPORT void Erase();

protected:

  //! Fills Graphic3d_Group.
  Standard_EXPORT void compute();

  //! Invalidates objects. It leads recompute.
  void invalidate()
  {
    myToCompute = Standard_True;
  }

  //! Sets offset 2d for transform persistence.
  Standard_EXPORT void setOffset2d();

private:

  //! Custom Graphic3d_Structure implementation.
  class TrihedronStructure;

protected:

  Handle(Prs3d_ShadingAspect)        mySphereShadingAspect;
  Handle(Prs3d_LineAspect)           mySphereLineAspect;

  Handle(Prs3d_TextAspect)           myTextAspect;

  Handle(Prs3d_ShadingAspect)        myArrowShadingAspects[3];
  Handle(Prs3d_LineAspect)           myArrowLineAspects[3];

  Handle(Graphic3d_Structure)        myStructure;
  Handle(Graphic3d_TransformPers)    myTransformPers;

  Standard_Real                      myScale;
  Standard_Real                      myRatio;
  Standard_Real                      myDiameter;
  Standard_Integer                   myNbFacettes;
  Standard_Boolean                   myIsWireframe;
  Standard_Boolean                   myToCompute;

};

DEFINE_STANDARD_HANDLE (V3d_Trihedron, Standard_Transient)

#endif
