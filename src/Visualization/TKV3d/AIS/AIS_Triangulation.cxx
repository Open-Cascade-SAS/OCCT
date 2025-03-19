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

#include <AIS_Triangulation.hxx>

#include <AIS_DisplayMode.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Standard_Type.hxx>
#include <Poly_Triangulation.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_Triangulation, AIS_InteractiveObject)

AIS_Triangulation::AIS_Triangulation(const Handle(Poly_Triangulation)& Triangulation)
{
  myTriangulation = Triangulation;
  myNbNodes       = Triangulation->NbNodes();
  myNbTriangles   = Triangulation->NbTriangles();
  myFlagColor     = 0;
}

//=================================================================================================

void AIS_Triangulation::SetTransparency(const Standard_Real theValue)
{
  if (!myDrawer->HasOwnShadingAspect())
  {
    myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());
    if (myDrawer->HasLink())
    {
      *myDrawer->ShadingAspect()->Aspect() = *myDrawer->Link()->ShadingAspect()->Aspect();
    }
  }

  // override transparency
  myDrawer->ShadingAspect()->SetTransparency(theValue, myCurrentFacingModel);
  myDrawer->SetTransparency((Standard_ShortReal)theValue);

  updatePresentation();
}

//=================================================================================================

void AIS_Triangulation::UnsetTransparency()
{
  myDrawer->SetTransparency(0.0f);
  if (!myDrawer->HasOwnShadingAspect())
  {
    return;
  }
  else if (HasColor() || HasMaterial())
  {
    myDrawer->ShadingAspect()->SetTransparency(0.0, myCurrentFacingModel);
  }

  updatePresentation();
}

//=================================================================================================

void AIS_Triangulation::updatePresentation()
{
  if (HasVertexColors())
  {
    SetToUpdate(AIS_WireFrame);
  }
  else
  {
    // modify shading presentation without re-computation
    const PrsMgr_Presentations&        aPrsList  = Presentations();
    Handle(Graphic3d_AspectFillArea3d) anAreaAsp = myDrawer->ShadingAspect()->Aspect();
    for (PrsMgr_Presentations::Iterator aPrsIter(aPrsList); aPrsIter.More(); aPrsIter.Next())
    {
      if (aPrsIter.Value()->Mode() != AIS_WireFrame)
      {
        continue;
      }

      const Handle(Prs3d_Presentation)& aPrs = aPrsIter.Value();
      for (Graphic3d_SequenceOfGroup::Iterator aGroupIt(aPrs->Groups()); aGroupIt.More();
           aGroupIt.Next())
      {
        const Handle(Graphic3d_Group)& aGroup = aGroupIt.Value();
        aGroup->SetGroupPrimitivesAspect(anAreaAsp);
      }
    }
  }
}

//=================================================================================================

