// File:	BRepAlgo_Tool.cxx
// Created:	Mon Oct 23 14:25:07 1995
// Author:	Yves FRICAUD
//		<yfr@stylox>


#include <BRepAlgo_Tool.ixx>

#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopExp.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Edge.hxx>


  

//=======================================================================
//function : Deboucle3D
//purpose  : 
//=======================================================================

TopoDS_Shape BRepAlgo_Tool::Deboucle3D(const TopoDS_Shape& S,
					 const TopTools_MapOfShape& Boundary)
{
  TopoDS_Shape SS;

  switch ( S.ShapeType()) {
  case TopAbs_FACE:
    {
    }
    break;
  case TopAbs_SHELL: 
    {
      // si le shell contient des bords libres qui n'appartiennent pas aux
      // bord libres des bouchons ( Boundary) on l'enleve.
      TopTools_IndexedDataMapOfShapeListOfShape Map;
      TopExp::MapShapesAndAncestors(S,TopAbs_EDGE,TopAbs_FACE,Map);
      
      Standard_Boolean JeGarde = Standard_True;
      for ( Standard_Integer i = 1; i <= Map.Extent() && JeGarde; i++) {
	if (Map(i).Extent() < 2) {
	  const TopoDS_Edge& anEdge = TopoDS::Edge(Map.FindKey(i));
	  if (!Boundary.Contains(anEdge)  && 
	      !BRep_Tool::Degenerated(anEdge) )
	    JeGarde = Standard_False;
	}
      }
      if ( JeGarde) SS = S;
    }
    break;
  case TopAbs_COMPOUND:  
  case TopAbs_SOLID:
    {
      // on itere sur les sous-shape et on ajoute les non vides.
      TopoDS_Iterator it(S);
      TopoDS_Shape SubShape;
      Standard_Boolean NbSub = 0;
      BRep_Builder B;
      if (S.ShapeType() == TopAbs_COMPOUND) {
	B.MakeCompound(TopoDS::Compound(SS));
      }
      else {
	B.MakeSolid(TopoDS::Solid(SS));
      }
      for ( ; it.More(); it.Next()) {
	const TopoDS_Shape& CurS = it.Value();
	SubShape = Deboucle3D(CurS,Boundary);
	if ( !SubShape.IsNull()) {
	  B.Add(SS, SubShape);
	  NbSub++;
	}
      }
      if (NbSub == 0) SS = TopoDS_Shape();
    }
    break;
  default:
    break;
  }
  return SS;
}
