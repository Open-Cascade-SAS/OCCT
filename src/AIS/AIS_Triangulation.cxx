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

#include <AIS_DisplayMode.hxx>
#include <AIS_Triangulation.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Standard_Type.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Triangulation.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <TShort_Array1OfShortReal.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TShort_HArray1OfShortReal.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>


IMPLEMENT_STANDARD_RTTIEXT(AIS_Triangulation,AIS_InteractiveObject)

AIS_Triangulation::AIS_Triangulation(const Handle(Poly_Triangulation)& Triangulation)
{
  myTriangulation = Triangulation;
  myNbNodes       = Triangulation->NbNodes();
  myNbTriangles   = Triangulation->NbTriangles();
  myFlagColor     = 0;
}

//=======================================================================
//function : SetTransparency
//purpose  :
//=======================================================================
void AIS_Triangulation::SetTransparency (const Standard_Real theValue)
{
  if (!myDrawer->HasOwnShadingAspect())
  {
    myDrawer->SetShadingAspect (new Prs3d_ShadingAspect());
    if (myDrawer->HasLink())
    {
      *myDrawer->ShadingAspect()->Aspect() = *myDrawer->Link()->ShadingAspect()->Aspect();
    }
  }

  // override transparency
  myDrawer->ShadingAspect()->SetTransparency (theValue, myCurrentFacingModel);
  myDrawer->SetTransparency ((Standard_ShortReal )theValue);

  updatePresentation();
}

//=======================================================================
//function : UnsetTransparency
//purpose  :
//=======================================================================
void AIS_Triangulation::UnsetTransparency()
{
  myDrawer->SetTransparency (0.0f);
  if (!myDrawer->HasOwnShadingAspect())
  {
    return;
  }
  else if (HasColor() || HasMaterial())
  {
    myDrawer->ShadingAspect()->SetTransparency (0.0, myCurrentFacingModel);
  }

  updatePresentation();
}

//=======================================================================
//function : updatePresentation
//purpose  :
//=======================================================================
void AIS_Triangulation::updatePresentation()
{
  if (HasVertexColors())
  {
    SetToUpdate (AIS_WireFrame);
  }
  else
  {
    // modify shading presentation without re-computation
    const PrsMgr_Presentations&        aPrsList  = Presentations();
    Handle(Graphic3d_AspectFillArea3d) anAreaAsp = myDrawer->ShadingAspect()->Aspect();
    for (Standard_Integer aPrsIt = 1; aPrsIt <= aPrsList.Length(); ++aPrsIt)
    {
      const PrsMgr_ModedPresentation& aPrsModed = aPrsList.Value (aPrsIt);
      if (aPrsModed.Mode() != AIS_WireFrame)
      {
        continue;
      }

      const Handle(Prs3d_Presentation)& aPrs = aPrsModed.Presentation()->Presentation();

      for (Graphic3d_SequenceOfGroup::Iterator aGroupIt (aPrs->Groups()); aGroupIt.More(); aGroupIt.Next())
      {
        const Handle(Graphic3d_Group)& aGroup = aGroupIt.Value();
        if (aGroup->IsGroupPrimitivesAspectSet (Graphic3d_ASPECT_FILL_AREA))
        {
          aGroup->SetGroupPrimitivesAspect (anAreaAsp);
        }
      }
    }

    myRecomputeEveryPrs = Standard_False; // no mode to recalculate - only viewer update
    myToRecomputeModes.Clear();
  }
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
      Standard_Boolean hasVColors  = HasVertexColors();

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
            anArray->AddVertex(nodes(i), attenuateColor(colors(i), ambient));
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
            anArray->AddVertex(nodes(i), attenuateColor(colors(i), ambient));
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
//purpose  :
//=======================================================================
Graphic3d_Vec4ub AIS_Triangulation::attenuateColor (const Standard_Integer theColor,
                                                    const Standard_Real    theComposition)
{
  const Standard_Byte* anRgbx = reinterpret_cast<const Standard_Byte*> (&theColor);

  // If IsTranparent() is false alpha value will be ignored anyway.
  Standard_Byte anAlpha = IsTransparent() ? static_cast<Standard_Byte> (255.0 - myDrawer->ShadingAspect()->Aspect()->FrontMaterial().Transparency() * 255.0)
                                          : 255;

  return Graphic3d_Vec4ub ((Standard_Byte)(theComposition * anRgbx[0]),
                           (Standard_Byte)(theComposition * anRgbx[1]),
                           (Standard_Byte)(theComposition * anRgbx[2]),
                           anAlpha);
}
