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

#ifndef _Graphic3d_CLight_HeaderFile
#define _Graphic3d_CLight_HeaderFile

#include <gp_Dir.hxx>
#include <Graphic3d_TypeOfLightSource.hxx>
#include <Graphic3d_Vec.hxx>
#include <NCollection_List.hxx>
#include <TCollection_AsciiString.hxx>
#include <Quantity_ColorRGBA.hxx>

//! Generic light source definition.
//! This class defines arbitrary light source - see Graphic3d_TypeOfLightSource enumeration.
//! Some parameters are applicable only to particular light type;
//! calling methods unrelated to current type will throw an exception.
class Graphic3d_CLight : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_CLight, Standard_Transient)
public:
  //! Empty constructor, which should be followed by light source properties configuration.
  Standard_EXPORT Graphic3d_CLight(Graphic3d_TypeOfLightSource theType);

  //! Copy parameters from another light source excluding source type.
  Standard_EXPORT void CopyFrom(const Handle(Graphic3d_CLight)& theLight);

  //! Returns the Type of the Light, cannot be changed after object construction.
  Graphic3d_TypeOfLightSource Type() const { return myType; }

  //! Returns light source name; empty string by default.
  const TCollection_AsciiString& Name() const { return myName; }

  //! Sets light source name.
  void SetName(const TCollection_AsciiString& theName) { myName = theName; }

  //! Returns the color of the light source; WHITE by default.
  const Quantity_Color& Color() const { return myColor.GetRGB(); }

  //! Defines the color of a light source by giving the basic color.
  Standard_EXPORT void SetColor(const Quantity_Color& theColor);

  //! Check that the light source is turned on; TRUE by default.
  //! This flag affects all occurrences of light sources, where it was registered and activated;
  //! so that it is possible defining an active light in View which is actually in disabled state.
  Standard_Boolean IsEnabled() const { return myIsEnabled; }

  //! Change enabled state of the light state.
  //! This call does not remove or deactivate light source in Views/Viewers;
  //! instead it turns it OFF so that it just have no effect.
  Standard_EXPORT void SetEnabled(Standard_Boolean theIsOn);

  //! Return TRUE if shadow casting is enabled; FALSE by default.
  //! Has no effect in Ray-Tracing rendering mode.
  Standard_Boolean ToCastShadows() const { return myToCastShadows; }

  //! Enable/disable shadow casting.
  Standard_EXPORT void SetCastShadows(Standard_Boolean theToCast);

  //! Returns true if the light is a headlight; FALSE by default.
  //! Headlight flag means that light position/direction are defined not in a World coordinate
  //! system, but relative to the camera orientation.
  Standard_Boolean IsHeadlight() const { return myIsHeadlight; }

  //! Alias for IsHeadlight().
  Standard_Boolean Headlight() const { return myIsHeadlight; }

  //! Setup headlight flag.
  Standard_EXPORT void SetHeadlight(Standard_Boolean theValue);

  //! @name positional/spot light properties
