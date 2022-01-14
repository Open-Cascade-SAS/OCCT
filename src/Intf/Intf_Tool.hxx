// Created on: 1993-06-23
// Created by: Didier PIFFAULT
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Intf_Tool_HeaderFile
#define _Intf_Tool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
class gp_Lin2d;
class Bnd_Box2d;
class gp_Hypr2d;
class gp_Parab2d;
class gp_Lin;
class Bnd_Box;
class gp_Hypr;
class gp_Parab;


//! Provides services to create box for infinites
//! lines in a given contexte.
class Intf_Tool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Intf_Tool();
  
  Standard_EXPORT void Lin2dBox (const gp_Lin2d& theLin2d, const Bnd_Box2d& bounding, Bnd_Box2d& boxLin);
  
  Standard_EXPORT void Hypr2dBox (const gp_Hypr2d& theHypr2d, const Bnd_Box2d& bounding, Bnd_Box2d& boxHypr);
  
  Standard_EXPORT void Parab2dBox (const gp_Parab2d& theParab2d, const Bnd_Box2d& bounding, Bnd_Box2d& boxHypr);
  
  Standard_EXPORT void LinBox (const gp_Lin& theLin, const Bnd_Box& bounding, Bnd_Box& boxLin);
  
  Standard_EXPORT void HyprBox (const gp_Hypr& theHypr, const Bnd_Box& bounding, Bnd_Box& boxHypr);
  
  Standard_EXPORT void ParabBox (const gp_Parab& theParab, const Bnd_Box& bounding, Bnd_Box& boxHypr);
  
  Standard_EXPORT Standard_Integer NbSegments() const;
  
  Standard_EXPORT Standard_Real BeginParam (const Standard_Integer SegmentNum) const;
  
  Standard_EXPORT Standard_Real EndParam (const Standard_Integer SegmentNum) const;




protected:





private:

  
  Standard_EXPORT Standard_Integer Inters2d (const gp_Hypr2d& theCurve, const Bnd_Box2d& Domain);
  
  Standard_EXPORT Standard_Integer Inters2d (const gp_Parab2d& theCurve, const Bnd_Box2d& Domain);
  
  Standard_EXPORT Standard_Integer Inters3d (const gp_Hypr& theCurve, const Bnd_Box& Domain);
  
  Standard_EXPORT Standard_Integer Inters3d (const gp_Parab& theCurve, const Bnd_Box& Domain);


  Standard_Integer nbSeg;
  Standard_Real beginOnCurve[6];
  Standard_Real endOnCurve[6];
  Standard_Integer bord[12];
  Standard_Real xint[12];
  Standard_Real yint[12];
  Standard_Real zint[12];
  Standard_Real parint[12];


};







#endif // _Intf_Tool_HeaderFile
