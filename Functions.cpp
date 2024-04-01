#include "Functions.h"


#define EXIT_OK 1;
#define EXIT_ERR 0;

unsigned long FLOOR_INTERVAL = 100000;  //might need to tweak
unsigned long STOP_TIME = 2000;         //might need to tweak

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

//============================================= CREATE STOP =================================================
Stop* create_Stop(int floor) {
  // make a pointer to a stop
  Stop* new_Stop = (Stop*)malloc(sizeof(Stop));

  //set the stopFloor to be what is parsed into the function
  new_Stop->p_next = NULL;
  new_Stop->stop = floor;

  return new_Stop;
}

//============================================= INSERT STOP ================================================
int insert_Stop(int floor, Stop** stop_Head) {
  //takes in the head of the list to insert from
  Stop* new_Stop = create_Stop(floor);

  if (new_Stop == NULL) return EXIT_ERR;

  // if stops list is empty, insert a new stop at the beginning
  if ((*stop_Head) == NULL) {
    (*stop_Head) = new_Stop;
    return EXIT_OK;
  }

  //otherwise traverse until you find the last item
  Stop* temp = (*stop_Head);
  if (temp->p_next == NULL) {
    if (temp->stop == new_Stop->stop) {
      return EXIT_OK;
    } else {
      temp->p_next = new_Stop;
      return EXIT_OK;
    }
  }

  while (temp->p_next != NULL) {
    if (temp->stop == new_Stop->stop) {
      //Stop already exists in list
      return EXIT_OK;  //there will only ever be floors 1 to 4 in the elevator
    }
    temp = temp->p_next;
  }

  temp->p_next = new_Stop;
  return EXIT_OK;
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

  //WHENEVER INSERT REQUEST IS CALLED ALSO CALL THE FUNCTION TO INSERT INTO THE IN-ORDER REQUEST QUEUE

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
  /* REMEMBER TO INCREASE THE COUNT OF THE LIST AFTER EACH INSERT
     * MAYBE TAKE IN THE LIST AS A PARAMETER AND INCREASE THE COUNT AFTER EACH INSERT
     */


  /* KEEP COUNT OF LIST */
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


int req_del(int delete_floor, request** request_head, _List* req_List, _List* curr_list, Stop** stop_Head, int direction) {

  int insert_result = 1;
  request* temp;

  if ((*request_head) == NULL) {
    return EXIT_ERR;  //cannot delete from an empty list
  }

  if ((*request_head)->pickUp == delete_floor && (*request_head)->req_dir == direction) {
    while ((*request_head)->pickUp == delete_floor && (*request_head)->req_dir == direction) {


      temp = *request_head;
      //INSERT HERE
      insert_result = insert_curr(&(curr_list->p_head), curr_list, temp->pickUp, temp->dropOff);
      //insert stop

      if (insert_result == 0) {
        return EXIT_ERR;  //elevator is full, exit
      } else {
        insert_Stop(temp->dropOff, stop_Head);
      }

      (*request_head) = temp->next_req;
      free(temp);
      req_List->p_head = temp->next_req;
      (req_List->count) -= 1;

      if ((*request_head) == NULL) {
        //All requests were entered into the current passengers list
        return EXIT_OK;
      }
    }
  }
  temp = (*request_head);
  while (temp->next_req != NULL) {
    if ((temp->next_req->pickUp) == delete_floor && (*request_head)->req_dir == direction) {
      request* new_nextNode = temp->next_req->next_req;
      request* nodeToDelete = temp->next_req;
      //INSERT
      insert_result = insert_curr(&(curr_list->p_head), curr_list, nodeToDelete->pickUp, nodeToDelete->dropOff);

      if (insert_result == 0) {
        return EXIT_ERR;  //exit the function since the elevator is full
      } else {
        insert_Stop(nodeToDelete->dropOff, stop_Head);
      }

      temp->next_req = new_nextNode;
      free(nodeToDelete);
      (req_List->count) -= 1;
    } else {
      temp = temp->next_req;
    }
  }
  return EXIT_OK;
}


//============================================== CURR DEL ======================================================

int curr_del(int delete_floor, request** passenger_Head, _List* curr_list) {


  if ((passenger_Head) == NULL) {
    Serial.println("Cannot deliver, elevator empty");
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

  return EXIT_OK
}

//================================================ STOP DEL ========================================================

int stop_del(int delete_floor, Stop** stop_head) {


  if ((stop_head) == NULL) {
    return EXIT_ERR;
  }
  Stop* temp;
  // if deleting from current passangers list, then passenger has been delivered
  if ((*stop_head)->stop == delete_floor) {
    while ((*stop_head)->stop == delete_floor) {
      temp = *stop_head;
      *stop_head = temp->p_next;

      free(temp);

      if (*stop_head == NULL) {
        //all passengers delivered
        return EXIT_OK;
      }
    }
  }
  temp = *stop_head;
  while (temp->p_next != NULL) {

    if (temp->p_next->stop == delete_floor) {
      Stop* new_nextNode = temp->p_next->p_next;
      Stop* nodeToDelete = temp->p_next;
      temp->p_next = new_nextNode;
      free(nodeToDelete);  //passanger dropped off
    } else {
      temp = temp->p_next;
    }
  }

  return EXIT_OK
}




//===================================== RETURN STOPS ====================================================

//returns the closest stop of elevator depending on direction of travel
int return_Stop(direction curr_dir, Stop* stop_Head) {
  //If moving up, return the smallest stop in the dropOff list
  if (curr_dir == UP) {
    int smallest_Stop = stop_Head->stop;
    Stop* temp = stop_Head->p_next;

    while (temp != NULL) {
      if (temp->stop < smallest_Stop) {
        smallest_Stop = temp->stop;
      }

      temp = temp->p_next;
    }
    return smallest_Stop;
  } else {
    int largest_Stop = stop_Head->stop;
    Stop* temp = stop_Head->p_next;

    while (temp != NULL) {
      if (temp->stop > largest_Stop) {
        largest_Stop = temp->stop;
      }

      temp = temp->p_next;
    }
    return largest_Stop;
  }
  //if moving down, return the largest stop in the dropOff list
}

//===================================== SET ELEVATOR ===============================================
void set_Elev(Elevator* elevator, int desired_Floor, int req_BUTTON, request** req_Head,
              _List* req_List, _List* curr_List, Stop** stop_Head) {

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

      //Check if a request wants to be entered
      if (digitalRead(req_BUTTON) == HIGH) {
        //enter request
      }

      if ((start_Time % FLOOR_INTERVAL) == 0) {
        elevator->current_Floor++;
        //CHECK DROP OFF AND PICKUP LISTS TO SEE IF NEED TO OFFLOAD OR ONBOARD ELEV
        Serial.print("Current Floor: ");
        Serial.println(elevator->current_Floor);
        //check if there's anyone to dropOff
        curr_del(elevator->current_Floor, &(curr_List->p_head), curr_List);

        //check if there's anyone to pickup
        req_del(elevator->current_Floor, req_Head, req_List, curr_List, stop_Head, elevator->curr_dir);
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

      //Check if a request wants to be entered
      if (digitalRead(req_BUTTON) == HIGH) {
        //enter request
      }

      if ((start_Time % FLOOR_INTERVAL) == 0) {
        elevator->current_Floor--;
        //CHECK DROP OFF AND PICKUP LISTS TO SEE IF NEED TO OFFLOAD OR ONBOARD ELEV
        Serial.print("Current Floor: ");
        Serial.println(elevator->current_Floor);

        //check if there's anyone to dropOff
        curr_del(elevator->current_Floor, &(curr_List->p_head), curr_List);

        //check if there's anyone to pickup
        req_del(elevator->current_Floor, req_Head, req_List, curr_List, stop_Head, elevator->curr_dir);
      }

      start_Time++;
    }
  }

  //INCLUDE DELAY TO SIMULATE STATIONARY TIME AFTER STOPPED

  //if elev is empty, but there are still requests

  // if (((curr_List->p_head) == NULL) && (req_Head != NULL)) {
  //   set_Elev(elevator, (*req_Head)->pickUp, req_BUTTON, req_Head, curr_List);
  // }

  //update isIdle
  set_Idle(*req_Head, (curr_List->p_head));
}

//===================================== SET ELEVATOR IDLE ===============================================

void set_Elev_idle(Elevator* elevator, int desired_Floor, int req_BUTTON, request** req_Head, _List* req_List,
                   _List* curr_List, Stop** stop_Head) {

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

      //Check if a request wants to be entered
      if (digitalRead(req_BUTTON) == HIGH) {
        //enter request
      }

      if ((start_Time % FLOOR_INTERVAL) == 0) {
        elevator->current_Floor++;
        //CHECK DROP OFF AND PICKUP LISTS TO SEE IF NEED TO OFFLOAD OR ONBOARD ELEV
        Serial.print("Current Floor: ");
        Serial.println(elevator->current_Floor);
      }

      if (elevator->current_Floor == (*req_Head)->pickUp) {
        //pickup the passenger and call the regular set elevator function from what is returned
        //from return stops
        req_del(elevator->current_Floor, req_Head, req_List, curr_List, stop_Head, (*req_Head)->req_dir);

        Serial.println("Picked up Passenger");
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

      //Check if a request wants to be entered
      if (digitalRead(req_BUTTON) == HIGH) {
        //enter request
      }

      if ((start_Time % FLOOR_INTERVAL) == 0) {
        elevator->current_Floor--;
        Serial.println("Current Floor: ");
        Serial.println(elevator->current_Floor);
      }

      if (elevator->current_Floor == (*req_Head)->pickUp) {
        //pickup the passenger and call the regular set elevator function from what is returned
        //from return stops

        req_del(elevator->current_Floor, req_Head, req_List, curr_List, stop_Head, (*req_Head)->req_dir);
        Serial.print("Picked up Passenger");
      }
      start_Time++;
    }
  }

  isIdle = false;
}