public:
  //! Returns location of positional/spot light; (0, 0, 0) by default.
  const gp_Pnt& Position() const { return myPosition; }

  //! Setup location of positional/spot light.
  Standard_EXPORT void SetPosition(const gp_Pnt& thePosition);

  //! Returns location of positional/spot light.
  void Position(Standard_Real& theX, Standard_Real& theY, Standard_Real& theZ) const
  {
    theX = myPosition.X();
    theY = myPosition.Y();
    theZ = myPosition.Z();
  }

  //! Setup location of positional/spot light.
  void SetPosition(Standard_Real theX, Standard_Real theY, Standard_Real theZ)
  {
    SetPosition(gp_Pnt(theX, theY, theZ));
  }

  //! Returns constant attenuation factor of positional/spot light source; 1.0f by default.
  //! Distance attenuation factors of reducing positional/spot light intensity depending on the
  //! distance from its position:
  //! @code
  //!   float anAttenuation = 1.0 / (ConstAttenuation() + LinearAttenuation() * theDistance +
  //!   QuadraticAttenuation() * theDistance * theDistance);
  //! @endcode
  Standard_ShortReal ConstAttenuation() const { return myParams.x(); }

  //! Returns linear attenuation factor of positional/spot light source; 0.0 by default.
  //! Distance attenuation factors of reducing positional/spot light intensity depending on the
  //! distance from its position:
  //! @code
  //!   float anAttenuation = 1.0 / (ConstAttenuation() + LinearAttenuation() * theDistance +
  //!   QuadraticAttenuation() * theDistance * theDistance);
  //! @endcode
  Standard_ShortReal LinearAttenuation() const { return myParams.y(); }

  //! Returns the attenuation factors.
  void Attenuation(Standard_Real& theConstAttenuation, Standard_Real& theLinearAttenuation) const
  {
    theConstAttenuation  = ConstAttenuation();
    theLinearAttenuation = LinearAttenuation();
  }

  //! Defines the coefficients of attenuation; values should be >= 0.0 and their summ should not be
  //! equal to 0.
  Standard_EXPORT void SetAttenuation(Standard_ShortReal theConstAttenuation,
                                      Standard_ShortReal theLinearAttenuation);

  //! @name directional/spot light additional properties
public:
  //! Returns direction of directional/spot light.
  gp_Dir Direction() const { return gp_Dir(myDirection.x(), myDirection.y(), myDirection.z()); }

  //! Sets direction of directional/spot light.
  Standard_EXPORT void SetDirection(const gp_Dir& theDir);

  //! Returns the theVx, theVy, theVz direction of the light source.
  void Direction(Standard_Real& theVx, Standard_Real& theVy, Standard_Real& theVz) const
  {
    theVx = myDirection.x();
    theVy = myDirection.y();
    theVz = myDirection.z();
  }

  //! Sets direction of directional/spot light.
  void SetDirection(Standard_Real theVx, Standard_Real theVy, Standard_Real theVz)
  {
    SetDirection(gp_Dir(theVx, theVy, theVz));
  }

  //! Returns location of positional/spot/directional light, which is the same as returned by
  //! Position().
  const gp_Pnt& DisplayPosition() const { return myPosition; }

  //! Setup location of positional/spot/directional light,
  //! which is the same as SetPosition() but allows directional light source
  //! (technically having no position, but this point can be used for displaying light source
  //! presentation).
  Standard_EXPORT void SetDisplayPosition(const gp_Pnt& thePosition);

  //! @name spotlight additional definition parameters
public:
  //! Returns an angle in radians of the cone created by the spot; 30 degrees by default.
  Standard_ShortReal Angle() const { return myParams.z(); }

  //! Angle in radians of the cone created by the spot, should be within range (0.0, M_PI).
  Standard_EXPORT void SetAngle(Standard_ShortReal theAngle);

  //! Returns intensity distribution of the spot light, within [0.0, 1.0] range; 1.0 by default.
  //! This coefficient should be converted into spotlight exponent within [0.0, 128.0] range:
  //! @code
  //!   float aSpotExponent = Concentration() * 128.0;
  //!   anAttenuation *= pow (aCosA, aSpotExponent);"
  //! @endcode
  //! The concentration factor determines the dispersion of the light on the surface, the default
  //! value (1.0) corresponds to a minimum of dispersion.
  Standard_ShortReal Concentration() const { return myParams.w(); }

  //! Defines the coefficient of concentration; value should be within range [0.0, 1.0].
  Standard_EXPORT void SetConcentration(Standard_ShortReal theConcentration);

  //! @name Ray-Tracing / Path-Tracing light properties
