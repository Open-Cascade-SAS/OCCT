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

#ifndef _SelectMgr_SelectionImageFiller_HeaderFile
#define _SelectMgr_SelectionImageFiller_HeaderFile

#include <Image_PixMap.hxx>
#include <math_BullardGenerator.hxx>
#include <NCollection_Map.hxx>
#include <StdSelect_TypeOfSelectionImage.hxx>

class SelectMgr_ViewerSelector;

//! Abstract class for filling pixel with color.
//! This is internal tool for SelectMgr_ViewerSelector::ToPixMap().
class SelectMgr_SelectionImageFiller : public Standard_Transient
{
public:
  //! Create filler of specified type.
  static Handle(SelectMgr_SelectionImageFiller) CreateFiller(
    Image_PixMap&                  thePixMap,
    SelectMgr_ViewerSelector*      theSelector,
    StdSelect_TypeOfSelectionImage theType);

public:
  //! Main constructor.
  SelectMgr_SelectionImageFiller(Image_PixMap& thePixMap, SelectMgr_ViewerSelector* theSelector)
      : myImage(&thePixMap),
        myMainSel(theSelector)
  {
  }

  //! Fill pixel at specified position.
  virtual void Fill(const Standard_Integer theCol,
                    const Standard_Integer theRow,
                    const Standard_Integer thePicked) = 0;

  //! Flush results into final image.
  virtual void Flush() {}

protected:
  //! Find the new unique random color.
  void randomPastelColor(Quantity_Color& theColor)
  {
    for (;;)
    {
      nextRandomPastelColor(theColor);
      if (myUniqueColors.Add(theColor))
      {
        return;
      }
    }
  }

  //! Fills the given color as random.
  void nextRandomPastelColor(Quantity_Color& theColor)
  {
    theColor = Quantity_Color(Standard_Real(myBullardGenerator.NextInt() % 256) / 255.0,
                              Standard_Real(myBullardGenerator.NextInt() % 256) / 255.0,
                              Standard_Real(myBullardGenerator.NextInt() % 256) / 255.0,
                              Quantity_TOC_sRGB);
  }

protected:
  Image_PixMap*                   myImage;
  SelectMgr_ViewerSelector*       myMainSel;
  math_BullardGenerator           myBullardGenerator;
  NCollection_Map<Quantity_Color> myUniqueColors;
};

#endif
