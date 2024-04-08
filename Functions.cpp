#include "Arduino.h"
#include "Functions.h"


#define EXIT_OK 1;
#define EXIT_ERR 0;

unsigned long FLOOR_INTERVAL = 30000;  //might need to tweak

extern bool isIdle;


//============================================= SET IDLE ====================================================
bool set_Idle(request* req_Head, request* curr_Head) {
  if ((req_Head == NULL) && (curr_Head == NULL)) {
    isIdle = true;
    return true;
  }

  else {
    isIdle = false;
    return false;
  }
}

//===============================CREATE REQUEST=====================================================
request* create_Request(int pickUp, int dropOff) {

  request* new_request = (request*)malloc(sizeof(request));
  new_request->next_req = NULL;
  new_request->pickUp = pickUp;
  new_request->dropOff = dropOff;

  //set the direction of the request
  if (pickUp > dropOff) {
    new_request->req_dir = DOWN;
  } else {
    new_request->req_dir = UP;
  }

  return new_request;
}

//===============================INSERT REQUEST======================================================

int insert_request(request** request_head, _List* req_List, int pickUp, int dropOff) {

  request* new_request = create_Request(pickUp, dropOff);
  if (new_request == NULL) return EXIT_ERR;

  //insert at head
  if ((*request_head) == NULL) {
    (*request_head) = new_request;
    req_List->p_head = new_request;
    req_List->count++;
    Serial.print("Number of unpicked passengers:");
    Serial.println(req_List->count);  //print out number of requests still in list

    return EXIT_OK;
  } else {
    request* temp = (*request_head);
    //traverse till last request in list
    while (temp->next_req != NULL) {
      temp = temp->next_req;
    }

    temp->next_req = new_request;
    req_List->count++;
    Serial.print("Number of unpicked passengers:");
    Serial.println(req_List->count);
  }
  return EXIT_OK;
}
//==================================== INSERT CURR ==========================================================

int insert_curr(request** curr_Head, _List* curr_list, int pickUp, int dropOff) {

  request* new_Passenger = create_Request(pickUp, dropOff);
  if (new_Passenger == NULL) {
    Serial.println("Error creating new request. Source: insert_curr");
    return EXIT_ERR;
  }

  if (curr_list->count == 10) {
    Serial.println("Elevator is full");
    return EXIT_ERR;
  }
  if ((*curr_Head) == NULL) {
    (*curr_Head) = new_Passenger;
    curr_list->p_head = new_Passenger;
    (curr_list->count) += 1;
    return EXIT_OK;
  } else {
    request* temp = (*curr_Head);
    //traverse till last request in list
    while (temp->next_req != NULL) {
      temp = temp->next_req;
    }
    temp->next_req = new_Passenger;
    (curr_list->count) += 1;
  }
  return EXIT_OK;
}
//===================================== REQ DEL ========================================================


int req_del(int delete_floor, request** request_head, _List* req_List, _List* curr_list, int direction) {

  int insert_result = 1;
  request* temp;

  if ((*request_head) == NULL) {
    return EXIT_ERR;  //cannot delete from an empty list
  }

  //delete from head case
  if ((*request_head)->pickUp == delete_floor && (*request_head)->req_dir == direction) {
    while ((*request_head)->pickUp == delete_floor && (*request_head)->req_dir == direction) {


      temp = *request_head;
      //insert into current passengers list
      insert_result = insert_curr(&(curr_list->p_head), curr_list, temp->pickUp, temp->dropOff);

      if (insert_result == 0) {
        return EXIT_ERR;  //elevator is full, exit
      }

      (*request_head) = temp->next_req;
      free(temp);
      req_List->p_head = temp->next_req;
      (req_List->count) -= 1;
      Serial.println("Picked up Passenger");

      if ((*request_head) == NULL) {
        //All requests were entered into the current passengers list
        return EXIT_OK;
      }
    }
  }

  //delete from middle or end
  temp = (*request_head);
  while (temp->next_req != NULL) {
    if ((temp->next_req->pickUp) == delete_floor && (*request_head)->req_dir == direction) {
      request* new_nextNode = temp->next_req->next_req;
      request* nodeToDelete = temp->next_req;

      //insert into current passengers list
      insert_result = insert_curr(&(curr_list->p_head), curr_list, nodeToDelete->pickUp, nodeToDelete->dropOff);

      if (insert_result == 0) {
        return EXIT_ERR;  //exit the function since the elevator is full
      }
      temp->next_req = new_nextNode;
      free(nodeToDelete);

      Serial.println("Picked up Passenger");
      (req_List->count) -= 1;
    } else {
      temp = temp->next_req;
    }
  }
  return EXIT_OK;
}


