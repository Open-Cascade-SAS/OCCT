// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//  Macros to help static Handles not to be "constructed" before main run
//  In order to avoid it, the Handle to be statically reserved is encapsulated
//  in a structure itself designated through a Null Pointer :
//  Only the pointer is declared static, and initialized to NULL : then,
//  there is no routine to call for static construction

//  Remember that the objet designated by a static Handle should not be created
//  in the static declaration, but must anyway be created, during main run,
//  once before its first use : this is the initialization step.


//  This set of macros allows user to simply declare and use "static" Handles.
//  It is available once having included this file :
//  ***************************************************
//         #include <Interface_Statics.hxx>

//  Static construction is replaced by using the macro StaticHandle :
//  ***************************************************
//    Old statement  :  static Handle(pk_class) object;
//    Is replaced by :  StaticHandle(pk_class,object);
//    which creates a null pointer called 'object_s' and typed 'pk_class_struc'

//  For first initialisation and use, several ways are available, all of them
//  give an access to the Handle through a reference.
//  It is required to initialize the static structure once, the macros Init*
//  assume that it is created once and only once, even if they are called
//  more than once.
//  It is possible : to create the object at initialization time by a macro,
//  or to create it after the macro call through its reference :

//  ***************************************************
//  Old statement (in a routine, not static) :
//          if (object.IsNull()) object = new pk_class (..args if any..);
//  can be maintained, but preceeded by an initialization :
//          InitHandle(pk_class,object);         // -> Null Handle

//  ***************************************************
//  or it can be replaced by a direct formula (creation called only once) :
//          InitHandleVoid(pk_class,object);     // for a void constructor
//   or     InitHandleArgs(pk_class,object,(..args..));
//               (the arglist between embedded parentheses)
//   or     InitHandleVal(pk_class,object,val);  // i.e. object = val;

//  To simply use this pseudo-static object, consider
//  either the static variable  object_s->H
//  ***************************************************
//  or take it by the macro (which does not initialize it)
//          UseHandle(pk_class,object);


//  Declaration of a static Handle : first use for a given type
#define StaticHandle(type,var) static struct type##_struc { Handle(type) H; } *var##_s = NULL

//  Another declaration for an already declared type (with StaticHandle)
#define StaticHandleA(type,var) static struct type##_struc *var##_s = NULL

//  Using it (IT MUST HAVE BEEN FORMERLY INITIALIZED)
#define UseHandle(type,var) Handle(type)& var = var##_s->H

//  Initializing it (as Null Handle)
#define InitHandle(type,var) \
if(!var##_s) { var##_s=new type##_struc;  }\
Handle(type)& var = var##_s->H;

//  Initializing it and Creating it by a Void Constructor
#define InitHandleVoid(type,var) \
if(!var##_s) { var##_s=new type##_struc; var##_s->H=new type; }\
Handle(type)& var = var##_s->H;

//  Initializing it and Creating it by a Constructor with Arguments
//    (give them grouped in their parentheses)
#define InitHandleArgs(type,var,args) \
if(!var##_s) { var##_s=new type##_struc; var##_s->H=new type args; }\
Handle(type)& var = var##_s->H;

//  Initializing it from an already determined Value
#define InitHandleVal(type,var,value) \
if(!var##_s) { var##_s=new type##_struc; var##_s->H=value; }\
Handle(type)& var = var##_s->H;
