// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _Graphic3d_CameraTile_HeaderFile
#define _Graphic3d_CameraTile_HeaderFile

#include <Graphic3d_Vec2.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
#include <Standard_TypeDef.hxx>

//! Class defines the area (Tile) inside a view.
class Graphic3d_CameraTile
{
public:
  Graphic3d_Vec2i TotalSize; //!< total size of the View area, in pixels
  Graphic3d_Vec2i TileSize;  //!< size of the Tile, in pixels
  // clang-format off
  Graphic3d_Vec2i Offset;    //!< the lower-left corner of the Tile relative to the View area (or upper-left if IsTopDown is true), in pixels
  bool            IsTopDown; //!< indicate the offset coordinate system - lower-left (default) or top-down
  // clang-format on

public:
  //! Default constructor.
  //! Initializes the empty Tile of zero size and lower-left offset orientation.
  //! Such Tile is considered uninitialized (invalid).
  Graphic3d_CameraTile()
      : IsTopDown(false)
  {
  }

  //! Return true if Tile has been defined.
  bool IsValid() const
  {
    return TotalSize.x() > 0 && TotalSize.y() > 0 && TileSize.x() > 0 && TileSize.y() > 0;
  }

  //! Return offset position from lower-left corner.
  Graphic3d_Vec2i OffsetLowerLeft() const
  {
    return Graphic3d_Vec2i(Offset.x(), !IsTopDown ? Offset.y() : TotalSize.y() - Offset.y() - 1);
  }

  //! Return the copy cropped by total size
  Graphic3d_CameraTile Cropped() const
  {
    Graphic3d_CameraTile aTile = *this;
    if (!IsValid())
    {
      return aTile;
    }

    aTile.Offset.x() = Max(Offset.x(), 0);
    aTile.Offset.y() = Max(Offset.y(), 0);

    const Standard_Integer anX = Min(Offset.x() + TileSize.x(), TotalSize.x());
    const Standard_Integer anY = Min(Offset.y() + TileSize.y(), TotalSize.y());
    aTile.TileSize.x()         = anX - Offset.x();
    aTile.TileSize.y()         = anY - Offset.y();
    return aTile;
  }

  //! Equality check.
  bool operator==(const Graphic3d_CameraTile& theOther) const
  {
    const Graphic3d_Vec2i anOffset1 = OffsetLowerLeft();
    const Graphic3d_Vec2i anOffset2 = theOther.OffsetLowerLeft();
    return TotalSize.x() == theOther.TotalSize.x() && TotalSize.y() == theOther.TotalSize.y()
           && TileSize.x() == theOther.TileSize.x() && TileSize.y() == theOther.TileSize.y()
           && anOffset1.x() == anOffset2.x() && anOffset1.y() == anOffset2.y();
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;
};

#endif // _Graphic3d_CameraTile_HeaderFile
