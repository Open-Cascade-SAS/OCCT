// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( TCD )
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

#ifndef _IGESGraph_TextDisplayTemplate_HeaderFile
#define _IGESGraph_TextDisplayTemplate_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
class IGESGraph_TextFontDef;
class gp_Pnt;

//! defines IGES TextDisplayTemplate Entity,
//! Type <312>, form <0, 1> in package IGESGraph
//!
//! Used to set parameters for display of information
//! which has been logically included in another entity
//! as a parameter value
class IGESGraph_TextDisplayTemplate : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESGraph_TextDisplayTemplate();

  //! This method is used to set the fields of the class
  //! TextDisplayTemplate
  //! - aWidth         : Character box width
  //! - aHeight        : Character box height
  //! - afontCode      : Font code
  //! - aFontEntity    : Text Font Definition Entity
  //! - aSlantAngle    : Slant angle
  //! - aRotationAngle : Rotation angle
  //! - aMirrorFlag    : Mirror Flag
  //! - aRotationFlag  : Rotate internal text flag
  //! - aCorner        : Lower left corner coordinates(Form No. 0),
  //! Increments from coordinates (Form No. 1)
  Standard_EXPORT void Init(const double                              aWidth,
                            const double                              aHeight,
                            const int                                 aFontCode,
                            const occ::handle<IGESGraph_TextFontDef>& aFontEntity,
                            const double                              aSlantAngle,
                            const double                              aRotationAngle,
                            const int                                 aMirrorFlag,
                            const int                                 aRotationFlag,
                            const gp_XYZ&                             aCorner);

  //! Sets <me> to be Incremental (Form 1) if <mode> is True,
  //! or Basolute (Form 0) else
  Standard_EXPORT void SetIncremental(const bool mode);

  //! returns True if entity is Incremental (Form 1).
  //! False if entity is Absolute (Form 0).
  Standard_EXPORT bool IsIncremental() const;

  //! returns Character Box Width.
  Standard_EXPORT double BoxWidth() const;

  //! returns Character Box Height.
  Standard_EXPORT double BoxHeight() const;

  //! returns False if theFontEntity is Null, True otherwise.
  Standard_EXPORT bool IsFontEntity() const;

  //! returns the font code.
  Standard_EXPORT int FontCode() const;

  //! returns Text Font Definition Entity used to define the font.
  Standard_EXPORT occ::handle<IGESGraph_TextFontDef> FontEntity() const;

  //! returns slant angle of character in radians.
  Standard_EXPORT double SlantAngle() const;

  //! returns Rotation angle of text block in radians.
  Standard_EXPORT double RotationAngle() const;

  //! returns Mirror flag
  //! Mirror flag : 0 = no mirroring.
  //! 1 = mirror axis perpendicular to text base line.
  //! 2 = mirror axis is text base line.
  Standard_EXPORT int MirrorFlag() const;

  //! returns Rotate internal text flag.
  //! Rotate internal text flag : 0 = text horizontal.
  //! 1 = text vertical.
  Standard_EXPORT int RotateFlag() const;

  //! If IsIncremental() returns False,
  //! gets coordinates of lower left corner
  //! of first character box.
  //! If IsIncremental() returns True,
  //! gets increments from X, Y, Z coordinates
  //! found in parent entity.
  Standard_EXPORT gp_Pnt StartingCorner() const;

  //! If IsIncremental() returns False,
  //! gets coordinates of lower left corner
  //! of first character box.
  //! If IsIncremental() returns True,
  //! gets increments from X, Y, Z coordinates
  //! found in parent entity.
  Standard_EXPORT gp_Pnt TransformedStartingCorner() const;

  DEFINE_STANDARD_RTTIEXT(IGESGraph_TextDisplayTemplate, IGESData_IGESEntity)

private:
  double                             theBoxWidth;
  double                             theBoxHeight;
  int                                theFontCode;
  occ::handle<IGESGraph_TextFontDef> theFontEntity;
  double                             theSlantAngle;
  double                             theRotationAngle;
  int                                theMirrorFlag;
  int                                theRotateFlag;
  gp_XYZ                             theCorner;
};

#endif // _IGESGraph_TextDisplayTemplate_HeaderFile
