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

#include <SelectMgr_SelectionImageFiller.hxx>

#include <SelectMgr_ViewerSelector.hxx>

namespace
{
//! Help class for filling pixel with random color.
class GeneratedEntityColorFiller : public SelectMgr_SelectionImageFiller
{
public:
  GeneratedEntityColorFiller(Image_PixMap& thePixMap, SelectMgr_ViewerSelector* theSelector)
      : SelectMgr_SelectionImageFiller(thePixMap, theSelector)
  {
    // generate per-entity colors in the order as they have been activated
    for (SelectMgr_SelectableObjectSet::Iterator anObjIter(theSelector->SelectableObjects());
         anObjIter.More();
         anObjIter.Next())
    {
      const occ::handle<SelectMgr_SelectableObject>& anObj = anObjIter.Value();
      for (NCollection_Sequence<occ::handle<SelectMgr_Selection>>::Iterator aSelIter(
             anObj->Selections());
           aSelIter.More();
           aSelIter.Next())
      {
        const occ::handle<SelectMgr_Selection>& aSel = aSelIter.Value();
        for (NCollection_Vector<occ::handle<SelectMgr_SensitiveEntity>>::Iterator aSelEntIter(
               aSel->Entities());
             aSelEntIter.More();
             aSelEntIter.Next())
        {
          const occ::handle<SelectMgr_SensitiveEntity>& aSens = aSelEntIter.Value();
          if (!myMapEntityColors.IsBound(aSens->BaseSensitive()))
          {
            Quantity_Color aColor;
            randomPastelColor(aColor);
            myMapEntityColors.Bind(aSens->BaseSensitive(), aColor);
          }
        }
      }
    }
  }

  void Fill(const int theCol, const int theRow, const int thePicked) override
  {
    if (thePicked < 1 || thePicked > myMainSel->NbPicked())
    {
      myImage->SetPixelColor(theCol, theRow, Quantity_Color(Quantity_NOC_BLACK));
      return;
    }

    const occ::handle<Select3D_SensitiveEntity>& aPickedEntity = myMainSel->PickedEntity(thePicked);
    Quantity_Color                               aColor(Quantity_NOC_BLACK);
    myMapEntityColors.Find(aPickedEntity, aColor);
    myImage->SetPixelColor(theCol, theRow, aColor);
  }

protected:
  NCollection_DataMap<occ::handle<Select3D_SensitiveEntity>, Quantity_Color> myMapEntityColors;
};

//! Help class for filling pixel with random color.
class GeneratedEntityTypeColorFiller : public SelectMgr_SelectionImageFiller
{
public:
  GeneratedEntityTypeColorFiller(Image_PixMap& thePixMap, SelectMgr_ViewerSelector* theSelector)
      : SelectMgr_SelectionImageFiller(thePixMap, theSelector)
  {
    // generate per-entity colors in the order as they have been activated
    for (SelectMgr_SelectableObjectSet::Iterator anObjIter(theSelector->SelectableObjects());
         anObjIter.More();
         anObjIter.Next())
    {
      const occ::handle<SelectMgr_SelectableObject>& anObj = anObjIter.Value();
      for (NCollection_Sequence<occ::handle<SelectMgr_Selection>>::Iterator aSelIter(
             anObj->Selections());
           aSelIter.More();
           aSelIter.Next())
      {
        const occ::handle<SelectMgr_Selection>& aSel = aSelIter.Value();
        for (NCollection_Vector<occ::handle<SelectMgr_SensitiveEntity>>::Iterator aSelEntIter(
               aSel->Entities());
             aSelEntIter.More();
             aSelEntIter.Next())
        {
          const occ::handle<SelectMgr_SensitiveEntity>& aSens = aSelEntIter.Value();
          if (!myMapEntityColors.IsBound(aSens->BaseSensitive()->DynamicType()))
          {
            Quantity_Color aColor;
            randomPastelColor(aColor);
            myMapEntityColors.Bind(aSens->BaseSensitive()->DynamicType(), aColor);
          }
        }
      }
    }
  }

  void Fill(const int theCol, const int theRow, const int thePicked) override
  {
    if (thePicked < 1 || thePicked > myMainSel->NbPicked())
    {
      myImage->SetPixelColor(theCol, theRow, Quantity_Color(Quantity_NOC_BLACK));
      return;
    }

    const occ::handle<Select3D_SensitiveEntity>& aPickedEntity = myMainSel->PickedEntity(thePicked);
    Quantity_Color                               aColor(Quantity_NOC_BLACK);
    myMapEntityColors.Find(aPickedEntity->DynamicType(), aColor);
    myImage->SetPixelColor(theCol, theRow, aColor);
  }

protected:
  NCollection_DataMap<occ::handle<Standard_Type>, Quantity_Color> myMapEntityColors;
};

//! Help class for filling pixel with normalized depth of ray.
class NormalizedDepthFiller : public SelectMgr_SelectionImageFiller
{
public:
  NormalizedDepthFiller(Image_PixMap&             thePixMap,
                        SelectMgr_ViewerSelector* theSelector,
                        const bool                theToInverse)
      : SelectMgr_SelectionImageFiller(thePixMap, theSelector),
        myDepthMin(RealLast()),
        myDepthMax(-RealLast()),
        myToInverse(theToInverse)
  {
    myUnnormImage.InitZero(Image_Format_GrayF, thePixMap.SizeX(), thePixMap.SizeY());
  }

