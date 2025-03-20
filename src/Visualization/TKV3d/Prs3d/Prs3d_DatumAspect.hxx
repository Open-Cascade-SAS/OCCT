// Created on: 1993-07-30
// Created by: Jean-Louis FRENKEL
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Prs3d_DatumAspect_HeaderFile
#define _Prs3d_DatumAspect_HeaderFile

#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_DatumAttribute.hxx>
#include <Prs3d_DatumAxes.hxx>
#include <Prs3d_DatumParts.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_TextAspect.hxx>

//! A framework to define the display of datums.
class Prs3d_DatumAspect : public Prs3d_BasicAspect
{
  DEFINE_STANDARD_RTTIEXT(Prs3d_DatumAspect, Prs3d_BasicAspect)
public:
  //! An empty constructor.
  Standard_EXPORT Prs3d_DatumAspect();

  //! Returns line aspect for specified part.
  const Handle(Prs3d_LineAspect)& LineAspect(Prs3d_DatumParts thePart) const
  {
    return myLineAspects[thePart];
  }

  //! Returns shading aspect for specified part.
  const Handle(Prs3d_ShadingAspect)& ShadingAspect(Prs3d_DatumParts thePart) const
  {
    return myShadedAspects[thePart];
  }

  //! Returns the text attributes for rendering label of specified part
  //! (Prs3d_DatumParts_XAxis/Prs3d_DatumParts_YAxis/Prs3d_DatumParts_ZAxis).
  const Handle(Prs3d_TextAspect)& TextAspect(Prs3d_DatumParts thePart) const
  {
    return myTextAspects[thePart];
  }

  //! Sets text attributes for rendering labels.
  void SetTextAspect(const Handle(Prs3d_TextAspect)& theTextAspect)
  {
    myTextAspects[Prs3d_DatumParts_XAxis] = theTextAspect;
    myTextAspects[Prs3d_DatumParts_YAxis] = theTextAspect;
    myTextAspects[Prs3d_DatumParts_ZAxis] = theTextAspect;
  }

  //! Returns the point aspect of origin wireframe presentation
  const Handle(Prs3d_PointAspect)& PointAspect() const { return myPointAspect; }

  //! Returns the point aspect of origin wireframe presentation
  void SetPointAspect(const Handle(Prs3d_PointAspect)& theAspect) { myPointAspect = theAspect; }

  //! Returns the arrow aspect of presentation.
  const Handle(Prs3d_ArrowAspect)& ArrowAspect() const { return myArrowAspect; }

  //! Sets the arrow aspect of presentation
  void SetArrowAspect(const Handle(Prs3d_ArrowAspect)& theAspect) { myArrowAspect = theAspect; }

  //! Returns true if the given part is used in axes of aspect
  Standard_EXPORT Standard_Boolean DrawDatumPart(Prs3d_DatumParts thePart) const;

  //! Sets the axes used in the datum aspect
  void SetDrawDatumAxes(Prs3d_DatumAxes theType) { myAxes = theType; }

  //! Returns axes used in the datum aspect
  Prs3d_DatumAxes DatumAxes() const { return myAxes; }

  //! Returns the attribute of the datum type
  Standard_Real Attribute(Prs3d_DatumAttribute theType) const { return myAttributes[theType]; }

  //! Sets the attribute of the datum type
  void SetAttribute(Prs3d_DatumAttribute theType, const Standard_Real theValue)
  {
    myAttributes[theType] = theValue;
  }

  //! Returns the length of the displayed first axis.
  Standard_EXPORT Standard_Real AxisLength(Prs3d_DatumParts thePart) const;

  //! Sets the lengths of the three axes.
  void SetAxisLength(Standard_Real theL1, Standard_Real theL2, Standard_Real theL3)
  {
    myAttributes[Prs3d_DatumAttribute_XAxisLength] = theL1;
    myAttributes[Prs3d_DatumAttribute_YAxisLength] = theL2;
    myAttributes[Prs3d_DatumAttribute_ZAxisLength] = theL3;
  }

  //! @return true if axes labels are drawn; TRUE by default.
  Standard_Boolean ToDrawLabels() const { return myToDrawLabels; }

  //! Sets option to draw or not to draw text labels for axes
  void SetDrawLabels(Standard_Boolean theToDraw) { myToDrawLabels = theToDraw; }

  void SetToDrawLabels(Standard_Boolean theToDraw) { myToDrawLabels = theToDraw; }

  //! @return true if axes arrows are drawn; TRUE by default.
  Standard_Boolean ToDrawArrows() const { return myToDrawArrows; }

  //! Sets option to draw or not arrows for axes
  void SetDrawArrows(Standard_Boolean theToDraw) { myToDrawArrows = theToDraw; }

  //! Performs deep copy of attributes from another aspect instance.
  Standard_EXPORT void CopyAspectsFrom(const Handle(Prs3d_DatumAspect)& theOther);

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

public:
  //! Returns type of arrow for a type of axis
  Standard_EXPORT static Prs3d_DatumParts ArrowPartForAxis(Prs3d_DatumParts thePart);

public:
  //! Returns the text attributes for rendering labels.
  Standard_DEPRECATED(
    "This method is deprecated - TextAspect() with axis parameter should be called instead")

  const Handle(Prs3d_TextAspect)& TextAspect() const
  {
    return myTextAspects[Prs3d_DatumParts_XAxis];
  }

protected:
  Handle(Prs3d_ShadingAspect) myShadedAspects[Prs3d_DatumParts_NB];
  Handle(Prs3d_LineAspect)    myLineAspects[Prs3d_DatumParts_NB];
  Handle(Prs3d_TextAspect)    myTextAspects[Prs3d_DatumParts_NB];
  Handle(Prs3d_PointAspect)   myPointAspect;
  Handle(Prs3d_ArrowAspect)   myArrowAspect;
  Standard_Real               myAttributes[Prs3d_DatumAttribute_NB];
  Prs3d_DatumAxes             myAxes;
  Standard_Boolean            myToDrawLabels;
  Standard_Boolean            myToDrawArrows;
};

DEFINE_STANDARD_HANDLE(Prs3d_DatumAspect, Prs3d_BasicAspect)

#endif // _Prs3d_DatumAspect_HeaderFile
