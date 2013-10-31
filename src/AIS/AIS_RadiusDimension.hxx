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

//!  A framework to define display of radii. <br>
//! These displays serve as relational references in 3D <br>
//! presentations of surfaces, and are particularly useful <br>
//! in viewing fillets. The display consists of arrows and <br>
//! text giving the length of a radius. This display is <br>
//! recalculated if the applicative owner shape changes <br>
//! in dimension, and the text gives the modified length. <br>
//! The algorithm analyzes a length along a face as an <br>
//! arc. It then reconstructs the circle corresponding to <br>
//! the arc and calculates the radius of this circle. <br>

#ifndef _AIS_RadiusDimension_HeaderFile
#define _AIS_RadiusDimension_HeaderFile

#include <AIS.hxx>
#include <AIS_Dimension.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <TopoDS_Shape.hxx>

DEFINE_STANDARD_HANDLE(AIS_RadiusDimension,AIS_Dimension)

class AIS_RadiusDimension : public AIS_Dimension
{
public:
  Standard_EXPORT  AIS_RadiusDimension (const gp_Circ& theCircle);

  Standard_EXPORT  AIS_RadiusDimension (const gp_Circ& theCircle,
                                         const gp_Pnt& theAttachPoint);

  Standard_EXPORT  AIS_RadiusDimension (const gp_Circ& theCircle,
                                        const Handle(Prs3d_DimensionAspect)& theDimensionAspect,
                                        const Standard_Real theExtensionSize = 1.0);

  Standard_EXPORT  AIS_RadiusDimension (const gp_Circ& theCircle,
                                        const gp_Pnt& theAttachPoint,
                                        const Handle(Prs3d_DimensionAspect)& theDimensionAspect,
                                        const Standard_Real theExtensionSize = 1.0);
  //! Constructs the radius display object defined by the <br>
  //! shape aShape, the dimension aVal, and the text aText.
  Standard_EXPORT  AIS_RadiusDimension (const TopoDS_Shape& aShape);

  Standard_EXPORT void SetFlyout (const Standard_Real theFlyoutSize);

  Standard_EXPORT Standard_Real GetFlyout () const;

  DEFINE_STANDARD_RTTI(AIS_RadiusDimension)

protected:
  //! Computes dimension value in display units
  Standard_EXPORT virtual void computeValue ();
    //! Fills default plane object if it is possible to count plane automatically.
  Standard_EXPORT virtual void countDefaultPlane ();

private: 
  virtual  void Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                         const Handle(Prs3d_Presentation)& thePresentation,
                         const Standard_Integer theMode = 0);

// Fields

  //! Defines flyout lines and direction
  //! Flyout direction in the working plane (stored in the base AIS_Dimension).
  //! can be negative , or positive and is defined by the sign of <myFlyout> value.
  //! The direction vector is counting using the working plane.
  //! <myFlyout> value defined the size of flyout.
  Standard_Real myFlyout;
  gp_Circ myCircle;
};
#endif
