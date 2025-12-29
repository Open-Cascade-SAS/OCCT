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
#include <NCollection_Vec2.hxx>
#include <Standard_TypeDef.hxx>
#include <NCollection_Vec3.hxx>
#include <NCollection_Vec4.hxx>
#include <NCollection_Mat4.hxx>
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
  Standard_EXPORT void CopyFrom(const occ::handle<Graphic3d_CLight>& theLight);

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
  bool IsEnabled() const { return myIsEnabled; }

  //! Change enabled state of the light state.
  //! This call does not remove or deactivate light source in Views/Viewers;
  //! instead it turns it OFF so that it just have no effect.
  Standard_EXPORT void SetEnabled(bool theIsOn);

  //! Return TRUE if shadow casting is enabled; FALSE by default.
  //! Has no effect in Ray-Tracing rendering mode.
  bool ToCastShadows() const { return myToCastShadows; }

  //! Enable/disable shadow casting.
  Standard_EXPORT void SetCastShadows(bool theToCast);

  //! Returns true if the light is a headlight; FALSE by default.
  //! Headlight flag means that light position/direction are defined not in a World coordinate
  //! system, but relative to the camera orientation.
  bool IsHeadlight() const { return myIsHeadlight; }

  //! Alias for IsHeadlight().
  bool Headlight() const { return myIsHeadlight; }

  //! Setup headlight flag.
  Standard_EXPORT void SetHeadlight(bool theValue);

  //! @name positional/spot light properties
public:
  //! Returns location of positional/spot light; (0, 0, 0) by default.
  const gp_Pnt& Position() const { return myPosition; }

  //! Setup location of positional/spot light.
  Standard_EXPORT void SetPosition(const gp_Pnt& thePosition);

  //! Returns location of positional/spot light.
  void Position(double& theX, double& theY, double& theZ) const
  {
    theX = myPosition.X();
    theY = myPosition.Y();
    theZ = myPosition.Z();
  }

  //! Setup location of positional/spot light.
  void SetPosition(double theX, double theY, double theZ) { SetPosition(gp_Pnt(theX, theY, theZ)); }

  //! Returns constant attenuation factor of positional/spot light source; 1.0f by default.
  //! Distance attenuation factors of reducing positional/spot light intensity depending on the
  //! distance from its position:
  //! @code
  //!   float anAttenuation = 1.0 / (ConstAttenuation() + LinearAttenuation() * theDistance +
  //!   QuadraticAttenuation() * theDistance * theDistance);
  //! @endcode
  float ConstAttenuation() const { return myParams.x(); }

  //! Returns linear attenuation factor of positional/spot light source; 0.0 by default.
  //! Distance attenuation factors of reducing positional/spot light intensity depending on the
  //! distance from its position:
  //! @code
  //!   float anAttenuation = 1.0 / (ConstAttenuation() + LinearAttenuation() * theDistance +
  //!   QuadraticAttenuation() * theDistance * theDistance);
  //! @endcode
  float LinearAttenuation() const { return myParams.y(); }

  //! Returns the attenuation factors.
  void Attenuation(double& theConstAttenuation, double& theLinearAttenuation) const
  {
    theConstAttenuation  = ConstAttenuation();
    theLinearAttenuation = LinearAttenuation();
  }

  //! Defines the coefficients of attenuation; values should be >= 0.0 and their summ should not be
  //! equal to 0.
  Standard_EXPORT void SetAttenuation(float theConstAttenuation, float theLinearAttenuation);

  //! @name directional/spot light additional properties
public:
  //! Returns direction of directional/spot light.
  gp_Dir Direction() const { return gp_Dir(myDirection.x(), myDirection.y(), myDirection.z()); }

  //! Sets direction of directional/spot light.
  Standard_EXPORT void SetDirection(const gp_Dir& theDir);

  //! Returns the theVx, theVy, theVz direction of the light source.
  void Direction(double& theVx, double& theVy, double& theVz) const
  {
    theVx = myDirection.x();
    theVy = myDirection.y();
    theVz = myDirection.z();
  }

  //! Sets direction of directional/spot light.
  void SetDirection(double theVx, double theVy, double theVz)
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
  float Angle() const { return myParams.z(); }

  //! Angle in radians of the cone created by the spot, should be within range (0.0, M_PI).
  Standard_EXPORT void SetAngle(float theAngle);

  //! Returns intensity distribution of the spot light, within [0.0, 1.0] range; 1.0 by default.
  //! This coefficient should be converted into spotlight exponent within [0.0, 128.0] range:
  //! @code
  //!   float aSpotExponent = Concentration() * 128.0;
  //!   anAttenuation *= pow (aCosA, aSpotExponent);"
  //! @endcode
  //! The concentration factor determines the dispersion of the light on the surface, the default
  //! value (1.0) corresponds to a minimum of dispersion.
  float Concentration() const { return myParams.w(); }

  //! Defines the coefficient of concentration; value should be within range [0.0, 1.0].
  Standard_EXPORT void SetConcentration(float theConcentration);

  //! @name Ray-Tracing / Path-Tracing light properties
