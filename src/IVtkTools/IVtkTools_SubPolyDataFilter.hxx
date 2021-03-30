// Created on: 2011-10-27
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

#ifndef IVtkTOOLS_SUBPOLYDATAFILTER_H
#define IVtkTOOLS_SUBPOLYDATAFILTER_H

#include <IVtkTools.hxx>

#include <Standard_WarningsDisable.hxx>
#include <vtkPolyDataAlgorithm.h>
#include <Standard_WarningsRestore.hxx>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251) // avoid warning C4251: "class needs to have dll-interface..."
#endif

//! @class IVtkTools_SubPolyDataFilter 
//! @brief Cells filter according to the given set of cells ids.
class Standard_EXPORT IVtkTools_SubPolyDataFilter : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(IVtkTools_SubPolyDataFilter,vtkPolyDataAlgorithm)

  static IVtkTools_SubPolyDataFilter *New();
  void PrintSelf (std::ostream& theOs, vtkIndent theIndent);

  //! Set ids to be passed through this filter.
  void SetData(const IVtk_IdTypeMap theSet);

  //! Add ids to be passed through this filter.
  void AddData(const IVtk_IdTypeMap theSet);

  //! Set ids to be passed through this filter.
  void SetData(const IVtk_ShapeIdList theIds);

  //! Add ids to be passed through this filter.
  void AddData(const IVtk_ShapeIdList theIds);

  //! Clear ids set to be passed through this filter.
  void Clear();

  //! Set ids array name.
  void SetIdsArrayName(const char* theArrayName);

  void SetDoFiltering (const bool theDoFiltering);

protected:
  //! @brief Filter cells according to the given set of ids.
  //! Note: Data arrays are not passed through if filtering is turned on.
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) Standard_OVERRIDE;

  IVtkTools_SubPolyDataFilter();
  virtual ~IVtkTools_SubPolyDataFilter();

protected:
  //! Set of ids to be passed through this filter.
  IVtk_IdTypeMap myIdsSet;
  const char*    myIdsArrayName;
  bool           myDoFiltering;
  bool           myToCopyNormals;
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // IVtkTOOLS_SUBPOLYDATAFILTER_H
