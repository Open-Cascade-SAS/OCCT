// Created: 2016-05-01
// Author: Andrey Betenev
// Copyright: Open CASCADE 2016
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

#ifndef _RWStl_Reader_HeaderFile
#define _RWStl_Reader_HeaderFile

#include <gp_XYZ.hxx>
#include <Standard_ReadLineBuffer.hxx>
#include <Standard_IStream.hxx>

class Message_ProgressRange;

//! An abstract class implementing procedure to read STL file.
//!
//! This class is not bound to particular data structure and can be used to read the file directly into arbitrary data model.
//! To use it, create descendant class and implement methods addNode() and addTriangle().
//!
//! Call method Read() to read the file. In the process of reading, the tool will call methods addNode() and addTriangle() to fill the mesh data structure.
//!
//! The nodes with equal coordinates are merged automatically  on the fly.
class RWStl_Reader : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(RWStl_Reader, Standard_Transient)
public:

  //! Default constructor.
  Standard_EXPORT RWStl_Reader();

  //! Reads data from STL file (either binary or Ascii).
  //! This function supports reading multi-domain STL files formed by concatenation of several "plain" files.
  //! The mesh nodes are not merged between domains.
  //! Unicode paths can be given in UTF-8 encoding.
  //! Format is recognized automatically by analysis of the file header.
  //! Returns true if success, false on error or user break.
  Standard_EXPORT Standard_Boolean Read (const char* theFile,
                                         const Message_ProgressRange& theProgress);

  //! Guess whether the stream is an Ascii STL file, by analysis of the first bytes (~200).
  //! If the stream does not support seekg() then the parameter isSeekgAvailable should
  //! be passed as 'false', in this case the function attempts to put back the read symbols
  //! to the stream which thus must support ungetc().
  //! Returns true if the stream seems to contain Ascii STL.
  Standard_EXPORT Standard_Boolean IsAscii (Standard_IStream& theStream,
                                            const bool isSeekgAvailable);

  //! Reads STL data from binary stream.
  //! The stream must be opened in binary mode.
  //! Stops after reading the number of triangles recorded in the file header.
  //! Returns true if success, false on error or user break.
  Standard_EXPORT Standard_Boolean ReadBinary (Standard_IStream& theStream,
                                               const Message_ProgressRange& theProgress);

  //! Reads data from the stream assumed to contain Ascii STL data.
  //! The stream can be opened either in binary or in Ascii mode.
  //! Reading stops at the position specified by theUntilPos,
  //! or end of file is reached, or when keyword "endsolid" is found.
  //! Empty lines are not supported and will read to reading failure.
  //! If theUntilPos is non-zero, reads not more than until that position.
  //! Returns true if success, false on error or user break.
  Standard_EXPORT Standard_Boolean ReadAscii (Standard_IStream& theStream,
                                              Standard_ReadLineBuffer& theBuffer,
                                              const std::streampos theUntilPos,
                                              const Message_ProgressRange& theProgress);

public:

  //! Callback function to be implemented in descendant.
  //! Should create new node with specified coordinates in the target model, and return its ID as integer.
  virtual Standard_Integer AddNode (const gp_XYZ& thePnt) = 0;

  //! Callback function to be implemented in descendant.
  //! Should create new triangle built on specified nodes in the target model.
  virtual void AddTriangle (Standard_Integer theN1, Standard_Integer theN2, Standard_Integer theN3) = 0;

public:

  //! Return merge tolerance; M_PI/2 by default - all nodes are merged regardless angle between triangles.
  Standard_Real MergeAngle() const { return myMergeAngle; }

  //! Set merge angle in radians.
  //! Specify something like M_PI/4 (45 degrees) to avoid merge nodes between triangles at sharp corners.
  void SetMergeAngle (Standard_Real theAngleRad) { myMergeAngle = theAngleRad; }

  //! Return linear merge tolerance; 0.0 by default (only 3D points with exactly matching coordinates are merged).
  double MergeTolerance() const { return myMergeTolearance; }

  //! Set linear merge tolerance.
  void SetMergeTolerance (double theTolerance) { myMergeTolearance = theTolerance; }

protected:

  Standard_Real myMergeAngle;
  Standard_Real myMergeTolearance;

};

#endif
