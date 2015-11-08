#include "angelscript.h"
#include "contextmgr/contextmgr.h"
#include "debugger/debugger.h"
#include "scriptany/scriptany.h"
#include "scriptarray/scriptarray.h"
#include "scriptbuilder/scriptbuilder.h"
#include "scriptdictionary/scriptdictionary.h"
#include "scriptfile/scriptfile.h"
#include "scriptfile/scriptfilesystem.h"
#include "scriptgrid/scriptgrid.h"
#include "scripthandle/scripthandle.h"
#include "scripthelper/scripthelper.h"
#include "scriptmath/scriptmath.h"
#include "scriptmath/scriptmathcomplex.h"
#include "serializer/serializer.h"
#include "weakref/weakref.h"

/***************************************************************************
 Please be sure that \ is the last character in multiline macro's. 
 There cannot be a whitespace after that.
***************************************************************************/


// A helper assert for debugging declarations
#define AS_ASSERT(r)DYNAMIC_ASSERT(r,"Error");

// helper macro's for stringification
#define S(x) #x
#define STRING(x) S(x)

// helpers for variable argument list
#define GLUE(x, y) x y

#define RETURN_ARG_COUNT(_1_, _2_, _3_, _4_, _5_, count, ...) count
#define EXPAND_ARGS(args) RETURN_ARG_COUNT args
#define COUNT_ARGS_MAX5(...) EXPAND_ARGS((__VA_ARGS__, 5, 4, 3, 2, 1, 0))

#define OVERLOAD_MACRO2(name, count) name##count
#define OVERLOAD_MACRO1(name, count) OVERLOAD_MACRO2(name, count)
#define OVERLOAD_MACRO(name, count) OVERLOAD_MACRO1(name, count)

#define CALL_OVERLOAD(name, ...) GLUE(OVERLOAD_MACRO(name, COUNT_ARGS_MAX5(__VA_ARGS__)), (__VA_ARGS__))

// overloaded copy constructors, do no use directly
#define DEFINE_COPY_CTOR2(arg1                  , native) void ASCopy_##AS_CLASS(arg1                  , AS_CLASS * p) { new(p)AS_CLASS native ; }
#define DEFINE_COPY_CTOR3(arg1, arg2            , native) void ASCopy_##AS_CLASS(arg1, arg2            , AS_CLASS * p) { new(p)AS_CLASS native ; }
#define DEFINE_COPY_CTOR4(arg1, arg2, arg3      , native) void ASCopy_##AS_CLASS(arg1, arg2, arg3      , AS_CLASS * p) { new(p)AS_CLASS native ; }
#define DEFINE_COPY_CTOR5(arg1, arg2, arg3, arg4, native) void ASCopy_##AS_CLASS(arg1, arg2, arg3, arg4, AS_CLASS * p) { new(p)AS_CLASS native ; }

// overloaded add function for copy constructors, do not use directly
#define ADD_COPY_CTOR2(script, arg1) AS_ASSERT(RegClassBehavior(STRING(AS_CLASS), asBEHAVE_CONSTRUCT, S + "void f" + STRING(script), asFUNCTIONPR(ASCopy_##AS_CLASS, ( arg1, AS_CLASS*) , void)))
#define ADD_COPY_CTOR3(script, arg1, arg2) AS_ASSERT(RegClassBehavior(STRING(AS_CLASS), asBEHAVE_CONSTRUCT, S + "void f" + STRING(script), asFUNCTIONPR(ASCopy_##AS_CLASS, ( arg1, arg2, AS_CLASS*) , void)))
#define ADD_COPY_CTOR4(script, arg1, arg2, arg3) AS_ASSERT(RegClassBehavior(STRING(AS_CLASS), asBEHAVE_CONSTRUCT, S + "void f" + STRING(script), asFUNCTIONPR(ASCopy_##AS_CLASS, ( arg1, arg2, arg3, AS_CLASS*) , void)))
#define ADD_COPY_CTOR5(script, arg1, arg2, arg3, arg4) AS_ASSERT(RegClassBehavior(STRING(AS_CLASS), asBEHAVE_CONSTRUCT, S + "void f" + STRING(script), asFUNCTIONPR(ASCopy_##AS_CLASS, ( arg1, arg2, arg3, arg4, AS_CLASS*) , void)))

