// karl yerkes 2011.05.05
//
//
#include <stdio.h>
#include <unistd.h>
#include "owl.h"
#include "lo/lo.h"
#define MARKER_COUNT 180
// #define SERVER_NAME "192.168.2.2"
#define SERVER_NAME "192.168.0.127"
#define CLIENT_NAME "127.0.0.1" // broadcast address
//#define CLIENT_NAME "255.255.255.255" // broadcast address
#define CLIENT_PORT "9999"
#define INIT_FLAGS 0

void owl_print_error(const char *s, int n);

int main() {
  OWLMarker markers[MARKER_COUNT];
  int tracker;
  lo_address a = lo_address_new(CLIENT_NAME, CLIENT_PORT);
  lo_bundle b;
  lo_message m;
  lo_timetag now;

	if (owlInit(SERVER_NAME, INIT_FLAGS) < 0) {
		owl_print_error("Couldn't initialize: is another OWL client running?\n", owlGetError());
		return 0;
	}
	
  // create tracker 0
  tracker = 0;
  owlTrackeri(tracker, OWL_CREATE, OWL_POINT_TRACKER);

  // set markers
  for(int i = 0; i < MARKER_COUNT; i++)
    owlMarkeri(MARKER(tracker, i), OWL_SET_LED, i);

  // activate tracker
  owlTracker(tracker, OWL_ENABLE);

  // flush requests and check for errors
  if(!owlGetStatus()) {
    owl_print_error("error in point tracker setup", owlGetError());
    return 0;
  }

  // set default frequency
  owlSetFloat(OWL_FREQUENCY, 120);  //OWL_MAX_FREQUENCY (this is 480)
  //owlSetFloat(OWL_FREQUENCY, OWL_MAX_FREQUENCY);  //OWL_MAX_FREQUENCY (this is 480)
  
  // start streaming
  owlSetInteger(OWL_STREAMING, OWL_ENABLE);

  // main loop
  while(1) {
    int err;

    // get some markers
    int n = owlGetMarkers(markers, MARKER_COUNT);

    // check for error
    if((err = owlGetError()) != OWL_NO_ERROR) {
      owl_print_error("error", err);
      break;
    }

    // no data yet
    if(n == 0) {
      usleep(800);
      continue;
    }

    if(n > 0) {
      bool gotAtLeastOne = false;
      for(int i = 0; i < n; i++)
        if(markers[i].cond > 0) {
          gotAtLeastOne = true;
          break;
        }

      if (gotAtLeastOne) {
        lo_timetag_now(&now);
        b = lo_bundle_new(now);
        for(int i = 0; i < n; i++)
          if(markers[i].cond > 0) {
            m = lo_message_new();
            lo_message_add_int32(m, i);
            lo_message_add_float(m, markers[i].x);
            lo_message_add_float(m, markers[i].y);
            lo_message_add_float(m, markers[i].z);
            lo_bundle_add_message(b, "/marker", m);
          }
          lo_send_bundle(a, b);
          lo_bundle_free_messages(b);
      }
    }
  }
  
  // cleanup
  owlDone();
}

void owl_print_error(const char *s, int n) {
  if(n < 0) printf("%s: %d\n", s, n);
  else if(n == OWL_NO_ERROR) printf("%s: No Error\n", s);
  else if(n == OWL_INVALID_VALUE) printf("%s: Invalid Value\n", s);
  else if(n == OWL_INVALID_ENUM) printf("%s: Invalid Enum\n", s);
  else if(n == OWL_INVALID_OPERATION) printf("%s: Invalid Operation\n", s);
  else printf("%s: 0x%x\n", s, n);
}