//============================================== CURR DEL ======================================================

int curr_del(int delete_floor, request** passenger_Head, _List* curr_list) {


  if ((*passenger_Head) == NULL) {
    return EXIT_ERR;
  }
  request* temp;
  // if deleting from current passangers list, then passenger has been delivered
  if ((*passenger_Head)->dropOff == delete_floor) {
    while ((*passenger_Head)->dropOff == delete_floor) {
      temp = *passenger_Head;
      *passenger_Head = temp->next_req;

      free(temp);
      curr_list->count--;
      Serial.println("Delivered Passanger");
      Serial.print("Current number of passengers: ");
      Serial.println(curr_list->count);

      if (*passenger_Head == NULL) {
        //all passengers delivered
        return EXIT_OK;
      }
    }
  }
  temp = *passenger_Head;
  while (temp->next_req != NULL) {

    if (temp->next_req->dropOff == delete_floor) {
      request* new_nextNode = temp->next_req->next_req;
      request* nodeToDelete = temp->next_req;
      temp->next_req = new_nextNode;
      free(nodeToDelete);  //passanger dropped off
      curr_list->count--;  //see if there is anywhere else this has to be donr

      Serial.println("Delivered Passanger");
      Serial.print("Current number of passengers: ");
      Serial.println(curr_list->count);

    } else {
      temp = temp->next_req;
    }
  }

  return EXIT_OK;
}

