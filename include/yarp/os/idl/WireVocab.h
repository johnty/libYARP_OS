// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_WIREVOCAB_
#define _YARP2_WIREVOCAB_

#include <yarp/os/api.h>
#include <iosfwd>

namespace yarp {
    namespace os {
        namespace idl {
            class WireVocab;
        }
    }
}

class YARP_OS_API yarp::os::idl::WireVocab {
public:
    virtual ~WireVocab() {}
    virtual int fromString(const std::string& input) = 0;
    virtual std::string toString(int input) = 0;
};

#endif
