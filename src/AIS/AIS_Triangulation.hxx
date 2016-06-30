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

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <Standard_Integer.hxx>
#include <AIS_InteractiveObject.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_Real.hxx>
class Poly_Triangulation;
class Prs3d_Presentation;


class AIS_Triangulation;
DEFINE_STANDARD_HANDLE(AIS_Triangulation, AIS_InteractiveObject)

//! Interactive object that draws data from  Poly_Triangulation, optionally with colors associated
//! with each triangulation vertex. For maximum efficiency colors are represented as 32-bit integers
//! instead of classic Quantity_Color values.
//! Interactive selection of triangles and vertices is not yet implemented.
class AIS_Triangulation : public AIS_InteractiveObject
{

public:

  
  //! Constructs the Triangulation display object
  Standard_EXPORT AIS_Triangulation(const Handle(Poly_Triangulation)& aTriangulation);
  

  //! Set the color for each node.
  //! Each 32-bit color is Alpha << 24 + Blue << 16 + Green << 8 + Red
  //! Order of color components is essential for further usage by OpenGL
  Standard_EXPORT void SetColors (const Handle(TColStd_HArray1OfInteger)& aColor);
  

  //! Get the color for each node.
  //! Each 32-bit color is Alpha << 24 + Blue << 16 + Green << 8 + Red
  Standard_EXPORT Handle(TColStd_HArray1OfInteger) GetColors() const;

  //! Returns true if triangulation has vertex colors.
  Standard_Boolean HasVertexColors() const
  {
    return (myFlagColor == 1);
  }
  
  Standard_EXPORT void SetTriangulation (const Handle(Poly_Triangulation)& aTriangulation);
  
  //! Returns Poly_Triangulation .
  Standard_EXPORT Handle(Poly_Triangulation) GetTriangulation() const;

  //! Sets the value aValue for transparency in the reconstructed compound shape.
  Standard_EXPORT virtual void SetTransparency (const Standard_Real aValue = 0.6) Standard_OVERRIDE;

  //! Removes the setting for transparency in the reconstructed compound shape.
  Standard_EXPORT virtual void UnsetTransparency() Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(AIS_Triangulation,AIS_InteractiveObject)

protected:

  Standard_EXPORT void updatePresentation();



private:

  
  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager, const Handle(Prs3d_Presentation)& aPresentation, const Standard_Integer aMode = 0) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& aSelection, const Standard_Integer aMode) Standard_OVERRIDE;
  
  //! Attenuates 32-bit color by a given attenuation factor (0...1):
  //! aColor = Alpha << 24 + Blue << 16 + Green << 8 + Red
  //! All color components are multiplied by aComponent, the result is then packed again as 32-bit integer.
  //! Color attenuation is applied to the vertex colors in order to have correct visual result
  //! after glColorMaterial(GL_AMBIENT_AND_DIFFUSE). Without it, colors look unnatural and flat.
  Standard_EXPORT Graphic3d_Vec4ub attenuateColor (const Standard_Integer theColor, const Standard_Real theComponent);

  Handle(Poly_Triangulation) myTriangulation;
  Handle(TColStd_HArray1OfInteger) myColor;
  Standard_Integer myFlagColor;
  Standard_Integer myNbNodes;
  Standard_Integer myNbTriangles;


};







#endif // _AIS_Triangulation_HeaderFile