/***************************************************************************
  Macro's for public use start here
****************************************************************************/

#define DEFINE_FORWARD(name) AS_ASSERT(RegClass<name>(STRING(name), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK | asOBJ_POD))

// Before adding a class, use undef and define to set the class name.
// Example use:
//    #undef AS_CLASS
//    #define AS_CLASS Vec
#define AS_CLASS empty

// This generates a constructor and destructor for a class. Cannot be called inside a function.
// Example use: DEFINE_CTOR_DTOR();
#define DEFINE_CTOR_DTOR() void ASConstruct_##AS_CLASS(AS_CLASS *thisPointer){ new(thisPointer)  AS_CLASS(); } \
                           void ASDestruct_##AS_CLASS (AS_CLASS *thisPointer){     thisPointer->~AS_CLASS(); }


// copy constructor generator with variable arguments. Call with (arg1, ... , native).
// Up to 4 arguments (+ native) are supported. The last argument (native) passes arguments to the native function.
// Example use: DEFINE_COPY_CTOR(Byte l, Byte a, (l, a));
#define DEFINE_COPY_CTOR(...) CALL_OVERLOAD(DEFINE_COPY_CTOR, __VA_ARGS__)
                     
// A simple class that does not need a constructor or destructor
// Example use: DEFINE_SIMPLE_CLASS();
#define DEFINE_SIMPLE_CLASS() AS_ASSERT(RegClass<AS_CLASS>(STRING(AS_CLASS), asOBJ_VALUE | asOBJ_POD))

