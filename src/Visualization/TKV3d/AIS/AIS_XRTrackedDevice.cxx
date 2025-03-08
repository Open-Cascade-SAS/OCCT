// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <AIS_XRTrackedDevice.hxx>

#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_Texture2Dmanual.hxx>
#include <Graphic3d_Group.hxx>
#include <Image_Texture.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Select3D_SensitivePrimitiveArray.hxx>
#include <SelectMgr_EntityOwner.hxx>

//! Texture holder.
class AIS_XRTrackedDevice::XRTexture : public Graphic3d_Texture2D
{
public:
  //! Constructor.
  XRTexture(const Handle(Image_Texture)& theImageSource,
            const Graphic3d_TextureUnit  theUnit = Graphic3d_TextureUnit_BaseColor)
      : Graphic3d_Texture2D(""),
        myImageSource(theImageSource)
  {
    if (!theImageSource->TextureId().IsEmpty())
    {
      myTexId = theImageSource->TextureId();
    }
    myParams->SetTextureUnit(theUnit);
    myIsColorMap =
      theUnit == Graphic3d_TextureUnit_BaseColor || theUnit == Graphic3d_TextureUnit_Emissive;
  }

  //! Image reader.
  virtual Handle(Image_PixMap) GetImage(const Handle(Image_SupportedFormats)& theSupported)
    Standard_OVERRIDE
  {
    return myImageSource->ReadImage(theSupported);
  }

protected:
  Handle(Image_Texture) myImageSource;
};

IMPLEMENT_STANDARD_RTTIEXT(AIS_XRTrackedDevice, AIS_InteractiveObject)

//=================================================================================================

AIS_XRTrackedDevice::AIS_XRTrackedDevice(const Handle(Graphic3d_ArrayOfTriangles)& theTris,
                                         const Handle(Image_Texture)&              theTexture)
    : myTris(theTris),
      myLaserColor(Quantity_NOC_BLUE),
      myLaserLength(0.0f),
      myUnitFactor(1.0f),
      myRole(Aspect_XRTrackedDeviceRole_Other),
      myToShowAxes(false)
{
  myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());
  myDrawer->ShadingAspect()->SetMaterial(Graphic3d_NameOfMaterial_DEFAULT);
  myDrawer->ShadingAspect()->SetColor(Quantity_NOC_WHITE);
  if (!theTexture.IsNull())
  {
    myDrawer->ShadingAspect()->Aspect()->SetTextureMap(new XRTexture(theTexture));
    myDrawer->ShadingAspect()->Aspect()->SetTextureMapOn(true);
  }
}

//=================================================================================================

AIS_XRTrackedDevice::AIS_XRTrackedDevice()
    : myLaserColor(Quantity_NOC_BLUE),
      myLaserLength(0.0f),
      myUnitFactor(1.0f),
      myRole(Aspect_XRTrackedDeviceRole_Other),
      myToShowAxes(true)
{
  //
}

//=================================================================================================

void AIS_XRTrackedDevice::SetLaserColor(const Quantity_Color& theColor)
{
  if (!myLaserColor.IsEqual(theColor))
  {
    myLaserColor = theColor;
    computeLaserRay();
  }
}

//=================================================================================================

void AIS_XRTrackedDevice::SetLaserLength(Standard_ShortReal theLength)
{
  if (myLaserLength != theLength)
  {
    myLaserLength = theLength;
    computeLaserRay();
  }
}

//=================================================================================================

void AIS_XRTrackedDevice::computeLaserRay()
{
  if (myRayGroup.IsNull())
  {
    return;
  }

  if (!myRayGroup->IsEmpty())
  {
    myRayGroup->Clear();
  }
  if (myLaserLength <= 0.0f)
  {
    return;
  }

  Handle(Graphic3d_ArrayOfPrimitives) aLines =
    new Graphic3d_ArrayOfSegments(2, 0, Graphic3d_ArrayFlags_VertexColor);
  aLines->AddVertex(gp_Pnt(0.0, 0.0, 0.0), myLaserColor);
  aLines->AddVertex(gp_Pnt(0.0, 0.0, -myLaserLength), myLaserColor);
  myRayGroup->SetGroupPrimitivesAspect(myDrawer->LineAspect()->Aspect());
  myRayGroup->AddPrimitiveArray(aLines, false); // do not extend camera frustum by ray
}

//=================================================================================================

void AIS_XRTrackedDevice::Compute(const Handle(PrsMgr_PresentationManager)&,
                                  const Handle(Prs3d_Presentation)& thePrs,
                                  const Standard_Integer            theMode)
{
  if (theMode != 0)
  {
    return;
  }

  thePrs->SetInfiniteState(myInfiniteState);
  Handle(Graphic3d_Group) aGroup = thePrs->NewGroup();
  if (!myTris.IsNull())
  {
    aGroup->SetGroupPrimitivesAspect(myDrawer->ShadingAspect()->Aspect());
    aGroup->AddPrimitiveArray(myTris);
  }

  if (myToShowAxes || myTris.IsNull())
  {
    const float aSize = 0.1f * myUnitFactor;
    aGroup->SetGroupPrimitivesAspect(myDrawer->LineAspect()->Aspect());
    Handle(Graphic3d_ArrayOfPrimitives) aLines =
      new Graphic3d_ArrayOfSegments(6, 0, Graphic3d_ArrayFlags_VertexColor);
    aLines->AddVertex(gp_Pnt(0.0, 0.0, 0.0), Quantity_Color(Quantity_NOC_RED));
    aLines->AddVertex(gp_Pnt(aSize, 0.0, 0.0), Quantity_Color(Quantity_NOC_RED));
    aLines->AddVertex(gp_Pnt(0.0, 0.0, 0.0), Quantity_Color(Quantity_NOC_GREEN));
    aLines->AddVertex(gp_Pnt(0.0, aSize, 0.0), Quantity_Color(Quantity_NOC_GREEN));
    aLines->AddVertex(gp_Pnt(0.0, 0.0, 0.0), Quantity_Color(Quantity_NOC_BLUE));
    aLines->AddVertex(gp_Pnt(0.0, 0.0, aSize), Quantity_Color(Quantity_NOC_BLUE));
    aGroup->AddPrimitiveArray(aLines);
  }

  myRayGroup = thePrs->NewGroup();
  computeLaserRay();
}

//=================================================================================================

void AIS_XRTrackedDevice::ComputeSelection(const Handle(SelectMgr_Selection)& theSel,
                                           const Standard_Integer             theMode)
{
  if (theMode != 0)
  {
    return;
  }

  if (!myTris.IsNull())
  {
    Handle(SelectMgr_EntityOwner)            anOwner = new SelectMgr_EntityOwner(this);
    Handle(Select3D_SensitivePrimitiveArray) aSensitive =
      new Select3D_SensitivePrimitiveArray(anOwner);
    aSensitive->InitTriangulation(myTris->Attributes(), myTris->Indices(), TopLoc_Location(), true);
    theSel->Add(aSensitive);
  }
}
