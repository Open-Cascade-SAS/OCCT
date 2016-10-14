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
//function : fillArrays
//purpose  :
//=======================================================================
void Prs3d_ToolQuadric::fillArrays (const gp_Trsf& theTrsf, TColgp_Array1OfPnt& theArray, NCollection_Array1<gp_Dir>& theNormals)
{
  Standard_ShortReal aStepU = 1.0f / mySlicesNb;
  Standard_ShortReal aStepV = 1.0f / myStacksNb;

  for (Standard_Integer aU = 0; aU <= mySlicesNb; aU++)
  {
    const Standard_Real aParamU = aU * aStepU;
    for (Standard_Integer aV = 0; aV <= myStacksNb; aV++)
    {
      const Standard_ShortReal aParamV = aV * aStepV;
      const Standard_Integer   aVertId = aU * (myStacksNb + 1) + aV + 1;
      gp_Pnt aVertex = Vertex(aParamU, aParamV);
      gp_Dir aNormal = Normal(aParamU, aParamV);

      aVertex.Transform (theTrsf);
      aNormal.Transform (theTrsf);

      theArray.SetValue (aVertId, aVertex);
      theNormals.SetValue (aVertId, aNormal);
    }
  }
}

//=======================================================================
//function : FIllArray
//purpose  :
//=======================================================================
void Prs3d_ToolQuadric::FillArray (Handle(Graphic3d_ArrayOfTriangles)& theArray, const gp_Trsf& theTrsf)
{
  const Standard_Integer aTrianglesNb = TrianglesNb();
  if (theArray.IsNull())
  {
    theArray = new Graphic3d_ArrayOfTriangles (aTrianglesNb * 3, 0, Standard_True);
  }

  Poly_Array1OfTriangle aPolyTriangles (1, aTrianglesNb);
  TColgp_Array1OfPnt anArray (1, aTrianglesNb * 3);
  NCollection_Array1<gp_Dir> aNormals (1, aTrianglesNb * 3);
  fillArrays (theTrsf, anArray, aNormals);

  // Fill primitives
  for (Standard_Integer aU = 0; aU < mySlicesNb; ++aU)
  {
    for (Standard_Integer aV = 1; aV <= myStacksNb; ++aV)
    {
      theArray->AddVertex (anArray.Value (aU * (myStacksNb + 1) + aV), aNormals.Value (aU * (myStacksNb + 1) + aV));
      theArray->AddVertex (anArray.Value ((aU + 1) * (myStacksNb + 1) + aV), aNormals.Value ((aU + 1) * (myStacksNb + 1) + aV));
      theArray->AddVertex (anArray.Value ((aU + 1) * (myStacksNb + 1) + (aV + 1)), aNormals.Value ((aU + 1) * (myStacksNb + 1) + (aV + 1)));
      theArray->AddVertex (anArray.Value ((aU + 1) * (myStacksNb + 1) + (aV + 1)), aNormals.Value ((aU + 1) * (myStacksNb + 1) + (aV + 1)));
      theArray->AddVertex (anArray.Value (aU * (myStacksNb + 1) + (aV + 1)), aNormals.Value (aU * (myStacksNb + 1) + (aV + 1)));
      theArray->AddVertex (anArray.Value (aU * (myStacksNb + 1) + aV), aNormals.Value (aU * (myStacksNb + 1) + aV));
    }
  }
}

//=======================================================================
//function : FillTriangulation
//purpose  :
//=======================================================================
void Prs3d_ToolQuadric::FillArray (Handle(Graphic3d_ArrayOfTriangles)& theArray,
                                   Handle(Poly_Triangulation)& theTriangulation,
                                   const gp_Trsf& theTrsf)
{
  const Standard_Integer aTrianglesNb = TrianglesNb();
  theArray = new Graphic3d_ArrayOfTriangles(aTrianglesNb * 3, 0, Standard_True);

  Poly_Array1OfTriangle aPolyTriangles(1, aTrianglesNb);
  TColgp_Array1OfPnt anArray(1, aTrianglesNb * 3);
  NCollection_Array1<gp_Dir> aNormals(1, aTrianglesNb * 3);
  fillArrays(theTrsf, anArray, aNormals);

  // Fill triangles
  for (Standard_Integer aU = 0, anIndex = 0; aU < mySlicesNb; ++aU)
  {
    for (Standard_Integer aV = 1; aV <= myStacksNb; ++aV)
    {
      theArray->AddVertex(anArray.Value(aU * (myStacksNb + 1) + aV), aNormals.Value(aU * (myStacksNb + 1) + aV));
      theArray->AddVertex(anArray.Value((aU + 1) * (myStacksNb + 1) + aV), aNormals.Value((aU + 1) * (myStacksNb + 1) + aV));
      theArray->AddVertex(anArray.Value((aU + 1) * (myStacksNb + 1) + (aV + 1)), aNormals.Value((aU + 1) * (myStacksNb + 1) + (aV + 1)));
      theArray->AddVertex(anArray.Value((aU + 1) * (myStacksNb + 1) + (aV + 1)), aNormals.Value((aU + 1) * (myStacksNb + 1) + (aV + 1)));
      theArray->AddVertex(anArray.Value(aU * (myStacksNb + 1) + (aV + 1)), aNormals.Value(aU * (myStacksNb + 1) + (aV + 1)));
      theArray->AddVertex(anArray.Value(aU * (myStacksNb + 1) + aV), aNormals.Value(aU * (myStacksNb + 1) + aV));

      aPolyTriangles.SetValue (++anIndex, Poly_Triangle(aU * (myStacksNb + 1) + aV,
                               (aU + 1) * (myStacksNb + 1) + aV,
                               (aU + 1) * (myStacksNb + 1) + (aV + 1)));
      aPolyTriangles.SetValue (++anIndex, Poly_Triangle((aU + 1) * (myStacksNb + 1) + (aV + 1),
                               aU * (myStacksNb + 1) + (aV + 1),
                               aU * (myStacksNb + 1) + aV));
    }
  }

  theTriangulation = new Poly_Triangulation (anArray, aPolyTriangles);
}