//===================================== SET ELEVATOR ===============================================
void set_Elev(Elevator* elevator, int desired_Floor, int req_BUTTON, request** req_Head,
              _List* req_List, _List* curr_List) {

  askAU();

  if (elevator->current_Floor == desired_Floor) {
    //pickup the passenger and call the regular set elevator function from what is returned
    //from return stops
    curr_del(elevator->current_Floor, &(curr_List->p_head), curr_List);

    //check if there's anyone to pickup
    req_del(elevator->current_Floor, req_Head, req_List, curr_List, elevator->curr_dir);
    return;
  }

  int distance = abs(desired_Floor - elevator->current_Floor);
  unsigned long start_Time = 1;  //store the time the elevator starts moving

  //if desired_Floor > currFloor elevator moves up
  if ((desired_Floor > elevator->current_Floor)) {
    elevator->curr_dir = UP;
    Serial.println("Going Up!");

    Serial.print("Current Floor: ");
    Serial.println(elevator->current_Floor);

    while (start_Time <= (distance * FLOOR_INTERVAL)) {

      //Check if a request wants to be entered
      if (analogRead(req_BUTTON) < 60) {  //They pressed "RIGHT"
        //request_Menu(); //enter request mode on the
        Serial.println("User entered request");
        //Serial.println(analogRead(req_BUTTON));
        askAU();
        while (analogRead(req_BUTTON) < 60)
          ;
        askAU();
      } else if (analogRead(req_BUTTON) < 600) {  // That means they picked yes
        Serial.println("Engaging AUM");
        //Serial.println(analogRead(req_BUTTON));

        aum();  // Begin the Authorized User Mode Function
      }

      if ((start_Time % FLOOR_INTERVAL) == 0) {
        elevator->current_Floor++;
        //CHECK DROP OFF AND PICKUP LISTS TO SEE IF NEED TO OFFLOAD OR ONBOARD ELEV
        Serial.print("Current Floor: ");
        Serial.println(elevator->current_Floor);
        //check if there's anyone to dropOff
        curr_del(elevator->current_Floor, &(curr_List->p_head), curr_List);

        //check if there's anyone to pickup
        req_del(elevator->current_Floor, req_Head, req_List, curr_List, elevator->curr_dir);
      }
      start_Time++;
    }
  }

  else if ((desired_Floor < elevator->current_Floor)) {
    Serial.println("Going Down!");
    elevator->curr_dir = DOWN;

    Serial.print("Current Floor: ");
    Serial.println(elevator->current_Floor);

    while (start_Time <= (distance * FLOOR_INTERVAL)) {

      //Check if a request wants to be entered
      if (analogRead(req_BUTTON) < 60) {  //They pressed "RIGHT"
        Serial.println("User entered request");
        Serial.println(analogRead(req_BUTTON));

        askAU();
        while (analogRead(req_BUTTON) < 60)
          ;
        askAU();
      } else if (analogRead(req_BUTTON) < 600) {  // That means they picked yes
        Serial.println("Engaging AUM");
        Serial.println(analogRead(req_BUTTON));

        aum();  // Begin the Authorized User Mode Function
      }

      if ((start_Time % FLOOR_INTERVAL) == 0) {
        elevator->current_Floor--;
        //CHECK DROP OFF AND PICKUP LISTS TO SEE IF NEED TO OFFLOAD OR ONBOARD ELEV
        Serial.print("Current Floor: ");
        Serial.println(elevator->current_Floor);

        //check if there's anyone to dropOff
        curr_del(elevator->current_Floor, &(curr_List->p_head), curr_List);

        //check if there's anyone to pickup
        req_del(elevator->current_Floor, req_Head, req_List, curr_List, elevator->curr_dir);
      }
      start_Time++;
    }
  }
  set_Idle(*req_Head, (curr_List->p_head));
}


//===================================== SET ELEVATOR IDLE ===============================================

void set_Elev_idle(Elevator* elevator, int desired_Floor, int req_BUTTON, request** req_Head, _List* req_List,
                   _List* curr_List) {

  askAU();

  if (elevator->current_Floor == desired_Floor) {
    //pickup the passenger and call the regular set elevator function from what is returned
    //from return stops
    req_del(elevator->current_Floor, req_Head, req_List, curr_List, (*req_Head)->req_dir);

    //Serial.println("Picked up Passenger");
    isIdle = false;
    return;
  }


  int distance = abs(desired_Floor - elevator->current_Floor);
  unsigned long start_Time = 1;  //store the time the elevator starts moving

  //if desired_Floor > currFloor elevator moves up
  if ((desired_Floor > elevator->current_Floor)) {
    elevator->curr_dir = UP;

    Serial.println("Going Up!");

    Serial.print("Current Floor: ");
    Serial.println(elevator->current_Floor);


    while (start_Time <= (distance * FLOOR_INTERVAL)) {

      //Check if a request wants to be entered
      if (analogRead(req_BUTTON) < 60) {  //They pressed "RIGHT"
        Serial.println("User entered request");
        askAU();
        while (analogRead(req_BUTTON) < 60)
          ;
        askAU();
      } else if (analogRead(req_BUTTON) < 600) {  // That means they picked yes
        Serial.println("Engaging AUM");

        aum();  // Begin the Authorized User Mode Function
      }

      if ((start_Time % FLOOR_INTERVAL) == 0) {
        elevator->current_Floor++;
        Serial.print("Current Floor: ");
        Serial.println(elevator->current_Floor);
      }
      if (elevator->current_Floor == desired_Floor) {
        //pickup the passenger and call the regular set elevator function
        req_del(elevator->current_Floor, req_Head, req_List, curr_List, (*req_Head)->req_dir);
      }

      start_Time++;
    }

  } else if ((desired_Floor < elevator->current_Floor)) {
    Serial.println("Going Down!");
    elevator->curr_dir = DOWN;

    Serial.print("Current Floor: ");
    Serial.println(elevator->current_Floor);

    while (start_Time <= (distance * FLOOR_INTERVAL)) {

      //Check if a request wants to be entered
      if (analogRead(req_BUTTON) < 60) {  //They pressed "RIGHT"
        Serial.println("User entered request");
        askAU();
        while (analogRead(req_BUTTON) < 60)
          ;
        askAU();
      } else if (analogRead(req_BUTTON) < 600) {  // That means they picked yes
        Serial.println("Engaging AUM");
        aum();  // Begin the Authorized User Mode Function
      }

      if ((start_Time % FLOOR_INTERVAL) == 0) {
        elevator->current_Floor--;
        Serial.print("Current Floor: ");
        Serial.println(elevator->current_Floor);
      }
      if (elevator->current_Floor == desired_Floor) {
        //pickup the passenger and call the regular set elevator function
        req_del(elevator->current_Floor, req_Head, req_List, curr_List, (*req_Head)->req_dir);
      }
      start_Time++;
    }
  }

  isIdle = false;
}

