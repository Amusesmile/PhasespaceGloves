//
//  alloOwl.h
//  PhasespaceGloves
//
//  Created by Joshua Dickinson on 1/26/13.
//  Copyright (c) 2013 Joshua Dickinson. All rights reserved.
//

#ifndef PhasespaceGloves_alloOwl_h
#define PhasespaceGloves_alloOwl_h

#include "./owl/owl.h"
#include "./owl/libowlsock.cc"
#include "./owl/owl_math.cc"
#include "allocore/al_Allocore.hpp"

#define SERVER_NAME "192.168.0.127"
#define MARKER_COUNT 180

using namespace al;

void owl_print_error(const char *s, int n)
{
    if(n < 0) printf("%s: %d\n", s, n);
    else if(n == OWL_NO_ERROR) printf("%s: No Error\n", s);
    else if(n == OWL_INVALID_VALUE) printf("%s: Invalid Value\n", s);
    else if(n == OWL_INVALID_ENUM) printf("%s: Invalid Enum\n", s);
    else if(n == OWL_INVALID_OPERATION) printf("%s: Invalid Operation\n", s);
    else printf("%s: 0x%x\n", s, n);
}

struct KinematicState
{
    KinematicState()
    {}
    
    void update(Vec3f newPosition)
    {
        
        Vec3f newVelocity = newPosition-position;
        Vec3f newAcceleration = newVelocity-velocity;
        Vec3f newJerk = newAcceleration-acceleration;
        position.set(newPosition);
        velocity.set(newVelocity);
        acceleration.set(newAcceleration);
        jerk.set(newJerk);
    }
    
    Vec3f position, velocity, acceleration, jerk;
};

class PhasespaceManager
{
public:
    PhasespaceManager()
    {
    }
    
    void owl_fetch()
    {
        for(int i = 0;i<MARKER_COUNT;i++)
        {
            
            markerPositions[i].update(Vec3f(rnd::uniform(-0.5,0.5),rnd::uniform(-0.5,0.5), rnd::uniform(-0.5,0.5)));
            //cout << markerPositions[i].position.x << endl;
        }
        
//        int numberOfMarkers = owlGetMarkers(markers, MARKER_COUNT);// get some markers
//        int error = owlGetError();
//        if(error != OWL_NO_ERROR)
//        {
//            owl_print_error("error", error);
//            return;
//        }
//        if(numberOfMarkers > 0)
//        {
//            bool gotAtLeastOne = false;
//            for(int i = 0; i < numberOfMarkers; i++)
//            {
//                if(markers[i].cond > 0)
//                {
//                    gotAtLeastOne = true;
//                }
//            }
//            
//            if (gotAtLeastOne)
//            {
//                for(int i = 0; i < numberOfMarkers; i++)
//                {
//                    if(markers[i].cond > 0)
//                    {
//                        printf("marker: %d	x: %f y: %f z: %f\n", i, markers[i].x, markers[i].y, markers[i].z);
//                        //                    masterPositions[i].x = markers[i].x*-1.0;
//                        //                    masterPositions[i].y = markers[i].y;
//                        //                    masterPositions[i].z = markers[i].z*-1.0;
//                        //                    inFrameArray[i] = true;
//                        //
//                        //                    if(gloveSet == false || (gloveSet2 == false && numberOfGloves > 1)){
//                        //                        gloveCheck(i);//sets the glove offset value so that we can use any strand of lights.
//                        //                    }
//                    }
//                }
//            }
//        }
    }
    
    static void *OwlCallback(void*)
    {
        while(1)
        {
            cout << "fetching owl\n";
            master().owl_fetch();
        }
    }
    
    void startThread(const char* serverName = SERVER_NAME, int owlFrequency = 120, int initFlags = 0)
    {
        //initialize
//        if (owlInit(serverName, initFlags) < 0)
//        {
//            owl_print_error("Couldn't initialize: is another OWL client running? There can only be one OWL client running at a time.\n", owlGetError());
//            return;
//        }
//        else
//        {
//            int tracker = 0;//create tracker 0
//            owlTrackeri(tracker, OWL_CREATE, OWL_POINT_TRACKER);// set markers
//            for(int i = 0; i < MARKER_COUNT; i++)
//            {
//                owlMarkeri(MARKER(tracker, i), OWL_SET_LED, i);// activate tracker
//            }
//            owlTracker(tracker, OWL_ENABLE);// flush requests and check for errors
//            if(!owlGetStatus())
//            {
//                owl_print_error("error in point tracker setup", owlGetError());
//                return;
//            }
//            
//            owlSetFloat(OWL_FREQUENCY, owlFrequency);// set default frequency OWL_MAX_FREQUENCY (this is 480)
//            owlSetInteger(OWL_STREAMING, OWL_ENABLE);// start streaming
//        }
        
        cout << "Starting owl_fetch thread\n";
        int pthreadErrorCode = pthread_create(&owlThread, NULL, OwlCallback, NULL);
        if (pthreadErrorCode)
        {
            printf("ERROR; Could't start owlThread. Return code from pthread_create() is %d\n", pthreadErrorCode);
            exit(-1);
        }
    }
    
    void stopThread()
    {
        owlDone();
        pthread_cancel(owlThread);
    }
    
    static PhasespaceManager& master(){
        static PhasespaceManager *master = new PhasespaceManager;
        return *master;
    }
    
    KinematicState* getStates()
    {
        return master().markerPositions;
    }
    
private:
    pthread_t owlThread;
    OWLMarker markers[MARKER_COUNT];
    KinematicState markerPositions[MARKER_COUNT];
    int markerNumberOfAbsentFrames[MARKER_COUNT];
};

#endif
