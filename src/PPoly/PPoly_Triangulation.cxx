#include <PPoly_Triangulation.ixx>

//=======================================================================
//function :
//purpose  : 
//=======================================================================


PPoly_Triangulation::PPoly_Triangulation
(const Standard_Real Defl,
 const Handle(PColgp_HArray1OfPnt)& Nodes,
 const Handle(PPoly_HArray1OfTriangle)& Triangles) :
 myDeflection(Defl),
 myNodes(Nodes),
 myTriangles(Triangles)
{
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

PPoly_Triangulation::PPoly_Triangulation
(const Standard_Real Defl,
 const Handle(PColgp_HArray1OfPnt)& Nodes, 
 const Handle(PColgp_HArray1OfPnt2d)& UVNodes, 
 const Handle(PPoly_HArray1OfTriangle)& Triangles) :
 myDeflection(Defl),
 myNodes(Nodes),
 myUVNodes(UVNodes),
 myTriangles(Triangles)
{
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

Standard_Real PPoly_Triangulation::Deflection() const 
{
  return myDeflection;
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

void PPoly_Triangulation::Deflection(const Standard_Real Defl)
{
  myDeflection = Defl;
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

Standard_Integer PPoly_Triangulation::NbNodes() const 
{
  return myNodes->Length();
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

Standard_Integer PPoly_Triangulation::NbTriangles() const 
{
  return myTriangles->Length();
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

Standard_Boolean PPoly_Triangulation::HasUVNodes() const 
{
  return !myUVNodes.IsNull();
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

Handle(PColgp_HArray1OfPnt) PPoly_Triangulation::Nodes() const 
{
  return myNodes;
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

Handle(PColgp_HArray1OfPnt2d) PPoly_Triangulation::UVNodes() const 
{
  return myUVNodes;
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

Handle(PPoly_HArray1OfTriangle) PPoly_Triangulation::Triangles() const 
{
  return myTriangles;
}
