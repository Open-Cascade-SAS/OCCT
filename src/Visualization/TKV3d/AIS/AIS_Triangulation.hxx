// Created on: 2009-12-10
// Created by: Paul SUPRYATKIN
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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

#ifndef _AIS_Triangulation_HeaderFile
#define _AIS_Triangulation_HeaderFile

#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <AIS_InteractiveObject.hxx>

class Poly_Triangulation;

//! Interactive object that draws data from Poly_Triangulation, optionally with colors associated
//! with each triangulation vertex. For maximum efficiency colors are represented as 32-bit integers
//! instead of classic Quantity_Color values.
//! Interactive selection of triangles and vertices is not yet implemented.
class AIS_Triangulation : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTIEXT(AIS_Triangulation, AIS_InteractiveObject)
public:
  //! Constructs the Triangulation display object
  Standard_EXPORT AIS_Triangulation(const occ::handle<Poly_Triangulation>& aTriangulation);

  //! Set the color for each node.
  //! Each 32-bit color is Alpha << 24 + Blue << 16 + Green << 8 + Red
  //! Order of color components is essential for further usage by OpenGL
  Standard_EXPORT void SetColors(const occ::handle<NCollection_HArray1<int>>& aColor);

  //! Get the color for each node.
  //! Each 32-bit color is Alpha << 24 + Blue << 16 + Green << 8 + Red
  Standard_EXPORT occ::handle<NCollection_HArray1<int>> GetColors() const;

  //! Returns true if triangulation has vertex colors.
  bool HasVertexColors() const { return (myFlagColor == 1); }

  Standard_EXPORT void SetTriangulation(const occ::handle<Poly_Triangulation>& aTriangulation);

  //! Returns Poly_Triangulation .
  Standard_EXPORT occ::handle<Poly_Triangulation> GetTriangulation() const;

  //! Sets the value aValue for transparency in the reconstructed compound shape.
  Standard_EXPORT virtual void SetTransparency(const double aValue = 0.6) override;

  //! Removes the setting for transparency in the reconstructed compound shape.
  Standard_EXPORT virtual void UnsetTransparency() override;

protected:
  Standard_EXPORT void updatePresentation();

private:
  Standard_EXPORT virtual void Compute(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                                       const occ::handle<Prs3d_Presentation>&         thePrs,
                                       const int theMode) override;

  Standard_EXPORT virtual void ComputeSelection(const occ::handle<SelectMgr_Selection>& theSel,
                                                const int theMode) override;

  //! Attenuates 32-bit color by a given attenuation factor (0...1):
  //! aColor = Alpha << 24 + Blue << 16 + Green << 8 + Red
  //! All color components are multiplied by aComponent, the result is then packed again as 32-bit
  //! integer. Color attenuation is applied to the vertex colors in order to have correct visual
  //! result after glColorMaterial(GL_AMBIENT_AND_DIFFUSE). Without it, colors look unnatural and
  //! flat.
  Standard_EXPORT NCollection_Vec4<uint8_t> attenuateColor(const int theColor,
                                                  const double    theComponent);

  occ::handle<Poly_Triangulation>       myTriangulation;
  occ::handle<NCollection_HArray1<int>> myColor;
  int                 myFlagColor;
  int                 myNbNodes;
  int                 myNbTriangles;
};

#endif // _AIS_Triangulation_HeaderFile
