// Copyright (c) Open CASCADE 2023
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

#ifndef _StepVisual_TriangulatedSurfaceSet_HeaderFile_
#define _StepVisual_TriangulatedSurfaceSet_HeaderFile_

#include <Standard.hxx>
#include <StepVisual_TessellatedSurfaceSet.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray2OfInteger.hxx>

//! Representation of STEP entity TriangulatedSurfaceSet
class StepVisual_TriangulatedSurfaceSet : public StepVisual_TessellatedSurfaceSet
{
public:
  //! default constructor
  Standard_EXPORT StepVisual_TriangulatedSurfaceSet();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>&   theRepresentationItemName,
                            const occ::handle<StepVisual_CoordinatesList>& theTessellatedFaceCoordinates,
                            const int                    theTessellatedFacePnmax,
                            const occ::handle<TColStd_HArray2OfReal>&      theTessellatedFaceNormals,
                            const occ::handle<TColStd_HArray1OfInteger>&   thePnindex,
                            const occ::handle<TColStd_HArray2OfInteger>&   theTriangles);

  //! Returns field Pnindex
  const occ::handle<TColStd_HArray1OfInteger> Pnindex() const { return myPnindex; }

  //! Sets field Pnindex
  void SetPnindex(const occ::handle<TColStd_HArray1OfInteger>& thePnindex) { myPnindex = thePnindex; }

  //! Returns number of Pnindex
  Standard_EXPORT int NbPnindex() const;

  //! Returns value of Pnindex by its num
  Standard_EXPORT int PnindexValue(const int theNum) const;

  //! Returns field Triangles
  const occ::handle<TColStd_HArray2OfInteger> Triangles() const { return myTriangles; }

  //! Sets field Triangles
  void SetTriangles(const occ::handle<TColStd_HArray2OfInteger>& theTriangles)
  {
    myTriangles = theTriangles;
  }

  //! Returns number of Triangles
  Standard_EXPORT int NbTriangles() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_TriangulatedSurfaceSet, StepVisual_TessellatedSurfaceSet)

private:
  occ::handle<TColStd_HArray1OfInteger> myPnindex;
  occ::handle<TColStd_HArray2OfInteger> myTriangles;
};

#endif // _StepVisual_TriangulatedSurfaceSet_HeaderFile_
