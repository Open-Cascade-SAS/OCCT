// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <gp_GTrsf.hxx>
#include <IGESGeom_TransformationMatrix.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGeom_TransformationMatrix, IGESData_TransfEntity)

IGESGeom_TransformationMatrix::IGESGeom_TransformationMatrix() = default;

void IGESGeom_TransformationMatrix::Init(const occ::handle<NCollection_HArray2<double>>& aMatrix)
{
  if (aMatrix.IsNull())
  {
    theData = new NCollection_HArray2<double>(1, 3, 1, 4);
    theData->Init(0.0);
    int i = 1;
    for (; i <= 3; i++)
      theData->SetValue(i, i, 1.0);
  }
  if ((aMatrix->RowLength() != 4) || (aMatrix->ColLength() != 3))
    throw Standard_DimensionMismatch("IGESGeom_TransformationMatrix : Init");

  theData = aMatrix;
  if (theData.IsNull())
  {
    return;
  }
  InitTypeAndForm(124, FormNumber());
}

void IGESGeom_TransformationMatrix::SetFormNumber(const int fm)
{
  if (theData.IsNull())
    std::cout << "Inavalid Transformation Data" << std::endl;
  if ((fm < 0 || fm > 1) && (fm < 10 || fm > 12))
    throw Standard_OutOfRange("IGESGeom_TransformationMatrix : SetFormNumber");
  InitTypeAndForm(124, fm);
}

double IGESGeom_TransformationMatrix::Data(const int I, const int J) const
{
  return theData->Value(I, J);
}

gp_GTrsf IGESGeom_TransformationMatrix::Value() const
{
  gp_GTrsf Matrix;
  if (!theData.IsNull())
  {
    for (int I = 1; I <= 3; I++)
    {
      for (int J = 1; J <= 4; J++)
      {
        Matrix.SetValue(I, J, theData->Value(I, J));
      }
    }
  }

  return Matrix;
}
