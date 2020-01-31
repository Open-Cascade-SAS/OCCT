// Created on: 2020-01-25
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef Convert_Tools_H
#define Convert_Tools_H

#include <NCollection_List.hxx>
#include <Quantity_Color.hxx>
#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <Standard_SStream.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx> 

//! \class Convert_Tools
//! \brief The tool that gives auxiliary methods converting.
class Convert_Tools
{
public:
  //! Reads Shape using BREP reader
  //! \param theFileName a file name
  //! \return shape or NULL
  Standard_EXPORT static TopoDS_Shape ReadShape (const TCollection_AsciiString& theFileName);


  //! Creates shape presentations on the stream if possible. Tries to init some OCCT base for a new presentation
  //! \param theStream source of presentation
  //! \param thePresentations container to collect new presentations
  Standard_EXPORT static void ConvertStreamToPresentations (const Standard_SStream& theSStream,
                                                            const Standard_Integer theStartPos,
                                                            const Standard_Integer theLastPos,
                                                            NCollection_List<Handle(Standard_Transient)>& thePresentations);

  //! Converts stream to color if possible. It processes Quantity_Color, Quantity_ColorRGBA
  //! \param theStream source of presentation
  //! \param theColor [out] converted color
  //! \returns true if done
  Standard_EXPORT static Standard_Boolean ConvertStreamToColor (const Standard_SStream& theSStream,
                                                                Quantity_Color& theColor);
};

#endif
