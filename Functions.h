#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <Arduino.h>

// Structure of a request
typedef struct req {
  int curr_Floor;  //store current floor
  int pickUp;      //store pickup floor
  int dropOff;     //store dropOff floor
  int direction;   //store direction

  struct* req next_req;

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


// Structure of a stop list
typedef struct {
  Stop* head;

} stop_List;


//==========================FUNCTION PROTOTYPES=====================

//Create a request
void create_Request();

//insert a request into the right list
void insert_Request();

//create a stop struct
void create_Stop();

//insert a stop into the right list
void insert_Stop();

//returns the next stop of elevator depending on direction of travel
int return_Stop();

//checks if the elevator is idle
void check_Idle();

//sets the elevator to a floor and stops it for a while
void set_Elev();




#endif