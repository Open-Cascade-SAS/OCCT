// Created on: 2021-10-14
// Created by: Artem CHESNOKOV
// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _Aspect_SkydomeBackground_Header
#define _Aspect_SkydomeBackground_Header

#include <gp_Dir.hxx>
#include <NCollection_Vec3.hxx>
#include <Standard_TypeDef.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

//! This class allows the definition of a window skydome background.
class Aspect_SkydomeBackground
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a window skydome background.
  //! By default skydome is initialized with sun at its zenith (0.0, 1.0, 0.0),
  //! average clody (0.2), zero time parameter, zero fogginess, 512x512 texture size.
  Standard_EXPORT Aspect_SkydomeBackground();

  //! Creates a window skydome background with given parameters.
  //! @param[in] theSunDirection direction to the sun (moon). Sun direction with negative Y
  //! component
  //!                            represents moon with (-X, -Y, -Z) direction.
  //! @param[in] theCloudiness   cloud intensity, 0.0 means no clouds at all and 1.0 - high clody.
  //! @param[in] theTime         time parameter of simulation. Might be tweaked to slightly change
  //! appearance.
  //! @param[in] theFogginess    fog intensity, 0.0 means no fog and 1.0 - high fogginess
  //! @param[in] theSize         size of cubemap side in pixels.
  Standard_EXPORT Aspect_SkydomeBackground(const gp_Dir&      theSunDirection,
                                           float theCloudiness,
                                           float theTime,
                                           float theFogginess,
                                           int   theSize);

  //! Destructor.
  Standard_EXPORT ~Aspect_SkydomeBackground();

  //! Get sun direction. By default this value is (0, 1, 0)
  //! Sun direction with negative Y component represents moon with (-X, -Y, -Z) direction.
  const gp_Dir& SunDirection() const { return mySunDirection; }

  //! Get cloud intensity. By default this value is 0.2
  //! 0.0 means no clouds at all and 1.0 - high clody.
  float Cloudiness() const { return myCloudiness; }

  //! Get time of cloud simulation. By default this value is 0.0
  //! This value might be tweaked to slightly change appearance of clouds.
  float TimeParameter() const { return myTime; }

  //! Get fog intensity. By default this value is 0.0
  //! 0.0 means no fog and 1.0 - high fogginess
  float Fogginess() const { return myFogginess; }

  //! Get size of cubemap. By default this value is 512
  int Size() const { return mySize; }

  //! Set sun direction. By default this value is (0, 1, 0)
  //! Sun direction with negative Y component represents moon with (-X, -Y, -Z) direction.
  void SetSunDirection(const gp_Dir& theSunDirection) { mySunDirection = theSunDirection; }

  //! Set cloud intensity. By default this value is 0.2
  //! 0.0 means no clouds at all and 1.0 - high clody.
  Standard_EXPORT void SetCloudiness(float theCloudiness);

  //! Set time of cloud simulation. By default this value is 0.0
  //! This value might be tweaked to slightly change appearance of clouds.
  void SetTimeParameter(float theTime) { myTime = theTime; }

  //! Set fog intensity. By default this value is 0.0
  //! 0.0 means no fog and 1.0 - high fogginess
  Standard_EXPORT void SetFogginess(float theFogginess);

  //! Set size of cubemap. By default this value is 512
  Standard_EXPORT void SetSize(int theSize);

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

private:
  gp_Dir             mySunDirection; //!< Sun (moon) light direction.
  float myCloudiness;   //!< Cloud intensity.
  float myTime;         //!< Simulation time parameter.
  float myFogginess;    //!< Fog intensity
  int   mySize;         //!< Size of cubemap in pixels
};

#endif // _Aspect_SkydomeBackground_Header