  //! Accumulate the data.
  void Fill(const int theCol, const int theRow, const int thePicked) override
  {
    if (myUnnormImage.IsEmpty())
    {
      return;
    }

    if (thePicked < 1 || thePicked > myMainSel->NbPicked())
    {
      myUnnormImage.ChangeValue<float>(theRow, theCol) = ShortRealLast();
      return;
    }

    const SelectMgr_SortCriterion& aSortCriterion    = myMainSel->PickedData(thePicked);
    myUnnormImage.ChangeValue<float>(theRow, theCol) = float(aSortCriterion.Depth);
    myDepthMin                                       = std::min(myDepthMin, aSortCriterion.Depth);
    myDepthMax                                       = std::max(myDepthMax, aSortCriterion.Depth);
  }

  //! Normalize the depth values.
  void Flush() override
  {
    float aFrom  = 0.0f;
    float aDelta = 1.0f;
    if (myDepthMin <= myDepthMax)
    {
      aFrom  = float(myDepthMin);
      aDelta = float(myDepthMax) - float(myDepthMin);
      if (aDelta <= ShortRealEpsilon())
      {
        aDelta = 1.0f;
      }
    }
    for (size_t aRowIter = 0; aRowIter < myUnnormImage.SizeY(); ++aRowIter)
    {
      for (size_t aColIter = 0; aColIter < myUnnormImage.SizeX(); ++aColIter)
      {
        float aDepth = myUnnormImage.Value<float>(aRowIter, aColIter);
        if (aDepth <= -ShortRealLast() || aDepth >= ShortRealLast())
        {
          myImage->SetPixelColor(int(aColIter),
                                 int(aRowIter),
                                 Quantity_ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f));
          continue;
        }

        float aNormDepth = (aDepth - aFrom) / aDelta;
        if (myToInverse)
        {
          aNormDepth = 1.0f - aNormDepth;
        }
        myImage->SetPixelColor(int(aColIter),
                               int(aRowIter),
                               Quantity_ColorRGBA(aNormDepth, aNormDepth, aNormDepth, 1.0f));
      }
    }
  }

private:
  Image_PixMap myUnnormImage;
  double       myDepthMin;
  double       myDepthMax;
  bool         myToInverse;
};

//! Help class for filling pixel with unnormalized depth of ray.
class UnnormalizedDepthFiller : public SelectMgr_SelectionImageFiller
{
public:
  UnnormalizedDepthFiller(Image_PixMap& thePixMap, SelectMgr_ViewerSelector* theSelector)
      : SelectMgr_SelectionImageFiller(thePixMap, theSelector)
  {
  }

  void Fill(const int theCol, const int theRow, const int thePicked) override
  {
    if (thePicked < 1 || thePicked > myMainSel->NbPicked())
    {
      myImage->SetPixelColor(theCol, theRow, Quantity_ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f));
      return;
    }

    const SelectMgr_SortCriterion& aSortCriterion = myMainSel->PickedData(thePicked);
    const float                    aDepth         = float(aSortCriterion.Depth);
    myImage->SetPixelColor(
      theCol,
      theRow,
      Quantity_ColorRGBA(NCollection_Vec4<float>(aDepth, aDepth, aDepth, 1.0f)));
  }
};

//! Help class for filling pixel with color of detected object.
class GeneratedOwnerColorFiller : public SelectMgr_SelectionImageFiller
{
public:
  GeneratedOwnerColorFiller(Image_PixMap& thePixMap, SelectMgr_ViewerSelector* theSelector)
      : SelectMgr_SelectionImageFiller(thePixMap, theSelector)
  {
    // generate per-owner colors in the order as they have been activated
    for (SelectMgr_SelectableObjectSet::Iterator anObjIter(theSelector->SelectableObjects());
         anObjIter.More();
         anObjIter.Next())
    {
      const occ::handle<SelectMgr_SelectableObject>& anObj = anObjIter.Value();
      for (NCollection_Sequence<occ::handle<SelectMgr_Selection>>::Iterator aSelIter(
             anObj->Selections());
           aSelIter.More();
           aSelIter.Next())
      {
        const occ::handle<SelectMgr_Selection>& aSel = aSelIter.Value();
        for (NCollection_Vector<occ::handle<SelectMgr_SensitiveEntity>>::Iterator aSelEntIter(
               aSel->Entities());
             aSelEntIter.More();
             aSelEntIter.Next())
        {
          const occ::handle<SelectMgr_SensitiveEntity>& aSens   = aSelEntIter.Value();
          const occ::handle<SelectMgr_EntityOwner>&     anOwner = aSens->BaseSensitive()->OwnerId();
          if (!myMapOwnerColors.IsBound(anOwner))
          {
            Quantity_Color aColor;
            randomPastelColor(aColor);
            myMapOwnerColors.Bind(anOwner, aColor);
          }
        }
      }
    }
  }

