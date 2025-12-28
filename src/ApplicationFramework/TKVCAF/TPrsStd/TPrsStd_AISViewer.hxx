// Created on: 1998-09-30
// Created by: Denis PASCAL
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _TPrsStd_AISViewer_HeaderFile
#define _TPrsStd_AISViewer_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_Attribute.hxx>
class AIS_InteractiveContext;
class Standard_GUID;
class TDF_Label;
class V3d_Viewer;
class TDF_RelocationTable;

//! The groundwork to define an interactive viewer attribute.
//! This attribute stores an interactive context at the root label.
//! You can only have one instance of this class per data framework.
class TPrsStd_AISViewer : public TDF_Attribute
{

public:
  //! class methods
  //! =============
  Standard_EXPORT static const Standard_GUID& GetID();

  //! returns True if there is an AISViewer attribute in
  //! <acces> Data Framework.
  Standard_EXPORT static bool Has(const TDF_Label& acces);

  //! create and set an AISViewer at. Raise an exception if
  //! Has.
  Standard_EXPORT static occ::handle<TPrsStd_AISViewer> New(
    const TDF_Label&                           access,
    const occ::handle<AIS_InteractiveContext>& selector);

  //! create and set an AISAttribute at root label. The
  //! interactive context is build. Raise an exception if
  //! Has.
  Standard_EXPORT static occ::handle<TPrsStd_AISViewer> New(const TDF_Label&               acces,
                                                            const occ::handle<V3d_Viewer>& viewer);

  //! Finds the viewer attribute at the label access, the
  //! root of the data framework. Calling this function can be used to initialize an AIS viewer
  Standard_EXPORT static bool Find(const TDF_Label& acces, occ::handle<TPrsStd_AISViewer>& A);

  Standard_EXPORT static bool Find(const TDF_Label& acces, occ::handle<AIS_InteractiveContext>& IC);

  Standard_EXPORT static bool Find(const TDF_Label& acces, occ::handle<V3d_Viewer>& V);

  //! AISViewer methods
  //! =================
  Standard_EXPORT static void Update(const TDF_Label& acces);

  Standard_EXPORT TPrsStd_AISViewer();

  //! Updates the viewer at the label access.
  //! access is the root of the data framework.
  Standard_EXPORT void Update() const;

  //! Sets the interactive context ctx for this attribute.
  Standard_EXPORT void SetInteractiveContext(const occ::handle<AIS_InteractiveContext>& ctx);

  //! Returns the interactive context in this attribute.
  Standard_EXPORT occ::handle<AIS_InteractiveContext> GetInteractiveContext() const;

  Standard_EXPORT const Standard_GUID& ID() const override;

  Standard_EXPORT void Restore(const occ::handle<TDF_Attribute>& with) override;

  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;

  Standard_EXPORT void Paste(const occ::handle<TDF_Attribute>&       into,
                             const occ::handle<TDF_RelocationTable>& RT) const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(TPrsStd_AISViewer, TDF_Attribute)

private:
  occ::handle<AIS_InteractiveContext> myInteractiveContext;
};

#endif // _TPrsStd_AISViewer_HeaderFile
