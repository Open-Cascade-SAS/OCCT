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

#include <IVtkTools_SubPolyDataFilter.hxx>
#include <IVtkVTK_ShapeData.hxx>

// prevent disabling some MSVC warning messages by VTK headers
#ifdef _MSC_VER
  #pragma warning(push)
#endif
#include <vtkCellData.h>
#include <vtkFloatArray.h>
#include <vtkGenericCell.h>
#include <vtkIdList.h>
#include <vtkIdTypeArray.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#ifdef _MSC_VER
  #pragma warning(pop)
#endif

namespace
{
//! Modified version of vtkPolyData::CopyCells() that includes copying of normals.
//! How to ask vtkPolyData::CopyCells() to do that automatically?
static void copyCells(vtkPolyData* theDst, vtkPolyData* theSrc, vtkIdList* theIdList)
{
  // theDst->CopyCells (theSrc, theIdList);

  const vtkIdType aNbPts       = theSrc->GetNumberOfPoints();
  vtkDataArray*   anOldNormals = theSrc->GetPointData()->GetNormals();

  if (theDst->GetPoints() == NULL)
  {
    theDst->SetPoints(vtkSmartPointer<vtkPoints>::New());
  }

  vtkSmartPointer<vtkIdList>      aNewCellPts = vtkSmartPointer<vtkIdList>::New();
  vtkSmartPointer<vtkGenericCell> aCell       = vtkSmartPointer<vtkGenericCell>::New();
  NCollection_Vec3<double>        anXYZ;
  vtkPointData*                   aNewPntData  = theDst->GetPointData();
  vtkCellData*                    aNewCellData = theDst->GetCellData();
  vtkPoints*                      aNewPoints   = theDst->GetPoints();
  vtkSmartPointer<vtkFloatArray>  aNewNormals;
  if (anOldNormals != NULL)
  {
    aNewNormals = vtkSmartPointer<vtkFloatArray>::New();
    aNewNormals->SetName("Normals");
    aNewNormals->SetNumberOfComponents(3);
    theDst->GetPointData()->SetNormals(aNewNormals);
  }

  // clang-format off
    vtkSmartPointer<vtkIdList> aPntMap = vtkSmartPointer<vtkIdList>::New(); // maps old pt ids into new
  // clang-format on
  aPntMap->SetNumberOfIds(aNbPts);
  for (vtkIdType i = 0; i < aNbPts; ++i)
  {
    aPntMap->SetId(i, -1);
  }

  // Filter the cells
  for (vtkIdType aCellIter = 0; aCellIter < theIdList->GetNumberOfIds(); ++aCellIter)
  {
    theSrc->GetCell(theIdList->GetId(aCellIter), aCell);
    vtkIdList*      aCellPts   = aCell->GetPointIds();
    const vtkIdType aNbCellPts = aCell->GetNumberOfPoints();
    for (vtkIdType i = 0; i < aNbCellPts; ++i)
    {
      const vtkIdType aPtId  = aCellPts->GetId(i);
      vtkIdType       aNewId = aPntMap->GetId(aPtId);
      if (aNewId < 0)
      {
        theSrc->GetPoint(aPtId, anXYZ.ChangeData());

        aNewId = aNewPoints->InsertNextPoint(anXYZ.GetData());
        aPntMap->SetId(aPtId, aNewId);
        aNewPntData->CopyData(theSrc->GetPointData(), aPtId, aNewId);

        if (anOldNormals != NULL)
        {
          anOldNormals->GetTuple(aPtId, anXYZ.ChangeData());
          aNewNormals->InsertNextTuple(anXYZ.GetData());
        }
      }
      aNewCellPts->InsertId(i, aNewId);
    }

    const vtkIdType aNewCellId = theDst->InsertNextCell(aCell->GetCellType(), aNewCellPts);
    aNewCellData->CopyData(theSrc->GetCellData(), theIdList->GetId(aCellIter), aNewCellId);
    aNewCellPts->Reset();
  }
}
} // namespace

vtkStandardNewMacro(IVtkTools_SubPolyDataFilter)

  //================================================================
  // Function : Constructor
  // Purpose  :
  //================================================================
  IVtkTools_SubPolyDataFilter::IVtkTools_SubPolyDataFilter()
    : myIdsArrayName(IVtkVTK_ShapeData::ARRNAME_SUBSHAPE_IDS()),
      myDoFiltering(true),
      myToCopyNormals(true)
{
  //
}

//=================================================================================================

IVtkTools_SubPolyDataFilter::~IVtkTools_SubPolyDataFilter() {}