//=============================================== AU MODE PICK ===================================================================

void au_mode_pick(Elevator* elevator, int desired_Floor) {
  if (elevator->current_Floor == desired_Floor) {
    Serial.println("Picked up AU");
    return;
  }


  int distance = abs(desired_Floor - elevator->current_Floor);
  unsigned long start_Time = 1;  //store the time the elevator starts moving

  //ADD AN IF STATEMENT TO CHECK CURRENT FLOOR ON THE FIRST FLOOR BECAUSE I'M CHANGING THE LOOP

  //if desired_Floor > currFloor elevator moves up
  if ((desired_Floor > elevator->current_Floor)) {
    elevator->curr_dir = UP;

    Serial.println("Going Up!");

    Serial.print("Current Floor: ");
    Serial.println(elevator->current_Floor);


    while (start_Time <= (distance * FLOOR_INTERVAL)) {


      if ((start_Time % FLOOR_INTERVAL) == 0) {
        elevator->current_Floor++;
        //CHECK DROP OFF AND PICKUP LISTS TO SEE IF NEED TO OFFLOAD OR ONBOARD ELEV
        Serial.print("Current Floor: ");
        Serial.println(elevator->current_Floor);
      }
      if (elevator->current_Floor == desired_Floor) {

        Serial.println("Picked up AU");
      }

      start_Time++;
    }

  } else if ((desired_Floor < elevator->current_Floor)) {
    Serial.println("Going Down!");
    elevator->curr_dir = DOWN;

    Serial.print("Current Floor: ");
    Serial.println(elevator->current_Floor);

    while (start_Time <= (distance * FLOOR_INTERVAL)) {


      if ((start_Time % FLOOR_INTERVAL) == 0) {
        elevator->current_Floor--;
        Serial.print("Current Floor: ");
        Serial.println(elevator->current_Floor);
      }
      if (elevator->current_Floor == desired_Floor) {
        Serial.println("Picked up AU");
      }
      start_Time++;
    }
  }
}

//========================================= AU MODE DROP ======================================================================