  void Fill(const int theCol, const int theRow, const int thePicked) override
  {
    if (thePicked < 1 || thePicked > myMainSel->NbPicked())
    {
      myImage->SetPixelColor(theCol, theRow, Quantity_Color(Quantity_NOC_BLACK));
      return;
    }

    const occ::handle<SelectMgr_EntityOwner>& aPickedOwner = myMainSel->Picked(thePicked);
    Quantity_Color                            aColor(Quantity_NOC_BLACK);
    myMapOwnerColors.Find(aPickedOwner, aColor);
    myImage->SetPixelColor(theCol, theRow, aColor);
  }

protected:
  NCollection_DataMap<occ::handle<SelectMgr_EntityOwner>, Quantity_Color> myMapOwnerColors;
};

//! Help class for filling pixel with random color for each selection mode.
class GeneratedSelModeColorFiller : public SelectMgr_SelectionImageFiller
{
public:
  GeneratedSelModeColorFiller(Image_PixMap& thePixMap, SelectMgr_ViewerSelector* theSelector)
      : SelectMgr_SelectionImageFiller(thePixMap, theSelector)
  {
    // generate standard modes in proper order, consider custom objects would use similar scheme
    // clang-format off
      myMapSelectionModeColors.Bind (     0, Quantity_NOC_WHITE);          // default (entire object selection)
    // clang-format on
    myMapSelectionModeColors.Bind(1, Quantity_NOC_YELLOW);              // TopAbs_VERTEX
    myMapSelectionModeColors.Bind(2, Quantity_NOC_GREEN);               // TopAbs_EDGE
    myMapSelectionModeColors.Bind(3, Quantity_NOC_RED);                 // TopAbs_WIRE
    myMapSelectionModeColors.Bind(4, Quantity_NOC_BLUE1);               // TopAbs_FACE
    myMapSelectionModeColors.Bind(5, Quantity_NOC_CYAN1);               // TopAbs_SHELL
    myMapSelectionModeColors.Bind(6, Quantity_NOC_PURPLE);              // TopAbs_SOLID
    myMapSelectionModeColors.Bind(7, Quantity_NOC_MAGENTA1);            // TopAbs_COMPSOLID
    myMapSelectionModeColors.Bind(8, Quantity_NOC_BROWN);               // TopAbs_COMPOUND
    myMapSelectionModeColors.Bind(0x0010, Quantity_NOC_PINK);           // MeshVS_SMF_Volume
    myMapSelectionModeColors.Bind(0x001E, Quantity_NOC_LIMEGREEN);      // MeshVS_SMF_Element
    myMapSelectionModeColors.Bind(0x001F, Quantity_NOC_DARKOLIVEGREEN); // MeshVS_SMF_All
    myMapSelectionModeColors.Bind(0x0100, Quantity_NOC_GOLD);           // MeshVS_SMF_Group
  }

  void Fill(const int theCol, const int theRow, const int thePicked) override
  {
    if (thePicked < 1 || thePicked > myMainSel->NbPicked())
    {
      myImage->SetPixelColor(theCol, theRow, Quantity_Color(Quantity_NOC_BLACK));
      return;
    }

    int                                            aSelectionMode = -1;
    const occ::handle<SelectMgr_SelectableObject>& aSelectable =
      myMainSel->Picked(thePicked)->Selectable();
    const occ::handle<Select3D_SensitiveEntity>& anEntity = myMainSel->PickedEntity(thePicked);
    for (NCollection_Sequence<occ::handle<SelectMgr_Selection>>::Iterator aSelIter(
           aSelectable->Selections());
         aSelIter.More();
         aSelIter.Next())
    {
      const occ::handle<SelectMgr_Selection>& aSelection = aSelIter.Value();
      for (NCollection_Vector<occ::handle<SelectMgr_SensitiveEntity>>::Iterator aSelEntIter(
             aSelection->Entities());
           aSelEntIter.More();
           aSelEntIter.Next())
      {
        if (aSelEntIter.Value()->BaseSensitive() == anEntity)
        {
          aSelectionMode = aSelection->Mode();
          break;
        }
      }
    }
    if (aSelectionMode == -1)
    {
      myImage->SetPixelColor(theCol, theRow, Quantity_Color(Quantity_NOC_BLACK));
      return;
    }

    if (!myMapSelectionModeColors.IsBound(aSelectionMode))
    {
      Quantity_Color aColor;
      randomPastelColor(aColor);
      myMapSelectionModeColors.Bind(aSelectionMode, aColor);
    }

    const Quantity_Color& aColor = myMapSelectionModeColors.Find(aSelectionMode);
    myImage->SetPixelColor(theCol, theRow, aColor);
  }

protected:
  NCollection_DataMap<int, Quantity_Color> myMapSelectionModeColors;
};

