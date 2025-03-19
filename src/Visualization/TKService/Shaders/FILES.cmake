# Source files for Shaders package
set(OCCT_Shaders_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}")

set(OCCT_Shaders_FILES
  Shaders_Declarations_glsl.pxx
  Shaders_DeclarationsImpl_glsl.pxx
  Shaders_LightShadow_glsl.pxx
  Shaders_Display_fs.pxx
  Shaders_PBRCookTorrance_glsl.pxx
  Shaders_PBRDirectionalLight_glsl.pxx
  Shaders_PBRDistribution_glsl.pxx
  Shaders_PBREnvBaking_fs.pxx
  Shaders_PBREnvBaking_vs.pxx
  Shaders_PBRFresnel_glsl.pxx
  Shaders_PBRGeometry_glsl.pxx
  Shaders_PBRIllumination_glsl.pxx
  Shaders_PBRPointLight_glsl.pxx
  Shaders_PBRSpotLight_glsl.pxx
  Shaders_PhongDirectionalLight_glsl.pxx
  Shaders_PhongPointLight_glsl.pxx
  Shaders_PhongSpotLight_glsl.pxx
  Shaders_PointLightAttenuation_glsl.pxx
  Shaders_RaytraceBase_fs.pxx
  Shaders_RaytraceRender_fs.pxx
  Shaders_PathtraceBase_fs.pxx
  Shaders_RaytraceBase_vs.pxx
  Shaders_RaytraceSmooth_fs.pxx
  Shaders_TangentSpaceNormal_glsl.pxx
  Shaders_SkydomBackground_fs.pxx
)
