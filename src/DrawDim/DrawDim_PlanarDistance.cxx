// File:	DrawDim_PlanarDistance.cxx
// Created:	Wed Jan 10 14:25:34 1996
// Author:	Denis PASCAL
//		<dp@zerox>


#include <DrawDim_PlanarDistance.ixx>
#include <DrawDim.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>  
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx> 
#include <gp_Lin.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <TopExp.hxx>



//=======================================================================
//function : Draw
//purpose  : 
//=======================================================================

void DrawDim_PlanarDistance::Draw 
(const gp_Pnt& point, const TopoDS_Edge& edge, Draw_Display& dis) const
{    
  Standard_Real f,l;
  Handle(Geom_Curve) line = BRep_Tool::Curve(edge,f,l);
  GeomAPI_ProjectPointOnCurve pj (point,Handle(Geom_Curve)::DownCast(line));
  if (pj.NbPoints() == 1) { 
    gp_Pnt first = point;
    gp_Pnt last = pj.Point(1);
    dis.Draw (first,last);  

    gp_Pnt p ((first.X()+ last.X())/2,(first.Y()+ last.Y())/2,(first.Z()+ last.Z())/2);
    DrawText(p,dis);
  }
}

//=======================================================================
//function : DrawDim_PlanarDistance
//purpose  : 
//=======================================================================

DrawDim_PlanarDistance::DrawDim_PlanarDistance (const TopoDS_Face& face,
						const TopoDS_Shape& geom1, 
						const TopoDS_Shape& geom2)
{
  myPlane = face; myGeom1 = geom1; myGeom2 = geom2;
}

//=======================================================================
//function : DrawDim_PlanarDistance
//purpose  : 
//=======================================================================

DrawDim_PlanarDistance::DrawDim_PlanarDistance (const TopoDS_Shape& geom1, 
						const TopoDS_Shape&  geom2)
{  
  myGeom1 = geom1; myGeom2 = geom2;
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void DrawDim_PlanarDistance::DrawOn(Draw_Display& dis) const 
{
  if (myGeom1.ShapeType() == TopAbs_VERTEX && myGeom2.ShapeType() == TopAbs_VERTEX) {
    gp_Pnt first = BRep_Tool::Pnt(TopoDS::Vertex(myGeom1));
    gp_Pnt last  = BRep_Tool::Pnt(TopoDS::Vertex(myGeom2));  
    dis.Draw (first,last);  

    gp_Pnt p ((first.X()+ last.X())/2,(first.Y()+ last.Y())/2,(first.Z()+ last.Z())/2);
    DrawText(p,dis);
    return;
  }

  else if (myGeom1.ShapeType() == TopAbs_VERTEX && myGeom2.ShapeType() == TopAbs_EDGE) {
    gp_Pnt point = BRep_Tool::Pnt(TopoDS::Vertex(myGeom1)); 
    Draw (point,TopoDS::Edge(myGeom2),dis);
    return;
  }  

  else if (myGeom1.ShapeType() == TopAbs_EDGE && myGeom2.ShapeType() == TopAbs_VERTEX) {
    gp_Pnt point = BRep_Tool::Pnt(TopoDS::Vertex(myGeom2)); 
    Draw (point,TopoDS::Edge(myGeom1),dis);
    return;
  }

  else if (myGeom1.ShapeType() == TopAbs_EDGE && myGeom2.ShapeType() == TopAbs_EDGE) {
    Standard_Real f,l;
    Handle(Geom_Curve) C = BRep_Tool::Curve (TopoDS::Edge(myGeom1),f,l);  
    if (!C.IsNull()) {
      Handle(Geom_Line) L  = Handle(Geom_Line)::DownCast(C);  
      if (!L.IsNull()) {
	gp_Pnt point = L->Lin().Location();  
	TopoDS_Edge edge = TopoDS::Edge(myGeom2); 
	Draw (point,edge,dis);
	return;
      }
    }
  } 
  cout << " DrawDim_PlanarDistance::DrawOn : dimension error" << endl;
}
