
#include <PPoly_Polygon2D.ixx>

PPoly_Polygon2D::PPoly_Polygon2D(const Handle(PColgp_HArray1OfPnt2d)& Nodes,
				 const Standard_Real Defl) :
				 myDeflection(Defl),
				 myNodes(Nodes)
{
}

Standard_Real PPoly_Polygon2D::Deflection() const 
{
  return myDeflection;
}

void PPoly_Polygon2D::Deflection(const Standard_Real Defl)
{
  myDeflection = Defl;
}

Standard_Integer PPoly_Polygon2D::NbNodes() const 
{
  return myNodes->Length();

}

Handle(PColgp_HArray1OfPnt2d) PPoly_Polygon2D::Nodes() const 
{
  return myNodes;
}

void PPoly_Polygon2D::Nodes(const Handle(PColgp_HArray1OfPnt2d)& Nodes)
{
  myNodes = Nodes;
}
