// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/FakeFace.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/Protocol.h>
#include <yarp/os/impl/FakeTwoWayStream.h>

using namespace yarp::os::impl;

static Logger fakeFaceLog("FakeFace", Logger::get());

bool FakeFace::open(const Address& address) {
    // happy to open without fuss
    return true;
}

void FakeFace::close() {
}

InputProtocol *FakeFace::read() {
    fakeFaceLog.fail("not yet implemented");
    return NULL;
}

OutputProtocol *FakeFace::write(const Address& address) {
    Protocol *prot = new Protocol(new FakeTwoWayStream());
    return prot;
}

