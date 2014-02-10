// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_WIREPORTABLE_
#define _YARP2_WIREPORTABLE_

#include <yarp/os/Portable.h>

namespace yarp {
    namespace os {
        namespace idl {
            class WireReader;
            class WireWriter;
            class WirePortable;
	}
    }
}

/**
 *
 * A "tamed" Portable, that promises to serialize itself in an IDL-friendly
 * way.
 *
 */
class YARP_OS_API yarp::os::idl::WirePortable : public yarp::os::Portable {
public:
    using yarp::os::Portable::read;
    using yarp::os::Portable::write;
    virtual bool read(yarp::os::idl::WireReader& reader) = 0;
    virtual bool write(yarp::os::idl::WireWriter& writer) = 0;
};

#endif

