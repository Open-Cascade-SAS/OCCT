// Copyright (c) 2017 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#ifndef CafShapePrs_h
#define CafShapePrs_h

#include <TDF_Label.hxx>
#include <XCAFPrs_AISObject.hxx>
#include <XCAFPrs_Style.hxx>

typedef NCollection_DataMap<TopoDS_Shape, Handle(AIS_ColoredDrawer), TopTools_ShapeMapHasher> CafDataMapOfShapeColor;
typedef NCollection_DataMap<TDF_Label, Handle(AIS_InteractiveObject)> MapOfPrsForShapes;

//! Interactive object for shape in DECAF document
class CafShapePrs : public XCAFPrs_AISObject
{
  DEFINE_STANDARD_RTTIEXT(CafShapePrs, XCAFPrs_AISObject)
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

};

#endif // CafShapePrs_h
