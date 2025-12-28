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
  Standard_EXPORT ~OpenGl_GraduatedTrihedron() override;

  //! Draw the element.
  Standard_EXPORT void Render(const occ::handle<OpenGl_Workspace>& theWorkspace) const override;

  //! Release OpenGL resources.
  Standard_EXPORT void Release(OpenGl_Context* theCtx) override;

  //! Setup configuration.
  Standard_EXPORT void SetValues(const Graphic3d_GraduatedTrihedron& theData);

  //! Sets up-to-date values of scene bounding box.
  //! Can be used in callback mechanism to get up-to-date values.
  //! @sa Graphic3d_GraduatedTrihedron::CubicAxesCallback
  Standard_EXPORT void SetMinMax(const NCollection_Vec3<float>& theMin,
                                 const NCollection_Vec3<float>& theMax);

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

private:
  //! Axis of trihedron. It incapsulates geometry and style.
  class Axis
  {
  public:
    NCollection_Vec3<float>       Direction;
    Quantity_Color                NameColor;
    OpenGl_Aspects                LineAspect;
    mutable OpenGl_Text           Label;
    mutable OpenGl_PrimitiveArray Tickmark;
    mutable OpenGl_PrimitiveArray Line;
    mutable OpenGl_PrimitiveArray Arrow;

  public:
    Axis(const Graphic3d_GraduatedTrihedron::AxisAspect& theAspect =
           Graphic3d_GraduatedTrihedron::AxisAspect(),
         const NCollection_Vec3<float>& theDirection = NCollection_Vec3<float>(1.0f, 0.0f, 0.0f));

    ~Axis();

    Axis& operator=(const Axis& theOther);

    void InitArrow(const occ::handle<OpenGl_Context>& theContext,
                   const float                        theLength,
                   const NCollection_Vec3<float>&     theNormal) const;

    void InitTickmark(const occ::handle<OpenGl_Context>& theContext,
                      const NCollection_Vec3<float>&     theDir) const;

    void InitLine(const occ::handle<OpenGl_Context>& theContext,
                  const NCollection_Vec3<float>&     theDir) const;

    void Release(OpenGl_Context* theCtx);
  };

private:
  //! Struct for triple of orthonormal vectors
  //! and origin point, and axes for tickmarks.
  //! It may be not a right or left coordinate system.
  struct GridAxes
  {
  public:
    GridAxes()
        : Origin(0, 0, 0)
    {
      Axes[0] = NCollection_Vec3<float>(1.0f, 0.0f, 0.0f);
      Axes[1] = NCollection_Vec3<float>(0.0f, 1.0f, 0.0f);
      Axes[2] = NCollection_Vec3<float>(0.0f, 0.0f, 1.0f);

      Ticks[0] = NCollection_Vec3<float>(0.0f, 0.0f, 0.0f);
      Ticks[1] = NCollection_Vec3<float>(0.0f, 0.0f, 0.0f);
      Ticks[2] = NCollection_Vec3<float>(0.0f, 0.0f, 0.0f);
    }

  public: //! @name Main grid directions
    NCollection_Vec3<float> Origin;
    NCollection_Vec3<float> Axes[3];

  public: //! @name Directions for tickmarks
    NCollection_Vec3<float> Ticks[3];
  };

private:
  //! Initialize or update GL resources for rendering trihedron.
  //! @param[in] theContext  the GL context.
  void initGlResources(const occ::handle<OpenGl_Context>& theContext) const;

  //! Gets normal of the view out of user.
  //! @param[in] theContext  OpenGL Context
  //! @param[out] theNormal  normal of the view out of user
  //! @return distance corresponding to 1 pixel
  float getNormal(const occ::handle<OpenGl_Context>& theContext,
                  NCollection_Vec3<float>&           theNormal) const;

  //! Gets distance to point (theX, theY, theZ) of bounding box along the normal
  //! @param[in] theNormal  normal of the view out of user
  //! @param[in] theCenter  geometry center of bounding box
  //! @param[in] theX  x of target point
  //! @param[in] theY  y of target point
  //! @param[in] theZ  z of terget point
  float getDistanceToCorner(const NCollection_Vec3<float>& theNormal,
                            const NCollection_Vec3<float>& theCenter,
                            const float                    theX,
                            const float                    theY,
                            const float                    theZ) const;

  //! Gets axes of grid
  //! @param[in] theCorners  the corners of grid
  //! @param[out] theGridAxes  grid axes, the base of graduated trihedron grid.
  char16_t getGridAxes(const float theCorners[8], GridAxes& theGridAxes) const;

  //! Render line from the transformed primitive array myLine
  //! @param[in] theWorkspace  the OpenGl Workspace
  //! @param[in] theMat  theMat that contains base transformation and is used for applying
  //!        translation and rotation
  //! @param thaTx the X for vector of translation
  //! @param thaTy the Y for vector of translation
  //! @param thaTz the Z for vector of translation
  void renderLine(const OpenGl_PrimitiveArray&         theLine,
                  const occ::handle<OpenGl_Workspace>& theWorkspace,
                  const NCollection_Mat4<float>&       theMat,
                  const float                          theXt,
                  const float                          theYt,
                  const float                          theZt) const;

  //! Render grid lines perpendecular the axis of input index
  //! @param[in] theWorkspace  the OpenGl Workspace
  //! @param[in] theIndex  index of axis
  //! @param[in] theGridAxes  grid axes
  //! @param[in] theMat  theMat that contains base transformation and is used for applying
  //!        translation and rotation
  void renderGridPlane(const occ::handle<OpenGl_Workspace>& theWorkspace,
                       const int&                           theIndex,
                       const GridAxes&                      theGridAxes,
                       NCollection_Mat4<float>&             theMat) const;

  //! Render the axis of input index
  //! @param[in] theWorkspace  the OpenGl Workspace
  //! @param[in] theIndex  index of axis
  //! @param[in] theMat  theMat that contains base transformation and is used for applying
  //!        translation and rotation
  void renderAxis(const occ::handle<OpenGl_Workspace>& theWorkspace,
                  const int&                           theIndex,
                  const NCollection_Mat4<float>&       theMat) const;

  //! Render grid labels, tickmark lines and labels
  //! @param[in] theWorkspace  the OpenGl Workspace
  //! @param[in] theMat  theMat that contains base transformation and is used for applying
  //!        translation and rotation
  //! @param[in] theIndex  index of axis
  //! @param[in] theGridAxes  grid axes
  //! @param[in] theDpix  distance corresponding to 1 pixel
  void renderTickmarkLabels(const occ::handle<OpenGl_Workspace>& theWorkspace,
                            const NCollection_Mat4<float>&       theMat,
                            const int                            theIndex,
                            const GridAxes&                      theGridAxes,
                            const float                          theDpix) const;

protected: //! @name Scene bounding box values
  NCollection_Vec3<float> myMin;
  NCollection_Vec3<float> myMax;

protected:
  mutable Axis                         myAxes[3]; //!< Axes for trihedron
  mutable Graphic3d_GraduatedTrihedron myData;
  mutable OpenGl_Aspects               myGridLineAspect; //!< Color grid properties

protected: //! @name Labels properties
  mutable OpenGl_Text    myLabelValues;
  mutable OpenGl_Aspects myAspectLabels;
  mutable OpenGl_Aspects myAspectValues;

private:
  mutable bool myIsInitialized;

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
