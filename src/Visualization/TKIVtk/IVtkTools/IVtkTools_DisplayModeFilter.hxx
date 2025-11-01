// Created on: 2011-11-15
// Created by: Roman KOZLOV
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

#ifndef IVtkTOOLS_DISPLAYMODEFILTER_H
#define IVtkTOOLS_DISPLAYMODEFILTER_H

#include <IVtkTools.hxx>
#include <IVtkTools_SubPolyDataFilter.hxx>
#include <NCollection_DataMap.hxx>

#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable : 4251) // avoid warning C4251: "class needs to have dll-interface..."
#endif

//! @class IVtkTools_DisplayModeFilter
//! @brief Cells filter according to the selected display mode by mesh parts types.
//! This filter is used to get parts of a shape according to different
//! display modes.
class Standard_EXPORT IVtkTools_DisplayModeFilter : public IVtkTools_SubPolyDataFilter
{
public:
  vtkTypeMacro(IVtkTools_DisplayModeFilter, IVtkTools_SubPolyDataFilter)

    static IVtkTools_DisplayModeFilter* New();
  void PrintSelf(std::ostream& os, vtkIndent indent) Standard_OVERRIDE;

  //! Set display mode to define cells types to be passed through this filter.
  void SetDisplayMode(const IVtk_DisplayMode aMode);

  //! Display or not shared vertices.
  void SetDisplaySharedVertices(const bool doDisplay);

  //! Get current display mode.
  IVtk_DisplayMode GetDisplayMode() const;

  //! Returns list of displaying mesh element types for the given display mode
  const IVtk_IdTypeMap& MeshTypesForMode(IVtk_DisplayMode theMode) const;

  //! Set a list of displaying mesh element types for the given display mode
  void SetMeshTypesForMode(IVtk_DisplayMode theMode, const IVtk_IdTypeMap& theMeshTypes);

  //! Draw Boundary of faces for shading mode
  void SetFaceBoundaryDraw(bool theToDraw);

  //! Returns True if drawing Boundary of faces for shading mode is defined.
  bool FaceBoundaryDraw() const { return myDrawFaceBoundaries; }

  //! Returns TRUE if vertex normals should be included for smooth shading within DM_Shading mode or
  //! not.
  bool IsSmoothShading() const { return myIsSmoothShading; }

  //! Set if vertex normals should be included for smooth shading or not.
  void SetSmoothShading(bool theIsSmooth);

protected:
  //! Filter cells according to the given set of ids.
  virtual int RequestData(vtkInformation*,
                          vtkInformationVector**,
                          vtkInformationVector*) Standard_OVERRIDE;

  IVtkTools_DisplayModeFilter();
  virtual ~IVtkTools_DisplayModeFilter();

protected:
  // clang-format off
  IVtk_DisplayMode      myDisplayMode;             //!< Display mode defining mesh types to pass through this filter
  IVtk_IdTypeMap        myModesDefinition[2];
  bool                  myDoDisplaySharedVertices;

  bool                  myDrawFaceBoundaries;      //!< Draw Face boundaries within shading display mode
  bool                  myIsSmoothShading;         //!< include vertex normals for smooth shading or not
  // clang-format on
};

#ifdef _MSC_VER
  #pragma warning(pop)
#endif

#endif // IVtkTOOLS_DISPLAYMODEFILTER_H
