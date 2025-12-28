// Created on: 1994-06-02
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _IGESSelect_SplineToBSpline_HeaderFile
#define _IGESSelect_SplineToBSpline_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_Transformer.hxx>
class Interface_CopyControl;
class Interface_Graph;
class Interface_Protocol;
class Interface_CheckIterator;
class Interface_InterfaceModel;
class Standard_Transient;
class TCollection_AsciiString;

//! This type of Transformer allows to convert Spline Curves (IGES
//! type 112) and Surfaces (IGES Type 126) to BSpline Curves (IGES
//! type 114) and Surfac (IGES Type 128). All other entities are
//! rebuilt as identical but on the basis of this conversion.
//!
//! It also gives an option to, either convert as such (i.e. each
//! starting part of the spline becomes a segment of the bspline,
//! with continuity C0 between segments), or try to increase
//! continuity as far as possible to C1 or to C2.
//!
//! It does nothing if the starting model contains no Spline
//! Curve (IGES Type 112) or Surface (IGES Type 126). Else,
//! converting and rebuilding implies copying of entities.
class IGESSelect_SplineToBSpline : public IFSelect_Transformer
{

public:
  //! Creates a Transformer SplineToBSpline. If <tryC2> is True,
  //! it will in addition try to upgrade continuity up to C2.
  Standard_EXPORT IGESSelect_SplineToBSpline(const bool tryC2);

  //! Returns the option TryC2 given at creation time
  Standard_EXPORT bool OptionTryC2() const;

  //! Performs the transformation, if there is at least one Spline
  //! Curve (112) or Surface (126). Does nothing if there is none.
  Standard_EXPORT bool Perform(const Interface_Graph&                 G,
                               const occ::handle<Interface_Protocol>& protocol,
                               Interface_CheckIterator&               checks,
                               occ::handle<Interface_InterfaceModel>& newmod) override;

  //! Returns the transformed entities.
  //! If original data contained no Spline Curve or Surface,
  //! the result is identity : <entto> = <entfrom>
  //! Else, the copied counterpart is returned : for a Spline Curve
  //! or Surface, it is a converted BSpline Curve or Surface. Else,
  //! it is the result of general service Copy (rebuilt as necessary
  //! by BSPlines replacing Splines).
  Standard_EXPORT bool Updated(const occ::handle<Standard_Transient>& entfrom,
                               occ::handle<Standard_Transient>&       entto) const override;

  //! Returns a text which defines the way a Transformer works :
  //! "Conversion Spline to BSpline" and as opted,
  //! " trying to upgrade continuity"
  Standard_EXPORT TCollection_AsciiString Label() const override;

  DEFINE_STANDARD_RTTIEXT(IGESSelect_SplineToBSpline, IFSelect_Transformer)

private:
  bool                               thetryc2;
  bool                               thefound;
  occ::handle<Interface_CopyControl> themap;
};

#endif // _IGESSelect_SplineToBSpline_HeaderFile