public:
  //! Returns the intensity of light source; 1.0 by default.
  Standard_ShortReal Intensity() const { return myIntensity; }

  //! Modifies the intensity of light source, which should be > 0.0.
  Standard_EXPORT void SetIntensity(Standard_ShortReal theValue);

  //! Returns the smoothness of light source (either smoothing angle for directional light or
  //! smoothing radius in case of positional light); 0.0 by default.
  Standard_ShortReal Smoothness() const { return mySmoothness; }

  //! Modifies the smoothing radius of positional/spot light; should be >= 0.0.
  Standard_EXPORT void SetSmoothRadius(Standard_ShortReal theValue);

  //! Modifies the smoothing angle (in radians) of directional light source; should be within range
  //! [0.0, M_PI/2].
  Standard_EXPORT void SetSmoothAngle(Standard_ShortReal theValue);

  //! Returns TRUE if maximum distance of point light source is defined.
  bool HasRange() const { return myDirection.w() != 0.0f; }

  //! Returns maximum distance on which point light source affects to objects and is considered
  //! during illumination calculations. 0.0 means disabling range considering at all without any
  //! distance limits. Has sense only for point light sources (positional and spot).
  Standard_ShortReal Range() const { return myDirection.w(); }

  //! Modifies maximum distance on which point light source affects to objects and is considered
  //! during illumination calculations. Positional and spot lights are only point light sources. 0.0
  //! means disabling range considering at all without any distance limits.
  Standard_EXPORT void SetRange(Standard_ShortReal theValue);

  //! @name low-level access methods
public:
  //! @return light resource identifier string
  const TCollection_AsciiString& GetId() const { return myId; }

  //! Packed light parameters.
  const Graphic3d_Vec4& PackedParams() const { return myParams; }

  //! Returns the color of the light source with dummy Alpha component, which should be ignored.
  const Graphic3d_Vec4& PackedColor() const { return myColor; }

  //! Returns direction of directional/spot light and range for positional/spot light in alpha
  //! channel.
  const Graphic3d_Vec4& PackedDirectionRange() const { return myDirection; }

  //! Returns direction of directional/spot light.
  Graphic3d_Vec3 PackedDirection() const { return myDirection.xyz(); }

  //! @return modification counter
  Standard_Size Revision() const { return myRevision; }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

private:
  //! Access positional/spot light constant attenuation coefficient from packed vector.
  Standard_ShortReal& changeConstAttenuation() { return myParams.x(); }

  //! Access positional/spot light linear attenuation coefficient from packed vector.
  Standard_ShortReal& changeLinearAttenuation() { return myParams.y(); }

  //! Access spotlight angle parameter from packed vector.
  Standard_ShortReal& changeAngle() { return myParams.z(); }

  //! Access spotlight concentration parameter from packed vector.
  Standard_ShortReal& changeConcentration() { return myParams.w(); }

private:
  //! Generate unique object id.
  void makeId();

  //! Update modification counter.
  void updateRevisionIf(bool theIsModified)
  {
    if (theIsModified)
    {
      ++myRevision;
    }
  }

private:
  Graphic3d_CLight(const Graphic3d_CLight&);
  Graphic3d_CLight& operator=(const Graphic3d_CLight&);

protected:
  TCollection_AsciiString myId;                      //!< resource id
  TCollection_AsciiString myName;                    //!< user given name
  gp_Pnt                  myPosition;                //!< light position
  Quantity_ColorRGBA      myColor;                   //!< light color
  Graphic3d_Vec4          myDirection;               //!< direction of directional/spot light
  Graphic3d_Vec4          myParams;                  //!< packed light parameters
                                                     // clang-format off
  Standard_ShortReal                mySmoothness;  //!< radius for point light or cone angle for directional light
                                                     // clang-format on
  Standard_ShortReal                myIntensity;     //!< intensity multiplier for light
  const Graphic3d_TypeOfLightSource myType;          //!< Graphic3d_TypeOfLightSource enumeration
  Standard_Size                     myRevision;      //!< modification counter
  Standard_Boolean                  myIsHeadlight;   //!< flag to mark head light
  Standard_Boolean                  myIsEnabled;     //!< enabled state
  Standard_Boolean                  myToCastShadows; //!< casting shadows is requested
};

DEFINE_STANDARD_HANDLE(Graphic3d_CLight, Standard_Transient)

#endif // Graphic3d_CLight_HeaderFile
