// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia, Anne van Rossum
 * Authors: Paul Fitzpatrick, Anne van Rossum
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_PLATFORMSTDIO_
#define _YARP2_PLATFORMSTDIO_

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE
#  include <ace/OS_NS_stdio.h>
#else
#  include <stdio.h>
#  ifndef ACE_OS
#    define ACE_OS
#  endif
#  ifndef ACE_UNUSED_ARG
#    define ACE_UNUSED_ARG(x) do {/*null*/} while (&x<0)
#  endif
#endif

#endif
