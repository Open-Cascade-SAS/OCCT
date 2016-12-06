// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef CafShapePrs_h
#define CafShapePrs_h

#include <TDF_Label.hxx>
#include <TDF_LabelMapHasher.hxx>
#include <XCAFPrs_AISObject.hxx>
#include <XCAFPrs_Style.hxx>

typedef NCollection_DataMap<TopoDS_Shape, Handle(AIS_ColoredDrawer), TopTools_ShapeMapHasher> CafDataMapOfShapeColor;
typedef NCollection_DataMap<TDF_Label, Handle(AIS_InteractiveObject), TDF_LabelMapHasher> MapOfPrsForShapes;

//! Interactive object for shape in DECAF document
class CafShapePrs : public XCAFPrs_AISObject
{
  
public:
  
  //! Default constructor.
  CafShapePrs (const TDF_Label&                theLabel,
               const XCAFPrs_Style&            theStyle,
               const Graphic3d_MaterialAspect& theMaterial);
  
  //! Search custom aspect for specified shape.
  Standard_Boolean FindCustomAspects (const TopoDS_Shape&        theShape,
                                      Handle(AIS_ColoredDrawer)& theAspects) const
  {
    return myShapeColors.Find (theShape, theAspects);
  }
  
  //! Access the styles map.
  const CafDataMapOfShapeColor& ShapeColors() const { return myShapeColors; }
  
  //! Override default style.
  virtual void DefaultStyle (XCAFPrs_Style& theStyle) const Standard_OVERRIDE
  {
    theStyle = myDefStyle;
  }
  
protected:
  
  XCAFPrs_Style myDefStyle; //!< default style
  
public: //! @name Declaration of CASCADE RTTI
  
  DEFINE_STANDARD_RTTIEXT(CafShapePrs, XCAFPrs_AISObject)
  
};

DEFINE_STANDARD_HANDLE(CafShapePrs, XCAFPrs_AISObject)

#endif // CafShapePrs_h
