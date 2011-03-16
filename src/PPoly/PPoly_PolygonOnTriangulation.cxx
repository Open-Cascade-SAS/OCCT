

#include <PPoly_PolygonOnTriangulation.ixx>

//=======================================================================
//function : PPoly_PolygonOnTriangulation
//purpose  : 
//=======================================================================

PPoly_PolygonOnTriangulation::PPoly_PolygonOnTriangulation()
{
}

//=======================================================================
//function : PPoly_PolygonOnTriangulation
//purpose  : 
//=======================================================================

PPoly_PolygonOnTriangulation::PPoly_PolygonOnTriangulation
(const Handle(PColStd_HArray1OfInteger)& Nodes,
 const Standard_Real                     Defl) :
    myDeflection(Defl),
    myNodes(Nodes)
{
}

//=======================================================================
//function : PPoly_PolygonOnTriangulation
//purpose  : 
//=======================================================================

PPoly_PolygonOnTriangulation::PPoly_PolygonOnTriangulation
(const Handle(PColStd_HArray1OfInteger)& Nodes,
 const Standard_Real                     Defl,
 const Handle(PColStd_HArray1OfReal)&    Param) :
    myDeflection(Defl),
    myNodes(Nodes),
    myParameters(Param)
{
}

//=======================================================================
//function : Deflection
//purpose  : 
//=======================================================================

Standard_Real PPoly_PolygonOnTriangulation::Deflection() const 
{
  return myDeflection;
}

//=======================================================================
//function : Deflection
//purpose  : 
//=======================================================================

void PPoly_PolygonOnTriangulation::Deflection(const Standard_Real D)
{
  myDeflection  = D;
}

//=======================================================================
//function : NbNodes
//purpose  : 
//=======================================================================

Standard_Integer PPoly_PolygonOnTriangulation::NbNodes() const 
{
  return myNodes->Length();
}

//=======================================================================
//function : Nodes
//purpose  : 
//=======================================================================

Handle(PColStd_HArray1OfInteger) PPoly_PolygonOnTriangulation::Nodes() const 
{
  return myNodes;
}

//=======================================================================
//function : Nodes
//purpose  : 
//=======================================================================

void PPoly_PolygonOnTriangulation::Nodes(const Handle(PColStd_HArray1OfInteger)& Nodes)
{
  myNodes = Nodes;
}


//=======================================================================
//function : HasParameters
//purpose  : 
//=======================================================================

Standard_Boolean PPoly_PolygonOnTriangulation::HasParameters() const 
{
  return (!myParameters.IsNull());
}

//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================

Handle(PColStd_HArray1OfReal) PPoly_PolygonOnTriangulation::Parameters() const 
{
  return myParameters;
}

//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================

void PPoly_PolygonOnTriangulation::Parameters(const Handle(PColStd_HArray1OfReal)& Param) 
{
  myParameters = Param;
}

