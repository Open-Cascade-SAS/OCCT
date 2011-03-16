#include "stdafx.h"

#include "Sample2D_Line.h"

IMPLEMENT_STANDARD_HANDLE(Sample2D_Line,AIS2D_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(Sample2D_Line,AIS2D_InteractiveObject)

Sample2D_Line::Sample2D_Line ()
    :AIS2D_InteractiveObject()
{
}

void Sample2D_Line::SetContext(const Handle(AIS2D_InteractiveContext)& theContext)
{
  AIS2D_InteractiveObject::SetContext(theContext);

  Handle(Prs2d_AspectLine) aLineAspect = new Prs2d_AspectLine;  
  aLineAspect->SetTypeOfFill(Graphic2d_TOPF_FILLED);
  aLineAspect->SetWidthIndex(0);

  for (int i=0;i<=13;i++)
   for (int j=0;j<=5;j++)
   {
		Graphic2d_Array1OfVertex anArrayVertex(1,5);
		anArrayVertex(1)=Graphic2d_Vertex(20.*i,10.*j+0);
		anArrayVertex(2)=Graphic2d_Vertex(20.*i+10,10.*j+0);
		anArrayVertex(3)=Graphic2d_Vertex(20.*i+10,10.*j+5);
		anArrayVertex(4)=Graphic2d_Vertex(20.*i,10.*j+5);
		anArrayVertex(5)=Graphic2d_Vertex(20.*i,10.*j+0);
		Handle(Graphic2d_Polyline) aPolyline = new Graphic2d_Polyline(this,anArrayVertex);
		
		SetAspect(aLineAspect, aPolyline);

		aPolyline->SetInteriorColorIndex(i); // to use with Graphic2d_TOPF_FILLED
		aPolyline->SetTypeIndex(j);          // type of the polyline		
	}
}