// A class that needs a constructor and destructor. Use DEFINE_CTOR_DTOR before calling this
// Example use: DEFINE_COMPLEX_CLASS();
#define DEFINE_COMPLEX_CLASS() AS_ASSERT(RegClassBehavior(STRING(AS_CLASS), asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ASConstruct_##AS_CLASS))); \
                               AS_ASSERT(RegClassBehavior(STRING(AS_CLASS), asBEHAVE_DESTRUCT , "void f()", asFUNCTION(ASDestruct_##AS_CLASS)))

// add copy constructor to class. Call with (script, arg1, ... ).
// Up to 4 arguments (+ script) are supported. The first argument (script) defines the script interface.
// Example use: ADD_COPY_CTOR( (Byte, Byte = 255), Byte, Byte);
#define ADD_COPY_CTOR(...) CALL_OVERLOAD(ADD_COPY_CTOR, __VA_ARGS__)

// Add class method. First argument is the name of the method, second it's return type
// Example use: ADD_METHOD(clear, Str &); to pass a native Str method `Str & Str::clear();'
#define ADD_METHOD(name, result) AS_ASSERT(RegClassMethod(STRING(AS_CLASS), S + STRING(result) + " " + STRING(name) + "()"      , asMETHOD   (AS_CLASS, name)))

// add const class method. This works the same as before, but the method is declared constant
#define ADD_CONST_METHOD(name, result) AS_ASSERT(RegClassMethod(STRING(AS_CLASS), S + STRING(result) + " " + STRING(name) + "() const", asMETHOD   (AS_CLASS, name)))

// add class method with arguments. Here the second argument should contain the method's arguments.
// Example use: ADD_METHOD_ARG(set, (C Vec &in), Vec&); to pass a native Vec method `Vec & Vec::set(C Vec &);'
//#define ADD_METHOD_ARG(name, arg, result) AS_ASSERT(RegClassMethod(STRING(AS_CLASS), S + STRING(result) + " " + STRING(name) + STRING(arg), asMETHODPR(AS_CLASS, name, arg, result)))

//#define ADD_CONST_METHOD_ARG(name, arg, result) AS_ASSERT(RegClassMethod(STRING(AS_CLASS), S + STRING(result) + " " + STRING(name) + STRING(arg) + " const", asMETHODPR(AS_CLASS, name, arg const, result)))

// add class method that requires default arguments (defaults must be set in scriptArg). When dealing with default
// arguments the argument list must be entered twice, first with, then without the default arguments.
// Example use: ADD_METHOD_ARG_DEFAULT(set, (Byte, Byte = 255), (Byte, Byte), Color&)
#define ADD_METHOD_ARG(name, scriptArg, nativeArg, result) AS_ASSERT(RegClassMethod(STRING(AS_CLASS), S + STRING(result) + " " + STRING(name) + STRING(scriptArg), asMETHODPR(AS_CLASS, name, nativeArg, result)))

#define ADD_CONST_METHOD_ARG(name, scriptArg, nativeArg, result) AS_ASSERT(RegClassMethod(STRING(AS_CLASS), S + STRING(result) + " " + STRING(name) + STRING(scriptArg) + " const", asMETHODPR(AS_CLASS, name, nativeArg const, result)))

// Add a class property.
// Example use: ADD_PROPERTY(r, Byte); to pass the class property r of type Byte
#define ADD_PROPERTY(name, type) AS_ASSERT(RegClassProperty(STRING(AS_CLASS), S + STRING(type) + " " + STRING(name), asOFFSET(AS_CLASS, name)))

// Register an object of the current class
// Example use: ADD_OBJECT(Ms); Could refer to the engine's object Ms of the class Mouse.
#define ADD_OBJECT(name)       AS_ASSERT(RegGlobalProperty(S + STRING(AS_CLASS) + " " + STRING(name), &name))

#define ADD_CONST_OBJECT(name) AS_ASSERT(RegGlobalProperty(S + "const" + STRING(AS_CLASS) + " " + STRING(name), &name))

// Register existing operator with the class
#define ADD_OPP_EQUALS() AS_ASSERT(RegClassMethod(STRING(AS_CLASS), S + "bool opEquals(const " + STRING(AS_CLASS) + " &in) const", asMETHOD  (AS_CLASS, operator==)))
#define ADD_OPP_ASSIGN() AS_ASSERT(RegClassMethod(STRING(AS_CLASS), S + STRING(AS_CLASS) + " &opAssign(const "    + STRING(AS_CLASS) + " &in)"      , asMETHODPR(AS_CLASS, operator =, (C AS_CLASS&), AS_CLASS&)))

#define ADD_OPP_ADD_ASSIGN(scriptArg, nativeArg) AS_ASSERT(RegClassMethod(STRING(AS_CLASS), S + STRING(AS_CLASS) + " &opAddAssign(" + STRING(scriptArg) + ")", asMETHODPR(AS_CLASS, operator+=, (nativeArg), AS_CLASS&)))
#define ADD_OPP_SUB_ASSIGN(scriptArg, nativeArg) AS_ASSERT(RegClassMethod(STRING(AS_CLASS), S + STRING(AS_CLASS) + " &opSubAssign(" + STRING(scriptArg) + ")", asMETHODPR(AS_CLASS, operator-=, (nativeArg), AS_CLASS&)))
#define ADD_OPP_MUL_ASSIGN(scriptArg, nativeArg) AS_ASSERT(RegClassMethod(STRING(AS_CLASS), S + STRING(AS_CLASS) + " &opMulAssign(" + STRING(scriptArg) + ")", asMETHODPR(AS_CLASS, operator*=, (nativeArg), AS_CLASS&)))
#define ADD_OPP_DIV_ASSIGN(scriptArg, nativeArg) AS_ASSERT(RegClassMethod(STRING(AS_CLASS), S + STRING(AS_CLASS) + " &opDivAssign(" + STRING(scriptArg) + ")", asMETHODPR(AS_CLASS, operator/=, (nativeArg), AS_CLASS&)))

#define ADD_OPP_ADD(scriptArg, nativeArg) AS_ASSERT(RegClassMethod(STRING(AS_CLASS), S + STRING(AS_CLASS) + " opAdd(" + STRING(scriptArg) + ")", asFUNCTIONPR(operator+, (C AS_CLASS &, nativeArg), AS_CLASS), asCALL_CDECL_OBJFIRST))
#define ADD_OPP_SUB(scriptArg, nativeArg) AS_ASSERT(RegClassMethod(STRING(AS_CLASS), S + STRING(AS_CLASS) + " opSub(" + STRING(scriptArg) + ")", asFUNCTIONPR(operator-, (C AS_CLASS &, nativeArg), AS_CLASS), asCALL_CDECL_OBJFIRST))
#define ADD_OPP_MUL(scriptArg, nativeArg) AS_ASSERT(RegClassMethod(STRING(AS_CLASS), S + STRING(AS_CLASS) + " opMul(" + STRING(scriptArg) + ")", asFUNCTIONPR(operator*, (C AS_CLASS &, nativeArg), AS_CLASS), asCALL_CDECL_OBJFIRST))
#define ADD_OPP_DIV(scriptArg, nativeArg) AS_ASSERT(RegClassMethod(STRING(AS_CLASS), S + STRING(AS_CLASS) + " opDiv(" + STRING(scriptArg) + ")", asFUNCTIONPR(operator/, (C AS_CLASS &, nativeArg), AS_CLASS), asCALL_CDECL_OBJFIRST))

#define ADD_OPP_ADD_R(scriptArg, nativeArg) AS_ASSERT(RegClassMethod(STRING(AS_CLASS), S + STRING(AS_CLASS) + " opAdd_r(" + STRING(scriptArg) + ")", asFUNCTIONPR(operator+, (nativeArg, C AS_CLASS &), AS_CLASS), asCALL_CDECL_OBJLAST))
#define ADD_OPP_SUB_R(scriptArg, nativeArg) AS_ASSERT(RegClassMethod(STRING(AS_CLASS), S + STRING(AS_CLASS) + " opSub_r(" + STRING(scriptArg) + ")", asFUNCTIONPR(operator-, (nativeArg, C AS_CLASS &), AS_CLASS), asCALL_CDECL_OBJLAST))
#define ADD_OPP_MUL_R(scriptArg, nativeArg) AS_ASSERT(RegClassMethod(STRING(AS_CLASS), S + STRING(AS_CLASS) + " opMul_r(" + STRING(scriptArg) + ")", asFUNCTIONPR(operator*, (nativeArg, C AS_CLASS &), AS_CLASS), asCALL_CDECL_OBJLAST))
#define ADD_OPP_DIV_R(scriptArg, nativeArg) AS_ASSERT(RegClassMethod(STRING(AS_CLASS), S + STRING(AS_CLASS) + " opDiv_r(" + STRING(scriptArg) + ")", asFUNCTIONPR(operator/, (nativeArg, C AS_CLASS &), AS_CLASS), asCALL_CDECL_OBJLAST))

#define GLOBAL_FUNC(script,native)       AS_ASSERT(RegGlobalFunc(script, asFUNCTION   native, asCALL_CDECL))
#define GLOBAL_FUNC_ARG(script, native)  AS_ASSERT(RegGlobalFunc(script, asFUNCTIONPR native, asCALL_CDECL))


// Register an enumeration type
#define START_ENUM(name) { C Str8 EN = STRING(name); AS_ASSERT(RegEnum(STRING(name))
#define ADD_EVAL(name) AS_ASSERT(RegEnumVal(EN, STRING(name), name))
#define END_ENUM() }