void au_mode_drop(Elevator* elevator, int desired_Floor) {
  if (elevator->current_Floor == desired_Floor) {
    Serial.println("Dropped off AU");
    return;
  }


  int distance = abs(desired_Floor - elevator->current_Floor);
  unsigned long start_Time = 1;  //store the time the elevator starts moving

  //ADD AN IF STATEMENT TO CHECK CURRENT FLOOR ON THE FIRST FLOOR BECAUSE I'M CHANGING THE LOOP

  //if desired_Floor > currFloor elevator moves up
  if ((desired_Floor > elevator->current_Floor)) {
    elevator->curr_dir = UP;

    Serial.println("Going Up!");

    Serial.print("Current Floor: ");
    Serial.println(elevator->current_Floor);


    while (start_Time <= (distance * FLOOR_INTERVAL)) {


      if ((start_Time % FLOOR_INTERVAL) == 0) {
        elevator->current_Floor++;
        //CHECK DROP OFF AND PICKUP LISTS TO SEE IF NEED TO OFFLOAD OR ONBOARD ELEV
        Serial.print("Current Floor: ");
        Serial.println(elevator->current_Floor);
      }
      if (elevator->current_Floor == desired_Floor) {

        Serial.println("Dropped off AU");
      }

      start_Time++;
    }

    // INCLUDE DELAY TO SIMULATE STATIONARY TIME AFTER STOPPED

  } else if ((desired_Floor < elevator->current_Floor)) {
    Serial.println("Going Down!");
    elevator->curr_dir = DOWN;

    Serial.print("Current Floor: ");
    Serial.println(elevator->current_Floor);

    while (start_Time <= (distance * FLOOR_INTERVAL)) {


      if ((start_Time % FLOOR_INTERVAL) == 0) {
        elevator->current_Floor--;
        //CHECK DROP OFF AND PICKUP LISTS TO SEE IF NEED TO OFFLOAD OR ONBOARD ELEV
        Serial.print("Current Floor: ");
        Serial.println(elevator->current_Floor);
      }
      if (elevator->current_Floor == desired_Floor) {
        Serial.println("Dropped off AU");
      }
      start_Time++;
    }
  }
}

//================================== SET ELEV AU ======================================================================
void set_Elev_AU(Elevator* elevator, int desired_Floor, request** req_Head,
                 _List* curr_List) {

  if (elevator->current_Floor == desired_Floor) {
    //pickup the passenger and call the regular set elevator function from what is returned
    //from return stops
    curr_del(elevator->current_Floor, &(curr_List->p_head), curr_List);
    return;
  }

  int distance = abs(desired_Floor - elevator->current_Floor);
  unsigned long start_Time = 1;  //store the time the elevator starts moving

  //if desired_Floor > currFloor elevator moves up
  if ((desired_Floor > elevator->current_Floor)) {
    elevator->curr_dir = UP;
    Serial.println("Going Up!");

    Serial.print("Current Floor: ");
    Serial.println(elevator->current_Floor);

    while (start_Time <= (distance * FLOOR_INTERVAL)) {

      if ((start_Time % FLOOR_INTERVAL) == 0) {
        elevator->current_Floor++;
        //CHECK DROP OFF AND PICKUP LISTS TO SEE IF NEED TO OFFLOAD OR ONBOARD ELEV
        Serial.print("Current Floor: ");
        Serial.println(elevator->current_Floor);
        //check if there's anyone to dropOff
        curr_del(elevator->current_Floor, &(curr_List->p_head), curr_List);
      }
      start_Time++;
    }
  }

  else if ((desired_Floor < elevator->current_Floor)) {
    Serial.println("Going Down!");
    elevator->curr_dir = DOWN;

    Serial.print("Current Floor: ");
    Serial.println(elevator->current_Floor);

    while (start_Time <= (distance * FLOOR_INTERVAL)) {

      if ((start_Time % FLOOR_INTERVAL) == 0) {
        elevator->current_Floor--;
        //CHECK DROP OFF AND PICKUP LISTS TO SEE IF NEED TO OFFLOAD OR ONBOARD ELEV
        Serial.print("Current Floor: ");
        Serial.println(elevator->current_Floor);

        //check if there's anyone to dropOff
        curr_del(elevator->current_Floor, &(curr_List->p_head), curr_List);
      }
      start_Time++;
    }
  }
  set_Idle(*req_Head, (curr_List->p_head));
}
