#include "stdafx.h"

#include "Sample2D_Rectangle.h"

IMPLEMENT_STANDARD_HANDLE(Sample2D_Rectangle,AIS2D_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(Sample2D_Rectangle,AIS2D_InteractiveObject)

Sample2D_Rectangle::Sample2D_Rectangle (Quantity_Length     aXPosition  , 
                      Quantity_Length     aYPosition  ,
                      Quantity_Length     aWidth      ,
                      Quantity_Length     aHeight     ,
                      Quantity_PlaneAngle anAngle     )
    :myArray1OfVertex(1,5),AIS2D_InteractiveObject()
{
  myXPosition   = aXPosition;
  myYPosition   = aYPosition; 
  myWidth       = aWidth ;
  myHeight      = aHeight ;
  myAngle       = anAngle;
  ComputeArrayOfVertex();

}

void Sample2D_Rectangle::SetContext(const Handle(AIS2D_InteractiveContext)& theContext)
{
  AIS2D_InteractiveObject::SetContext(theContext);

  // add the polyline in the representation, for all modes
  Handle(Graphic2d_Polyline) ThePolyline;
  ThePolyline = new Graphic2d_Polyline(this, myArray1OfVertex);
  ThePolyline->SetColorIndex ( 1 );
  ThePolyline->SetWidthIndex ( 1 );
  ThePolyline->SetTypeIndex  ( 1 );
}

void Sample2D_Rectangle::ComputeArrayOfVertex()
{
  Standard_Real BottomLeftX   = myXPosition          ;
  Standard_Real BottomLeftY   = myYPosition          ;

  Standard_Real BottomRightX  = myXPosition+(cos(myAngle)*myWidth )  ;
  Standard_Real BottomRightY  = myYPosition+(sin(myAngle)*myWidth )  ;

  Standard_Real TopRightX     = myXPosition+(cos(myAngle)*myWidth )+(-sin(myAngle)*myHeight ) ;
  Standard_Real TopRightY     = myYPosition+(sin(myAngle)*myWidth )+( cos(myAngle)*myHeight ) ;

  Standard_Real TopLeftX      = myXPosition+ (-sin(myAngle)*myHeight ) ;
  Standard_Real TopLeftY      = myYPosition+ ( cos(myAngle)*myHeight ) ;

  myArray1OfVertex.SetValue(1,Graphic2d_Vertex(BottomLeftX  , BottomLeftY  ));
  myArray1OfVertex.SetValue(2,Graphic2d_Vertex(BottomRightX , BottomRightY ));
  myArray1OfVertex.SetValue(3,Graphic2d_Vertex(TopRightX    , TopRightY    ));
  myArray1OfVertex.SetValue(4,Graphic2d_Vertex(TopLeftX     , TopLeftY     ));
  myArray1OfVertex.SetValue(5,Graphic2d_Vertex(BottomLeftX  , BottomLeftY  ));
}


