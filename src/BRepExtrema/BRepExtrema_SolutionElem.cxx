// File:	BRepExtrema_SolutionElem.cxx
// Created:	Mon Apr 22 17:03:37 1996
// Author:	Maria PUMBORIOS
// Author:      Herve LOUESSARD 
//		<mps@sgi64>



#include <BRepExtrema_SolutionElem.ixx>

/*********************************************************************************/
/*********************************************************************************/

BRepExtrema_SolutionElem::BRepExtrema_SolutionElem()
: myDist(0.), myPoint(0.,0.,0.), mySupType(BRepExtrema_IsVertex), myPar1(0.), myPar2(0.)
{
} 

/*********************************************************************************/

BRepExtrema_SolutionElem::BRepExtrema_SolutionElem(const Standard_Real d, 
                                                   const gp_Pnt& Pt, 
                                                   const BRepExtrema_SupportType SolType, 
                                                   const TopoDS_Vertex& vertex)
: myDist(d), myPoint(Pt), mySupType(SolType), myVertex(vertex), myPar1(0.), myPar2(0.)
{ 
}

/*********************************************************************************/

 BRepExtrema_SolutionElem::BRepExtrema_SolutionElem(const Standard_Real d, 
						      const gp_Pnt& Pt, 
						      const BRepExtrema_SupportType SolType, 
						      const TopoDS_Edge& edge, 
						      const Standard_Real t)
: myDist(d), myPoint(Pt), mySupType(SolType), myEdge(edge), myPar1(t), myPar2(0.)
{ 
}

/*********************************************************************************/

 BRepExtrema_SolutionElem::BRepExtrema_SolutionElem(const Standard_Real d, 
						      const gp_Pnt& Pt, 
						      const BRepExtrema_SupportType SolType, 
						      const TopoDS_Face& face, 
						      const Standard_Real u, 
						      const Standard_Real v)
: myDist(d), myPoint(Pt), mySupType(SolType), myFace(face), myPar1(u), myPar2(v)
{ 
}

/*********************************************************************************/

Standard_Real BRepExtrema_SolutionElem::Dist() const 
{ return(myDist);
}

/*********************************************************************************/

gp_Pnt BRepExtrema_SolutionElem::Point() const 
{ return (myPoint);
}

/*********************************************************************************/

BRepExtrema_SupportType BRepExtrema_SolutionElem::SupportKind() const 
{ return (mySupType);
}

/*********************************************************************************/

TopoDS_Vertex BRepExtrema_SolutionElem::Vertex() const 
{ return (myVertex);
}

/*********************************************************************************/

TopoDS_Edge BRepExtrema_SolutionElem::Edge() const 
{ return (myEdge);
}

/*********************************************************************************/

TopoDS_Face BRepExtrema_SolutionElem::Face() const 
{ return (myFace); 
}

/*********************************************************************************/

void BRepExtrema_SolutionElem::EdgeParameter(Standard_Real& par1) const
 { par1 = myPar1;
}

/*********************************************************************************/

void BRepExtrema_SolutionElem::FaceParameter(Standard_Real& par1, Standard_Real& par2) const
{ par1 = myPar1;
  par2 = myPar2;
}

/*********************************************************************************/
/*********************************************************************************/
