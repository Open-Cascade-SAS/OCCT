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

#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_TransformPers.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_TextAspect.hxx>
#include <V3d_TypeOfAxe.hxx>

class V3d_View;

//! Class for presentation of trihedron object.
class V3d_Trihedron : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(V3d_Trihedron, Standard_Transient)
public:
  //! Creates a default trihedron.
  Standard_EXPORT V3d_Trihedron();

  //! Destructor.
  Standard_EXPORT virtual ~V3d_Trihedron();

  //! Return TRUE if wireframe presentation is set; FALSE by default.
  bool IsWireframe() const { return myIsWireframe; }

  //! Switch wireframe / shaded trihedron.
  void SetWireframe(const bool theAsWireframe)
  {
    if (myIsWireframe != theAsWireframe)
    {
      invalidate();
      myIsWireframe = theAsWireframe;
    }
  }

  //! Return trihedron position.
  const occ::handle<Graphic3d_TransformPers> TransformPersistence() const { return myTransformPers; }

  //! Setup the corner to draw the trihedron.
  Standard_EXPORT void SetPosition(const Aspect_TypeOfTriedronPosition thePosition);

  //! Return scale factor.
  double Scale() const { return myScale; }

  //! Setup the scale factor.
  Standard_EXPORT void SetScale(const double theScale);

  //! Return size ratio factor.
  double SizeRatio() const { return myRatio; }

  //! Setup the size ratio factor.
  Standard_EXPORT void SetSizeRatio(const double theRatio);

  //! Return arrow diameter.
  double ArrowDiameter() const { return myDiameter; }

  //! Setup the arrow diameter.
  Standard_EXPORT void SetArrowDiameter(const double theDiam);

  //! Return number of facets for tessellation.
  int NbFacets() const { return myNbFacettes; }

  //! Setup the number of facets for tessellation.
  Standard_EXPORT void SetNbFacets(const int theNbFacets);

  //! Return text aspect for specified axis.
  //! @param[in] theAxis  axis index
  //! @return text aspect
  const occ::handle<Prs3d_TextAspect>& LabelAspect(V3d_TypeOfAxe theAxis) const
  {
    return myTextAspects[theAxis];
  }

  //! Setup per-label color.
  Standard_EXPORT void SetLabelsColor(const Quantity_Color& theXColor,
                                      const Quantity_Color& theYColor,
                                      const Quantity_Color& theZColor);

  //! Setup color of text labels.
  Standard_EXPORT void SetLabelsColor(const Quantity_Color& theColor);

  //! Return shading aspect for specified axis.
  //! @param[in] theAxis  axis index
  //! @return shading aspect
  const occ::handle<Prs3d_ShadingAspect>& ArrowAspect(V3d_TypeOfAxe theAxis) const
  {
    return myArrowShadingAspects[theAxis];
  }

  //! Setup colors of arrows.
  Standard_EXPORT void SetArrowsColor(const Quantity_Color& theXColor,
                                      const Quantity_Color& theYColor,
                                      const Quantity_Color& theZColor);

  //! Return shading aspect of origin sphere.
  const occ::handle<Prs3d_ShadingAspect>& OriginAspect() const { return mySphereShadingAspect; }

  //! Return axis text.
  //! @param[in] theAxis  axis index
  //! @return text of the label
  const TCollection_AsciiString& Label(V3d_TypeOfAxe theAxis) const { return myLabels[theAxis]; }

  //! Setup per-axis text.
  Standard_EXPORT void SetLabels(const TCollection_AsciiString& theX,
                                 const TCollection_AsciiString& theY,
                                 const TCollection_AsciiString& theZ);

  //! Display trihedron.
  void Display(const occ::handle<V3d_View>& theView) { Display(*theView); }

  //! Display trihedron.
  Standard_EXPORT void Display(const V3d_View& theView);

  //! Erase trihedron.
  Standard_EXPORT void Erase();

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

protected:
  //! Fills Graphic3d_Group.
  Standard_EXPORT void compute();

  //! Invalidates objects. It leads recompute.
  void invalidate() { myToCompute = true; }

  //! Sets offset 2d for transform persistence.
  Standard_EXPORT void setOffset2d();

private:
  //! Custom Graphic3d_Structure implementation.
  class TrihedronStructure;

protected:
  occ::handle<Prs3d_ShadingAspect> mySphereShadingAspect;
  occ::handle<Prs3d_TextAspect>    myTextAspects[3];
  occ::handle<Prs3d_ShadingAspect> myArrowShadingAspects[3];
  TCollection_AsciiString     myLabels[3];

  occ::handle<Graphic3d_Structure>     myStructure;
  occ::handle<Graphic3d_TransformPers> myTransformPers;

  double    myScale;
  double    myRatio;
  double    myDiameter;
  int myNbFacettes;
  bool myIsWireframe;
  bool myToCompute;
};

#endif
