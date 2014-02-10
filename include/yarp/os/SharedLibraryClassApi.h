// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_YARPLIBRARYCLASSAPI_
#define _YARP2_YARPLIBRARYCLASSAPI_

#include <yarp/conf/system.h>
#include <yarp/os/Vocab.h>
#include <string.h>

namespace yarp {
    namespace os {
        struct SharedLibraryClassApi;
    }
}

// Be careful loading C++ classes from DLLs.  Generally you
// need an exact or very close match between compilers used
// to compile those DLLs and your own code.

#define YARP_SHAREDLIBRARYCLASSAPI_PADDING (30-2*(YARP_POINTER_SIZE/4))
#include <yarp/os/begin_pack_for_net.h>
extern "C" {

    /**
     *
     * Collection of hooks for creating/destroying a plugin.
     * Be careful to check carefully for compatibility before 
     * using create() or destroy().
     *
     */
    struct yarp::os::SharedLibraryClassApi {
    public:
        NetInt32 startCheck;    // Constant: this should be 'Y' 'A' 'R' 'P'.
                                // Don't touch anything further if it isn't.
        NetInt32 structureSize; // size of the SharedLibraryClassApi.
                                // If this doesn't match what you expect,
                                // Don't touch anything further if it isn't.
        NetInt32 systemVersion; // Overall version of plugin system.
                                // This does *not* cover compiler version etc.
        void *(*create)();      // Instantiate a plugin object.
        void (*destroy)(void *obj);               // Destroy a plugin object.
        int (*getVersion)(char *ver, int len);    // Plugin-related version.
        int (*getAbi)(char *abi, int len);        // Compiler-related version.
        int (*getClassName)(char *name, int len); // Name of plugin (subclass).
        int (*getBaseClassName)(char *name, int len);  // Name superclass.
        NetInt32 roomToGrow[YARP_SHAREDLIBRARYCLASSAPI_PADDING]; // Padding.
        NetInt32 endCheck;      // Constant: should be 'P' 'L' 'U' 'G'.
    };
}
#include <yarp/os/end_pack_for_net.h>

#define YARP_SHARED_CLASS_FN extern "C" YARP_EXPORT

/**
 *
 * Macro to create a bunch of functions with undecorated names that can
 * be found within a plugin library to handle creation/deletion of that
 * plugin.  Use with care.
 * 
 * @param factoryname the name of the "hook" function to make.  A collection
 * of other helper functions with names composed of the factoryname with
 * _create/_destroy/... appended.
 *
 * @param classname the class that the hook will be able to instantiate.
 *
 * @param basename the superclass that the user of the plugin should be
 * working with.
 *
 */
#define YARP_DEFINE_SHARED_SUBCLASS(factoryname,classname,basename)       \
    YARP_SHARED_CLASS_FN void *factoryname ## _create () { return (basename *)new classname; } \
    YARP_SHARED_CLASS_FN void factoryname ## _destroy (void *obj) { delete (classname *)obj; } \
    YARP_SHARED_CLASS_FN int factoryname ## _getVersion (char *ver, int len) { return 0; }   \
    YARP_SHARED_CLASS_FN int factoryname ## _getAbi (char *abi, int len) { return 0; }       \
    YARP_SHARED_CLASS_FN int factoryname ## _getClassName (char *name, int len) { char cname[] = # classname; strncpy(name,cname,len); return strlen(cname)+1; } \
    YARP_SHARED_CLASS_FN int factoryname ## _getBaseClassName (char *name, int len) { char cname[] = # basename; strncpy(name,cname,len); return strlen(cname)+1; } \
    YARP_SHARED_CLASS_FN int factoryname(void *api,int len) { \
    struct yarp::os::SharedLibraryClassApi *sapi = (struct yarp::os::SharedLibraryClassApi *) api; \
    if (len<(int)sizeof(yarp::os::SharedLibraryClassApi)) return -1;    \
    sapi->startCheck = VOCAB4('Y','A','R','P'); \
    sapi->structureSize = sizeof(yarp::os::SharedLibraryClassApi);  \
    sapi->systemVersion = 2; \
    sapi->create = factoryname ## _create; \
    sapi->destroy = factoryname ## _destroy; \
    sapi->getVersion = factoryname ## _getVersion; \
    sapi->getAbi = factoryname ## _getAbi; \
    sapi->getClassName = factoryname ## _getClassName; \
    sapi->getBaseClassName = factoryname ## _getBaseClassName; \
    for (int i=0; i<YARP_SHAREDLIBRARYCLASSAPI_PADDING; i++) { sapi->roomToGrow[i] = 0; } \
    sapi->endCheck = VOCAB4('P','L','U','G');        \
    return sapi->startCheck;                    \
    }

#define YARP_DEFAULT_FACTORY_NAME "yarp_default_factory"
#define YARP_DEFINE_DEFAULT_SHARED_CLASS(classname) YARP_DEFINE_SHARED_SUBCLASS(yarp_default_factory,classname,classname)
#define YARP_DEFINE_SHARED_CLASS(factoryname,classname) YARP_DEFINE_SHARED_SUBCLASS(factoryname,classname,classname)

#endif
