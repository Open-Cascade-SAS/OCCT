// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2013 OPEN CASCADE SAS
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

#ifndef OpenGl_GraduatedTrihedron_HeaderFile
#define OpenGl_GraduatedTrihedron_HeaderFile

#include <Graphic3d_GraduatedTrihedron.hxx>
#include <NCollection_Array1.hxx>
#include <OpenGl_Aspects.hxx>
#include <OpenGl_Element.hxx>
#include <OpenGl_PrimitiveArray.hxx>
#include <OpenGl_Text.hxx>


//! This class allows to render Graduated Trihedron, i.e. trihedron with grid.
//! it is based on Graphic3d_GraduatedTrihedron parameters and support its customization
//! on construction level only.
//! @sa Graphic3d_GraduatedTrihedron
class OpenGl_GraduatedTrihedron : public OpenGl_Element
{
public:

  DEFINE_STANDARD_ALLOC

public:

  //! Default constructor.
  Standard_EXPORT OpenGl_GraduatedTrihedron();

  //! Destructor.
  Standard_EXPORT virtual ~OpenGl_GraduatedTrihedron();

  //! Draw the element.
  Standard_EXPORT virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const Standard_OVERRIDE;

  //! Release OpenGL resources.
  Standard_EXPORT virtual void Release (OpenGl_Context* theCtx) Standard_OVERRIDE;

  //! Setup configuration.
  Standard_EXPORT void SetValues (const Graphic3d_GraduatedTrihedron& theData);

  //! Sets up-to-date values of scene bounding box.
  //! Can be used in callback mechanism to get up-to-date values.
  //! @sa Graphic3d_GraduatedTrihedron::CubicAxesCallback
  Standard_EXPORT void SetMinMax (const OpenGl_Vec3& theMin,
                                  const OpenGl_Vec3& theMax);

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

private:

  //! Axis of trihedron. It incapsulates geometry and style.
  class Axis
  {
  public:

    OpenGl_Vec3         Direction;
    Quantity_Color      NameColor;
    OpenGl_Aspects      LineAspect;
    mutable OpenGl_Text Label;
    mutable OpenGl_PrimitiveArray Tickmark;
    mutable OpenGl_PrimitiveArray Line;
    mutable OpenGl_PrimitiveArray Arrow;

  public:

    Axis (const Graphic3d_GraduatedTrihedron::AxisAspect& theAspect =
                Graphic3d_GraduatedTrihedron::AxisAspect(),
          const OpenGl_Vec3& theDirection = OpenGl_Vec3 (1.0f, 0.0f, 0.0f));

    ~Axis();

    Axis& operator= (const Axis& theOther);

    void InitArrow (const Handle(OpenGl_Context)& theContext,
                    const Standard_ShortReal theLength,
                    const OpenGl_Vec3& theNormal) const;

    void InitTickmark (const Handle(OpenGl_Context)& theContext,
                       const OpenGl_Vec3& theDir) const;

    void InitLine (const Handle(OpenGl_Context)& theContext,
                   const OpenGl_Vec3& theDir) const;

    void Release (OpenGl_Context* theCtx);

  };

private:

  //! Struct for triple of orthonormal vectors
  //! and origin point, and axes for tickmarks.
  //! It may be not a right or left coordinate system.
  struct GridAxes
  {
  public:
    GridAxes()
    : Origin (0, 0, 0)
    {
      Axes[0] = OpenGl_Vec3 (1.0f, 0.0f, 0.0f);
      Axes[1] = OpenGl_Vec3 (0.0f, 1.0f, 0.0f);
      Axes[2] = OpenGl_Vec3 (0.0f, 0.0f, 1.0f);

      Ticks[0] = OpenGl_Vec3 (0.0f, 0.0f, 0.0f);
      Ticks[1] = OpenGl_Vec3 (0.0f, 0.0f, 0.0f);
      Ticks[2] = OpenGl_Vec3 (0.0f, 0.0f, 0.0f);
    }

  public: //! @name Main grid directions
    OpenGl_Vec3 Origin;
    OpenGl_Vec3 Axes[3];

  public: //! @name Directions for tickmarks
    OpenGl_Vec3 Ticks[3];
  };

private:

  //! Initialize or update GL resources for rendering trihedron.
  //! @param theContext [in] the GL context.
  void initGlResources (const Handle(OpenGl_Context)& theContext) const;

