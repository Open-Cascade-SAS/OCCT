// Created on: 2000-06-20
// Created by: Sergey MOZOKHIN
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _VrmlAPI_Writer_HeaderFile
#define _VrmlAPI_Writer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <VrmlAPI_RepresentationOfShape.hxx>
#include <Quantity_HArray1OfColor.hxx>
#include <Standard_Integer.hxx>

class VrmlConverter_Drawer;
class VrmlConverter_Projector;
class Vrml_Material;
class TopoDS_Shape;
class TDocStd_Document;


//! Creates and writes VRML files from Open
//! CASCADE shapes. A VRML file can be written to
//! an existing VRML file or to a new one.
class VrmlAPI_Writer 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a writer object with default parameters.
  Standard_EXPORT VrmlAPI_Writer();
  
  //! Resets all parameters (representation, deflection)
  //! to their default values..
  Standard_EXPORT void ResetToDefaults();
  
  //! Returns drawer object
  Standard_EXPORT Handle(VrmlConverter_Drawer) Drawer() const;
  
  //! Sets the deflection aDef of
  //! the mesh algorithm which is used to compute the shaded
  //! representation of the translated shape. The default
  //! value is -1. When the deflection value is less than
  //! 0, the deflection is calculated from the relative
  //! size of the shaped.
  Standard_EXPORT void SetDeflection (const Standard_Real aDef);
  
  //! Sets the representation of the
  //! shape aRep which is written to the VRML file. The three options are :
  //! -      shaded
  //! -      wireframe
  //! -      both shaded and wireframe (default)
  //! defined through the VrmlAPI_RepresentationOfShape enumeration.
  Standard_EXPORT void SetRepresentation (const VrmlAPI_RepresentationOfShape aRep);
  
  //! Set transparency to given material
  Standard_EXPORT void SetTransparencyToMaterial (Handle(Vrml_Material)& aMaterial, const Standard_Real aTransparency);
  
  Standard_EXPORT void SetShininessToMaterial (Handle(Vrml_Material)& aMaterial, const Standard_Real aShininess);
  
  Standard_EXPORT void SetAmbientColorToMaterial (Handle(Vrml_Material)& aMaterial, const Handle(Quantity_HArray1OfColor)& Color);
  
  Standard_EXPORT void SetDiffuseColorToMaterial (Handle(Vrml_Material)& aMaterial, const Handle(Quantity_HArray1OfColor)& Color);
  
  Standard_EXPORT void SetSpecularColorToMaterial (Handle(Vrml_Material)& aMaterial, const Handle(Quantity_HArray1OfColor)& Color);
  
  Standard_EXPORT void SetEmissiveColorToMaterial (Handle(Vrml_Material)& aMaterial, const Handle(Quantity_HArray1OfColor)& Color);
  
  //! Returns the representation of the shape which is
  //! written to the VRML file. Types of representation are set through the
  //! VrmlAPI_RepresentationOfShape enumeration.
  Standard_EXPORT VrmlAPI_RepresentationOfShape GetRepresentation() const;
  
  Standard_EXPORT Handle(Vrml_Material) GetFrontMaterial() const;
  
  Standard_EXPORT Handle(Vrml_Material) GetPointsMaterial() const;
  
  Standard_EXPORT Handle(Vrml_Material) GetUisoMaterial() const;
  
  Standard_EXPORT Handle(Vrml_Material) GetVisoMaterial() const;
  
  Standard_EXPORT Handle(Vrml_Material) GetLineMaterial() const;
  
  Standard_EXPORT Handle(Vrml_Material) GetWireMaterial() const;
  
  Standard_EXPORT Handle(Vrml_Material) GetFreeBoundsMaterial() const;
  
  Standard_EXPORT Handle(Vrml_Material) GetUnfreeBoundsMaterial() const;
  
  //! Converts the shape aShape to
  //! VRML format of the passed version and writes it to the file identified by aFile.
  Standard_EXPORT Standard_Boolean Write (const TopoDS_Shape& aShape, const Standard_CString aFile,
      const Standard_Integer aVersion = 2) const;

  //! Converts the document to VRML format of the passed version
  //! and writes it to the file identified by aFile.
  Standard_EXPORT Standard_Boolean WriteDoc(
    const Handle(TDocStd_Document) &theDoc,
    const Standard_CString theFile,
    const Standard_Real theScale) const;

protected:

  //! Converts the shape aShape to VRML format of version 1.0 and writes it
  //! to the file identified by aFileName using default parameters.
  Standard_EXPORT Standard_Boolean write_v1 (const TopoDS_Shape& aShape, const Standard_CString aFileName) const;
  
  //! Converts the shape aShape to VRML format of version 2.0 and writes it
  //! to the file identified by aFileName using default parameters.
  Standard_EXPORT Standard_Boolean write_v2 (const TopoDS_Shape& aShape, const Standard_CString aFileName) const;

private:

  VrmlAPI_RepresentationOfShape myRepresentation;
  Handle(VrmlConverter_Drawer) myDrawer;
  Standard_Real myDeflection;
  Handle(VrmlConverter_Projector) myPerespectiveCamera;
  Handle(VrmlConverter_Projector) myOrthographicCamera;
  Standard_Real myTransparency;
  Standard_Real myShininess;
  Handle(Vrml_Material) myFrontMaterial;
  Handle(Vrml_Material) myPointsMaterial;
  Handle(Vrml_Material) myUisoMaterial;
  Handle(Vrml_Material) myVisoMaterial;
  Handle(Vrml_Material) myLineMaterial;
  Handle(Vrml_Material) myWireMaterial;
  Handle(Vrml_Material) myFreeBoundsMaterial;
  Handle(Vrml_Material) myUnfreeBoundsMaterial;
  Standard_Real DX;
  Standard_Real DY;
  Standard_Real DZ;
  Standard_Real XUp;
  Standard_Real YUp;
  Standard_Real ZUp;
  Standard_Real Focus;

};

#endif // _VrmlAPI_Writer_HeaderFile
