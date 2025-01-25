// Created on: 2012-04-02

// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#ifndef _IVtkDraw_HighlightAndSelectionPipeline_HeaderFile
#define _IVtkDraw_HighlightAndSelectionPipeline_HeaderFile

#include <NCollection_DataMap.hxx>
#include <NCollection_Shared.hxx>
#include <Standard_Transient.hxx>
#include <TopoDS_Shape.hxx>

// prevent disabling some MSVC warning messages by VTK headers
#include <Standard_WarningsDisable.hxx>
#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <Standard_WarningsRestore.hxx>

#include <IVtk_Types.hxx>
#include <IVtkTools_DisplayModeFilter.hxx>
#include <IVtkTools_SubPolyDataFilter.hxx>

typedef NCollection_DataMap<IVtk_IdType, vtkSmartPointer<IVtkTools_DisplayModeFilter>>
  DisplayModeFiltersMap;
typedef NCollection_DataMap<IVtk_IdType, vtkSmartPointer<IVtkTools_SubPolyDataFilter>>
  SubShapesFiltersMap;

class Prs3d_Drawer;

class IVtkDraw_HighlightAndSelectionPipeline;
DEFINE_STANDARD_HANDLE(IVtkDraw_HighlightAndSelectionPipeline, Standard_Transient)

class IVtkDraw_HighlightAndSelectionPipeline : public Standard_Transient
{
public:
  DEFINE_STANDARD_RTTIEXT(IVtkDraw_HighlightAndSelectionPipeline, Standard_Transient)

public:
  //! Filters comprising the pipeline.
  enum FilterId
  {
    Filter_DM_Shape = 1, //!< Display Mode filter for shape.
    Filter_DM_Hili,      //!< Display Mode filter for highlighting.
    Filter_DM_Sel,       //!< Display Mode filter for selection.
    Filter_SUB_Hili,     //!< Sub-shapes filter for highlighting.
    Filter_SUB_Sel       //!< Sub-shapes filter for selection.
  };

public:
  IVtkDraw_HighlightAndSelectionPipeline(const TopoDS_Shape&         theShape,
                                         const Standard_Integer      theShapeID,
                                         const Handle(Prs3d_Drawer)& theDrawerLink);

  ~IVtkDraw_HighlightAndSelectionPipeline() {}

public:
  void AddToRenderer(vtkRenderer* theRenderer);
  void RemoveFromRenderer(vtkRenderer* theRenderer);

  inline vtkActor* Actor() { return myActor; }

  inline vtkMapper* Mapper() { return myMapper; }

  void ClearHighlightFilters();
  void ClearSelectionFilters();

  IVtkTools_DisplayModeFilter* GetDisplayModeFilter();
  IVtkTools_SubPolyDataFilter* GetHighlightFilter();
  IVtkTools_SubPolyDataFilter* GetSelectionFilter();
  IVtkTools_DisplayModeFilter* GetHighlightDMFilter();
  IVtkTools_DisplayModeFilter* GetSelectionDMFilter();

  void SharedVerticesSelectionOn();
  void SharedVerticesSelectionOff();

private:
  //! Auxiliary map of internal filters by their correspondent IDs.
  typedef NCollection_DataMap<FilterId, vtkSmartPointer<vtkAlgorithm>> FilterMap;

private:
  //! Actor.
  vtkSmartPointer<vtkActor> myActor;

  //! Polygonal mapper.
  vtkSmartPointer<vtkPolyDataMapper> myMapper;

  //! Actor for highlighting.
  vtkSmartPointer<vtkActor> myHiliActor;

  //! Polygonal mapper for highlighting.
  vtkSmartPointer<vtkPolyDataMapper> myHiliMapper;

  //! Actor for selection.
  vtkSmartPointer<vtkActor> mySelActor;

  //! Polygonal mapper for selection.
  vtkSmartPointer<vtkPolyDataMapper> mySelMapper;

  //! Map of involved VTK filters.
  FilterMap myFilterMap;
};

//! Mapping between OCCT topological shape IDs and their correspondent
//! visualization pipelines.
typedef NCollection_Shared<
  NCollection_DataMap<IVtk_IdType, Handle(IVtkDraw_HighlightAndSelectionPipeline)>>
  ShapePipelineMap;

#endif
