// Created on: 1995-08-09
// Created by: Arnaud BOUZY
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

#ifndef _AIS_Point_HeaderFile
#define _AIS_Point_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Standard_Integer.hxx>
#include <AIS_KindOfInteractive.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <SelectMgr_Selection.hxx>
#include <Quantity_NameOfColor.hxx>
class Geom_Point;
class Prs3d_Presentation;
class Prs3d_Projector;
class Geom_Transformation;
class Quantity_Color;
class TopoDS_Vertex;


class AIS_Point;
DEFINE_STANDARD_HANDLE(AIS_Point, AIS_InteractiveObject)

//! Constructs point datums to be used in construction of
//! composite shapes. The datum is displayed as the plus marker +.
class AIS_Point : public AIS_InteractiveObject
{

public:

  

  //! Initializes the point aComponent from which the point
  //! datum will be built.
  Standard_EXPORT AIS_Point(const Handle(Geom_Point)& aComponent);
  
  //! Returns index 1, the default index for a point.
    virtual Standard_Integer Signature() const Standard_OVERRIDE;
  
  //! Indicates that a point is a datum.
    virtual AIS_KindOfInteractive Type() const Standard_OVERRIDE;
  
  //! Returns the component specified in SetComponent.
  Standard_EXPORT Handle(Geom_Point) Component();
  
  //! Constructs an instance of the point aComponent.
  Standard_EXPORT void SetComponent (const Handle(Geom_Point)& aComponent);
  
  //! Returns true if the display mode selected is valid for point datums.
  Standard_EXPORT Standard_Boolean AcceptDisplayMode (const Standard_Integer aMode) const Standard_OVERRIDE;
  
  //! computes the presentation according to a point of view
  //! given by <aProjector>.
  //! To be Used when the associated degenerated Presentations
  //! have been transformed by <aTrsf> which is not a Pure
  //! Translation. The HLR Prs can't be deducted automatically
  //! WARNING :<aTrsf> must be applied
  //! to the object to display before computation  !!!
  Standard_EXPORT virtual void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Geom_Transformation)& aTrsf, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;
  
  //! Allows you to provide settings for the cp;pr aColor.
  Standard_EXPORT void SetColor (const Quantity_NameOfColor aColor) Standard_OVERRIDE;
  
  Standard_EXPORT void SetColor (const Quantity_Color& aColor) Standard_OVERRIDE;
  
  //! Allows you to remove color settings.
  Standard_EXPORT void UnsetColor() Standard_OVERRIDE;
  
  //! Allows you to provide settings for a marker. These include
  //! -   type of marker,
  //! -   marker color,
  //! -   scale factor.
  Standard_EXPORT void SetMarker (const Aspect_TypeOfMarker aType);
  
  //! Removes the marker settings.
  Standard_EXPORT void UnsetMarker();
  
  //! Returns true if the point datum has a marker.
    Standard_Boolean HasMarker() const;
  
  //! Converts a point into a vertex.
  Standard_EXPORT TopoDS_Vertex Vertex() const;




  DEFINE_STANDARD_RTTI(AIS_Point,AIS_InteractiveObject)

protected:

  
  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager, const Handle(Prs3d_Presentation)& aPresentation, const Standard_Integer aMode = 0) Standard_OVERRIDE;



private:

  
  Standard_EXPORT void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;
  
  Standard_EXPORT void ComputeSelection (const Handle(SelectMgr_Selection)& aSelection, const Standard_Integer aMode);
  
  Standard_EXPORT void UpdatePointValues();

  Handle(Geom_Point) myComponent;
  Standard_Boolean myHasTOM;
  Aspect_TypeOfMarker myTOM;


};


#include <AIS_Point.lxx>





#endif // _AIS_Point_HeaderFile
