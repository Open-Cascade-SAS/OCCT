// Created on: 2011-03-06
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef _Graphic3d_GraduatedTrihedron_HeaderFile
#define _Graphic3d_GraduatedTrihedron_HeaderFile

#include <Font_FontAspect.hxx>
#include <NCollection_Array1.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

class Graphic3d_CView;

//! Defines the class of a graduated trihedron.
//! It contains main style parameters for implementation of graduated trihedron
//! @sa OpenGl_GraduatedTrihedron
class Graphic3d_GraduatedTrihedron
{

public:

  //! Class that stores style for one graduated trihedron axis such as colors, lengths and customization flags.
  //! It is used in Graphic3d_GraduatedTrihedron.
  class AxisAspect
  {
  public:

    AxisAspect (const TCollection_ExtendedString theName = "", const Quantity_Color theNameColor = Quantity_NOC_BLACK,
                const Quantity_Color theColor = Quantity_NOC_BLACK,
                const Standard_Integer theValuesOffset = 10, const Standard_Integer theNameOffset = 30,
                const Standard_Integer theTickmarksNumber = 5, const Standard_Integer theTickmarksLength = 10,
                const Standard_Boolean theToDrawName = Standard_True,
                const Standard_Boolean theToDrawValues = Standard_True,
                const Standard_Boolean theToDrawTickmarks = Standard_True)
    : myName (theName),
      myToDrawName (theToDrawName),
      myToDrawTickmarks (theToDrawTickmarks),
      myToDrawValues (theToDrawValues),
      myNameColor (theNameColor),
      myTickmarksNumber (theTickmarksNumber),
      myTickmarksLength (theTickmarksLength),
      myColor (theColor),
      myValuesOffset (theValuesOffset),
      myNameOffset (theNameOffset)
    { }

  public:

    void SetName (const TCollection_ExtendedString& theName) { myName = theName; }
    const TCollection_ExtendedString& Name() const { return myName; }

    Standard_Boolean ToDrawName() const { return myToDrawName; }
    void SetDrawName (const Standard_Boolean theToDraw) { myToDrawName = theToDraw; }

    Standard_Boolean ToDrawTickmarks() const { return myToDrawTickmarks; }
    void SetDrawTickmarks (const Standard_Boolean theToDraw) { myToDrawTickmarks = theToDraw; }

    Standard_Boolean ToDrawValues() const { return myToDrawValues; }
    void SetDrawValues (const Standard_Boolean theToDraw) { myToDrawValues = theToDraw; }

    const Quantity_Color& NameColor() const { return myNameColor; }
    void SetNameColor (const Quantity_Color& theColor) { myNameColor = theColor; }

    //! Color of axis and values
    const Quantity_Color& Color() const { return myColor; }

    //! Sets color of axis and values
    void SetColor (const Quantity_Color& theColor) { myColor = theColor; }

    Standard_Integer TickmarksNumber() const { return myTickmarksNumber; }
    void SetTickmarksNumber (const Standard_Integer theValue) { myTickmarksNumber = theValue; }

    Standard_Integer TickmarksLength() const { return myTickmarksLength; }
    void SetTickmarksLength (const Standard_Integer theValue) { myTickmarksLength = theValue; }

    Standard_Integer ValuesOffset() const { return myValuesOffset; }
    void SetValuesOffset (const Standard_Integer theValue) { myValuesOffset = theValue; }

    Standard_Integer NameOffset() const { return myNameOffset; }
    void SetNameOffset (const Standard_Integer theValue) { myNameOffset = theValue; }

  protected:

    TCollection_ExtendedString myName;

    Standard_Boolean myToDrawName;
    Standard_Boolean myToDrawTickmarks;
    Standard_Boolean myToDrawValues;

    Quantity_Color   myNameColor;

    Standard_Integer myTickmarksNumber; //!< Number of splits along axes
    Standard_Integer myTickmarksLength; //!< Length of tickmarks
    Quantity_Color   myColor;           //!< Color of axis and values

    Standard_Integer myValuesOffset;    //!< Offset for drawing values
    Standard_Integer myNameOffset;      //!< Offset for drawing name of axis
  };

public:

  typedef void (*MinMaxValuesCallback) (Graphic3d_CView*);

public:

  //! Default constructor
  //! Constructs the default graduated trihedron with grid, X, Y, Z axes, and tickmarks
  Graphic3d_GraduatedTrihedron (const TCollection_AsciiString& theNamesFont = "Arial",
                                const Font_FontAspect& theNamesStyle = Font_FA_Bold, const Standard_Integer theNamesSize = 12,
                                const TCollection_AsciiString& theValuesFont = "Arial",
                                const Font_FontAspect& theValuesStyle = Font_FA_Regular, const Standard_Integer theValuesSize = 12,
                                const Standard_ShortReal theArrowsLength = 30.0f, const Quantity_Color theGridColor = Quantity_NOC_WHITE,
                                const Standard_Boolean theToDrawGrid = Standard_True, const Standard_Boolean theToDrawAxes = Standard_True)
  : myCubicAxesCallback (NULL),
    myNamesFont (theNamesFont),
    myNamesStyle (theNamesStyle),
    myNamesSize (theNamesSize),
    myValuesFont (theValuesFont),
    myValuesStyle (theValuesStyle),
    myValuesSize (theValuesSize),
    myArrowsLength (theArrowsLength),
    myGridColor (theGridColor),
    myToDrawGrid (theToDrawGrid),
    myToDrawAxes (theToDrawAxes),
    myAxes(0, 2)
  {
    myAxes (0) = AxisAspect ("X", Quantity_NOC_RED, Quantity_NOC_RED);
    myAxes (1) = AxisAspect ("Y", Quantity_NOC_GREEN, Quantity_NOC_GREEN);
    myAxes (2) = AxisAspect ("Z", Quantity_NOC_BLUE1, Quantity_NOC_BLUE1);
  }

public:

