#include "stdafx.h"

#include "Sample2D_Face.h"

IMPLEMENT_STANDARD_HANDLE(Sample2D_Face,AIS2D_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(Sample2D_Face,AIS2D_InteractiveObject)

Sample2D_Face::Sample2D_Face (TopoDS_Face& aFace)
    :AIS2D_InteractiveObject(),myFace(aFace)

{
myMarkerIndex        = 1;
myMarkerWidth        = 5;
myMarkerHeight       = 5;
myFORWARDColorIndex  = 2;
myREVERSEDColorIndex = 3;
myINTERNALColorIndex = 4;
myEXTERNALColorIndex = 5;
myWidthIndex         = 1;
myTypeIndex          = 1;
}


#include "TopExp_Explorer.hxx"
#include "BRep_Tool.hxx"
#include "TopoDS.hxx"
#include "TopoDS_Edge.hxx"
#include "TopoDS.hxx"
#include "TopoDS_Vertex.hxx"
#include "TopExp.hxx"
#include "Graphic2d_Marker.hxx"
#include "Graphic2d_PolylineMarker.hxx"
#include "gp.hxx"
#include "Prs2d_AspectLine.hxx"
#include "Graphic2d_TypeOfPolygonFilling.hxx"

void Sample2D_Face::SetContext(const Handle(AIS2D_InteractiveContext)& theContext) 
{
    if(myFace.IsNull() || myFace.ShapeType()!=TopAbs_FACE) return;

    AIS2D_InteractiveObject::SetContext(theContext);	

    Handle(Prs2d_AspectLine) aLineAspect_FORWARD = new Prs2d_AspectLine;  
    
	aLineAspect_FORWARD->SetTypeOfFill(Graphic2d_TOPF_EMPTY);
    aLineAspect_FORWARD->SetTypeIndex(myTypeIndex);
    aLineAspect_FORWARD->SetWidthIndex(myWidthIndex);
    aLineAspect_FORWARD->SetColorIndex(myFORWARDColorIndex);

	Handle(Prs2d_AspectLine) aLineAspect_REVERSED = new Prs2d_AspectLine;  
    aLineAspect_REVERSED->SetTypeOfFill(Graphic2d_TOPF_EMPTY);
    aLineAspect_REVERSED->SetTypeIndex(myTypeIndex);
    aLineAspect_REVERSED->SetWidthIndex(myWidthIndex);
    aLineAspect_REVERSED->SetColorIndex(myREVERSEDColorIndex);

	Handle(Prs2d_AspectLine) aLineAspect_INTERNAL = new Prs2d_AspectLine;  
    aLineAspect_INTERNAL->SetTypeOfFill(Graphic2d_TOPF_EMPTY);
    aLineAspect_INTERNAL->SetTypeIndex(myTypeIndex);
    aLineAspect_INTERNAL->SetWidthIndex(myWidthIndex);
    aLineAspect_INTERNAL->SetColorIndex(myINTERNALColorIndex);

	Handle(Prs2d_AspectLine) aLineAspect_EXTERNAL = new Prs2d_AspectLine;  
    aLineAspect_EXTERNAL->SetTypeOfFill(Graphic2d_TOPF_EMPTY);
    aLineAspect_EXTERNAL->SetTypeIndex(myTypeIndex);
    aLineAspect_EXTERNAL->SetWidthIndex(myWidthIndex);
    aLineAspect_EXTERNAL->SetColorIndex(myEXTERNALColorIndex);

    // pcurves of a face

    Handle(Graphic2d_SetOfCurves) segment_FORWARD;
    segment_FORWARD = new Graphic2d_SetOfCurves(this); 

    Handle(Graphic2d_SetOfCurves) segment_REVERSED;
    segment_REVERSED = new Graphic2d_SetOfCurves(this); 

    Handle(Graphic2d_SetOfCurves) segment_INTERNAL;
    segment_INTERNAL = new Graphic2d_SetOfCurves(this); 

    Handle(Graphic2d_SetOfCurves) segment_EXTERNAL;
    segment_EXTERNAL = new Graphic2d_SetOfCurves(this); 

    Standard_Real f,l;
    Standard_Integer i = 1;
    myFace.Orientation(TopAbs_FORWARD);
    TopExp_Explorer ex(myFace,TopAbs_EDGE);
    while (ex.More()) {
      const Handle(Geom2d_Curve) Curve = BRep_Tool::CurveOnSurface
	         (TopoDS::Edge(ex.Current()),TopoDS::Face(myFace),f,l);

      Handle(Geom2d_TrimmedCurve) c = new Geom2d_TrimmedCurve(Curve,f,l);

      if (c.IsNull())
      {
        // null curve --> EDGE have a null lenght
        // --> get first and last Vertices
        TopoDS_Edge CurrentEdge= TopoDS::Edge(ex.Current());

        TopoDS_Vertex FirstVertex = TopExp::FirstVertex(CurrentEdge);
        TopoDS_Vertex LastVertex  = TopExp::LastVertex(CurrentEdge);

        if (FirstVertex != LastVertex)
          Standard_Failure::Raise("Null Edge pcurve But different vertices");

         gp_Pnt aPnt  = BRep_Tool::Pnt(FirstVertex);


        Handle(Graphic2d_Marker) aMarker= 
           new Graphic2d_Marker(this,myMarkerIndex,
                                aPnt.X() ,aPnt.Y(),
                                myMarkerWidth,myMarkerHeight,0);

        switch (ex.Current().Orientation())
        {
          case TopAbs_FORWARD:  aMarker->SetColorIndex (myFORWARDColorIndex);   break;
          case TopAbs_REVERSED: aMarker->SetColorIndex (myREVERSEDColorIndex);  break;
          case TopAbs_INTERNAL: aMarker->SetColorIndex (myINTERNALColorIndex);  break;
          case TopAbs_EXTERNAL: aMarker->SetColorIndex (myEXTERNALColorIndex);  break;
          default : break;
        }
        ex.Next();
      }
      else
      {
        gp_Pnt2d p1,p2;
        gp_Vec2d v;
        c->D1(l,p1,v);
        Graphic2d_Array1OfVertex aListVertex(1,3);
        if (v.Magnitude() > gp::Resolution()) {
            Standard_Real L = 2;
            Standard_Real H = 1;
            gp_Dir2d d(v);
            p2.SetCoord(- L*d.X() - H*d.Y(),- L*d.Y() + H*d.X());
            aListVertex(1)=Graphic2d_Vertex(p2.X(),p2.Y());
            
            p2.SetCoord(- L*d.X() + H*d.Y(),- L*d.Y() - H*d.X());
            aListVertex(2)=Graphic2d_Vertex(0.,0.);
            aListVertex(3)=Graphic2d_Vertex(p2.X(),p2.Y()); 
        }

        Handle(Graphic2d_PolylineMarker) aMarker= 
          new Graphic2d_PolylineMarker(this,p1.X(),p1.Y(),aListVertex);

		// Display the Curve

		switch (ex.Current().Orientation())
        {
		case TopAbs_FORWARD: {
             segment_FORWARD->Add(c); 
             SetAspect(aLineAspect_FORWARD, aMarker);  
             break;
		}
		case TopAbs_REVERSED: {
             segment_REVERSED->Add(c); 
             SetAspect(aLineAspect_REVERSED, aMarker); 
             break;
		}
		case TopAbs_INTERNAL: {
             segment_INTERNAL->Add(c); 
             SetAspect(aLineAspect_INTERNAL, aMarker);
             break;
		}
		case TopAbs_EXTERNAL: {
             segment_EXTERNAL->Add(c); 
             SetAspect(aLineAspect_EXTERNAL, aMarker);
             break;
		}
          default : break;
        }
      }
      ex.Next();
    }



	SetAspect(aLineAspect_FORWARD, segment_FORWARD);
	SetAspect(aLineAspect_REVERSED, segment_REVERSED);
    SetAspect(aLineAspect_INTERNAL, segment_INTERNAL);
	SetAspect(aLineAspect_EXTERNAL, segment_EXTERNAL);
}




