#include <PPoly_Polygon3D.ixx>

//=======================================================================
//function :
//purpose  : 
//=======================================================================


PPoly_Polygon3D::PPoly_Polygon3D(const Handle(PColgp_HArray1OfPnt)& Nodes,
				 const Standard_Real Defl) :
				 myDeflection(Defl),
				 myNodes(Nodes)
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================


PPoly_Polygon3D::PPoly_Polygon3D(const Handle(PColgp_HArray1OfPnt)& Nodes,
				 const Handle(PColStd_HArray1OfReal)& Param,
				 const Standard_Real Defl) :
				 myDeflection(Defl),
				 myNodes(Nodes),
				 myParameters(Param)
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================


Standard_Real PPoly_Polygon3D::Deflection() const 
{
  return myDeflection;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================


void PPoly_Polygon3D::Deflection(const Standard_Real Defl)
{
  myDeflection = Defl;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================


Standard_Integer PPoly_Polygon3D::NbNodes() const 
{
  return myNodes->Length();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================


Handle(PColgp_HArray1OfPnt) PPoly_Polygon3D::Nodes() const 
{
  return myNodes;
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

void PPoly_Polygon3D::Nodes(const Handle(PColgp_HArray1OfPnt)& Nodes)
{
  myNodes = Nodes;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

Standard_Boolean PPoly_Polygon3D::HasParameters() const 
{
  return (!myParameters.IsNull());
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

void PPoly_Polygon3D::Parameters(const Handle(PColStd_HArray1OfReal)& Param)
{
  myParameters = Param;
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

Handle(PColStd_HArray1OfReal) PPoly_Polygon3D::Parameters() const 
{
  return myParameters;
}

