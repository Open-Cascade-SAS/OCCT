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

#include <NCollection_DataMap.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_DatumAttribute.hxx>
#include <Prs3d_DatumAxes.hxx>
#include <Prs3d_DatumMode.hxx>
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

  //! An empty framework to define the display of datums.
  Standard_EXPORT Prs3d_DatumAspect();

  //! Returns the right-handed coordinate system set in SetComponent.
  Standard_EXPORT Handle(Prs3d_LineAspect) LineAspect (Prs3d_DatumParts thePart) const;

  //! Returns the right-handed coordinate system set in SetComponent.
  Standard_EXPORT Handle(Prs3d_ShadingAspect) ShadingAspect (Prs3d_DatumParts thePart) const;

  //! Returns the right-handed coordinate system set in SetComponent.
  const Handle(Prs3d_TextAspect)& TextAspect() const { return myTextAspect; }

  //! Returns the point aspect of origin wireframe presentation
  const Handle(Prs3d_PointAspect)& PointAspect() const { return myPointAspect; }

  //! Returns the arrow aspect of presentation
  const Handle(Prs3d_ArrowAspect)& ArrowAspect() const { return myArrowAspect; }

  //! Returns the attributes for display of the first axis.
  Standard_DEPRECATED("This method is deprecated - LineAspect() should be called instead")
  const Handle(Prs3d_LineAspect)& FirstAxisAspect() const { return myLineAspects.Find (Prs3d_DP_XAxis); }

  //! Returns the attributes for display of the second axis.
  Standard_DEPRECATED("This method is deprecated - LineAspect() should be called instead")
  const Handle(Prs3d_LineAspect)& SecondAxisAspect() const { return myLineAspects.Find (Prs3d_DP_YAxis); }

  //! Returns the attributes for display of the third axis.
  Standard_DEPRECATED("This method is deprecated - LineAspect() should be called instead")
  const Handle(Prs3d_LineAspect)& ThirdAxisAspect() const { return myLineAspects.Find (Prs3d_DP_ZAxis); }

  //! Sets the DrawFirstAndSecondAxis attributes to active.
  Standard_DEPRECATED("This method is deprecated - SetDrawDatumAxes() should be called instead")
  Standard_EXPORT void SetDrawFirstAndSecondAxis (Standard_Boolean theToDraw);

  //! Returns true if the first and second axes can be drawn.
  Standard_DEPRECATED("This method is deprecated - DatumAxes() should be called instead")
  Standard_Boolean DrawFirstAndSecondAxis() const
  {
    return (myAxes & Prs3d_DA_XAxis) != 0
        && (myAxes & Prs3d_DA_YAxis) != 0;
  }

  //! Sets the DrawThirdAxis attributes to active.
  Standard_DEPRECATED("This method is deprecated - SetDrawDatumAxes() should be called instead")
  Standard_EXPORT void SetDrawThirdAxis (Standard_Boolean theToDraw);

  //! Returns true if the third axis can be drawn.
  Standard_DEPRECATED("This method is deprecated - DatumAxes() should be called instead")
  Standard_Boolean DrawThirdAxis() const { return (myAxes & Prs3d_DA_ZAxis) != 0; }

  //! Returns true if the given part is used in axes of aspect
  Standard_EXPORT Standard_Boolean DrawDatumPart (Prs3d_DatumParts thePart) const;

  //! Sets the axes used in the datum aspect
  void SetDrawDatumAxes (Prs3d_DatumAxes theType) { myAxes = theType; }

  //! Returns axes used in the datum aspect
  Prs3d_DatumAxes DatumAxes() const { return myAxes; }

  //! Sets the attribute of the datum type
  void SetAttribute (Prs3d_DatumAttribute theType, const Standard_Real& theValue)
  {
    myAttributes.Bind (theType, theValue);
  }

  //! Returns the attribute of the datum type
  Standard_Real Attribute (Prs3d_DatumAttribute theType) const
  {
    return myAttributes.Find (theType);
  }

  //! Sets the lengths of the three axes.
  void SetAxisLength (Standard_Real theL1, Standard_Real theL2, Standard_Real theL3)
  {
    myAttributes.Bind (Prs3d_DA_XAxisLength, theL1);
    myAttributes.Bind (Prs3d_DA_YAxisLength, theL2);
    myAttributes.Bind (Prs3d_DA_ZAxisLength, theL3);
  }

  //! Returns the length of the displayed first axis.
  Standard_EXPORT Standard_Real AxisLength (Prs3d_DatumParts thePart) const;

  //! Returns the length of the displayed first axis.
  Standard_DEPRECATED("This method is deprecated - AxisLength() should be called instead")
  Standard_Real FirstAxisLength() const { return myAttributes.Find (Prs3d_DA_XAxisLength); }

  //! Returns the length of the displayed second axis.
  Standard_DEPRECATED("This method is deprecated - AxisLength() should be called instead")
  Standard_Real SecondAxisLength() const { return myAttributes.Find (Prs3d_DA_YAxisLength); }

  //! Returns the length of the displayed third axis.
  Standard_DEPRECATED("This method is deprecated - AxisLength() should be called instead")
  Standard_Real ThirdAxisLength() const { return myAttributes.Find (Prs3d_DA_ZAxisLength); }

  //! Sets option to draw or not to draw text labels for axes
  void SetToDrawLabels (Standard_Boolean theToDraw) { myToDrawLabels = theToDraw; }

  //! @return true if axes labels are drawn
  Standard_Boolean ToDrawLabels() const { return myToDrawLabels; }

  //! Returns type of arrow for a type of axis
  Standard_EXPORT Prs3d_DatumParts ArrowPartForAxis (Prs3d_DatumParts thePart) const;

private:
  Prs3d_DatumAxes myAxes;
  Standard_Boolean myToDrawLabels;
  NCollection_DataMap<Prs3d_DatumAttribute, Standard_Real> myAttributes;

  NCollection_DataMap<Prs3d_DatumParts, Handle(Prs3d_ShadingAspect)> myShadedAspects;
  NCollection_DataMap<Prs3d_DatumParts, Handle(Prs3d_LineAspect)> myLineAspects;

  Handle(Prs3d_TextAspect)  myTextAspect;
  Handle(Prs3d_PointAspect) myPointAspect;
  Handle(Prs3d_ArrowAspect) myArrowAspect;
};

DEFINE_STANDARD_HANDLE(Prs3d_DatumAspect, Prs3d_BasicAspect)

#endif // _Prs3d_DatumAspect_HeaderFile
