// File:	DrawPlanarDim_PlanarDimension.cxx
// Created:	Wed Jan 10 16:16:34 1996
// Author:	Denis PASCAL
//		<dp@zerox>


#include <DrawDim_PlanarDimension.ixx>
#include <Standard_DomainError.hxx>
#include <Draw_Interpretor.hxx>


//=======================================================================
//function : SetPlane
//purpose  : 
//=======================================================================

void DrawDim_PlanarDimension::SetPlane(const TopoDS_Face& plane)
{
  myPlane = plane;
}

//=======================================================================
//function : GetPlane
//purpose  : 
//=======================================================================

TopoDS_Face DrawDim_PlanarDimension::GetPlane() const 
{
  return myPlane;
}



