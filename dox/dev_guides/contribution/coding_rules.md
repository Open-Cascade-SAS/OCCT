Coding Rules {#dev_guides__coding_rules}
======================================

@tableofcontents

@section OCCT_RULES_SECTION_1 Introduction

The purpose of this document is to define and formalize one style of programming for developers working on Open CASCADE Technology.
The establishment of a common style facilitates understanding and maintaining code developed by more than one programmer as well as making it easier for several people to co-operate in the development of the same framework.
In addition, following a common programming style enables the construction of tools that incorporate knowledge of these standards to help in the programming task.
Using a consistent coding style throughout a particular module, package, or project is important because it allows people other than the author, and the author himself, to easily understand and (hopefully) maintain the code.
Most programming styles are somewhat arbitrary, and this one is no exception. Some guidelines have been excerpted from the public domain of widely accepted practices.
This suggests that the guide will continue to evolve over time as new ideas and enhancements are added.

@subsection OCCT_RULES_SECTION_1_1 Scope of the rules in this document

Rules in this document was written for C++ code.
However, with minor exceptions due to language restrictions, them should be applied to any sources in Open CASCADE Technology framework, including:
- C/C++
- GLSL programs
- OpenCL kernels
- TCL scripts and test cases

@section OCCT_RULES_SECTION_2 Naming Conventions

@subsection OCCT_RULES_SECTION_2_1 General naming rules

The names considered in this section are mainly those which compound the interface to Open CASCADE Technology libraries as well as source code itself.

### International language [MANDATORY]
All names are composed of English words and their abbreviations.
Open CASCADE Technology is an open source available for international community.

### Suggestive names
Names should be suggestive or, at least, contain a suggestive part.
Currently, there is no exact rule that would define how to generate suggestive names. However, usually names given to toolkits, packages, classes and methods are suggestive. Here are several examples:
- Packages containing words Geom or Geom2d in their names are related to geometrical data and operations.
- Packages containing words TopoDS or BRep in their names are related to topological data and operations.
- In OCAF, packages that define transient, persistent data classes and drivers to map between them, have similar names prefixed by 'T', 'P', and 'M' correspondingly (e.g. TDocStd, PDocStd, MDocStd).
- Packages ending with ...Test define Draw Harness plugins.
- Methods starting with Get... and Set... are usually responsible for (accordingly) retrieving/storing some data.

### Related names
Names that define logically connected functionality should have the same prefix (start with the same letters) or, at least, have any other common part in them.
As an example the method GetCoord can be given. It returns a triple of real values and is defined for directions, vectors and points. The logical connection is obvious.

### Camel Case style
Camel Case style is preferred for names.
For example:

~~~~~{.cpp}
Standard_Integer awidthofbox;  // this is bad
Standard_Integer width_of_box; // this is bad
Standard_Integer aWidthOfBox;  // this is OK
~~~~~

@subsection OCCT_RULES_SECTION_2_2 Names of development units
Usually unit (e.g. package) is a set of classes, methods, enumerations or any other sources implementing certain common functionality which, to the certain extent, is self contained and independent from other parts of library.

### Underscores in units names [MANDATORY]
Names of units should not contain underscores, except cases where usage of underscores is allowed explicitly.
Usually names of files consisting Open CASCADE Technology are constructed according to the rules defined in the appropriate sections of this document.

### File names extensions [MANDATORY]
The following extensions should be used for source files, depending on their type:

    .cdl - CDL declaration files
    .cxx - C++ source files
    .hxx - C++ header files
    .lxx - headers with definitions of inline methods (CDL packages)

@subsection OCCT_RULES_SECTION_2_3 Names of toolkits

The following rules are usually used in naming of toolkits:

### Prefix for toolkits names [MANDATORY]
Toolkits names are prefixed by TK, followed by suggestive part of name explaining the domain of functionality covered by the toolkit (e.g. TKOpenGl).

### Names of classes
Usually source files located in the unit have names that start from the name of the unit, separated from remaining part of file name (if any) by underscore "_".
For instance, names of files containing sources of C++ classes are constructed according to the following template.

### Naming of C++ class files
The following template should be used for names of files containing sources of C++ classes:

    <unit-name>_<class-name>.cxx (.hxx, .cdl etc.)

Files that contain sources related to whole unit are called by the name of unit with appropriate extension.

### Names of functions
The term 'function' here is defined as:
- Any class method
- Any package method
- Any non-member procedure or function

It is preferred to name public methods from upper case, while protected and private methods from low case.

~~~~~{.cpp}
class MyPackage_MyClass
{

public:

  Standard_Integer Value() const;
  void             SetValue (const Standard_Integer theValue);

private:

  void setIntegerValue (const Standard_Integer theValue);

};
~~~~~

@subsection OCCT_RULES_SECTION_2_4 Names of variables
There are several rules that describe currently accepted practice used for naming variables.

### Naming of variables
Name of variable should not conflict with the global names (packages, macros, functions, global variables etc.), either existing or possible.
The name of variable should not start with underscore(s).

See the following examples:

~~~~~{.cpp}
Standard_Integer Elapsed_Time = 0; // this is bad - possible class   name
Standard_Integer gp = 0;           // this is bad - existing package name
Standard_Integer aGp = 0;          // this is OK
Standard_Integer _KERNEL = 0;      // this is bad
Standard_Integer THE_KERNEL = 0;   // this is OK
~~~~~

### Names of function parameters
The name of a function (procedure, class method) parameter should start with 'the' followed by the rest of the name starting with capital letter.

See the following examples:

~~~~~{.cpp}
void Package_MyClass::MyFunction (const gp_Pnt& p);        // this is bad
void Package_MyClass::MyFunction (const gp_Pnt& theP);     // this is OK
void Package_MyClass::MyFunction (const gp_Pnt& thePoint); // this is preferred
~~~~~

### Names of class member variables
The name of a class member variable should start with 'my' followed by the rest of the name (rule for suggestive names applies) starting with capital letter.

See the following examples:

~~~~~{.cpp}
Standard_Integer counter;   // This is bad
Standard_Integer myC;       // This is OK
Standard_Integer myCounter; // This is preferred
~~~~~

### Names of global variables
It is strongly recommended to avoid defining any global variables.
However, as soon as global variable is necessary, the following rule applies.
Global variable name should be prefixed by the name of a class or a package where it is defined followed with '_my'.

See the following examples:

~~~~~{.cpp}
Standard_Integer MyPackage_myGlobalVariable = 0;
Standard_Integer MyPackage_MyClass_myGlobalVariable = 0;
~~~~~

Static constants within the file should be spelled upper-case and started with 'THE_' prefix:
~~~~~{.cpp}
namespace
{
  static const Standard_Real THE_CONSTANT_COEF = 3.14;
};
~~~~~

### Names of local variables
Local variable name should be constructed in such way that it can be distinguished from the name of a function parameter, a class member variable and a global variable.
It is preferred to prefix local variable names with 'a' and 'an' (also 'is', 'to' and 'has' for Boolean variables).

See the following examples:

~~~~~{.cpp}
Standard_Integer theI;    // this is bad
Standard_Integer i;       // this is bad
Standard_Integer index;   // this is bad
Standard_Integer anIndex; // this is OK
~~~~~

### Avoid dummy names
Avoid dummy names like I, j, k. Such names are meaningless and easy to mix up.
Code becomes more and more complicated when such dummy names used multiple times in code with different meaning, in cycles with different iteration ranges and so on.

See the following examples for preferred style:

~~~~~{.cpp}
void Average (const Standard_Real** theArray,
              Standard_Integer      theRowsNb,
              Standard_Integer      theRowLen,
              Standard_Real&        theResult)
{
  theResult = 0.0;
  for (Standard_Integer aRow = 0; aRow < aRowsNb; ++aRow)
  {
    for (Standard_Integer aCol = 0; aCol < aRowLen; ++aCol)
    {
      theResult += theArray[aRow][aCol];
    }
    theResult /= Standard_Real(aRowsNb * aRowLen);
  }
}
~~~~~

@section OCCT_RULES_SECTION_3 Formatting rules

In order to improve the open source readability and, consequently, maintainability, the following set of rules is applied.

### International language [MANDATORY]
All comments in all sources must be in English.

### Line length
In all sources try not to exceed 120 characters limit of line length.

### C++ style comments
Prefer C++ style comments in C++ sources.

### Commenting out unused code
Delete unused code instead of commenting it or using #define.

### Indentation in sources [MANDATORY]
Indentation in all sources should be set to two space characters.
Use of tabulation characters for indentation is disallowed.

### Separating spaces
Punctuation rules follow the rules of English.
C/C++ reserved words, commas, colons and semicolons should be followed by a space character if they are not at the end of line.
There should be no space characters after '(' and before ')'. Closing and opening brackets should be separated by a space character.
For better readability it is also recommended to surround conventional operators by a space character. See the following examples:

~~~~~{.cpp}
while (true)                            // NOT: while( true ) ...
{
  DoSomething (theA, theB, theC, theD); // NOT: DoSomething(theA,theB,theC,theD);
}
for (anIter = 0; anIter < 10; ++anIter) // NOT: for (anIter=0;anIter<10;++anIter){
{
  theA = (theB + theC) * theD;          // NOT: theA=(theB+theC)*theD
}
~~~~~

### Separate logical blocks
Separate logical blocks of code with one blank line and comments.
See the following example:

~~~~~{.cpp}
// check arguments
Standard_Integer anArgsNb = argCount();
if (anArgsNb < 3 || isSmthInvalid)
{
  return THE_ARG_INVALID;
}

// read and check header
...
...

// do our job
...
...
~~~~~

Notice that multiple blank lines should be avoided.

### Separate function bodies [MANDATORY]
Use function descriptive blocks to separate function bodies from each other.
Each descriptive block should contain at least a function name and description of purpose.
See the following example:

~~~~~{.cpp}
// ----------------------------------------------
// function : TellMeSmthGood
// purpose  : Gives me good news
// ----------------------------------------------
void TellMeSmthGood()
{
  ...
}

// ----------------------------------------------
// function : TellMeSmthBad
// purpose  : Gives me bad news
// ----------------------------------------------
void TellMeSmthBad()
{
  ...
}
~~~~~

### Block layout [MANDATORY]
Figure brackets '{', '}' and each operator (for, if, else, try, catch) should be on dedicated line.
General block should have layout similarly to the following:

~~~~~{.cpp}
while (expression)
{
  ...
}
~~~~~

Entering block increases and leaving block decreases indentation to one tabulation.

### Single-line operators
Single-line conditional operator (if, while, for etc.) can be written without brackets on the following line.

~~~~~{.cpp}
if (!myIsInit) return Standard_False; // bad

if (thePtr == NULL)                   // OK
  return Standard_False;

if (!theAlgo.IsNull())                // preferred
{
  DoSomething();
}
~~~~~

Code on the same line is less convenient for debugging.

### Use alignment
Use alignment wherever it enhances readability. See the following example:

~~~~~{.cpp}
MyPackage_MyClass anObject;
Standard_Real     aMinimum = 0.0;
Standard_Integer  aVal     = theVal;
switch (aVal)
{
  case 0:  computeSomething();              break;
  case 12: computeSomethingElse (aMinimum); break;
  case 3:
  default: computeSomethingElseYet();       break;
}
~~~~~

### Indentation of comments
Comments should be indented similar to the code which they refer to or can be on the same line if they are short.
Text should be delimited with single space character from slash.
See the following example:

~~~~~{.cpp}
while (expression)   //bad comment
{
  // this is a long multi-line comment
  // which is really required
  DoSomething();     // maybe, enough
  DoSomethingMore(); // again
}
~~~~~

### Early return statement
Prefer early return condition rather than collecting indentations.
Better write like this:

~~~~~{.cpp}
Standard_Integer ComputeSumm (const Standard_Integer* theArray,
                              const Standard_Size     theSize)
{
  Standard_Integer aSumm = 0;
  if (theArray == NULL || theSize == 0)
  {
    return 0;
  }

  ... computing summ ...
  return aSumm;
}
~~~~~

rather than:

~~~~~{.cpp}
Standard_Integer ComputeSumm (const Standard_Integer* theArray,
                              const Standard_Size     theSize)
{
  Standard_Integer aSumm = 0;
  if (theArray != NULL && theSize != 0)
  {
    ... computing summ ...
  }
  return aSumm;
}
~~~~~

to improve readability and reduce unnecessary indentation depth.

### Trailing spaces
Trailing spaces should be removed when possible.
Spaces at end of line are useless and do not affect functionality.

### Headers order
Split into groups: system headers, per framework headers, project headers; sort includes list alphabetically.
This rule can improve readability, allows detection of useless header's multiple inclusions and makes 3rd-party dependencies clearly visible.

~~~~~{.cpp}
// system headers
#include <iostream>
#include <windows.h>

// Qt headers
#include <QDataStream>
#include <QString>

// OCCT headers
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <NCollection_List.hxx>
~~~~~

@section OCCT_RULES_SECTION_4 Documentation rules

The source code is one of the most important references for documentation.
The comments in the source code should be complete enough to allow understanding of that code, and to serve as basis for other documents.
The main reasons why comments are regarded as documentation and should be maintained are:

- The comments are easy to reach - they are always together with source code
- It's easy to update description in the comment when source is modified
- The source itself represents a good context to describe various details that would require much more explanations in separate document
- As a summary, this is the most cost-effective documentation

The comments should be compatible with Doxygen tool for automatic documentation generation (thus should use compatible tags).

### Documenting classes [MANDATORY]
Each class should be documented in its header file (.hxx or .cdl).
The comment should give enough details for the reader to understand the purpose of the class and main way of work with it.

### Documenting class methods [MANDATORY]
Each class or package method should be documented in the header file (.hxx or .cdl).
The comment should explain the purpose of the method, its parameters, and returned value(s).
Accepted style is:

@verbatim
//! Method computes the square value.
//! @param theValue the input value
//! @return squared value
Standard_Export Standard_Real Square (Standard_Real theValue);
@endverbatim

### Documenting C/C++ sources
It is very desirable to put comments in the C/C++ sources of the package/class.
They should be detailed enough to allow any person to understand what does each part of code, and get familiar with it.
It is recommended to comment all static functions (like methods in headers), and at least each 10-100 lines of the function bodies.
There are also some rules that define how comments should be formatted, see section "Formatting Rules".
Following these rules is important for good comprehension of the comments;
moreover it makes possible to automatically generate user-oriented documentation directly from commented sources.

@section OCCT_RULES_SECTION_5 Application design

The following set of rules defines the common style which should be applied by any developer contributing to the open source.

### Allow for possible inheritance
Try to design general classes (objects) keeping possible inheritance in mind.
This rule means that making possible extensions of your class the user should not encounter with problems of private implementations.
Try to use protected members and virtual methods wherever you expect extensions in the future.

### Avoid friend declarations
Avoid using 'friend' classes or functions except some specific cases (ex., iteration) 'Friend' declarations increase coupling.

### Set/get methods
Avoid providing set/get methods for all fields of the class.
Intensive set/get functions break down encapsulation.

### Hiding virtual functions [MANDATORY]
Avoid hiding a base class virtual function by a redefined function with a different signature.
Most of the compilers issue warning on this.

### Avoid mixing error reporting strategies
Try not to mix different error indication/handling strategies (exceptions or returned values) on the same level of an application.

### Minimize compiler warnings [MANDATORY]
When compiling the source pay attention to and try to minimize compiler warnings.

### Avoid unnecessary inclusion
Try to minimize compilation dependencies by removing unnecessary inclusion.

@section OCCT_RULES_SECTION_6 General C/C++ rules

This section defines rules for writing portable and maintainable C/C++ source code.

### Wrapping of global variables [MANDATORY]
Use package or class methods returning reference to wrap global variables to reduces possible name space conflicts.

### Avoid private members
Use 'protected' members instead of 'private' wherever reasonable to enable future extensions.
Use 'private' fields if future extensions should be disabled.

### Constants and inlines over defines [MANDATORY]
Use constant variables (const) and inline functions instead of defines (#define).

### Avoid explicit numerical values [MANDATORY]
Avoid usage of explicit numeric values. Use named constants and enumerations instead.
Magic numbers are badly to read and maintain.

### Three mandatory methods
A class with any of (destructor, assignment operator, copy constructor) usually needs all of them.

### Virtual destructor
A class with virtual function(s) ought to have a virtual destructor.

### Default parameter value
Do not redefine a default parameter value in an inherited function.

### Use const modifier
Use const modifier wherever possible (functions parameters, return values etc.)

### Usage of goto [MANDATORY]
Avoid goto statement except the cases where it is really needed.

### Declaring variable in for() header
Declaring cycle variable in the header of the for() statement if not used out of cycle.

~~~~~{.cpp}
Standard_Real aMinDist = Precision::Infinite();
for (NCollection_Sequence<gp_Pnt>::Iterator aPntIter (theSequence);
     aPntIter.More(); aPntIter.Next())
{
  aMinDist = Min (aMinDist, theOrigin.Distance (aPntIter.Value()));
}
~~~~~

### Condition statements within zero
Avoid usage of C-style comparison for non-boolean variables:

~~~~~{.cpp}
void Function (Standard_Integer theValue,
               Standard_Real*   thePointer)
{
  if (!theValue)          // bad style - ambiguous logic
  {
    DoSome();
  }

  if (theValue == 0)      // OK
  {
    DoSome();
  }

  if (thePointer != NULL) // OK, predefined NULL makes pointer comparison cleaner to reader
  {                       // (nullptr should be used instead as soon as C++11 will be available)
    DoSome2();
  }
}
~~~~~

@section OCCT_RULES_SECTION_7 Portability issues

This chapter contains rules that are critical for cross-platform portability.

### Ensure code portability [MANDATORY]
It is required that source code must be portable to all platforms listed in the official 'Technical Requirements'.
The term 'portable' here means 'able to be built from source'.

The C++ source code should meet C++03 standard.
Any usage of compiler-specific features or further language versions (C++11, until all major compliers on all supported platforms do not implement all it features)
should be optional (escaped with appropriate preprocessor checks) and non-exclusive (alternative implementation should be provided, compatible with other compilers).

### Avoid usage of global variables [MANDATORY]
Avoid usage of global variables. Usage of global variables may cause problems of accessing them from another shared library.
Instead of global variables, use global (package or class) functions that return reference to static variable local to this function.
Another possible problem is the order of initialization of global variables defined in various libraries that may differ depending on platform, compiler and environment.

### Avoid explicit basic types
Avoid explicit usage of basic types (int, float, double etc.), use Open CASCADE Technology types (from package Standard - see Standard_Integer, Standard_Real, Standard_ShortReal, Standard_Boolean, Standard_CString and others) or specific typedef instead.

### Use sizeof() to calculate sizes [MANDATORY]
Do not assume sizes of types. Use sizeof() instead to calculate sizes.

### Empty line at end of file [MANDATORY]
In accordance with C++03 standard source files should be trailed by empty line.
It is recommended to follow this rule for any plain text files for consistency and for correct work of git difference tools.

@section OCCT_RULES_SECTION_8 Stability issues

The rules listed in this chapter are important for stability of the programs that use Open CASCADE Technology libraries.

### OSD::SetSignal() to catch exceptions
When using Open CASCADE Technology in an application, make sure to call OSD::SetSignal() function when the application is initialized.
This will install C handlers for run-time interrupt signals and exceptions,
so that low-level exceptions (such as access violation, division by zero etc.) will be redirected to C++ exceptions
(that use try {...} catch (Standard_Failure) {...} blocks).
The above rule is especially important for robustness of modeling algorithms.

### Cross-referenced handles
Take care about cycling of handled references to avoid chains which will never be freed.
For that purpose, use a pointer at one (subordinate) side. See the following example:

In MyPackage.cdl:

    class MyFirstHandle;
    class MySecondHandle;
    pointer MySecondPointer to MySecondHandle;
    ...

In MyPackage_MyFirstHandle.cdl:

    class MyFirstHandle from MyPackage
    ...
    is
    ...
      SetSecondHandleA (me: mutable; theSecond: MySecondHandle from MyPackage);
      SetSecondHandleB (me: mutable; theSecond: MySecondHandle from MyPackage);
    ...
    fields
    ...
      mySecondHandle  : MySecondHandle  from MyPackage;
      mySecondPointer : MySecondPointer from MyPackage;
    ...
    end MyFirstHandle from MyPackage;

In MyPackage_MySecondHandle.cdl:

    class MySecondHandle from MyPackage
    ...
    is
    ...
      SetFirstHandle (me: mutable; theFirst: MyFirstHandle from MyPackage);
    ...
    fields
    ...
      myFirstHandle : MyFirstHandle from MyPackage;
    ...
    end MySecondHandle from MyPackage;

In C++ code:

~~~~~{.cpp}
void MyFunction()
{
  Handle(MyPackage_MyFirstHandle)  anObj1 = new MyPackage_MyFirstHandle();
  Handle(MyPackage_MySecondHandle) anObj2 = new MyPackage_MySecondHandle();
  Handle(MyPackage_MySecondHandle) anObj3 = new MyPackage_MySecondHandle();

  anObj1->SetSecondHandleA(anObj2);
  anObj1->SetSecondHandleB(anObj3);
  anObj2->SetFirstHandle(anObj1);
  anObj3->SetFirstHandle(anObj1);

  // memory is not freed here !!!
  anObj1.Nullify();
  anObj2.Nullify();

  // memory is freed here
  anObj3.Nullify();
}
~~~~~

### C++ memory allocation
In C++ use new and delete operators instead of malloc() and free().
Try not to mix different memory allocation techniques.

### Match new and delete [MANDATORY]
Use the same form of new and delete.

~~~~~{.cpp}
aPtr1 = new TypeA[n];              ... ; delete[]        aPtr1;
aPtr2 = new TypeB();               ... ; delete          aPtr2;
aPtr3 = Standard::Allocate (4096); ... ; Standard::Free (aPtr3);
~~~~~

### Methods managing dynamical allocation [MANDATORY]
Define a destructor, a copy constructor and an assignment operator for classes with dynamically allocated memory.

### Uninitialized variables [MANDATORY]
Every variable should be initialized.

~~~~~{.cpp}
Standard_Integer aTmpVar1;     // bad
Standard_Integer aTmpVar2 = 0; // OK
~~~~~

Uninitialized variables might be kept only within performance-sensitive code blocks and only when their initialization is *guarantied* by following code.

### Do not hide global new
Avoid hiding the global new operator.

### Assignment operator
In operator=() assign to all data members and check for assignment to self.

### Float comparison
Don't check floats for equality or non-equality; check for GT, GE, LT or LE.

~~~~~{.cpp}
if (Abs (theFloat1 - theFloat2) < theTolerance)
{
  DoSome();
}
~~~~~

Package 'Precision' provides standard values for SI units and widely adopted by existing modeling algorithms:
- Precision::Confusion() for lengths in meters
- Precision::Angular() for angles in radians

as well as definition of infinity values within sanity range of double precision:
- Precision::Infinite()
- Precision::IsInfinite()
- Precision::IsPositiveInfinite()
- Precision::IsNegativeInfinite()

### Non-indexed iteration
Avoid usage of iteration over non-indexed collections of objects.
If such iteration is used, make sure that the result of the algorithm does not depend on order.

Since the order of iteration is unpredictable in this case, it frequently leads to different behavior of the application from one run to another,
thus embarrassing the debugging process.
It mostly concerns mapped objects for which pointers are involved in calculating the hash function.
For example, the hash function of TopoDS_Shape involves the address of TopoDS_TShape object.
Thus the order of the same shape in the TopTools_MapOfShape will vary in different sessions of the application.

### Do not throw in destructors
Do not throw from within destructor.

### Assigning to reference [MANDATORY]
Avoid possible assignments of the temporary object to a reference.
Different behavior for different compiler of different platforms.

@section OCCT_RULES_SECTION_9 Performance issues

These rules define the ways of avoiding possible loss of performance caused by ineffective programming.

### Class fields alignment
In a class, declare its fields in the decreasing order of their size for better alignment.
Generally, try to reduce misaligned accesses since they impact the performance (for example, on Intel machines).

### Fields initialization order [MANDATORY]
List class data members in the constructor's initialization list in the order they are declared.

~~~~~{.cpp}
class MyPackage_MyClass
{

public:

  MyPackage_MyClass()
  : myPropertyA (1),
    myPropertyB (2) {}

// NOT
// : myPropertyB (2),
//   myPropertyA (1) {}

private:

  Standard_Integer myPropertyA;
  Standard_Integer myPropertyB;

};
~~~~~

### Initialization over assignment
In class constructors prefer initialization over assignment.

~~~~~{.cpp}
MyPackage_MyClass()
: myPropertyA (1)  // preferred
{
  myPropertyB = 2; // not recommended
}
~~~~~

### Optimize caching
When programming procedures with extensive memory access, try to optimize them in terms of cache behavior.
Here is an example of how cache behavior can be impact:
On x86 this code

~~~~~{.cpp}
Standard_Real anArray[4096][2];
for (Standard_Integer anIter = 0; anIter < 4096; ++anIter)
{
  anArray[anIter][0] = anArray[anIter][1];
}
~~~~~

is more efficient than

~~~~~{.cpp}
Standard_Real anArray[2][4096];
for (Standard_Integer anIter = 0; anIter < 4096; ++anIter)
{
  anArray[0][anIter] = anArray[1][anIter];
}
~~~~~

since linear access (above) does not invalidate cache too often.

@section OCCT_RULES_SECTION_10 Examples

Here is C++ source file sample:

@verbatim
//! Sample documented class
class Package_Class
{

public: //! @name public methods

  //! Method computes the square value.
  //! @param theValue the input value
  //! @return squared value
  Standard_Export Standard_Real Square (const Standard_Real theValue);

private: //! @name private methods

  //! Auxiliary method
  void increment();

private: //! @name private fields

  Standard_Integer myCounter; //!< usage counter

};
@endverbatim

~~~~~{.cpp}
#include <Package_Class.hxx>

// ==========================================================
// function : Square
// purpose  : Method computes the square value
// ==========================================================
Standard_Real Package_Class::Square (const Standard_Real theValue)
{
  increment();
  return theValue * theValue;
}

// ==========================================================
// function : increment
// purpose  :
// ==========================================================
void Package_Class::increment()
{
  ++myCounter;
}
~~~~~

TCL script for Draw Harness:
~~~~~{.tcl}
# show fragments (solids) in shading with different colors
proc DisplayColored {theShape} {
  set aSolids [uplevel #0 explode $theShape so]
  set aColorIter 0
  set THE_COLORS {red green blue1 magenta1 yellow cyan1 brown}
  foreach aSolIter $aSolids {
    uplevel #0 vdisplay         $aSolIter
    uplevel #0 vsetcolor        $aSolIter [lindex $THE_COLORS [expr [incr aColorIter] % [llength $THE_COLORS]]]
    uplevel #0 vsetdispmode     $aSolIter 1
    uplevel #0 vsetmaterial     $aSolIter plastic
    uplevel #0 vsettransparency $aSolIter 0.5
  }
}

# load modules
pload MODELING VISUALIZATION

# create boxes
box bc  0 0 0 1 1 1
box br  1 0 0 1 1 2
compound bc br c

# show fragments (solids) in shading with different colors
vinit View1
vclear
vaxo
vzbufftrihedron
DisplayColored c
vfit
vdump $imagedir/${casename}.png 512 512
~~~~~

GLSL program:
~~~~~{.fs}
vec3 Ambient;  //!< Ambient  contribution of light sources
vec3 Diffuse;  //!< Diffuse  contribution of light sources
vec3 Specular; //!< Specular contribution of light sources

//! Computes illumination from light sources
vec4 ComputeLighting (in vec3 theNormal,
                      in vec3 theView,
                      in vec4 thePoint)
{
  // clear the light intensity accumulators
  Ambient  = occLightAmbient.rgb;
  Diffuse  = vec3 (0.0);
  Specular = vec3 (0.0);
  vec3 aPoint = thePoint.xyz / thePoint.w;
  for (int anIndex = 0; anIndex < occLightSourcesCount; ++anIndex)
  {
    int aType = occLight_Type (anIndex);
    if (aType == OccLightType_Direct)
    {
      directionalLight (anIndex, theNormal, theView);
    }
    else if (aType == OccLightType_Point)
    {
      pointLight (anIndex, theNormal, theView, aPoint);
    }
  }

  return vec4 (Ambient,  1.0) * occFrontMaterial_Ambient()
       + vec4 (Diffuse,  1.0) * occFrontMaterial_Diffuse()
       + vec4 (Specular, 1.0) * occFrontMaterial_Specular();
}

//! Entry point to the Fragment Shader
void main()
{
  gl_FragColor = computeLighting (normalize (Normal),
                                  normalize (View),
                                  Position);
}
~~~~~