  AxisAspect& ChangeXAxisAspect() { return myAxes(0); }
  AxisAspect& ChangeYAxisAspect() { return myAxes(1); }
  AxisAspect& ChangeZAxisAspect() { return myAxes(2); }

  AxisAspect& ChangeAxisAspect (const Standard_Integer theIndex)
  {
    Standard_OutOfRange_Raise_if (theIndex < 0 || theIndex > 2, "Graphic3d_GraduatedTrihedron::ChangeAxisAspect: theIndex is out of bounds [0,2].");
    return myAxes (theIndex);
  }

  const AxisAspect& XAxisAspect() const { return myAxes(0); }
  const AxisAspect& YAxisAspect() const { return myAxes(1); }
  const AxisAspect& ZAxisAspect() const { return myAxes(2); }

  const AxisAspect& AxisAspectAt (const Standard_Integer theIndex) const
  {
    Standard_OutOfRange_Raise_if (theIndex < 0 || theIndex > 2, "Graphic3d_GraduatedTrihedron::AxisAspect: theIndex is out of bounds [0,2].");
    return myAxes (theIndex);
  }

  Standard_ShortReal ArrowsLength() const { return myArrowsLength; }
  void SetArrowsLength (const Standard_ShortReal theValue) { myArrowsLength = theValue; }

  const Quantity_Color& GridColor() const { return myGridColor; }
  void SetGridColor (const Quantity_Color& theColor) {myGridColor = theColor; }

  Standard_Boolean ToDrawGrid() const { return myToDrawGrid; }
  void SetDrawGrid (const Standard_Boolean theToDraw) { myToDrawGrid = theToDraw; }

  Standard_Boolean ToDrawAxes() const { return myToDrawAxes; }
  void SetDrawAxes (const Standard_Boolean theToDraw) { myToDrawAxes = theToDraw; }

  const TCollection_AsciiString& NamesFont() const { return myNamesFont; }
  void SetNamesFont (const TCollection_AsciiString& theFont) { myNamesFont = theFont; }

  Font_FontAspect NamesFontAspect() const { return myNamesStyle; }
  void SetNamesFontAspect (Font_FontAspect theAspect) { myNamesStyle = theAspect; }

  Standard_Integer NamesSize() const { return myNamesSize; }
  void SetNamesSize (const Standard_Integer theValue) { myNamesSize = theValue; }

  const TCollection_AsciiString& ValuesFont () const { return myValuesFont; }
  void SetValuesFont (const TCollection_AsciiString& theFont) { myValuesFont = theFont; }

  Font_FontAspect ValuesFontAspect() const { return myValuesStyle; }
  void SetValuesFontAspect (Font_FontAspect theAspect) { myValuesStyle = theAspect; }

  Standard_Integer ValuesSize() const { return myValuesSize; }
  void SetValuesSize (const Standard_Integer theValue) { myValuesSize = theValue; }

  Standard_Boolean CubicAxesCallback(Graphic3d_CView* theView) const
  {
    if (myCubicAxesCallback != NULL)
    {
      myCubicAxesCallback (theView);
      return Standard_True;
    }
    return Standard_False;
  }
  void SetCubicAxesCallback (const MinMaxValuesCallback theCallback) { myCubicAxesCallback = theCallback; }

protected:

  MinMaxValuesCallback myCubicAxesCallback; //!< Callback function to define boundary box of displayed objects

  TCollection_AsciiString myNamesFont;  //!< Font name of names of axes: Courier, Arial, ...
  Font_FontAspect         myNamesStyle; //!< Style of names of axes: OSD_FA_Regular, OSD_FA_Bold,..
  Standard_Integer        myNamesSize;  //!< Size of names of axes: 8, 10,..

protected:

  TCollection_AsciiString myValuesFont;  //!< Font name of values: Courier, Arial, ...
  Font_FontAspect         myValuesStyle; //!< Style of values: OSD_FA_Regular, OSD_FA_Bold, ...
  Standard_Integer        myValuesSize;  //!< Size of values: 8, 10, 12, 14, ...

protected:

  Standard_ShortReal myArrowsLength;
  Quantity_Color     myGridColor;

  Standard_Boolean   myToDrawGrid;
  Standard_Boolean   myToDrawAxes;

  NCollection_Array1<AxisAspect> myAxes; //!< X, Y and Z axes parameters

};
#endif // Graphic3d_GraduatedTrihedron_HeaderFile
