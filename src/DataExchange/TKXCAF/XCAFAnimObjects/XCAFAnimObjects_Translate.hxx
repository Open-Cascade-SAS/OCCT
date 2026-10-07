// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _XCAFAnimObjects_Translate_HeaderFile
#define _XCAFAnimObjects_Translate_HeaderFile

#include <XCAFAnimObjects_Operation.hxx>
#include <gp_XYZ.hxx>

//! Animation operation that represents translation transformations.
//! This operation moves a shape along the X, Y, and Z axes without changing
//! its orientation or size. Translation is one of the most basic and common
//! animation operations, used to position shapes in 3D space.
//! The translation values are represented as XYZ triplets defining the
//! displacement vector along each axis.
class XCAFAnimObjects_Translate : public XCAFAnimObjects_Operation
{
public:
  //! Constructor for a translation operation with a single displacement vector.
  //! @param theTranslate XYZ values defining displacement along each axis
  Standard_EXPORT XCAFAnimObjects_Translate(const gp_XYZ& theTranslate);

  //! Constructor for a translation operation with multiple keyframes.
  //! @param theTranslate Array of XYZ values defining displacements for each keyframe
  //! @param theTimeStamps Array of time values corresponding to each keyframe
  Standard_EXPORT XCAFAnimObjects_Translate(const NCollection_Array1<gp_XYZ>& theTranslate,
                                            const NCollection_Array1<double>& theTimeStamps);

  //! Constructor that creates translation from a general presentation format.
  //! @param theGeneralPresentation 2D array where each row has 3 values (X,Y,Z) for translation
  //! @param theTimeStamps Array of time values corresponding to each keyframe
  Standard_EXPORT XCAFAnimObjects_Translate(
    const NCollection_Array2<double>& theGeneralPresentation,
    const NCollection_Array1<double>& theTimeStamps);

  //! Copy constructor.
  //! @param theOperation Source operation to copy from
  Standard_EXPORT XCAFAnimObjects_Translate(const Handle(XCAFAnimObjects_Translate)& theOperation);

  //! Returns the type identifier for this operation.
  //! @return XCAFAnimObjects_OperationType_Translate
  XCAFAnimObjects_OperationType GetType() const Standard_OVERRIDE
  {
    return XCAFAnimObjects_OperationType_Translate;
  }

  //! Returns the type name of this operation as a string.
  //! @return "Translate" string
  TCollection_AsciiString GetTypeName() const Standard_OVERRIDE { return "Translate"; }

  //! Converts the XYZ representation to a general 2D array format.
  //! Each row contains 3 values (X,Y,Z) for translation vectors.
  //! @return 2D array containing translation values
  Standard_EXPORT NCollection_Array2<double> GeneralPresentation() const Standard_OVERRIDE;

  //! Returns the raw XYZ array containing translation data.
  //! @return Array of XYZ values defining displacements at keyframes
  const NCollection_Array1<gp_XYZ>& TranslatePresentation() const
  {
    return myTranslatePresentation;
  }

private:
  //! Array of XYZ displacement vectors for keyframes
  NCollection_Array1<gp_XYZ> myTranslatePresentation;
};

#endif // _XCAFAnimObjects_Translate_HeaderFile
