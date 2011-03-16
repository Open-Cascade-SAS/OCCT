// File:      VrmlData_TextureTransform.hxx
// Created:   25.05.06 16:25:17
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2006


#ifndef VrmlData_TextureTransform_HeaderFile
#define VrmlData_TextureTransform_HeaderFile

#include <VrmlData_Node.hxx>
#include <gp_XY.hxx>

///  Implementation of the TextureTransform node
//

class VrmlData_TextureTransform : public VrmlData_Node
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor
   */
  inline VrmlData_TextureTransform () {}

  /**
   * Constructor
   */
  inline VrmlData_TextureTransform (const VrmlData_Scene& theScene,
                                    const char            * theName)
    : VrmlData_Node (theScene, theName)
  {}

  /**
   * Query the Center
   */
  inline const gp_XY&   Center          () const { return myCenter; }
  
  /**
   * Query the Rotation
   */
  inline Standard_Real  Rotation        () const { return myRotation; }
  
  /**
   * Query the Scale
   */
  inline const gp_XY&   Scale           () const { return myScale; }

  /**
   * Query the Translation
   */
  inline const gp_XY&   Translation     () const { return myTranslation; }

  /**
   * Set the Center
   */
  inline void           SetCenter       (const gp_XY& V) { myCenter = V; }

  /**
   * Set the Rotation
   */
  inline void           SetRotation     (const Standard_Real V)
  { myRotation = V; }

  /**
   * Set the Scale
   */
  inline void           SetScale        (const gp_XY& V) { myScale = V; }

  /**
   * Set the Translation
   */
  inline void           SetTranslation  (const gp_XY& V) { myTranslation = V; }

 protected:
  // ---------- PROTECTED METHODS ----------



 private:
  // ---------- PRIVATE FIELDS ----------

  gp_XY         myCenter;
  Standard_Real myRotation;
  gp_XY         myScale;
  gp_XY         myTranslation;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (VrmlData_TextureTransform)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_TextureTransform, VrmlData_Node)


#endif
