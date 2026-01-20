// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( Arun MENON )
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IGESDimen_DimensionDisplayData_HeaderFile
#define _IGESDimen_DimensionDisplayData_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;

//! Defines IGES Dimension Display Data, Type <406> Form <30>,
//! in package IGESDimen
//! The Dimensional Display Data Property is optional but when
//! present must be referenced by a dimension entity.
//! The information it contains could be extracted from the text,
//! leader and witness line data with difficulty.
class IGESDimen_DimensionDisplayData : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESDimen_DimensionDisplayData();

  Standard_EXPORT void Init(const int                  numProps,
                            const int                  aDimType,
                            const int                  aLabelPos,
                            const int                  aCharSet,
                            const occ::handle<TCollection_HAsciiString>& aString,
                            const int                  aSymbol,
                            const double                     anAng,
                            const int                  anAlign,
                            const int                  aLevel,
                            const int                  aPlace,
                            const int                  anOrient,
                            const double                     initVal,
                            const occ::handle<NCollection_HArray1<int>>& notes,
                            const occ::handle<NCollection_HArray1<int>>& startInd,
                            const occ::handle<NCollection_HArray1<int>>& endInd);

  //! returns the number of property values (14)
  Standard_EXPORT int NbPropertyValues() const;

  //! returns the dimension type
  Standard_EXPORT int DimensionType() const;

  //! returns the preferred label position
  Standard_EXPORT int LabelPosition() const;

  //! returns the character set interpretation
  Standard_EXPORT int CharacterSet() const;

  //! returns e.g., 8HDIAMETER
  Standard_EXPORT occ::handle<TCollection_HAsciiString> LString() const;

  Standard_EXPORT int DecimalSymbol() const;

  //! returns the witness line angle in radians
  Standard_EXPORT double WitnessLineAngle() const;

  //! returns the text alignment
  Standard_EXPORT int TextAlignment() const;

  //! returns the text level
  Standard_EXPORT int TextLevel() const;

  //! returns the preferred text placement
  Standard_EXPORT int TextPlacement() const;

  //! returns the arrowhead orientation
  Standard_EXPORT int ArrowHeadOrientation() const;

  //! returns the primary dimension initial value
  Standard_EXPORT double InitialValue() const;

  //! returns the number of supplementary notes or zero
  Standard_EXPORT int NbSupplementaryNotes() const;

  //! returns the Index'th supplementary note
  //! raises exception if Index <= 0 or Index > NbSupplementaryNotes()
  Standard_EXPORT int SupplementaryNote(const int Index) const;

  //! returns the Index'th note start index
  //! raises exception if Index <= 0 or Index > NbSupplementaryNotes()
  Standard_EXPORT int StartIndex(const int Index) const;

  //! returns the Index'th note end index
  //! raises exception if Index <= 0 or Index > NbSupplemetaryNotes()
  Standard_EXPORT int EndIndex(const int Index) const;

  DEFINE_STANDARD_RTTIEXT(IGESDimen_DimensionDisplayData, IGESData_IGESEntity)

private:
  int                 theNbPropertyValues;
  int                 theDimensionType;
  int                 theLabelPosition;
  int                 theCharacterSet;
  occ::handle<TCollection_HAsciiString> theLString;
  int                 theDecimalSymbol;
  double                    theWitnessLineAngle;
  int                 theTextAlignment;
  int                 theTextLevel;
  int                 theTextPlacement;
  int                 theArrowHeadOrientation;
  double                    theInitialValue;
  occ::handle<NCollection_HArray1<int>> theSupplementaryNotes;
  occ::handle<NCollection_HArray1<int>> theStartIndex;
  occ::handle<NCollection_HArray1<int>> theEndIndex;
};

#endif // _IGESDimen_DimensionDisplayData_HeaderFile
