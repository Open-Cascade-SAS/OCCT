// Created on: 1997-01-21
// Created by: Prestataire Christiane ARMAND
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _AIS_Circle_HeaderFile
#define _AIS_Circle_HeaderFile

#include <AIS_InteractiveObject.hxx>

class Geom_Circle;

//! Constructs circle datums to be used in construction of
//! composite shapes.
class AIS_Circle : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTIEXT(AIS_Circle, AIS_InteractiveObject)
public:

  //! Initializes this algorithm for constructing AIS circle
  //! datums initializes the circle aCircle
  Standard_EXPORT AIS_Circle(const Handle(Geom_Circle)& aCircle);
  
  //! Initializes this algorithm for constructing AIS circle datums.
  //! Initializes the circle theCircle, the arc
  //! starting point theUStart, the arc ending point theUEnd,
  //! and the type of sensitivity theIsFilledCircleSens.
  Standard_EXPORT AIS_Circle(const Handle(Geom_Circle)& theCircle, const Standard_Real theUStart, const Standard_Real theUEnd, const Standard_Boolean theIsFilledCircleSens = Standard_False);
  
  //! computes the presentation according to a point of view
  //! given by <aProjector>.
  //! To be Used when the associated degenerated Presentations
  //! have been transformed by <aTrsf> which is not a Pure
  //! Translation. The HLR Prs can't be deducted automatically
  //! WARNING :<aTrsf> must be applied
  //! to the object to display before computation  !!!
  Standard_EXPORT virtual void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Geom_Transformation)& aTrsf, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;
  
  //! Returns index 6 by default.
  virtual Standard_Integer Signature() const Standard_OVERRIDE { return 6; }

  //! Indicates that the type of Interactive Object is a datum.
  virtual AIS_KindOfInteractive Type() const Standard_OVERRIDE { return AIS_KOI_Datum; }

  //! Returns the circle component defined in SetCircle.
  const Handle(Geom_Circle)& Circle() const { return myComponent; }

  //! Constructs instances of the starting point and the end
  //! point parameters, theU1 and theU2.
  void Parameters (Standard_Real& theU1, Standard_Real& theU2) const
  {
    theU1 = myUStart;
    theU2 = myUEnd;
  }

  //! Allows you to provide settings for the circle datum aCircle.
  void SetCircle (const Handle(Geom_Circle)& theCircle) { myComponent = theCircle; }

  //! Allows you to set the parameter theU for the starting point of an arc.
  void SetFirstParam (const Standard_Real theU)
  {
    myUStart = theU;
    myCircleIsArc = Standard_True;
  }

  //! Allows you to provide the parameter theU for the end point of an arc.
  void SetLastParam (const Standard_Real theU)
  {
    myUEnd = theU;
    myCircleIsArc = Standard_True;
  }

  Standard_EXPORT void SetColor (const Quantity_Color& aColor) Standard_OVERRIDE;
  
  //! Assigns the width aValue to the solid line boundary of the circle datum.
  Standard_EXPORT void SetWidth (const Standard_Real aValue) Standard_OVERRIDE;
  
  //! Removes color from the solid line boundary of the circle datum.
  Standard_EXPORT void UnsetColor() Standard_OVERRIDE;
  
  //! Removes width settings from the solid line boundary of the circle datum.
  Standard_EXPORT void UnsetWidth() Standard_OVERRIDE;
  
  //! Returns the type of sensitivity for the circle;
  Standard_Boolean IsFilledCircleSens() const { return myIsFilledCircleSens; }

  //! Sets the type of sensitivity for the circle. If theIsFilledCircleSens set to Standard_True
  //! then the whole circle will be detectable, otherwise only the boundary of the circle.
  void SetFilledCircleSens (const Standard_Boolean theIsFilledCircleSens) { myIsFilledCircleSens = theIsFilledCircleSens; }

private:

  Standard_EXPORT void Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager, const Handle(Prs3d_Presentation)& aPresentation, const Standard_Integer aMode = 0) Standard_OVERRIDE;
  
  Standard_EXPORT void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;
  
  Standard_EXPORT void ComputeSelection (const Handle(SelectMgr_Selection)& aSelection, const Standard_Integer aMode) Standard_OVERRIDE;
  
  Standard_EXPORT void ComputeCircle (const Handle(Prs3d_Presentation)& aPresentation);
  
  Standard_EXPORT void ComputeArc (const Handle(Prs3d_Presentation)& aPresentation);
  
  Standard_EXPORT void ComputeCircleSelection (const Handle(SelectMgr_Selection)& aSelection);
  
  Standard_EXPORT void ComputeArcSelection (const Handle(SelectMgr_Selection)& aSelection);

private:

  Handle(Geom_Circle) myComponent;
  Standard_Real myUStart;
  Standard_Real myUEnd;
  Standard_Boolean myCircleIsArc;
  Standard_Boolean myIsFilledCircleSens;

};

DEFINE_STANDARD_HANDLE(AIS_Circle, AIS_InteractiveObject)

#endif // _AIS_Circle_HeaderFile