public:
  //! Returns the intensity of light source; 1.0 by default.
  float Intensity() const { return myIntensity; }

  //! Modifies the intensity of light source, which should be > 0.0.
  Standard_EXPORT void SetIntensity(float theValue);

  //! Returns the smoothness of light source (either smoothing angle for directional light or
  //! smoothing radius in case of positional light); 0.0 by default.
  float Smoothness() const { return mySmoothness; }

  //! Modifies the smoothing radius of positional/spot light; should be >= 0.0.
  Standard_EXPORT void SetSmoothRadius(float theValue);

  //! Modifies the smoothing angle (in radians) of directional light source; should be within range
  //! [0.0, M_PI/2].
  Standard_EXPORT void SetSmoothAngle(float theValue);

  //! Returns TRUE if maximum distance of point light source is defined.
  bool HasRange() const { return myDirection.w() != 0.0f; }

  //! Returns maximum distance on which point light source affects to objects and is considered
  //! during illumination calculations. 0.0 means disabling range considering at all without any
  //! distance limits. Has sense only for point light sources (positional and spot).
  float Range() const { return myDirection.w(); }

  //! Modifies maximum distance on which point light source affects to objects and is considered
  //! during illumination calculations. Positional and spot lights are only point light sources. 0.0
  //! means disabling range considering at all without any distance limits.
  Standard_EXPORT void SetRange(float theValue);

  //! @name low-level access methods
public:
  //! @return light resource identifier string
  const TCollection_AsciiString& GetId() const { return myId; }

  //! Packed light parameters.
  const NCollection_Vec4<float>& PackedParams() const { return myParams; }

  //! Returns the color of the light source with dummy Alpha component, which should be ignored.
  const NCollection_Vec4<float>& PackedColor() const { return myColor; }

  //! Returns direction of directional/spot light and range for positional/spot light in alpha
  //! channel.
  const NCollection_Vec4<float>& PackedDirectionRange() const { return myDirection; }

  //! Returns direction of directional/spot light.
  NCollection_Vec3<float> PackedDirection() const { return myDirection.xyz(); }

  //! @return modification counter
  size_t Revision() const { return myRevision; }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

private:
  //! Access positional/spot light constant attenuation coefficient from packed vector.
  float& changeConstAttenuation() { return myParams.x(); }

  //! Access positional/spot light linear attenuation coefficient from packed vector.
  float& changeLinearAttenuation() { return myParams.y(); }

  //! Access spotlight angle parameter from packed vector.
  float& changeAngle() { return myParams.z(); }

  //! Access spotlight concentration parameter from packed vector.
  float& changeConcentration() { return myParams.w(); }

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
  Graphic3d_CLight(const Graphic3d_CLight&)            = delete;
  Graphic3d_CLight& operator=(const Graphic3d_CLight&) = delete;

protected:
  TCollection_AsciiString myId;                      //!< resource id
  TCollection_AsciiString myName;                    //!< user given name
  gp_Pnt                  myPosition;                //!< light position
  Quantity_ColorRGBA      myColor;                   //!< light color
  NCollection_Vec4<float> myDirection;               //!< direction of directional/spot light
  NCollection_Vec4<float> myParams;                  //!< packed light parameters
                                                     // clang-format off
  float                mySmoothness;  //!< radius for point light or cone angle for directional light
                                                     // clang-format on
  float                             myIntensity;     //!< intensity multiplier for light
  const Graphic3d_TypeOfLightSource myType;          //!< Graphic3d_TypeOfLightSource enumeration
  size_t                            myRevision;      //!< modification counter
  bool                              myIsHeadlight;   //!< flag to mark head light
  bool                              myIsEnabled;     //!< enabled state
  bool                              myToCastShadows; //!< casting shadows is requested
};

#endif // Graphic3d_CLight_HeaderFile
