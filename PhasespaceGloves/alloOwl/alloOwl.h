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
#define MARKERS_PER_GLOVE 8

using namespace al;

enum GloveMarkerMap
{
    ePinky = 0,
    eRingFinger = 1,
    eMiddleFinger = 2,
    eIndexFinger = 3,
    eBackHandNearPinky = 4,
    eBackHandNearThumb = 5,
    eThumbBase = 6,
    eThumbTip = 7
};

//flip the x and z axis to make the spatial data more intuitive
Vec3f AllosphereScalar(-1.0, 0.0, -1.0);

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
        for(int i = 0;i<MARKER_COUNT;i++)
        {
            markerPositions[i] = Vec3f(0.0, 0.0, 0.0);
            markerNumberOfAbsentFrames[i] = 0;
        }
    }
    
    void fetchOwl()
    {
        for(int i = 0;i<MARKER_COUNT;i++)
        {
            markerPositions[i] = Vec3f(rnd::uniform(-0.5,0.5),rnd::uniform(-0.1,0.1), rnd::uniform(-0.0,0.0));
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
//            for(int i = 0; i < numberOfMarkers; i++)
//            {
//                if(markers[i].cond > 0)
//                {
//                    markerPositions[i] = Vec3f(markers[i].x, markers[i].y, markers[i].z)*AllosphereScalar;
//                    markerNumberOfAbsentFrames[i] = 0;
//                }
//                else
//                {
//                    markerNumberOfAbsentFrames[i]++;
//                }
//            }
//        }
    }
    
    static void *OwlCallback(void*)
    {
        while(1)
        {
            cout << "fetching owl\n";
            master().fetchOwl();
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
        int pthreadErrorCode = pthread_create(&fetchOwlThread, NULL, OwlCallback, NULL);
        if (pthreadErrorCode)
        {
            printf("ERROR; Could't start fetchOwlThread. Return code from pthread_create() is %d\n", pthreadErrorCode);
            exit(-1);
        }
    }
    
    void stopThread()
    {
        owlDone();
        pthread_cancel(fetchOwlThread);
    }
    
    static PhasespaceManager& master(){
        static PhasespaceManager *master = new PhasespaceManager;
        return *master;
    }
    
    Vec3f* getMarkerPositions()
    {
        return master().markerPositions;
    }
    
    void getMarkers(Vec3f* positions, int* frames, int offset, int howMany)
    {
        for(int i = 0;i<howMany;i++)
        {
            positions[i] = markerPositions[i+offset];
            frames[i] = markerNumberOfAbsentFrames[i+offset];
        }
    }
    
private:
    pthread_t fetchOwlThread;
    OWLMarker markers[MARKER_COUNT];
    Vec3f markerPositions[MARKER_COUNT];
    int markerNumberOfAbsentFrames[MARKER_COUNT];
};

class PhasespaceGlove
{
    //holds position information about 8 sequentially numbered markers tracked by PhasespaceManager. Can also draw itself for debugging purposes. 
public:
    PhasespaceGlove(int _markerOffset = 0, float _hue = 0.0) : markerOffset(_markerOffset), hue(_hue)
    {
        for(int i = 0;i<MARKERS_PER_GLOVE;i++)
        {
            markerPositions[i] = Vec3f(0.0, 0.0, 0.0);
            numberOfAbsentFrames[i] = 0;
        }
    }
    
    void setMarkerOffset(int _markerOffset)
    {
        markerOffset = _markerOffset;
    }
    
    void fetchOwl()
    {
        //grabs the position and absent frame information from the master (global) marker manager
        PhasespaceManager::master().getMarkers(markerPositions, numberOfAbsentFrames, markerOffset, MARKERS_PER_GLOVE);
    }
    
    void updateCentroid()
    {
        Vec3f tempCentroid(0.0, 0.0, 0.0);
        for(int i = 0;i<MARKERS_PER_GLOVE;i++)
        {
            tempCentroid += markerPositions[i];
        }
        centroid.update(tempCentroid/(float)MARKERS_PER_GLOVE);
    }
    
    void onDraw(Graphics& g)
    {
        fetchOwl();
        updateCentroid();
        
        g.lineWidth(100.0);
        g.color(Color(HSV(hue, 1.0, 1.0),1.0));
        g.begin(g.POINTS);
        for(int i = 0;i<MARKERS_PER_GLOVE;i++)
        {
            g.vertex(markerPositions[i]);
        }
        g.end();
        
        g.pointSize(100.0);
        g.color(Color(HSV(0.0, 0.0, 1.0),1.0));
		g.begin(g.LINES);
        g.vertex(markerPositions[ePinky]);
        g.vertex(markerPositions[eBackHandNearPinky]);
        g.vertex(markerPositions[eRingFinger]);
        g.end();
        g.begin(g.LINES);
        g.vertex(markerPositions[eMiddleFinger]);
        g.vertex(markerPositions[eBackHandNearThumb]);
        g.vertex(markerPositions[eIndexFinger]);
        g.end();
        g.begin(g.LINES);
        g.vertex(markerPositions[eBackHandNearPinky]);
        g.vertex(markerPositions[eBackHandNearThumb]);
        g.vertex(markerPositions[eThumbBase]);
        g.vertex(markerPositions[eThumbTip]);
        g.end();
    }
    
private:
    int markerOffset;
    Vec3f markerPositions[MARKERS_PER_GLOVE];
    int numberOfAbsentFrames[MARKERS_PER_GLOVE];
    KinematicState centroid;
    float hue;
};

#endif
