// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
#include <Graphic3d_ArrayOfPrimitives.hxx>


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
void AIS_Triangulation::Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
                                const Handle(Prs3d_Presentation)& aPresentation,
                                const Standard_Integer aMode)
{
  switch (aMode) 
  {
    case 0:
      const TColgp_Array1OfPnt& nodes = myTriangulation->Nodes();             //Nodes
      const Poly_Array1OfTriangle& triangles = myTriangulation->Triangles();  //Triangle
      const TShort_Array1OfShortReal& normals = myTriangulation->Normals();   //Normal

      Standard_Boolean hasVNormals  = Standard_False;
      Standard_Boolean hasVColors   = Standard_False;
      if( normals.Length() > 0 )
        hasVNormals = Standard_True;
      if( myFlagColor == 1 )
        hasVColors = Standard_True; 

      Handle(Graphic3d_ArrayOfTriangles) array = 
         new Graphic3d_ArrayOfTriangles ( myNbNodes,        //maxVertexs
                                          myNbTriangles * 3,//maxEdges
                                          hasVNormals,      //hasVNormals
                                          hasVColors,       //hasVColors
                                          Standard_False,   //hasTexels
                                          Standard_True     //hasEdgeInfos
                                         );
      Handle(Graphic3d_Group) TheGroup = Prs3d_Root::CurrentGroup(aPresentation);
      Handle(Graphic3d_AspectFillArea3d) aspect = myDrawer->ShadingAspect()->Aspect();

      Standard_Integer i;
      Standard_Integer j;

      Standard_Real ambient = aspect->FrontMaterial().Ambient();
      for ( i = nodes.Lower(); i<= nodes.Upper(); i++ ){ 
        if( myFlagColor == 1 )
          array->AddVertex( nodes(i), AttenuateColor(myColor->Value(i),ambient));
        if( myFlagColor == 0 )
          array->AddVertex( nodes(i) );
        j = (i - nodes.Lower()) * 3;
        array->SetVertexNormal(i, normals(j+1), normals(j+2), normals(j+3));
      }

      Standard_Integer indexTriangle[3] = {0,0,0};
      for ( i = triangles.Lower(); i<= triangles.Upper(); i++ ) {
        triangles(i).Get(indexTriangle[0], indexTriangle[1], indexTriangle[2]);
        array->AddEdge(indexTriangle[0]);
        array->AddEdge(indexTriangle[1]);
        array->AddEdge(indexTriangle[2]);
      }
      TheGroup->SetPrimitivesAspect(aspect);
      TheGroup->BeginPrimitives();
      TheGroup->AddPrimitiveArray(array);
      TheGroup->EndPrimitives();
      break;
  }
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================
void AIS_Triangulation::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
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

