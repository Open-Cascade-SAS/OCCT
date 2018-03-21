// Created on: 1991-06-25
// Created by: Christophe MARION
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _BRepTest_HeaderFile
#define _BRepTest_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Draw_Interpretor.hxx>


//! Provides commands to test BRep.
class BRepTest 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Defines all the topology commands.
  Standard_EXPORT static void AllCommands (Draw_Interpretor& DI);
  
  //! Defines the basic commands.
  Standard_EXPORT static void BasicCommands (Draw_Interpretor& DI);
  
  //! Defines the commands to build edges and wires.
  Standard_EXPORT static void CurveCommands (Draw_Interpretor& DI);
  
  //! Defines the  commands  to perform add  fillets on
  //! wires and  edges.
  Standard_EXPORT static void Fillet2DCommands (Draw_Interpretor& DI);
  
  //! Defines the commands to build faces and shells.
  Standard_EXPORT static void SurfaceCommands (Draw_Interpretor& DI);
  
  //! Defines the commands to build primitives.
  Standard_EXPORT static void PrimitiveCommands (Draw_Interpretor& DI);
  
  //! Defines the commands to build primitives.
  Standard_EXPORT static void FillingCommands (Draw_Interpretor& DI);
  
  //! Defines the commands to sweep shapes.
  Standard_EXPORT static void SweepCommands (Draw_Interpretor& DI);
  
  //! Defines  the    commands   to perform  topological
  //! operations.
  Standard_EXPORT static void TopologyCommands (Draw_Interpretor& DI);
  
  //! Defines  the commands  to perform  add  fillets on
  //! shells.
  Standard_EXPORT static void FilletCommands (Draw_Interpretor& DI);
  
  //! Defines  the commands  to perform  add chamfers on
  //! shells.
  Standard_EXPORT static void ChamferCommands (Draw_Interpretor& DI);
  
  //! Defines commands to compute global properties.
  Standard_EXPORT static void GPropCommands (Draw_Interpretor& DI);
  
  //! Defines commands to compute and to explore the map of the
  //! Bisecting locus.
  Standard_EXPORT static void MatCommands (Draw_Interpretor& DI);
  
  //! Defines the commands to modify draft angles of the
  //! faces of a shape.
  Standard_EXPORT static void DraftAngleCommands (Draw_Interpretor& DI);
  
  //! Defines the commands to create features on a shape.
  Standard_EXPORT static void FeatureCommands (Draw_Interpretor& DI);
  
  //! Defines the auxiliary topology commands.
  Standard_EXPORT static void OtherCommands (Draw_Interpretor& DI);
  
  //! Defines the extrema commands.
  Standard_EXPORT static void ExtremaCommands (Draw_Interpretor& DI);
  
  //! Defines the checkshape command.
  Standard_EXPORT static void CheckCommands (Draw_Interpretor& DI);
  
  //! Defines the placement  command.
  Standard_EXPORT static void PlacementCommands (Draw_Interpretor& DI);
  
  //! Defines the commands to project a wire on a shape.
  Standard_EXPORT static void ProjectionCommands (Draw_Interpretor& DI);

  //! Defines the History commands for the algorithms.
  Standard_EXPORT static void HistoryCommands (Draw_Interpretor& DI);



protected:





private:





};







#endif // _BRepTest_HeaderFile
