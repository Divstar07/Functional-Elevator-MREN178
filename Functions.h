#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <Arduino.h>

enum direction { UP,
                 DOWN };


// Structure of a request
typedef struct req {
  int pickUp;         //store pickup floor
  int dropOff;        //store dropOff floor
  direction req_dir;  //store direction

  struct req* next_req;

} request;


// Structure of a request list, also used for current passangers
typedef struct {
  request* p_head;
  int count;

} _List;


// Structure of a stop
typedef struct stopFloor {
  int stop;
  struct stopFloor* p_next;
} Stop;


// Structure of a stop list, will have max 4 stops ever
typedef struct {
  Stop* head;  // initialize head as NULL when you initialize the list

} stop_List;

// Structure of elevator
typedef struct {
  int current_Floor;
  direction curr_dir;
} Elevator;


//==========================FUNCTION PROTOTYPES=====================

//Create a request
request* create_Request(int, int);

//insert a request into the right list
int insert_request(request**, _List*, int, int);

  //insert into current passengers list
  int insert_curr(request**, _List*, int, int);

  //delete a request from the right list
  int req_del(int, request**, _List*);

  //delete fromt current passangers list
  int curr_del(int, request**, _List*);

  //create a stop struct
  Stop* create_Stop();

/* insert a stop into the right list. If the stop already exists in the list, do not insert;
 * returning smallest stop is faster this way
 */


  int req_del(int, request**, _List*);

  int insert_Stop(int, Stop**);

  //returns the next stop of elevator depending on direction of travel
  int return_Stop();

//checks if the elevator is idle
void check_Idle();

//sets the elevator to a floor and stops it for a while
void set_Elev(Elevator*, int, int);

//checks if there is anyone to pickup or drop off on the floor parsed in
bool search_Floor(Stop*, int);



#endif