void AIS_Triangulation::Compute(const Handle(PrsMgr_PresentationManager)&,
                                const Handle(Prs3d_Presentation)& thePrs,
                                const Standard_Integer            theMode)
{
  if (theMode != 0)
  {
    return;
  }

  Standard_Boolean hasVNormals = myTriangulation->HasNormals();
  Standard_Boolean hasVColors  = HasVertexColors();

  Handle(Graphic3d_ArrayOfTriangles) anArray  = new Graphic3d_ArrayOfTriangles(myNbNodes,
                                                                              myNbTriangles * 3,
                                                                              hasVNormals,
                                                                              hasVColors,
                                                                              Standard_False);
  Handle(Graphic3d_Group)            aGroup   = thePrs->CurrentGroup();
  Handle(Graphic3d_AspectFillArea3d) anAspect = myDrawer->ShadingAspect()->Aspect();

  const Standard_Real anAmbient = 1.0;
  if (hasVNormals)
  {
    gp_Vec3f aNormal;
    if (hasVColors)
    {
      const TColStd_Array1OfInteger& colors = myColor->Array1();
      for (Standard_Integer aNodeIter = 1; aNodeIter <= myTriangulation->NbNodes(); ++aNodeIter)
      {
        anArray->AddVertex(myTriangulation->Node(aNodeIter),
                           attenuateColor(colors[aNodeIter], anAmbient));
        myTriangulation->Normal(aNodeIter, aNormal);
        anArray->SetVertexNormal(aNodeIter, aNormal.x(), aNormal.y(), aNormal.z());
      }
    }
    else // !hasVColors
    {
      for (Standard_Integer aNodeIter = 1; aNodeIter <= myTriangulation->NbNodes(); ++aNodeIter)
      {
        anArray->AddVertex(myTriangulation->Node(aNodeIter));
        myTriangulation->Normal(aNodeIter, aNormal);
        anArray->SetVertexNormal(aNodeIter, aNormal.x(), aNormal.y(), aNormal.z());
      }
    }
  }
  else // !hasVNormals
  {
    if (hasVColors)
    {
      const TColStd_Array1OfInteger& colors = myColor->Array1();
      for (Standard_Integer aNodeIter = 1; aNodeIter <= myTriangulation->NbNodes(); ++aNodeIter)
      {
        anArray->AddVertex(myTriangulation->Node(aNodeIter),
                           attenuateColor(colors[aNodeIter], anAmbient));
      }
    }
    else // !hasVColors
    {
      for (Standard_Integer aNodeIter = 1; aNodeIter <= myTriangulation->NbNodes(); ++aNodeIter)
      {
        anArray->AddVertex(myTriangulation->Node(aNodeIter));
      }
    }
  }

  Standard_Integer aTriIndices[3] = {0, 0, 0};
  for (Standard_Integer aTriIter = 1; aTriIter <= myTriangulation->NbTriangles(); ++aTriIter)
  {
    myTriangulation->Triangle(aTriIter).Get(aTriIndices[0], aTriIndices[1], aTriIndices[2]);
    anArray->AddEdge(aTriIndices[0]);
    anArray->AddEdge(aTriIndices[1]);
    anArray->AddEdge(aTriIndices[2]);
  }
  aGroup->SetPrimitivesAspect(anAspect);
  aGroup->AddPrimitiveArray(anArray);
}

//=================================================================================================

void AIS_Triangulation::ComputeSelection(const Handle(SelectMgr_Selection)& /*aSelection*/,
                                         const Standard_Integer /*aMode*/)
{
}

//=======================================================================
// function : SetColor
// purpose  : Set the color for each node.
//           Each 32-bit color is Alpha << 24 + Blue << 16 + Green << 8 + Red
//           Order of color components is essential for further usage by OpenGL
//=======================================================================
void AIS_Triangulation::SetColors(const Handle(TColStd_HArray1OfInteger)& aColor)
{
  myFlagColor = 1;
  myColor     = aColor;
}

//=======================================================================
// function : GetColor
// purpose  : Get the color for each node.
//           Each 32-bit color is Alpha << 24 + Blue << 16 + Green << 8 + Red
//           Order of color components is essential for further usage by OpenGL
//=======================================================================

Handle(TColStd_HArray1OfInteger) AIS_Triangulation::GetColors() const
{
  return myColor;
}

//=================================================================================================

void AIS_Triangulation::SetTriangulation(const Handle(Poly_Triangulation)& aTriangulation)
{
  myTriangulation = aTriangulation;
}

//=================================================================================================

Handle(Poly_Triangulation) AIS_Triangulation::GetTriangulation() const
{
  return myTriangulation;
}

//=================================================================================================

Graphic3d_Vec4ub AIS_Triangulation::attenuateColor(const Standard_Integer theColor,
                                                   const Standard_Real    theComposition)
{
  const Standard_Byte* anRgbx = reinterpret_cast<const Standard_Byte*>(&theColor);

  // If IsTranparent() is false alpha value will be ignored anyway.
  Standard_Byte anAlpha =
    IsTransparent()
      ? static_cast<Standard_Byte>(
          255.0 - myDrawer->ShadingAspect()->Aspect()->FrontMaterial().Transparency() * 255.0)
      : 255;

  return Graphic3d_Vec4ub((Standard_Byte)(theComposition * anRgbx[0]),
                          (Standard_Byte)(theComposition * anRgbx[1]),
                          (Standard_Byte)(theComposition * anRgbx[2]),
                          anAlpha);
}
