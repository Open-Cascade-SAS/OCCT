// File:        Poly_Triangulation.cxx
// Created:     Mon Mar  6 10:54:09 1995
// Author:      Laurent PAINNOT
//              <lpa@metrox>


#include <Poly_Triangulation.ixx>
#include <gp_Pnt.hxx>
#include <Poly_Triangle.hxx>


//=======================================================================
//function : Poly_Triangulation
//purpose  : 
//=======================================================================

Poly_Triangulation::Poly_Triangulation(const Standard_Integer NbNodes, 
                                       const Standard_Integer NbTriangles,
                                       const Standard_Boolean UVNodes) :
   myDeflection(0),
   myNbNodes(NbNodes),
   myNbTriangles(NbTriangles),
   myNodes(1, NbNodes),
   myTriangles(1, NbTriangles)
{
  if (UVNodes) myUVNodes = new TColgp_HArray1OfPnt2d(1, myNbNodes);
}

//=======================================================================
//function : Poly_Triangulation
//purpose  : 
//=======================================================================

Poly_Triangulation::Poly_Triangulation(const TColgp_Array1OfPnt&    Nodes, 
                                       const Poly_Array1OfTriangle& Triangles) :
   myDeflection(0),
   myNbNodes(Nodes.Length()),
   myNbTriangles(Triangles.Length()),
   myNodes(1, Nodes.Length()),
   myTriangles(1, Triangles.Length())
{
  myNodes = Nodes;
  myTriangles = Triangles;
}



//=======================================================================
//function : Poly_Triangulation
//purpose  : 
//=======================================================================

Poly_Triangulation::Poly_Triangulation(const TColgp_Array1OfPnt&    Nodes,
                                       const TColgp_Array1OfPnt2d&  UVNodes,
                                       const Poly_Array1OfTriangle& Triangles) :
   myDeflection(0),
   myNbNodes(Nodes.Length()),
   myNbTriangles(Triangles.Length()),
   myNodes(1, Nodes.Length()),
   myTriangles(1, Triangles.Length())
{
  myNodes = Nodes;
  myTriangles = Triangles;
  myUVNodes = new TColgp_HArray1OfPnt2d(1, myNbNodes);
  myUVNodes->ChangeArray1() = UVNodes;
}

//=======================================================================
//function : Deflection
//purpose  : 
//=======================================================================

Standard_Real Poly_Triangulation::Deflection() const 
{
  return myDeflection;
}

//=======================================================================
//function : Deflection
//purpose  : 
//=======================================================================

void Poly_Triangulation::Deflection(const Standard_Real D)
{
  myDeflection = D;
}



//=======================================================================
//function : RemoveUVNodes
//purpose  : 
//=======================================================================

void Poly_Triangulation::RemoveUVNodes()
{
  myUVNodes.Nullify();
}

//=======================================================================
//function : Nodes
//purpose  : 
//=======================================================================

const TColgp_Array1OfPnt& Poly_Triangulation::Nodes() const 
{
  return myNodes;
}

//=======================================================================
//function : ChangeNodes
//purpose  : 
//=======================================================================

TColgp_Array1OfPnt& Poly_Triangulation::ChangeNodes()
{
  return myNodes;
}

//=======================================================================
//function : UVNodes
//purpose  : 
//=======================================================================

const TColgp_Array1OfPnt2d& Poly_Triangulation::UVNodes() const 
{
  return myUVNodes->Array1();
}

//=======================================================================
//function : ChangeUVNodes
//purpose  : 
//=======================================================================

TColgp_Array1OfPnt2d& Poly_Triangulation::ChangeUVNodes()
{
  return myUVNodes->ChangeArray1();
}

//=======================================================================
//function : Triangles
//purpose  : 
//=======================================================================

const Poly_Array1OfTriangle& Poly_Triangulation::Triangles() const 
{
  return myTriangles;
}

//=======================================================================
//function : ChangeTriangles
//purpose  : 
//=======================================================================

Poly_Array1OfTriangle& Poly_Triangulation::ChangeTriangles()
{
  return myTriangles;
}


//=======================================================================
//function : SetNormals
//purpose  : 
//=======================================================================

void Poly_Triangulation::SetNormals
                        (const Handle(TShort_HArray1OfShortReal)& theNormals)
{

  if(theNormals.IsNull() || theNormals->Length() != 3*myNbNodes) {
    Standard_DomainError::Raise("Poly_Triangulation::SetNormals : wrong length");
  }

  myNormals = theNormals;

}

//=======================================================================
//function : Normals
//purpose  : 
//=======================================================================

const TShort_Array1OfShortReal& Poly_Triangulation::Normals() const
{

  if(myNormals.IsNull() || myNormals->Length() != 3*myNbNodes) {
    Standard_NullObject::Raise("Poly_Triangulation::Normals : "
                               "wrong length or null array");
  }

  return myNormals->Array1();

}

//=======================================================================
//function : ChangeNormals
//purpose  : 
//=======================================================================

TShort_Array1OfShortReal& Poly_Triangulation::ChangeNormals() 
{

  if(myNormals.IsNull() || myNormals->Length() != 3*myNbNodes) {
    Standard_NullObject::Raise("Poly_Triangulation::ChangeNormals : "
                               "wrong length or null array");
  }

  return myNormals->ChangeArray1();

}

//=======================================================================
//function : HasNormals
//purpose  : 
//=======================================================================

Standard_Boolean Poly_Triangulation::HasNormals() const
{

  if(myNormals.IsNull() || myNormals->Length() != 3*myNbNodes) {
    return Standard_False;
  }
  return Standard_True;
}



