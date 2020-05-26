// Copyright (c) 2020 OPEN CASCADE SAS
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

#include "Sample2D_Markers.h"

#include <Aspect_TypeOfMarker.hxx>

// generic marker
Sample2D_Markers::Sample2D_Markers (const Standard_Real theXPosition,
                                    const Standard_Real theYPosition,
                                    const Aspect_TypeOfMarker theMarkerType,
                                    const Quantity_Color theColor,
                                    const Standard_Real theScaleOrId)
: myArrayOfPoints (new Graphic3d_ArrayOfPoints(1))
{
  myXPosition = theXPosition;
  myYPosition = theYPosition;
  myMarkerType = theMarkerType;
  myColor = theColor;
  myIndex = theScaleOrId;
}

Sample2D_Markers::Sample2D_Markers (const Standard_Real theXPosition,
                                    const Standard_Real theYPosition,
                                    const Handle(Graphic3d_ArrayOfPoints)& theArrayOfPoints,
                                    const Aspect_TypeOfMarker theMarkerType,
                                    const Quantity_Color theColor,
                                    const Standard_Real theScaleOrId)
: myArrayOfPoints (new Graphic3d_ArrayOfPoints(6))
{
  myXPosition = theXPosition;
  myYPosition = theYPosition;
  myMarkerType = theMarkerType;
  myColor = theColor;
  myIndex = theScaleOrId;
  myArrayOfPoints = theArrayOfPoints;
}

void Sample2D_Markers::Compute (const Handle(PrsMgr_PresentationManager3d)& ,
                                const Handle(Prs3d_Presentation)& thePrs,
                                const Standard_Integer theMode)
{
  if (theMode != 0)
  {
    return;
  }

  if (myMarkerType == Aspect_TOM_USERDEFINED)
  {
    Handle(Graphic3d_AspectMarker3d) aMarker = new Graphic3d_AspectMarker3d(Aspect_TOM_POINT, myColor, myIndex);
    thePrs->CurrentGroup()->SetGroupPrimitivesAspect(aMarker);
    thePrs->CurrentGroup()->AddPrimitiveArray(myArrayOfPoints);
  }
  else
  {
    Handle(Graphic3d_AspectMarker3d) aMarker = new Graphic3d_AspectMarker3d(myMarkerType, myColor, myIndex);
    thePrs->CurrentGroup()->SetPrimitivesAspect(aMarker);
    Handle(Graphic3d_ArrayOfPoints) anArrayOfPoints = new Graphic3d_ArrayOfPoints(1);
    anArrayOfPoints->AddVertex(myXPosition, myYPosition, 0);
    thePrs->CurrentGroup()->AddPrimitiveArray(anArrayOfPoints);
  }
}