  //! Gets normal of the view out of user.
  //! @param theContext [in] OpenGL Context
  //! @param theNormal [out] normal of the view out of user
  //! @return distance corresponding to 1 pixel
  Standard_ShortReal getNormal (const Handle(OpenGl_Context)& theContext,
                                OpenGl_Vec3& theNormal) const;

  //! Gets distance to point (theX, theY, theZ) of bounding box along the normal
  //! @param theNormal [in] normal of the view out of user
  //! @param theCenter [in] geometry center of bounding box
  //! @param theX [in] x of target point
  //! @param theY [in] y of target point
  //! @param theZ [in] z of terget point
  Standard_ShortReal getDistanceToCorner (const OpenGl_Vec3& theNormal,
                                          const OpenGl_Vec3& theCenter,
                                          const Standard_ShortReal theX,
                                          const Standard_ShortReal theY,
                                          const Standard_ShortReal theZ) const;

  //! Gets axes of grid
  //! @param theCorners [in] the corners of grid
  //! @param theGridAxes [out] grid axes, the base of graduated trihedron grid.
  Standard_ExtCharacter getGridAxes (const Standard_ShortReal theCorners[8],
                                     GridAxes& theGridAxes) const;

  //! Render line from the transformed primitive array myLine
  //! @param theWorkspace [in] the OpenGl Workspace
  //! @param theMat [in] theMat that contains base transformation and is used for applying
  //!        translation and rotation
  //! @param thaTx the X for vector of translation
  //! @param thaTy the Y for vector of translation
  //! @param thaTz the Z for vector of translation
  void renderLine (const OpenGl_PrimitiveArray&    theLine,
                   const Handle(OpenGl_Workspace)& theWorkspace,
                   const OpenGl_Mat4&              theMat,
                   const Standard_ShortReal        theXt,
                   const Standard_ShortReal        theYt,
                   const Standard_ShortReal        theZt) const;

  //! Render grid lines perpendecular the axis of input index
  //! @param theWorkspace [in] the OpenGl Workspace
  //! @param theIndex [in] index of axis
  //! @param theGridAxes [in] grid axes
  //! @param theMat [in] theMat that contains base transformation and is used for applying
  //!        translation and rotation
  void renderGridPlane (const Handle(OpenGl_Workspace)& theWorkspace,
                        const Standard_Integer& theIndex,
                        const GridAxes& theGridAxes,
                        OpenGl_Mat4& theMat) const;


  //! Render the axis of input index
  //! @param theWorkspace [in] the OpenGl Workspace
  //! @param theIndex [in] index of axis
  //! @param theMat [in] theMat that contains base transformation and is used for applying
  //!        translation and rotation
  void renderAxis (const Handle(OpenGl_Workspace)& theWorkspace,
                   const Standard_Integer& theIndex,
                   const OpenGl_Mat4& theMat) const;

  //! Render grid labels, tickmark lines and labels
  //! @param theWorkspace [in] the OpenGl Workspace
  //! @param theMat [in] theMat that contains base transformation and is used for applying
  //!        translation and rotation
  //! @param theIndex [in] index of axis
  //! @param theGridAxes [in] grid axes
  //! @param theDpix [in] distance corresponding to 1 pixel
  void renderTickmarkLabels (const Handle(OpenGl_Workspace)& theWorkspace,
                             const OpenGl_Mat4& theMat,
                             const Standard_Integer theIndex,
                             const GridAxes& theGridAxes,
                             const Standard_ShortReal theDpix) const;

protected: //! @name Scene bounding box values

  OpenGl_Vec3 myMin;
  OpenGl_Vec3 myMax;

protected:

  mutable Axis myAxes[3]; //!< Axes for trihedron
  mutable Graphic3d_GraduatedTrihedron myData;
  mutable OpenGl_Aspects   myGridLineAspect; //!< Color grid properties

protected: //! @name Labels properties

  mutable OpenGl_Text    myLabelValues;
  mutable OpenGl_Aspects myAspectLabels;
  mutable OpenGl_Aspects myAspectValues;

private:

  mutable Standard_Boolean myIsInitialized;

  enum AxisFlags
  {
    XOO_XYO = 1 << 1,
    XOO_XOZ = 1 << 2,
    OYO_OYZ = 1 << 3,
    OYO_XYO = 1 << 4,
    OOZ_XOZ = 1 << 5,
    OOZ_OYZ = 1 << 6,
    OYZ_XYZ = 1 << 7,
    XOZ_XYZ = 1 << 8,
    XYO_XYZ = 1 << 9
  };
};

#endif //_OpenGl_GraduatedTrihedron_Header
