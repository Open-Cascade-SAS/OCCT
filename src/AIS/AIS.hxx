// Created on: 1996-12-11
// Created by: Robert COUBLANC
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _AIS_HeaderFile
#define _AIS_HeaderFile

#include <Prs3d_Presentation.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! Application Interactive Services provide the means to create links between an application GUI viewer and
//! the packages which are used to manage selection and presentation.
//! The tools AIS defined in order to do this include different sorts of entities:
//! both the selectable viewable objects themselves and the context and attribute managers to define their selection and display.
//! To orient the user as he works in a modeling environment, views and selections must be comprehensible.
//! There must be several different sorts of selectable and viewable object defined.
//! These must also be interactive, that is, connecting graphic representation and the underlying reference geometry.
//! These entities are called Interactive Objects, and are divided into four types:
//! -   the Datum
//! -   the Relation
//! -   the Object
//! -   None.
//! The Datum groups together the construction elements such as lines, circles, points, trihedra, plane trihedra, planes and axes.
//! The Relation is made up of constraints on one or more interactive shapes and the corresponding reference geometry.
//! For example, you might want to constrain two edges in a parallel relation.
//! This contraint is considered as an object in its own right, and is shown as a sensitive primitive.
//! This takes the graphic form of a perpendicular arrow marked with the || symbol and lying between the two edges.
//! The Object type includes topological shapes, and connections between shapes.
//! None, in order not to eliminate the object, tells the application to look further until it finds an object definition in its generation which is accepted.
//! Inside these categories, you have the possibility of an additional characterization by means of a signature.
//! The signature provides an index to the further characterization.
//! By default, the Interactive Object has a None type and a signature of 0 (equivalent to None.)
//! If you want to give a particular type and signature to your interactive object, you must redefine the two virtual methods: Type and Signature.
//! In the C++ inheritance structure of the package, each class representing a specific Interactive Object inherits AIS_InteractiveObject.
//! Among these inheriting classes, AIS_Relation functions as the abstract mother class for tinheriting classes defining display of specific relational constraints and types of dimension.
//! Some of these include:
//! -   display of constraints based on relations of symmetry, tangency, parallelism and concentricity
//! -   display of dimensions for angles, offsets, diameters, radii and chamfers.
//! No viewer can show everything at once with any coherence or clarity.
//! Views must be managed carefully both sequentially and at any given instant.
//! Another function of the view is that of a context to carry out design in.
//! The design changes are applied to the objects in the view and then extended to the underlying reference geometry by a solver.
//! To make sense of this complicated visual data, several display and selection tools are required.
//! To facilitate management, each object and each construction element has a selection priority.
//! There are also means to modify the default priority.
//! To define an environment of dynamic detection, you can use standard filter classes or create your own.
//! A filter questions the owner of the sensitive primitive to determine if it has the desired qualities.
//! If it answers positively, it is kept. If not, it is rejected.
//! The standard filters supplied in AIS include:
//! - AIS_AttributeFilter
//! - AIS_SignatureFilter
//! - AIS_TypeFilter.
//! A set of functions allows you to choose the interactive objects which you want to act on, the selection modes which you want to activate.
//! An interactive object can have a certain number of graphic attributes which are specific to it, such as visualization mode, color, and material.
//! By the same token, the interactive context has a set of graphic attributes, the Drawer which is valid by default for the objects it controls.
//! When an interactive object is visualized, the required graphic attributes are first taken from the object's own Drawer if one exists, or from the context drawer for the others.
class AIS 
{
public:

  DEFINE_STANDARD_ALLOC

};

#endif // _AIS_HeaderFile