//! Help class for filling pixel with color of detected shape.
class DetectedObjectColorFiller : public SelectMgr_SelectionImageFiller
{
public:
  DetectedObjectColorFiller(Image_PixMap& thePixMap, SelectMgr_ViewerSelector* theSelector)
      : SelectMgr_SelectionImageFiller(thePixMap, theSelector)
  {
  }

  void Fill(const int theCol, const int theRow, const int thePicked) override
  {
    Quantity_Color aColor(Quantity_NOC_BLACK);
    if (thePicked > 0 && thePicked <= myMainSel->NbPicked())
    {
      const occ::handle<SelectMgr_SelectableObject>& aSelectable =
        myMainSel->Picked(thePicked)->Selectable();
      aColor = aSelectable->Attributes()->Color();
    }
    myImage->SetPixelColor(theCol, theRow, aColor);
  }
};

//! Help class for filling pixel with normal direction value.
class SurfaceNormalFiller : public SelectMgr_SelectionImageFiller
{
public:
  SurfaceNormalFiller(Image_PixMap& thePixMap, SelectMgr_ViewerSelector* theSelector)
      : SelectMgr_SelectionImageFiller(thePixMap, theSelector)
  {
  }

  void Fill(const int theCol, const int theRow, const int thePicked) override
  {
    if (thePicked <= 0 || thePicked > myMainSel->NbPicked())
    {
      myImage->SetPixelColor(theCol, theRow, Quantity_NOC_BLACK);
    }
    else
    {
      const SelectMgr_SortCriterion& aPickedData = myMainSel->PickedData(thePicked);
      NCollection_Vec3<float>        aNormal     = aPickedData.Normal;
      aNormal.Normalize();
      if (aNormal.Modulus() > 0.0f)
      {
        myImage->SetPixelColor(theCol,
                               theRow,
                               Quantity_ColorRGBA(aNormal.x() * 0.5f + 0.5f,
                                                  aNormal.y() * 0.5f + 0.5f,
                                                  aNormal.z() * 0.5f + 0.5f,
                                                  1.0f));
      }
      else
      {
        myImage->SetPixelColor(theCol, theRow, Quantity_NOC_BLACK);
      }
    }
  }
};
} // namespace

//=================================================================================================

occ::handle<SelectMgr_SelectionImageFiller> SelectMgr_SelectionImageFiller::CreateFiller(
  Image_PixMap&                  thePixMap,
  SelectMgr_ViewerSelector*      theSelector,
  StdSelect_TypeOfSelectionImage theType)
{
  switch (theType)
  {
    case StdSelect_TypeOfSelectionImage_NormalizedDepth:
    case StdSelect_TypeOfSelectionImage_NormalizedDepthInverted: {
      return new NormalizedDepthFiller(thePixMap,
                                       theSelector,
                                       theType
                                         == StdSelect_TypeOfSelectionImage_NormalizedDepthInverted);
    }
    case StdSelect_TypeOfSelectionImage_UnnormalizedDepth: {
      return new UnnormalizedDepthFiller(thePixMap, theSelector);
    }
    case StdSelect_TypeOfSelectionImage_ColoredDetectedObject: {
      return new DetectedObjectColorFiller(thePixMap, theSelector);
    }
    case StdSelect_TypeOfSelectionImage_ColoredEntity: {
      return new GeneratedEntityColorFiller(thePixMap, theSelector);
    }
    case StdSelect_TypeOfSelectionImage_ColoredEntityType: {
      return new GeneratedEntityTypeColorFiller(thePixMap, theSelector);
    }
    case StdSelect_TypeOfSelectionImage_ColoredOwner: {
      return new GeneratedOwnerColorFiller(thePixMap, theSelector);
    }
    case StdSelect_TypeOfSelectionImage_ColoredSelectionMode: {
      return new GeneratedSelModeColorFiller(thePixMap, theSelector);
    }
    case StdSelect_TypeOfSelectionImage_SurfaceNormal: {
      return new SurfaceNormalFiller(thePixMap, theSelector);
    }
  }
  return occ::handle<SelectMgr_SelectionImageFiller>();
}
