
#include <PBRep_Polygon3D.ixx>



//=======================================================================
//function : PBRep_Polygon3D::PBRep_Polygon3D
//purpose  : 
//=======================================================================

PBRep_Polygon3D::PBRep_Polygon3D(const Handle(PPoly_Polygon3D)& aPPol,
				 const PTopLoc_Location&        aPLoc) : 
    PBRep_CurveRepresentation(aPLoc),
    myPolygon3D(aPPol)
     
{
}

//=======================================================================
//function : PBRep_Polygon3D::IsPolygon3D
//purpose  : 
//=======================================================================

Standard_Boolean PBRep_Polygon3D::IsPolygon3D() const 
{
  return Standard_True;
}

//=======================================================================
//function : PBRep_Polygon3D::IsPolygon3D
//purpose  : 
//=======================================================================

Handle(PPoly_Polygon3D) PBRep_Polygon3D::Polygon3D() const 
{
  return myPolygon3D;
}

