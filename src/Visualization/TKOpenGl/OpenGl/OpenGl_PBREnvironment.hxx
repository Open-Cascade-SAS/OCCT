// Author: Ilya Khramov
// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _OpenGl_PBREnvironment_HeaderFile
#define _OpenGl_PBREnvironment_HeaderFile

#include <OpenGl_Texture.hxx>
#include <OpenGl_VertexBuffer.hxx>

//! This class contains specular and diffuse maps required for Image Base Lighting (IBL) in PBR
//! shading model with it's generation methods.
class OpenGl_PBREnvironment : public OpenGl_NamedResource
{
  DEFINE_STANDARD_RTTIEXT(OpenGl_PBREnvironment, OpenGl_NamedResource)
public:
  //! Creates and initializes new PBR environment. It is the only way to create
  //! OpenGl_PBREnvironment.
  //! @param theCtx OpenGL context where environment will be created
  //! @param thePow2Size final size of texture's sides can be calculated as 2^thePow2Size;
  //!                    if thePow2Size less than 1 it will be set to 1
  //! @param theSpecMapLevelsNum number of mipmap levels used in specular IBL map;
  //!                            if theSpecMapLevelsNum less than 2 or less than Pow2Size + 1 it
  //!                            will be set to the corresponding values.
  //! @param theId OpenGl_Resource name
  //! @return handle to created PBR environment or NULL handle in case of fail
  Standard_EXPORT static Handle(OpenGl_PBREnvironment) Create(
    const Handle(OpenGl_Context)&  theCtx,
    unsigned int                   thePow2Size         = 9,
    unsigned int                   theSpecMapLevelsNum = 6,
    const TCollection_AsciiString& theId               = "PBREnvironment");

public:
  //! Binds diffuse and specular IBL maps to the corresponding texture units.
  Standard_EXPORT void Bind(const Handle(OpenGl_Context)& theCtx);

  //! Unbinds diffuse and specular IBL maps.
  Standard_EXPORT void Unbind(const Handle(OpenGl_Context)& theCtx);

  //! Fills all mipmaps of specular IBL map and diffuse IBL map with one color.
  //! So that environment illumination will be constant.
  Standard_EXPORT void Clear(const Handle(OpenGl_Context)& theCtx,
                             const Graphic3d_Vec3&         theColor = Graphic3d_Vec3(1.f));

  //! Generates specular and diffuse (irradiance) IBL maps.
  //! @param theCtx OpenGL context
  //! @param theEnvMap source environment map
  //! @param theZIsInverted flags indicates whether environment cubemap has inverted Z axis or not
  //! @param theIsTopDown flags indicates whether environment cubemap has top-down memory layout or
  //! not
  //! @param theDiffMapNbSamples number of samples in Monte-Carlo integration for diffuse IBL
  //! spherical harmonics calculation
  //! @param theSpecMapNbSamples number of samples in Monte-Carlo integration for specular IBL map
  //! generation
  //! @param theProbability controls strength of samples number reducing strategy during specular
  //! IBL map baking (see 'SpecIBLMapSamplesFactor' for details) theZIsInverted and theIsTopDown can
  //! be taken from Graphic3d_CubeMap source of environment cubemap. theDiffMapNbSamples and
  //! theSpecMapNbSamples is the main parameter directly affected to performance.
  Standard_EXPORT void Bake(const Handle(OpenGl_Context)& theCtx,
                            const Handle(OpenGl_Texture)& theEnvMap,
                            Standard_Boolean              theZIsInverted      = Standard_False,
                            Standard_Boolean              theIsTopDown        = Standard_True,
                            Standard_Size                 theDiffMapNbSamples = 1024,
                            Standard_Size                 theSpecMapNbSamples = 256,
                            Standard_ShortReal            theProbability      = 0.99f);

  //! Returns number of mipmap levels used in specular IBL map.
  //! It can be different from value passed to creation method.
  unsigned int SpecMapLevelsNumber() const { return mySpecMapLevelsNumber; }

  //! Returns size of IBL maps sides as power of 2.
  //! So that the real size can be calculated as 2^Pow2Size()
  unsigned int Pow2Size() const { return myPow2Size; }

  //! Checks whether the given sizes affects to the current ones.
  //! It can be imagined as creation of new PBR environment.
  //! If creation method with this values returns the PBR environment having real sizes which are
  //! equals to current ones then this method will return false. It is handful when sizes are
  //! required to be changed. If this method returns false there is no reason to recreate PBR
  //! environment in order to change sizes.
  Standard_EXPORT bool SizesAreDifferent(unsigned int thePow2Size,
                                         unsigned int theSpecMapLevelsNumber) const;

  //! Indicates whether IBL map's textures have to be bound or it is not obligate.
  bool IsNeededToBeBound() const { return myIsNeededToBeBound; }

  //! Releases all OpenGL resources.
  //! It must be called before destruction.
  Standard_EXPORT virtual void Release(OpenGl_Context* theCtx) Standard_OVERRIDE;

  //! Returns estimated GPU memory usage for holding data without considering overheads and
  //! allocation alignment rules.
  virtual Standard_Size EstimatedDataSize() const Standard_OVERRIDE
  {
    unsigned int aDiffIBLMapSidePixelsCount = 1 << myPow2Size;
    aDiffIBLMapSidePixelsCount *= aDiffIBLMapSidePixelsCount;
    Standard_Size anEstimatedDataSize = aDiffIBLMapSidePixelsCount;
    for (unsigned int i = 0; i < mySpecMapLevelsNumber; ++i)
    {
      anEstimatedDataSize += aDiffIBLMapSidePixelsCount >> (2 * i);
    }
    anEstimatedDataSize *= 6; // cubemap sides
    anEstimatedDataSize *= 3; // channels
    return anEstimatedDataSize;
  }

