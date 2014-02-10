// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2006 RobotCub Consortium
* Authors: Paul Fitzpatrick
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

// added test for threadRelease/threadInit functions, synchronization and 
// thread init success/failure notification, for thread and runnable classes
// -nat

#include <yarp/os/impl/ThreadImpl.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>

#include <yarp/os/impl/UnitTest.h>
//#include "TestList.h"

using namespace yarp::os::impl;
using namespace yarp::os;


class ThreadTest : public UnitTest {
public:

    Semaphore sema;
    Semaphore state;
    int expectCount;
    int gotCount;

private:

    class ThreadImmediateReturn: public Thread {
    public:
        virtual void run() {
        }
    };

    class ThreadDelay: public Thread {
    public:
        virtual void run() {
            Time::delay(0.5);
        }
    };

    class Thread0: public Thread {
    public:
        virtual void run() {
            Time::delay(0.01);
        }
    };

    class Thread1 : public Runnable {
    public:
        ThreadTest& owner;

        Thread1(ThreadTest& owner) : owner(owner) {}

        virtual void run() {
            for (int i=0; i<5; i++) {
                //ACE_OS::printf("tick %d\n", i);
                owner.state.wait();
                owner.expectCount++;
                owner.state.post();
                owner.sema.post();
                Time::delay(0.1);
            }
        }
    };


    class Thread2 : public ThreadImpl {
    public:
        ThreadTest& owner;

        Thread2(ThreadTest& owner) : owner(owner), mutex(1), finished(false) {}

        virtual void run() {
            bool done = false;
            while (!done) {
                owner.sema.wait();
                mutex.wait();
                done = finished;
                mutex.post();
                owner.state.wait();
                //ACE_OS::printf("burp\n");
                owner.gotCount++;
                owner.state.post();
            }
            while (owner.sema.check()) {
                owner.state.wait();
                owner.gotCount++;
                owner.state.post();
            }
            //ACE_OS::printf("done\n");
        }

        virtual void close() {
            mutex.wait();
            finished = true;
            owner.state.wait();
            owner.expectCount++;
            owner.state.post();
            owner.sema.post();
            mutex.post();
        }

    private:
        Semaphore mutex;
        bool finished;
    };


    class Thread3: public Thread {
    public:
        Thread3():state(-1){}
    
        bool onStopCalled;
        int state;
        virtual bool threadInit()
        {
            onStopCalled=false;
            state=0;
            return true;
        }

        virtual void run() {
            Time::delay(0.5);
            state++;
        }

        virtual void threadRelease()
        {state++;}

        virtual void onStop()
        { onStopCalled=true;}
    };

    class Thread4: public Thread {
    public:
        Thread4():state(-1), fail(false){}

        int state;
        bool fail;
        virtual bool threadInit()
        {
            state=0;
            return !fail;
        }

        void threadWillFail(bool f)
        {
            fail=f;
        }

        void afterStart(bool s)
        {
            if (!s)
            {state=-1;}
        }

        virtual void run() {
            Time::delay(1);
            state++;
        }

        virtual void threadRelease()
        {state++;}
    };

    class Thread5: public Thread {
    public:
        Thread5():state(0),fail(false){}
        int state;
        bool fail;

        void threadWillFail(bool f)
        {
            state=0;
            fail=f;
        }

        virtual bool threadInit()
        {
            Time::delay(0.5);
            state=1;
            return !fail;
        }

        virtual void afterStart(bool s)
        {
            if(!s)
                state++;
        }

        virtual void run() 
        {}

        virtual void threadRelease()
        {
            Time::delay(0.5);
            state++;
        }
    };


    class Runnable1: public Runnable {
    public:
        Runnable1():initCalled(false), 
            notified(false), 
            releaseCalled(false),
            executed(false)
            {}

        bool initCalled;
        bool notified;
        bool releaseCalled;
        bool executed;
        bool onStopCalled;

        virtual bool threadInit()
        {
            initCalled=true;
            onStopCalled=false;
            return true;
        }

        void afterStart(bool s)
        {   notified=true;}

        virtual void run() {
            Time::delay(0.5);
            executed=true;
        }

        virtual void threadRelease()
        {releaseCalled=true;}
    };

public:
    ThreadTest() : sema(0), state(1) {
        expectCount = 0;
        gotCount = 0;
    }

    virtual String getName() { return "ThreadTest"; }

