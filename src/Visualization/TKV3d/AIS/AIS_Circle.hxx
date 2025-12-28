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
  Standard_EXPORT AIS_Circle(const occ::handle<Geom_Circle>& aCircle);

  //! Initializes this algorithm for constructing AIS circle datums.
  //! Initializes the circle theCircle, the arc
  //! starting point theUStart, the arc ending point theUEnd,
  //! and the type of sensitivity theIsFilledCircleSens.
  Standard_EXPORT AIS_Circle(const occ::handle<Geom_Circle>& theCircle,
                             const double                    theUStart,
                             const double                    theUEnd,
                             const bool                      theIsFilledCircleSens = false);

  //! Returns index 6 by default.
  virtual int Signature() const override { return 6; }

  //! Indicates that the type of Interactive Object is a datum.
  virtual AIS_KindOfInteractive Type() const override { return AIS_KindOfInteractive_Datum; }

  //! Returns the circle component defined in SetCircle.
  const occ::handle<Geom_Circle>& Circle() const { return myComponent; }

  //! Constructs instances of the starting point and the end
  //! point parameters, theU1 and theU2.
  void Parameters(double& theU1, double& theU2) const
  {
    theU1 = myUStart;
    theU2 = myUEnd;
  }

  //! Allows you to provide settings for the circle datum aCircle.
  void SetCircle(const occ::handle<Geom_Circle>& theCircle) { myComponent = theCircle; }

  //! Allows you to set the parameter theU for the starting point of an arc.
  void SetFirstParam(const double theU)
  {
    myUStart      = theU;
    myCircleIsArc = true;
  }

  //! Allows you to provide the parameter theU for the end point of an arc.
  void SetLastParam(const double theU)
  {
    myUEnd        = theU;
    myCircleIsArc = true;
  }

  Standard_EXPORT void SetColor(const Quantity_Color& aColor) override;

  //! Assigns the width aValue to the solid line boundary of the circle datum.
  Standard_EXPORT void SetWidth(const double aValue) override;

  //! Removes color from the solid line boundary of the circle datum.
  Standard_EXPORT void UnsetColor() override;

  //! Removes width settings from the solid line boundary of the circle datum.
  Standard_EXPORT void UnsetWidth() override;

  //! Returns the type of sensitivity for the circle;
  bool IsFilledCircleSens() const { return myIsFilledCircleSens; }

  //! Sets the type of sensitivity for the circle. If theIsFilledCircleSens set to true
  //! then the whole circle will be detectable, otherwise only the boundary of the circle.
  void SetFilledCircleSens(const bool theIsFilledCircleSens)
  {
    myIsFilledCircleSens = theIsFilledCircleSens;
  }

private:
  Standard_EXPORT virtual void Compute(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                                       const occ::handle<Prs3d_Presentation>&         theprs,
                                       const int theMode) override;

  Standard_EXPORT virtual void ComputeSelection(const occ::handle<SelectMgr_Selection>& theSel,
                                                const int theMode) override;

  Standard_EXPORT void ComputeCircle(const occ::handle<Prs3d_Presentation>& aPresentation);

  Standard_EXPORT void ComputeArc(const occ::handle<Prs3d_Presentation>& aPresentation);

  Standard_EXPORT void ComputeCircleSelection(const occ::handle<SelectMgr_Selection>& aSelection);

  Standard_EXPORT void ComputeArcSelection(const occ::handle<SelectMgr_Selection>& aSelection);

  //! Replace aspects of already computed groups with the new value.
  void replaceWithNewLineAspect(const occ::handle<Prs3d_LineAspect>& theAspect);

private:
  occ::handle<Geom_Circle> myComponent;
  double                   myUStart;
  double                   myUEnd;
  bool                     myCircleIsArc;
  bool                     myIsFilledCircleSens;
};

#endif // _AIS_Circle_HeaderFile
