// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <inspector/DFBrowserPane_HelperExport.hxx>

#include <BRepTools.hxx>
#include <TCollection_AsciiString.hxx>

#include <QApplication>
#include <QFileDialog>

// =======================================================================
// function : AddShape
// purpose :
// =======================================================================
void DFBrowserPane_HelperExport::AddShape (const TopoDS_Shape& theShape, const QModelIndexList& theIndices)
{
  for (int anIndicesIt = 0, aCount = theIndices.size(); anIndicesIt < aCount; anIndicesIt++)
    myShapes[theIndices[anIndicesIt]] = theShape;
}

// =======================================================================
// function : OnButtonPressed
// purpose :
// =======================================================================
void DFBrowserPane_HelperExport::OnButtonPressed (const QModelIndex& theIndex)
{
  if (!myShapes.contains (theIndex))
    return;

  const TopoDS_Shape& aShape = myShapes[theIndex];

  if (aShape.IsNull())
    return;

  QString aFileExtension = ".brep";

#if QT_VERSION < 0x050000
  QString aFilter (tr ("BREP file (*%1*)").arg (aFileExtension));
#else
  QString aFilter;
#endif
  QString aSelectedFilter;
  QString aFileName = QFileDialog::getSaveFileName (0, tr ("Export shape to BREP file"), QString(),
                                                    aFilter, &aSelectedFilter);
  if (!aFileName.isEmpty()) {
    QApplication::setOverrideCursor (Qt::WaitCursor);
    if (aFileName.indexOf (aFileExtension) < 0)
      aFileName += QString (aFileExtension);
  
    const TCollection_AsciiString anAsciiName(aFileName.toUtf8().data());
    BRepTools::Write(aShape, anAsciiName.ToCString());
    QApplication::restoreOverrideCursor();
  }
}
