#ifndef _Select3D_TypeOfSensitivity_HeaderFile
#define _Select3D_TypeOfSensitivity_HeaderFile

#include <Standard_PrimitiveTypes.hxx>

//! Provides values for type of sensitivity in 3D.
//! These are used to specify whether it is the interior,
//! the boundary, or the exterior of a 3D sensitive entity which is sensitive.
enum Select3D_TypeOfSensitivity
{
Select3D_TOS_INTERIOR,
Select3D_TOS_BOUNDARY
};

#endif // _Select3D_TypeOfSensitivity_HeaderFile
