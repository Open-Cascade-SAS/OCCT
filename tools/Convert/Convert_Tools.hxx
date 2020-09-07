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

#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_XYZ.hxx>
#include <Bnd_Box.hxx>
#include <Bnd_OBB.hxx>
#include <NCollection_List.hxx>
#include <Quantity_Color.hxx>
#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <Standard_Dump.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx> 
#include <Standard_SStream.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QString>
#include <QVariant>
#include <Standard_WarningsRestore.hxx>

class Geom_Plane;
class Geom_Transformation;

//! \class Convert_Tools
//! \brief The tool that gives auxiliary methods for qt elements manipulation.
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

  //! Creates box shape
  //! \param theBoundingBox box shape parameters
  //! \return created shape
  Standard_EXPORT static Standard_Boolean CreateShape (const Bnd_Box& theBoundingBox, TopoDS_Shape& theShape);

  //! Creates box shape
  //! \param theBoundingBox box shape parameters
  //! \return created shape
  Standard_EXPORT static Standard_Boolean CreateShape (const Bnd_OBB& theBoundingBox, TopoDS_Shape& theShape);

  //! Creates box shape
  //! \param thePntMin minimum point on the bounding box
  //! \param thePntMax maximum point on the bounding box
  //! \return created shape
  Standard_EXPORT static Standard_Boolean CreateBoxShape (const gp_Pnt& thePntMin,
                                                          const gp_Pnt& thePntMax,
                                                          TopoDS_Shape& theShape);

  //! Creates presentation AIS_Plane
  //! \param thePlane source plane
  //! \param thePresentations container to collect new presentation/s
  Standard_EXPORT static void CreatePresentation (const Handle(Geom_Plane)& thePlane,
    NCollection_List<Handle(Standard_Transient)>& thePresentations);

  //! Creates two presentations base on gp_Trsf: box in initial place and transformed box
  //! \param thePlane source plane
  //! \param thePresentations container to collect new presentation/s
  Standard_EXPORT static void CreatePresentation (const gp_Trsf& theTrsf,
    NCollection_List<Handle(Standard_Transient)>& thePresentations);

};

#endif
