// Created on: 2011-10-14 
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

#ifndef __IVTKTOOLS_SHAPEDATASOURCE_H__
#define __IVTKTOOLS_SHAPEDATASOURCE_H__

#include <IVtkTools.hxx>
#include <IVtkOCC_Shape.hxx>
#include <IVtkVTK_ShapeData.hxx>
#include <vtkPolyDataAlgorithm.h>

class vtkIdTypeArray;
class vtkPolyData;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251) // avoid warning C4251: "class needs to have dll-interface..."
#endif

//! @class IVtkTools_ShapeDataSource.
//! @brief VTK data source for OCC shapes polygonal data.
class IVtkTools_EXPORT IVtkTools_ShapeDataSource : public vtkPolyDataAlgorithm
{
public:

  vtkTypeMacro(IVtkTools_ShapeDataSource, vtkPolyDataAlgorithm)

  static IVtkTools_ShapeDataSource* New();


public: //! @name Initialization

  //! Set the source OCCT shape.
  //! @param theOccShape [in] OCCT shape wrapper.
  void SetShape(const IVtkOCC_Shape::Handle& theOccShape);

  //! Get the source OCCT shape.
  //! @return occShape OCCT shape wrapper.
  IVtkOCC_Shape::Handle GetShape();
  inline void FastTransformModeOn() { myIsFastTransformMode = true; }
  inline void FastTransformModeOff() { myIsFastTransformMode = false; }

public: //! @name Data accessors

  //! Returns ID of the shape used as a topological input for this data source.
  //! @return requested ID.
  IVtk_IdType GetId() const;

  //! Checks if the internal OccShape pointer is the same the argument.
  //! @param [in] shape OccShape pointer to be checked.
  //! @return true if the two OccShape instances are the same, and false otherwise.
  Standard_Boolean Contains (const IVtkOCC_Shape::Handle& theOccShape) const;

  //! Access to the shape's sub-shape ids array
  //! @returns the array cast to vtkIdTypeArray
  vtkSmartPointer<vtkIdTypeArray> SubShapeIDs();

protected: //! @name Interface to override

  //! This is called by the superclass.
  //! This is the method you should override if you use this class as ancestor.
  //! Build output polygonal data set from the shape wrapper.
  //! @param theRequest [in] information about data object.
  //! In current implementation it is ignored.
  //! @param theInputVector [in] the input data. As adata source is the start
  //! stage of the VTK pipeline, theInputVector is empty and not used (no input port).
  //! @param theOutputVector [in] the pointer to output data, that is filled in this method.
  virtual int RequestData(vtkInformation* theRequest,
                          vtkInformationVector** theInputVector,
                          vtkInformationVector* theOutputVector);

protected: //! @name Internals

  //! Transforms the passed polygonal data by the given OCCT transformation
  //! matrix.
  //! @param theSource [in] source polygonal data to transform.
  //! @param theTrsf [in] transformation to apply.
  //! @return resulting polygonal data (transformed copy of source).
  vtkSmartPointer<vtkPolyData> transform (vtkPolyData* theSource, const gp_Trsf& theTrsf) const;

protected:

  IVtkTools_ShapeDataSource();
  virtual ~IVtkTools_ShapeDataSource();

private:

  IVtkTools_ShapeDataSource (const IVtkTools_ShapeDataSource&);
  IVtkTools_ShapeDataSource& operator= (const IVtkTools_ShapeDataSource&);

protected:

  IVtkOCC_Shape::Handle myOccShape; //!< Shape wrapper used as an input.
  IVtkVTK_ShapeData::Handle myPolyData; //!< Polygonal representation of shape.

  //! Indicates whether light-weighted processing for transformed shapes is
  //! enabled. If so, data source does not re-compute the discrete model for
  //! the input topological shape. It rather uses the already existing one
  //! and applies the necessary transformation to it.
  Standard_Boolean myIsFastTransformMode;

  //! Internal flag indicating that the current working shape is just a
  //! transformed copy of the previously processed one. This flag is used in
  //! a couple with "fast transformation" mode flag.
  Standard_Boolean myIsTransformOnly;

};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // __IVTKTOOLS_SHAPEDATA_H__
