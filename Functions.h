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

bool set_Idle(request*, request*);

Stop* create_Stop(int);

int insert_Stop(int, Stop**);

request* create_Request(int, int);

int insert_request(request**, _List*, int, int);

int insert_curr(request**, _List*, int, int);

int req_del(int, request**, _List*, _List*, Stop**, int);

int curr_del(int, request**, _List*);

int stop_del(int, Stop**);

int return_Stop(direction, Stop*);

void set_Elev(Elevator*, int, int, request**,
              _List*, _List*, Stop**);

void set_Elev_idle(Elevator*, int, int, request**, _List*,
                   _List*, Stop**);

void request_Menu();

void desired_floor();

void askAU();

void set_Elev_Empty(Elevator*, int, int, request**,
                    _List*, _List*, Stop**);

void au_mode_pick(Elevator*, int, Stop*);

void au_mode_drop(Elevator*, int, Stop*);


#endif