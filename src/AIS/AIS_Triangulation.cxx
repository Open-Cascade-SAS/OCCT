// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <AIS_Drawer.hxx>
#include <AIS_Triangulation.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Standard_DefineHandle.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Triangulation.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <TShort_Array1OfShortReal.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TShort_HArray1OfShortReal.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>


IMPLEMENT_STANDARD_HANDLE(AIS_Triangulation, AIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(AIS_Triangulation, AIS_InteractiveObject)

AIS_Triangulation::AIS_Triangulation(const Handle(Poly_Triangulation)& Triangulation)
{
  myTriangulation = Triangulation;
  myNbNodes       = Triangulation->NbNodes();
  myNbTriangles   = Triangulation->NbTriangles();
  myFlagColor     = 0;
}

//=======================================================================
//function : Compute
//purpose  :
//=======================================================================
void AIS_Triangulation::Compute(const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
                                const Handle(Prs3d_Presentation)& aPresentation,
                                const Standard_Integer aMode)
{
  switch (aMode) 
  {
    case 0:
      const TColgp_Array1OfPnt& nodes = myTriangulation->Nodes();             //Nodes
      const Poly_Array1OfTriangle& triangles = myTriangulation->Triangles();  //Triangle

      Standard_Boolean hasVNormals = myTriangulation->HasNormals();
      Standard_Boolean hasVColors  = (myFlagColor == 1);

      Handle(Graphic3d_ArrayOfTriangles) anArray = new Graphic3d_ArrayOfTriangles (myNbNodes, myNbTriangles * 3,
                                                                                   hasVNormals, hasVColors, Standard_False);
      Handle(Graphic3d_Group) TheGroup = Prs3d_Root::CurrentGroup(aPresentation);
      Handle(Graphic3d_AspectFillArea3d) aspect = myDrawer->ShadingAspect()->Aspect();

      Standard_Integer i;
      Standard_Integer j;

      Standard_Real ambient = aspect->FrontMaterial().Ambient();
      if (hasVNormals)
      {
        const TShort_Array1OfShortReal& normals = myTriangulation->Normals();
        if (hasVColors)
        {
          const TColStd_Array1OfInteger& colors = myColor->Array1();
          for ( i = nodes.Lower(); i <= nodes.Upper(); i++ )
          {
            j = (i - nodes.Lower()) * 3;
            anArray->AddVertex(nodes(i), AttenuateColor(colors(i), ambient));
            anArray->SetVertexNormal(i, normals(j+1), normals(j+2), normals(j+3));
          }
        }
        else // !hasVColors
        {
          for ( i = nodes.Lower(); i <= nodes.Upper(); i++ )
          {
            j = (i - nodes.Lower()) * 3;
            anArray->AddVertex(nodes(i));
            anArray->SetVertexNormal(i, normals(j+1), normals(j+2), normals(j+3));
          }
        }
      }
      else // !hasVNormals
      {
        if (hasVColors)
        {
          const TColStd_Array1OfInteger& colors = myColor->Array1();
          for ( i = nodes.Lower(); i <= nodes.Upper(); i++ )
          {
            anArray->AddVertex(nodes(i), AttenuateColor(colors(i), ambient));
          }
        }
        else // !hasVColors
        {
          for ( i = nodes.Lower(); i <= nodes.Upper(); i++ )
          {
            anArray->AddVertex(nodes(i));
          }
        }
      }

      Standard_Integer indexTriangle[3] = {0,0,0};
      for ( i = triangles.Lower(); i<= triangles.Upper(); i++ ) {
        triangles(i).Get(indexTriangle[0], indexTriangle[1], indexTriangle[2]);
        anArray->AddEdge(indexTriangle[0]);
        anArray->AddEdge(indexTriangle[1]);
        anArray->AddEdge(indexTriangle[2]);
      }
      TheGroup->SetPrimitivesAspect(aspect);
      TheGroup->AddPrimitiveArray(anArray);
      break;
  }
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================
void AIS_Triangulation::ComputeSelection(const Handle(SelectMgr_Selection)& /*aSelection*/,
                                         const Standard_Integer /*aMode*/)
{

}

//=======================================================================
//function : SetColor
//purpose  : Set the color for each node.
//           Each 32-bit color is Alpha << 24 + Blue << 16 + Green << 8 + Red
//           Order of color components is essential for further usage by OpenGL
//=======================================================================
void AIS_Triangulation::SetColors(const Handle(TColStd_HArray1OfInteger)& aColor)
{
  myFlagColor = 1;
  myColor = aColor;
}

//=======================================================================
//function : GetColor
//purpose  : Get the color for each node.
//           Each 32-bit color is Alpha << 24 + Blue << 16 + Green << 8 + Red
//           Order of color components is essential for further usage by OpenGL
//=======================================================================

Handle(TColStd_HArray1OfInteger) AIS_Triangulation::GetColors() const
{
  return myColor;
}


//=======================================================================
//function : SetTriangulation
//purpose  : 
//=======================================================================
void AIS_Triangulation::SetTriangulation(const Handle(Poly_Triangulation)& aTriangulation)
{
  myTriangulation = aTriangulation;
}

//=======================================================================
//function : GetTriangulation
//purpose  : 
//=======================================================================
Handle(Poly_Triangulation) AIS_Triangulation::GetTriangulation() const{
  return myTriangulation;
}

//=======================================================================
//function : AttenuateColor
//purpose  : Attenuates 32-bit color by a given attenuation factor (0...1):
//           aColor = Alpha << 24 + Blue << 16 + Green << 8 + Red
//           All color components are multiplied by aComponent, the result is then packed again as 32-bit integer.
//           Color attenuation is applied to the vertex colors in order to have correct visual result 
//           after glColorMaterial(GL_AMBIENT_AND_DIFFUSE). Without it, colors look unnatural and flat.
//=======================================================================

Standard_Integer AIS_Triangulation::AttenuateColor( const Standard_Integer aColor,
                                                    const Standard_Real aComposition)
{
  Standard_Integer  red,
                    green,
                    blue,
                    alpha;

  alpha = aColor&0xff000000;
  alpha >>= 24;

  blue = aColor&0x00ff0000;
  blue >>= 16;

  green = aColor&0x0000ff00;
  green >>= 8;

  red = aColor&0x000000ff;
  red >>= 0; 

  red   = (Standard_Integer)(aComposition * red);
  green = (Standard_Integer)(aComposition * green);
  blue  = (Standard_Integer)(aComposition * blue);

  Standard_Integer  color;
  color = red;
  color += green << 8;
  color += blue  << 16; 
  color += alpha << 24;
  return color;
}

