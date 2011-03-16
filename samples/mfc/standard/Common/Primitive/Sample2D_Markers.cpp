#include "stdafx.h"

#include "Sample2D_Markers.h"

IMPLEMENT_STANDARD_HANDLE(Sample2D_Markers,AIS2D_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(Sample2D_Markers,AIS2D_InteractiveObject)

#include "Graphic2d_Marker.hxx"
#include "Graphic2d_PolylineMarker.hxx"
#include "Graphic2d_CircleMarker.hxx"
#include "Graphic2d_EllipsMarker.hxx"

// generic marker
Sample2D_Markers::Sample2D_Markers (Standard_Integer    anIndex    , 
                                    Quantity_Length     aXPosition ,
                                    Quantity_Length     aYPosition ,
                                    Quantity_Length     aWidth     ,
                                    Quantity_Length     anHeight   ,
                                    Quantity_PlaneAngle anAngle    )
    :myListVertex(0,0),AIS2D_InteractiveObject()
{
  myCurrentTypeOfMarker = Sample2D_CTOM_Generic;
  myIndex      = anIndex;
  myXPosition  = aXPosition;
  myYPosition  = aYPosition; 
  myWidth      = aWidth    ;
  myHeight     = anHeight  ;
  myAngle      = anAngle   ; 
}


// Polyline marker
Sample2D_Markers::Sample2D_Markers (Quantity_Length          aXPosition , 
                    Quantity_Length          aYPosition ,
                    const Graphic2d_Array1OfVertex& aListVertex)
    :myListVertex(1,aListVertex.Length()),AIS2D_InteractiveObject()
{
  myCurrentTypeOfMarker = Sample2D_CTOM_Polyline;
  myXPosition  = aXPosition;
  myYPosition  = aYPosition; 
  myListVertex = aListVertex;
}

// circle marker
Sample2D_Markers::Sample2D_Markers (Quantity_Length aXPosition , 
                    Quantity_Length aYPosition ,
                    Quantity_Length X          ,
                    Quantity_Length Y          ,
   	                Quantity_Length Radius     )
    :myListVertex(0,0),AIS2D_InteractiveObject()
{
  myCurrentTypeOfMarker = Sample2D_CTOM_Circle;
  myXPosition  = aXPosition;
  myYPosition  = aYPosition; 
  myX          = X ;
  myY          = Y ;
  myRadius     =  Radius  ;
}

// ellips marker
Sample2D_Markers::Sample2D_Markers (Quantity_Length     aXPosition  , 
                    Quantity_Length     aYPosition  ,
                    Quantity_Length     X           ,
                    Quantity_Length     Y           ,
	            Quantity_Length     MajorRadius ,
                    Quantity_Length     MinorRadius ,
                    Quantity_PlaneAngle anAngle     )
    :myListVertex(0,0),AIS2D_InteractiveObject()
{
  myCurrentTypeOfMarker = Sample2D_CTOM_Ellips;
  myXPosition   = aXPosition;
  myYPosition   = aYPosition; 
  myX           = X ;
  myY           = Y ;
  myMajorRadius = MajorRadius;
  myMinorRadius = MinorRadius;
  myAngle       = anAngle;
}

void Sample2D_Markers::SetContext(const Handle(AIS2D_InteractiveContext)& theContext) 
{

	AIS2D_InteractiveObject::SetContext(theContext);

	if (myCurrentTypeOfMarker == Sample2D_CTOM_Generic) {
		if ( myIndex == 0 ) {
			Handle(Graphic2d_Marker) aMarker= 
			new Graphic2d_Marker(this,myXPosition ,myYPosition);
		}
		else {
			Handle(Graphic2d_Marker) aMarker= 
			new Graphic2d_Marker(this,myIndex,myXPosition ,myYPosition,myWidth,myHeight,myAngle);
		}

	}

 if (myCurrentTypeOfMarker == Sample2D_CTOM_Polyline)
   Handle(Graphic2d_PolylineMarker) aMarker= 
      new Graphic2d_PolylineMarker(this,myXPosition,myYPosition,myListVertex);

 if (myCurrentTypeOfMarker == Sample2D_CTOM_Circle)
   Handle(Graphic2d_CircleMarker) aMarker= 
      new Graphic2d_CircleMarker(this,myXPosition,myYPosition,myX ,myY,myRadius  );

 if (myCurrentTypeOfMarker == Sample2D_CTOM_Ellips)
   Handle(Graphic2d_EllipsMarker) aMarker= 
      new Graphic2d_EllipsMarker(this,myXPosition,myYPosition,myX,myY,myMajorRadius,myMinorRadius,myAngle );

}