  //! Checks completeness of PBR environment.
  //! Creation method returns only completed objects or null handles otherwise.
  Standard_Boolean IsComplete() const { return myIsComplete; }

  //! Destructor.
  //! Warning! 'Release' method must be called before destruction.
  //! Otherwise unhandled critical error will be generated.
  Standard_EXPORT virtual ~OpenGl_PBREnvironment();

private:
  //! Creates new PBR environment.
  //! Parameters and logic are described in 'Create' method documentation.
  Standard_EXPORT OpenGl_PBREnvironment(const Handle(OpenGl_Context)&  theCtx,
                                        unsigned int                   thePowOf2Size          = 9,
                                        unsigned int                   theSpecMapLevelsNumber = 6,
                                        const TCollection_AsciiString& theId = "PBREnvironment");

private:
  //! Enum classified the type of IBL map
  enum OpenGl_TypeOfIBLMap
  {
    OpenGl_TypeOfIBLMap_DiffuseSH,
    OpenGl_TypeOfIBLMap_Specular,
    OpenGl_TypeOfIBLMap_DiffuseFallback,
  };

  //! Parameters for baking IBL.
  struct BakingParams
  {
    Standard_Size      NbSpecSamples;
    Standard_Size      NbDiffSamples;
    Standard_Integer   EnvMapSize;
    Standard_ShortReal Probability;
    Standard_Boolean   IsZInverted;
    Standard_Boolean   IsTopDown;

    BakingParams()
        : NbSpecSamples(0),
          NbDiffSamples(0),
          EnvMapSize(1024),
          Probability(1.0f),
          IsZInverted(false),
          IsTopDown(false)
    {
    }
  };

  //! Initializes all textures.
  //! @return false in case of failed texture initialization
  //! Warning! Requires using of OpenGl_PBREnvironmentSentry.
  bool initTextures(const Handle(OpenGl_Context)& theCtx);

  //! Creates frame buffer object for IBL maps generation.
  //! @return false in case of failed FBO initialization
  //! Warning! Requires using of OpenGl_PBREnvironmentSentry.
  bool initFBO(const Handle(OpenGl_Context)& theCtx);

  //! Initializes vertex buffer object of screen rectangle.
  //! @return false in case of failed creation
  //! Warning! Requires using of OpenGl_PBREnvironmentSentry.
  bool initVAO(const Handle(OpenGl_Context)& theCtx);

  //! Responses for diffuse (irradiance) IBL map processing.
  //! @return false in case of failed baking or clearing
  //! Warning! Requires using of OpenGl_PBREnvironmentSentry.
  bool processDiffIBLMap(const Handle(OpenGl_Context)& theCtx, const BakingParams* theDrawParams);

  //! Responses for specular IBL map processing.
  //! @return false in case of failed baking or clearing
  //! Warning! Requires using of OpenGl_PBREnvironmentSentry.
  bool processSpecIBLMap(const Handle(OpenGl_Context)& theCtx, const BakingParams* theDrawParams);

  //! Checks completeness of frame buffer object for all targets
  //! (all cube map sides and levels of IBL maps).
  //! @return false in case of uncompleted frame buffer object.
  //! Warning! Requires using of OpenGl_PBREnvironmentSentry.
  bool checkFBOComplentess(const Handle(OpenGl_Context)& theCtx);

  //! Version of 'Bake' without OpenGl_PBREnvironmentSetnry.
  //! Warning! Requires using of OpenGl_PBREnvironmentSentry.
  void bake(const Handle(OpenGl_Context)& theCtx,
            const Handle(OpenGl_Texture)& theEnvMap,
            Standard_Boolean              theZIsInverted      = Standard_False,
            Standard_Boolean              theIsTopDown        = Standard_True,
            Standard_Size                 theDiffMapNbSamples = 1024,
            Standard_Size                 theSpecMapNbSamples = 256,
            Standard_ShortReal            theProbability      = 1.f);

  //! Version of 'Clear' without OpenGl_PBREnvironmentSetnry.
  //! Warning! Requires using of OpenGl_PBREnvironmentSentry.
  void clear(const Handle(OpenGl_Context)& theCtx,
             const Graphic3d_Vec3&         theColor = Graphic3d_Vec3(1.f));

private:
  // clang-format off
  unsigned int        myPow2Size;            //!< size of IBL maps sides (real size can be calculated as 2^myPow2Size)
  unsigned int        mySpecMapLevelsNumber; //!< number of mipmap levels used in specular IBL map

  OpenGl_Texture      myIBLMaps[3];          //!< IBL maps
  OpenGl_VertexBuffer myVBO;                 //!< vertex buffer object of screen rectangular
  GLuint              myFBO;                 //!< frame buffer object to generate or clear IBL maps

  Standard_Boolean    myIsComplete;          //!< completeness of PBR environment
  Standard_Boolean    myIsNeededToBeBound;   //!< indicates whether IBL map's textures have to be bound or it is not obligate
  Standard_Boolean    myCanRenderFloat;      //!< indicates whether driver supports rendering into floating point texture or not
  // clang-format on
};

#endif // _OpenGl_PBREnvironment_HeaderFile
