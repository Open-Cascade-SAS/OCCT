// Created on: 2016-02-04
// Created by: Anastasia BORISOVA
// Copyright (c) 2016 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <Prs3d_ToolQuadric.hxx>

#include <gp_Quaternion.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <TColgp_Array1OfPnt.hxx>

//=======================================================================
//function : FIllArray
//purpose  :
//=======================================================================
void Prs3d_ToolQuadric::FillArray (Handle(Graphic3d_ArrayOfTriangles)& theArray,
                                   const gp_Trsf& theTrsf) const
{
  if (theArray.IsNull())
  {
    theArray = new Graphic3d_ArrayOfTriangles (VerticesNb(), TrianglesNb() * 3, Graphic3d_ArrayFlags_VertexNormal);
  }

  const Standard_Real aStepU = 1.0f / mySlicesNb;
  const Standard_Real aStepV = 1.0f / myStacksNb;
  if (theArray->EdgeNumberAllocated() > 0)
  {
    // indexed array
    for (Standard_Integer aU = 0; aU <= mySlicesNb; ++aU)
    {
      const Standard_Real aParamU = aU * aStepU;
      for (Standard_Integer aV = 0; aV <= myStacksNb; ++aV)
      {
        const Standard_Real aParamV = aV * aStepV;
        const gp_Pnt aVertex = Vertex (aParamU, aParamV).Transformed (theTrsf);
        const gp_Dir aNormal = Normal (aParamU, aParamV).Transformed (theTrsf);
        theArray->AddVertex (aVertex, aNormal);

        if (aU != 0 && aV != 0)
        {
          const int aVertId = theArray->VertexNumber();
          theArray->AddTriangleEdges (aVertId, aVertId - myStacksNb - 2, aVertId - 1);
          theArray->AddTriangleEdges (aVertId - myStacksNb - 2, aVertId, aVertId - myStacksNb - 1);
        }
      }
    }
  }
  else
  {
    // non-indexed array
    for (Standard_Integer aU = 0; aU < mySlicesNb; ++aU)
    {
      const Standard_Real aParamU = aU * aStepU;
      for (Standard_Integer aV = 0; aV < myStacksNb; ++aV)
      {
        const Standard_Real aParamV = aV * aStepV;
        theArray->AddVertex (Vertex (aParamU, aParamV).Transformed (theTrsf),
                             Normal (aParamU, aParamV).Transformed (theTrsf));
        theArray->AddVertex (Vertex (aParamU + aStepU, aParamV).Transformed (theTrsf),
                             Normal (aParamU + aStepU, aParamV).Transformed (theTrsf));
        theArray->AddVertex (Vertex (aParamU + aStepU, aParamV + aStepV).Transformed (theTrsf),
                             Normal (aParamU + aStepU, aParamV + aStepV).Transformed (theTrsf));
        theArray->AddVertex (Vertex (aParamU + aStepU, aParamV + aStepV).Transformed (theTrsf),
                             Normal (aParamU + aStepU, aParamV + aStepV).Transformed (theTrsf));
        theArray->AddVertex (Vertex (aParamU, aParamV + aStepV).Transformed (theTrsf),
                             Normal (aParamU, aParamV + aStepV).Transformed (theTrsf));
        theArray->AddVertex (Vertex (aParamU, aParamV).Transformed (theTrsf),
                             Normal (aParamU, aParamV).Transformed (theTrsf));
      }
    }
  }
}

//=======================================================================
//function : CreateTriangulation
//purpose  :
//=======================================================================
Handle(Graphic3d_ArrayOfTriangles) Prs3d_ToolQuadric::CreateTriangulation (const gp_Trsf& theTrsf) const
{
  Handle(Graphic3d_ArrayOfTriangles) aTriangulation;
  FillArray (aTriangulation, theTrsf);
  return aTriangulation;
}

//=======================================================================
//function : CreatePolyTriangulation
//purpose  :
//=======================================================================
Handle(Poly_Triangulation) Prs3d_ToolQuadric::CreatePolyTriangulation (const gp_Trsf& theTrsf) const
{
  Handle(Poly_Triangulation) aTriangulation = new Poly_Triangulation (VerticesNb(), TrianglesNb(), Standard_False);
  TColgp_Array1OfPnt& aNodes = aTriangulation->ChangeNodes();
  Poly_Array1OfTriangle& aTriangles = aTriangulation->ChangeTriangles();

  Standard_ShortReal aStepU = 1.0f / mySlicesNb;
  Standard_ShortReal aStepV = 1.0f / myStacksNb;

  // Fill triangles
  for (Standard_Integer aU = 0, anIndex = 0; aU <= mySlicesNb; ++aU)
  {
    const Standard_Real aParamU = aU * aStepU;
    for (Standard_Integer aV = 0; aV <= myStacksNb; ++aV)
    {
      const Standard_ShortReal aParamV = aV * aStepV;
      const Standard_Integer   aVertId = aU * (myStacksNb + 1) + (aV + 1);
      gp_Pnt aVertex = Vertex (aParamU, aParamV).Transformed (theTrsf);

      aNodes.SetValue (aVertId, aVertex);
      if (aU != 0 && aV != 0)
      {
        aTriangles.SetValue (++anIndex, Poly_Triangle (aVertId, aVertId - myStacksNb - 2, aVertId - 1));
        aTriangles.SetValue (++anIndex, Poly_Triangle (aVertId - myStacksNb - 2, aVertId, aVertId - myStacksNb - 1));
      }
    }
  }
  return aTriangulation;
}

//=======================================================================
//function : FillArray
//purpose  :
//=======================================================================
void Prs3d_ToolQuadric::FillArray (Handle(Graphic3d_ArrayOfTriangles)& theArray,
                                   Handle(Poly_Triangulation)& theTriangulation,
                                   const gp_Trsf& theTrsf) const
{
  theArray = CreateTriangulation (theTrsf);
  theTriangulation = CreatePolyTriangulation (theTrsf);
}
