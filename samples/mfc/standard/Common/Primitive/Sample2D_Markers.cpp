#include "stdafx.h"

#include "Sample2D_Markers.h"

IMPLEMENT_STANDARD_HANDLE(Sample2D_Markers,AIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(Sample2D_Markers,AIS_InteractiveObject)

// generic marker
Sample2D_Markers::Sample2D_Markers (const Quantity_Length theXPosition , 
                   const Quantity_Length theYPosition ,
                   const Aspect_TypeOfMarker theMarkerType,
                   const Quantity_Color theColor,
                   const Standard_Real theScaleOrId)
                   :AIS_InteractiveObject(),myListVertex(1,1)
{
  myXPosition = theXPosition;
  myYPosition = theYPosition;
  myMarkerType = theMarkerType;
  myColor = theColor;
  myIndex = theScaleOrId;
}

Sample2D_Markers::Sample2D_Markers (const Quantity_Length theXPosition , 
                   const Quantity_Length theYPosition ,
                   const Graphic3d_Array1OfVertex& theListVertex,
                   const Aspect_TypeOfMarker theMarkerType,
                   const Quantity_Color theColor,
                   const Standard_Real theScaleOrId)
                   :AIS_InteractiveObject(),myListVertex(1,6)
{
  myXPosition = theXPosition;
  myYPosition = theYPosition;
  myMarkerType = theMarkerType;
  myColor = theColor;
  myIndex = theScaleOrId;
  myListVertex = theListVertex;
}


void Sample2D_Markers::Compute (  const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
                  const Handle(Prs3d_Presentation)& aPresentation,
                  const Standard_Integer aMode)
{
  if(myMarkerType == Aspect_TOM_USERDEFINED)
  {
    Handle(Graphic3d_AspectMarker3d) aMarker = new Graphic3d_AspectMarker3d(Aspect_TOM_POINT,myColor,myIndex);
    Prs3d_Root::CurrentGroup(aPresentation)->SetGroupPrimitivesAspect(aMarker);
    Prs3d_Root::CurrentGroup(aPresentation)->MarkerSet(myListVertex);
  }
  else
  {
    Handle(Graphic3d_AspectMarker3d) aMarker = new Graphic3d_AspectMarker3d(myMarkerType,myColor,myIndex);
    aPresentation->SetPrimitivesAspect(aMarker);
    Graphic3d_Vertex aV3d(myXPosition, myYPosition, 0.); 
    Prs3d_Root::CurrentGroup(aPresentation)->Marker(aV3d);
  }
}
