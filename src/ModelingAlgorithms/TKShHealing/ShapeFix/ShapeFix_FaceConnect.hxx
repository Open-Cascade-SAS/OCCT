// Copyright (c) 1999 Matra Datavision
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

#ifndef _ShapeFix_FaceConnect_HeaderFile
#define _ShapeFix_FaceConnect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
class TopoDS_Face;
class TopoDS_Shell;

//! Rebuilds selected connections between faces of a shell.
class ShapeFix_FaceConnect
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs an empty face-connectivity tool.
  Standard_EXPORT ShapeFix_FaceConnect();

  //! Registers two faces whose free edges should be connected by Build().
  //! Repeated registration of the same pair has no effect.
  //! @param[in] theFirstFace first face in the connection
  //! @param[in] theSecondFace second face in the connection
  //! @return false if either face is null; true otherwise
  Standard_EXPORT bool Add(const TopoDS_Face& theFirstFace, const TopoDS_Face& theSecondFace);

  //! Rebuilds registered face connections in the shell.
  //! @param[in] theShell shell containing the registered faces
  //! @param[in] theSewingTolerance tolerance used to sew free edges
  //! @param[in] theFixingTolerance precision used to repair rebuilt wires
  //! @return rebuilt shell, or the input shell when no registered connection can be rebuilt
  Standard_EXPORT TopoDS_Shell Build(const TopoDS_Shell& theShell,
                                     const double        theSewingTolerance,
                                     const double        theFixingTolerance);

  //! Clears all registered face connections and intermediate results.
  Standard_EXPORT void Clear();

private:
  Standard_EXPORT void collectFreeEdges(const TopoDS_Shell& theShell);

  Standard_EXPORT void sewConnectedFaces(const double theSewingTolerance);

  Standard_EXPORT TopoDS_Shell rebuildShell(const TopoDS_Shell& theShell,
                                            const double        theSewingTolerance,
                                            const double        theFixingTolerance);

  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    myConnected;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    myOriFreeEdges;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    myResFreeEdges;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    myResSharEdges;
};

#endif // _ShapeFix_FaceConnect_HeaderFile