    void testIsRunning()
    {
        report(0, "testing isRunning function");
        ThreadDelay foo;
        foo.start();
        checkTrue(foo.isRunning(), "thread is running");
        foo.stop(); //calls join
        checkTrue(!foo.isRunning(), "thread quit");
        report(0, "done");
    }

    void testSync() {
        report(0,"testing cross-thread synchronization...");
        int tct = ThreadImpl::getCount();
        Thread1 bozo(*this);
        Thread1 bozo2(*this);
        Thread2 burper(*this);
        ThreadImpl t1(&bozo);
        ThreadImpl t2(&bozo2);
        report(0,"starting threads ...");
        burper.start();
        t1.start();
        Time::delay(0.05);
        t2.start();
        checkEqual(ThreadImpl::getCount(),tct+3,"thread count");
        t1.join();
        t2.join();
        burper.close();
        burper.join();
        report(0,"... done threads");
        checkEqual(expectCount,gotCount,"thread event counts");
        checkEqual(true,expectCount==11,"thread event counts");
    }

    void testStartVersusStop() {
        report(0,"testing start/stop");
        Thread3 t;
        checkTrue(!t.isRunning(),"not active");
        t.start();
        checkTrue(t.isRunning(),"active");
        t.stop();
        checkTrue(!t.isRunning(),"not active");
        
        checkTrue(t.onStopCalled, "onStop was called");
        report(0,"done");
    }

    void testInitAndRelease()
    {
        report(0,"Checking init/release functions are actually called");
        Thread3 t;
        t.start();
        t.stop();
        checkEqual(true, t.state==2, "init/release were called");
        report(0,"done");
    }

    void testFailureSuccess()
    {
        report(0,"Checking init failure/success notification");
        Thread4 foo;
        foo.threadWillFail(false);
        foo.start();
        checkTrue(foo.isRunning(), "Thread is running");
        foo.stop();
        checkEqual(2, foo.state, "Init success was properly notified");

        foo.threadWillFail(true);
        foo.start();
        checkTrue(!foo.isRunning(), "Thread is not running");
        checkEqual(-1,foo.state, "Init failure was properly notified");

        report(0,"done");
    }

    void testInitReleaseSynchro()
    {
        Thread5 t;
        report(0,"Checking init/release synchronization");
        report(0,"Starting thread... thread will wait 0.5 second");
        t.start();
        checkEqual(1, t.state, "Start synchronized on init");

        report(0,"Stopping thread... thread will wait 0.5 second");
        t.stop();
        checkEqual(2, t.state, "Stop synchronized on release");

        t.threadWillFail(true);
        t.start();
        checkEqual(2, t.state, "Start synchronized on failed init");

        report(0, "done");
    }

    void testRunnable()
    {
        Runnable1 foo;
        report(0, "Checking runnable");
        ThreadImpl t(&foo);
        report(0, "Starting thread");
        t.start();
        report(0, "Stopping thread");
        t.close();

        checkTrue(foo.initCalled, "threadInit was called");
        checkTrue(foo.notified, "afterStart() was called");
        checkTrue(foo.executed, "thread main function was executed");
        checkTrue(foo.releaseCalled, "threadRelease was called");
        report(0, "done");
    }
 
    virtual void testMin() {
        report(0,"testing minimal thread functions to check for mem leakage...");
        for (int i=0; i<20; i++) {
            Thread0 t0, t1;
            t0.start();
            t1.start();
            t0.stop();
            t1.stop();
        }
        report(0,"...done");
    }


    virtual void testRunningFlag() {
        report(0,"testing running flag (bug #1695724)...");
        ThreadDelay t1;
        checkFalse(t1.isRunning(),"not running before start");
        t1.start();
        checkTrue(t1.isRunning(),"running after start");
        t1.stop();
        checkFalse(t1.isRunning(),"not running after stop");
        t1.start();
        checkTrue(t1.isRunning(),"running after start");
        t1.stop();
        checkFalse(t1.isRunning(),"not running after stop");

        ThreadImmediateReturn t0;
        t0.start();
        Time::delay(0.5);
        checkFalse(t0.isRunning(),"not running after thread exits");
    }

     virtual void runTests() {
        testMin();
        testSync();
        testIsRunning();
        testStartVersusStop();
        testInitReleaseSynchro();
        testFailureSuccess();
        testInitAndRelease();
        testRunnable();
        testRunningFlag();
       }
};

static ThreadTest theThreadTest;

UnitTest& getThreadTest() {
    return theThreadTest;
}