//================================================================
// Function : RequestData
// Purpose  : Filter cells according to the given set of ids.
//================================================================
int IVtkTools_SubPolyDataFilter::RequestData(vtkInformation*        vtkNotUsed(theRequest),
                                             vtkInformationVector** theInputVector,
                                             vtkInformationVector*  theOutputVector)
{
  // get the input and output
  vtkSmartPointer<vtkInformation> anInInfo  = theInputVector[0]->GetInformationObject(0);
  vtkSmartPointer<vtkInformation> anOutInfo = theOutputVector->GetInformationObject(0);

  vtkSmartPointer<vtkPolyData> anInput =
    vtkPolyData::SafeDownCast(anInInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkSmartPointer<vtkPolyData> anOutput =
    vtkPolyData::SafeDownCast(anOutInfo->Get(vtkDataObject::DATA_OBJECT()));

  anInput->Modified();

  if (myDoFiltering)
  {
    vtkSmartPointer<vtkCellData>    anInputCellData  = anInput->GetCellData();
    vtkSmartPointer<vtkCellData>    anOutputCellData = anOutput->GetCellData();
    vtkSmartPointer<vtkIdTypeArray> aDataArray =
      vtkIdTypeArray::SafeDownCast(anInputCellData->GetArray(myIdsArrayName));

    // List of cell ids to be passed
    vtkSmartPointer<vtkIdList> anIdList = vtkSmartPointer<vtkIdList>::New();
    anIdList->Allocate(myIdsSet.Extent()); // Allocate the list of ids

    const vtkIdType aSize = aDataArray.GetPointer() != NULL ? aDataArray->GetNumberOfTuples() : 0;
    if (aSize != 0)
    {
      anIdList->Allocate(aSize); // Allocate the list of ids
    }

    // Prepare the list of ids from the set of ids.
    // Iterate on input cells.
#if (VTK_MAJOR_VERSION >= 9)
    // Count number of different cells.
    int aNbVerts = 0, aNbLines = 0, aNbPolys = 0, aNbStrips = 0;
    int aNbVertPts = 0, aNbLinePts = 0, aNbPolyPts = 0, aNbStripPts = 0;
#endif
    if (!myIdsSet.IsEmpty())
    {
      for (vtkIdType anI = 0; anI < aSize; anI++)
      {
        if (myIdsSet.Contains(aDataArray->GetValue(anI)))
        {
          // Add a cell id to output if it's value is in the set.
          anIdList->InsertNextId(anI);
#if (VTK_MAJOR_VERSION >= 9)
          switch (anInput->GetCellType(anI))
          {
            case VTK_VERTEX:
              aNbVerts++;
              aNbVertPts++;
              break;
            case VTK_POLY_VERTEX:
              aNbVerts++;
              aNbVertPts += anInput->GetCell(anI)->GetNumberOfPoints();
              break;
            case VTK_LINE:
              aNbLines++;
              aNbLinePts += 2;
              break;
            case VTK_POLY_LINE:
              aNbLines++;
              aNbLinePts += anInput->GetCell(anI)->GetNumberOfPoints();
              break;
            case VTK_TRIANGLE:
              aNbPolys++;
              aNbPolyPts += 3;
              break;
            case VTK_QUAD:
              aNbPolys++;
              aNbPolyPts += 4;
              break;
            case VTK_POLYGON:
              aNbPolys++;
              aNbPolyPts += anInput->GetCell(anI)->GetNumberOfPoints();
              break;
            case VTK_TRIANGLE_STRIP:
              aNbStrips++;
              aNbStripPts += anInput->GetCell(anI)->GetNumberOfPoints();
              break;
          }
#endif
        }
      }
    }

    // Copy cells with their points according to the prepared list of cell ids.
    anOutputCellData->AllocateArrays(anInputCellData->GetNumberOfArrays());
    // Allocate output cells
#if (VTK_MAJOR_VERSION >= 9)
    anOutput->AllocateExact(aNbVerts,
                            aNbVertPts,
                            aNbLines,
                            aNbLinePts,
                            aNbPolys,
                            aNbPolyPts,
                            aNbStrips,
                            aNbStripPts);
#else
    anOutput->Allocate(anInput, anIdList->GetNumberOfIds());
#endif

    // Pass data arrays.
    // Create new arrays for output data
    for (Standard_Integer anI = 0; anI < anInputCellData->GetNumberOfArrays(); anI++)
    {
      vtkSmartPointer<vtkDataArray> anInArr = anInputCellData->GetArray(anI);
      vtkSmartPointer<vtkDataArray> anOutArr =
        vtkSmartPointer<vtkDataArray>::Take(vtkDataArray::CreateDataArray(anInArr->GetDataType()));

      anOutArr->SetName(anInArr->GetName());
      anOutArr->Allocate(anIdList->GetNumberOfIds() * anInArr->GetNumberOfComponents());
      anOutArr->SetNumberOfTuples(anIdList->GetNumberOfIds());
      anOutArr->SetNumberOfComponents(anInArr->GetNumberOfComponents());
      anOutputCellData->AddArray(anOutArr);
    }

    // Copy cells with ids from our list.
    if (myToCopyNormals)
    {
      copyCells(anOutput, anInput, anIdList);
    }
    else
    {
      anOutput->CopyCells(anInput, anIdList);
    }

    // Copy filtered arrays data
    for (Standard_Integer anI = 0; anI < anInputCellData->GetNumberOfArrays(); anI++)
    {
      vtkSmartPointer<vtkDataArray> anInArr  = anInputCellData->GetArray(anI);
      vtkSmartPointer<vtkDataArray> anOutArr = anOutputCellData->GetArray(anI);
      for (vtkIdType anOutId = 0; anOutId < anIdList->GetNumberOfIds(); anOutId++)
      {
        const vtkIdType anInId = anIdList->GetId(anOutId);
        anOutArr->SetTuple(anOutId, anInId, anInArr);
      }
    }
  }
  else
  {
    anOutput->CopyStructure(anInput);  // Copy points and cells
    anOutput->CopyAttributes(anInput); // Copy data arrays (sub-shapes ids)
  }

  return 1; // Return non-zero value if success and pipeline is not failed.
}

//=================================================================================================

void IVtkTools_SubPolyDataFilter::SetDoFiltering(const bool theDoFiltering)
{
  myDoFiltering = theDoFiltering;
}

//=================================================================================================

void IVtkTools_SubPolyDataFilter::PrintSelf(std::ostream& theOs, vtkIndent theIndent)
{
  this->Superclass::PrintSelf(theOs, theIndent);
  theOs << theIndent << "SubPolyData: " << "\n";
  theOs << theIndent << "   Number of cells to pass: " << myIdsSet.Extent() << "\n";
  theOs << theIndent << "   Cells ids to pass: {";
  // Print the content of the set of ids.
  IVtk_IdTypeMap::Iterator anIter(myIdsSet);
  while (anIter.More())
  {
    theOs << " " << anIter.Value();
    anIter.Next();
    if (anIter.More())
    {
      theOs << "; ";
    }
  }
  theOs << "}" << "\n";
}

//================================================================
// Function : Clear
// Purpose  : Clear ids set to be passed through this filter.
//================================================================
void IVtkTools_SubPolyDataFilter::Clear()
{
  myIdsSet.Clear();
}

//================================================================
// Function : SetData
// Purpose  : Set ids to be passed through this filter.
//================================================================
void IVtkTools_SubPolyDataFilter::SetData(const IVtk_IdTypeMap theSet)
{
  myIdsSet = theSet;
}

//================================================================
// Function : AddData
// Purpose  : Add ids to be passed through this filter.
//================================================================
void IVtkTools_SubPolyDataFilter::AddData(const IVtk_IdTypeMap theSet)
{
  for (IVtk_IdTypeMap::Iterator anIt(theSet); anIt.More(); anIt.Next())
  {
    if (!myIdsSet.Contains(anIt.Value()))
    {
      myIdsSet.Add(anIt.Value());
    }
  }
}

//================================================================
// Function : SetData
// Purpose  : Set ids to be passed through this filter.
//================================================================
void IVtkTools_SubPolyDataFilter::SetData(const IVtk_ShapeIdList theIdList)
{
  myIdsSet.Clear();
  AddData(theIdList);
}

//================================================================
// Function : AddData
// Purpose  : Add ids to be passed through this filter.
//================================================================
void IVtkTools_SubPolyDataFilter::AddData(const IVtk_ShapeIdList theIdList)
{
  for (IVtk_ShapeIdList::Iterator anIt(theIdList); anIt.More(); anIt.Next())
  {
    if (!myIdsSet.Contains(anIt.Value()))
    {
      myIdsSet.Add(anIt.Value());
    }
  }
}

//=================================================================================================

void IVtkTools_SubPolyDataFilter::SetIdsArrayName(const char* theArrayName)
{
  myIdsArrayName = theArrayName;
}
