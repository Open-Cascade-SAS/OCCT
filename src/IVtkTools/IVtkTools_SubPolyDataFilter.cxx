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
#include <vtkCellData.h>
#include <vtkIdList.h>
#include <vtkIdTypeArray.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>


vtkStandardNewMacro(IVtkTools_SubPolyDataFilter)

//================================================================
// Function : Constructor
// Purpose  : 
//================================================================
IVtkTools_SubPolyDataFilter::IVtkTools_SubPolyDataFilter()
{
  myIdsArrayName = IVtkVTK_ShapeData::ARRNAME_SUBSHAPE_IDS;
  myDoFiltering = true;
}

//================================================================
// Function : Destructor
// Purpose  : 
//================================================================
IVtkTools_SubPolyDataFilter::~IVtkTools_SubPolyDataFilter() { }

//================================================================
// Function : RequestData
// Purpose  : Filter cells according to the given set of ids.
//================================================================
int IVtkTools_SubPolyDataFilter::RequestData (vtkInformation *vtkNotUsed(theRequest),
                                              vtkInformationVector **theInputVector,
                                              vtkInformationVector *theOutputVector)
{
  // get the input and output
  vtkSmartPointer<vtkInformation> anInInfo = theInputVector[0]->GetInformationObject(0);
  vtkSmartPointer<vtkInformation> anOutInfo = theOutputVector->GetInformationObject(0);

  vtkSmartPointer<vtkPolyData> anInput = vtkPolyData::SafeDownCast(
    anInInfo->Get (vtkDataObject::DATA_OBJECT()));

  vtkSmartPointer<vtkPolyData> anOutput = vtkPolyData::SafeDownCast(
    anOutInfo->Get (vtkDataObject::DATA_OBJECT()));

  anInput->Modified();

  if (myDoFiltering)
  {
    vtkSmartPointer<vtkCellData> aCellData = anInput->GetCellData();
    vtkIdType aSize = 0;
    vtkSmartPointer<vtkIdTypeArray> aDataArray =
      vtkIdTypeArray::SafeDownCast (aCellData->GetArray (myIdsArrayName));

    // List of cell ids to be passed
    vtkSmartPointer<vtkIdList> anIdList = vtkSmartPointer<vtkIdList>::New();
    anIdList->Allocate(myIdsSet.Extent());  // Allocate the list of ids

    if (aDataArray.GetPointer() != NULL)
    {
      aSize = aDataArray->GetNumberOfTuples();
      anIdList->Allocate (aSize);  // Allocate the list of ids
    }

    // Prepare the list of ids from the set of ids.
    // Iterate on input cells.
    if (!myIdsSet.IsEmpty())
    {
      for (vtkIdType anI = 0; anI < aSize; anI++)
      {
        if (myIdsSet.Contains (aDataArray->GetValue (anI)))
        {
          // Add a cell id to output if it's value is in the set.
          anIdList->InsertNextId (anI);
        }
      }
    }

    // Copy cells with their points according to the prepared list of cell ids.
    anOutput->GetCellData()->AllocateArrays(anInput->GetCellData()->GetNumberOfArrays());
    anOutput->Allocate(anInput, anIdList->GetNumberOfIds());  // Allocate output cells
    // Pass data arrays.
    // Create new arrays for output data 
    vtkSmartPointer<vtkCellData> anInData = anInput->GetCellData();
    vtkSmartPointer<vtkCellData> anOutData = anOutput->GetCellData();
    vtkSmartPointer<vtkDataArray> anInArr, anOutArr;

    for (Standard_Integer anI = 0; anI < anInData->GetNumberOfArrays(); anI++)
    {
      anInArr = anInData->GetArray (anI);
      anOutArr = vtkSmartPointer<vtkDataArray>::Take(
        vtkDataArray::CreateDataArray(anInArr->GetDataType()));
      anOutArr->SetName(anInArr->GetName());
      anOutArr->Allocate(anIdList->GetNumberOfIds() * anInArr->GetNumberOfComponents());
      anOutArr->SetNumberOfTuples (anIdList->GetNumberOfIds());
      anOutArr->SetNumberOfComponents (anInArr->GetNumberOfComponents());
      anOutData->AddArray(anOutArr);
    }

    // Copy cells with ids from our list.
    anOutput->CopyCells (anInput, anIdList);

    // Copy filtered arrays data
    vtkIdType anOutId, anInId;

    for (Standard_Integer anI = 0; anI < anInData->GetNumberOfArrays(); anI++)
    {
      anInArr = anInData->GetArray (anI);
      anOutArr = anOutData->GetArray(anI);
      for (anOutId = 0; anOutId < anIdList->GetNumberOfIds(); anOutId++)
      {
        anInId = anIdList->GetId (anOutId);
        anOutArr->SetTuple (anOutId, anInId, anInArr);
      }
    }
  }
  else
  {
    anOutput->CopyStructure (anInput);  // Copy points and cells
    anOutput->CopyAttributes (anInput); // Copy data arrays (sub-shapes ids)
  }

  return 1; // Return non-zero value if success and pipeline is not failed.
}

//================================================================
// Function : SetDoFiltering
// Purpose  : 
//================================================================
void IVtkTools_SubPolyDataFilter::SetDoFiltering (const bool theDoFiltering)
{
  myDoFiltering = theDoFiltering;
}

//================================================================
// Function : PrintSelf
// Purpose  : 
//================================================================
void IVtkTools_SubPolyDataFilter::PrintSelf (std::ostream& theOs, vtkIndent theIndent)
{
  this->Superclass::PrintSelf (theOs,theIndent);
  theOs << theIndent << "SubPolyData: " << "\n"; 
  theOs << theIndent << "   Number of cells to pass: " << myIdsSet.Extent() << "\n";
  theOs << theIndent << "   Cells ids to pass: {" ;
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
void IVtkTools_SubPolyDataFilter::SetData (const IVtk_IdTypeMap theSet)
{
  myIdsSet = theSet;
}

//================================================================
// Function : AddData
// Purpose  : Add ids to be passed through this filter.
//================================================================
void IVtkTools_SubPolyDataFilter::AddData (const IVtk_IdTypeMap theSet)
{
  IVtk_IdTypeMap::Iterator anIt (theSet);
  for (; anIt.More(); anIt.Next())
  {
    if (!myIdsSet.Contains (anIt.Value()))
    {
      myIdsSet.Add (anIt.Value());
    }
  }
}

//================================================================
// Function : SetData
// Purpose  : Set ids to be passed through this filter.
//================================================================
void IVtkTools_SubPolyDataFilter::SetData (const IVtk_ShapeIdList theIdList)
{
  myIdsSet.Clear();
  AddData (theIdList);
}

//================================================================
// Function : AddData
// Purpose  : Add ids to be passed through this filter.
//================================================================
void IVtkTools_SubPolyDataFilter::AddData (const IVtk_ShapeIdList theIdList)
{
  IVtk_ShapeIdList::Iterator anIt (theIdList);
  for (; anIt.More(); anIt.Next())
  {
    if (!myIdsSet.Contains (anIt.Value()))
    {
      myIdsSet.Add (anIt.Value());
    }
  }
}

//! Set ids to be passed through this filter.
//================================================================
// Function : SetIdsArrayName
// Purpose  : 
//================================================================
void IVtkTools_SubPolyDataFilter::SetIdsArrayName (const char* theArrayName)
{
  myIdsArrayName = theArrayName;
}
