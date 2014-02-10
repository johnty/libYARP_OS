// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_FAKEFACE_
#define _YARP2_FAKEFACE_

#include <yarp/os/impl/Face.h>

namespace yarp {
    namespace os {
        namespace impl {
            class FakeFace;
        }
    }
}

/**
 * A dummy Face for testing purposes.
 */
class yarp::os::impl::FakeFace : public Face {
public:
    virtual bool open(const Address& address);
    virtual void close();
    virtual InputProtocol *read();
    virtual OutputProtocol *write(const Address& address);
private:
};

#endif
