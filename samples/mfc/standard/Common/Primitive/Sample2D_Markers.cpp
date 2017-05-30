#include "stdafx.h"

#include "Sample2D_Markers.h"

IMPLEMENT_STANDARD_RTTIEXT(Sample2D_Markers,AIS_InteractiveObject)

// generic marker
Sample2D_Markers::Sample2D_Markers (const Standard_Real theXPosition,
                   const Standard_Real theYPosition,
                   const Aspect_TypeOfMarker theMarkerType,
                   const Quantity_Color theColor,
                   const Standard_Real theScaleOrId)
                   :AIS_InteractiveObject(),myArrayOfPoints (new Graphic3d_ArrayOfPoints (1))
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
                   :AIS_InteractiveObject(),myArrayOfPoints (new Graphic3d_ArrayOfPoints (6))
{
  myXPosition = theXPosition;
  myYPosition = theYPosition;
  myMarkerType = theMarkerType;
  myColor = theColor;
  myIndex = theScaleOrId;
  myArrayOfPoints = theArrayOfPoints;
}


void Sample2D_Markers::Compute (const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
                                const Handle(Prs3d_Presentation)& aPresentation,
                                const Standard_Integer /*aMode*/)
{
  if(myMarkerType == Aspect_TOM_USERDEFINED)
  {
    Handle(Graphic3d_AspectMarker3d) aMarker = new Graphic3d_AspectMarker3d(Aspect_TOM_POINT,myColor,myIndex);
    Prs3d_Root::CurrentGroup(aPresentation)->SetGroupPrimitivesAspect(aMarker);
    Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray (myArrayOfPoints);
  }
  else
  {
    Handle(Graphic3d_AspectMarker3d) aMarker = new Graphic3d_AspectMarker3d(myMarkerType,myColor,myIndex);
    Prs3d_Root::CurrentGroup (aPresentation)->SetPrimitivesAspect(aMarker);
    Handle(Graphic3d_ArrayOfPoints) anArrayOfPoints = new Graphic3d_ArrayOfPoints (1);
    anArrayOfPoints->AddVertex (myXPosition, myYPosition, 0);
    Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray (anArrayOfPoints);
  }
}
