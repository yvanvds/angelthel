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

#define AS_ASSERT(r)DYNAMIC_ASSERT(r,"Error");

#define CLASS_DEFINE(name, target) C Str8 CN = name; AS_ASSERT(RegClass<target>(CN, asOBJ_VALUE | asOBJ_POD   ))

#define CLASS_CONSTRUCT(name, construct, destruct) C Str8 CN = name; AS_ASSERT(RegClassBehavior(CN, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct))); AS_ASSERT(RegClassBehavior(CN, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destruct)))

#define CLASS_COPY(script, native) AS_ASSERT(RegClassBehavior(CN, asBEHAVE_CONSTRUCT, S + "void f" + script, asFUNCTIONPR native))

#define CLASS_FUNC(script, native)       AS_ASSERT(RegClassMethod  (CN, script, asMETHOD   native ))
#define CLASS_FUNC_ARG(script, native)   AS_ASSERT(RegClassMethod  (CN, script, asMETHODPR native ))
#define CLASS_PROPERTY(script, native)   AS_ASSERT(RegClassProperty(CN, script, asOFFSET   native ))

#define CLASS_OBJECT(name, target)       AS_ASSERT(RegGlobalProperty(CN + " " + name, &target))

#define CLASS_OPP_EQUALS(script, native) AS_ASSERT(RegClassMethod(CN, S  + "bool opEquals(const " + script + " &in) const", asMETHOD  (native, operator==)))
#define CLASS_OPP_ASSIGN(script, native) AS_ASSERT(RegClassMethod(CN, CN + " &opAssign(const "    + script + " &in)"      , asMETHODPR(native, operator =, (C native&), native&)))

#define GLOBAL_FUNC(script,native)       AS_ASSERT(RegGlobalFunc(script, asFUNCTION   native, asCALL_CDECL))
#define GLOBAL_FUNC_ARG(script, native)  AS_ASSERT(RegGlobalFunc(script, asFUNCTIONPR native, asCALL_CDECL))
