#include "Functions.h"


#define EXIT_OK 1;
#define EXIT_ERR 0;

unsigned long FLOOR_INTERVAL = 100000;  //might need to tweak
unsigned long STOP_TIME = 2000;         //might need to tweak



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
  }

  //otherwise traverse until you find the last item
  Stop* temp = (*stop_Head);

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

int insert_request(request** request_head, _List* req_List, int pickUp, int dropOff, Stop** pickUp_Head) {

  request* new_request = create_Request(pickUp, dropOff);
  if (new_request == NULL) return EXIT_ERR;

  //WHENEVER INSERT REQUEST IS CALLED ALSO CALL THE FUNCTION TO INSERT INTO THE IN-ORDER REQUEST QUEUE

  if ((*request_head) == NULL) {
    (*request_head) = new_request;

    req_List->count++;
    Serial.print("Number of unpicked passengers:");
    Serial.println(req_List->count);  //print out number of requests still in list

    //insert the stop of the request into the pickup stops list
    insert_Stop(new_request->pickUp, pickUp_Head);

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

    insert_Stop(new_request->pickUp, pickUp_Head);  //insert into stops to pickup
  }
  return EXIT_OK;
}

//==================================== INSERT CURR ==========================================================

int insert_curr(request** curr_Head, _List* curr_list, int pickUp, int dropOff, Stop** dropOff_Head) {
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

    //insert into stops to drop off
    insert_Stop(new_Passenger->dropOff, dropOff_Head);

    return EXIT_OK;
  } else {
    request* temp = (*curr_Head);
    //traverse till last request in list
    while (temp->next_req != NULL) {
      temp = temp->next_req;
    }
    temp->next_req = new_Passenger;
    insert_Stop(new_Passenger->dropOff, dropOff_Head);
  }
  return EXIT_OK;
}

//===================================== REQ DEL ========================================================


int req_del(int delete_floor, request** request_head, _List* curr_list) {
  /* request_head is the head of the request list
   * passenger_Head is the head of the current passengers list
   * REMEMBER THESE ARE DOUBLE POINTERS
   */

  //REMEMBER FAILURE CONDITIONS. PRINT WHEN A PICKUP WAS MADE
  //UPDATE COUNT OF LIST

  int insert_result = 1;
  request* temp;

  if ((*request_head) == NULL) {
    return EXIT_ERR;  //cannot delete from an empty list
  }

  /* delete items from the list of requests and add them to the current passangers list
   * IF IT IS NOT FULL. add them one by one until it is full, then stop adding
   */

  if ((*request_head)->pickUp == delete_floor) {
    while ((*request_head)->pickUp == delete_floor) {


      temp = *request_head;
      //INSERT HERE
      insert_result = insert_curr(&(curr_list->p_head), curr_list, temp->pickUp, temp->dropOff);

      if (insert_result == 0) {
        return EXIT_ERR;  //elevator is full, exit
      }

      *request_head = temp->next_req;
      free(temp);
      if (request_head == NULL) {
        //All requests were entered into the current passengers list
        return EXIT_OK;
      }
    }
  }
  temp = (*request_head);
  while (temp->next_req != NULL) {
    if ((temp->next_req->pickUp) == delete_floor) {
      request* new_nextNode = temp->next_req->next_req;
      request* nodeToDelete = temp->next_req;
      //INSERT
      insert_result = insert_curr(&(curr_list->p_head), curr_list, nodeToDelete->pickUp, nodeToDelete->dropOff);

      if (insert_result == 0) {
        return EXIT_ERR;  //exit the function since the elevator is full
      }

      temp->next_req = new_nextNode;
      free(nodeToDelete);
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
    } else {
      temp = temp->next_req;
    }
  }

  return EXIT_OK
}

//==================================== SEARCH STOPS ===============================================
bool search_Floor(Stop* stop_Head, int cmp_Floor) {
  /* At each floor, search the stops to pick up list to see if there is any passanger to add to the list
   * and search the stops to drop off for the same reason
   * return true if a stop is found
   */
  if (stop_Head == NULL) return false;

  Stop* temp = stop_Head;
  //traverse stop list, and if a stop matching cmp_Floor is found, return true
  while (temp->p_next != NULL) {

    if (temp->stop == cmp_Floor) {
      return true;
    }
    temp = temp->p_next;
  }

  //if code reaches here, then a floor was not found
  return false;
}

//===================================== RETURN STOPS ==============================================
//returns the closest stop of elevator depending on direction of travel
int return_Stop(direction curr_dir, Stop* stop_Head) {
  //If moving up, return the smallest stop in the dropOff list
  //if moving down, return the largest stop in the dropOff list
}


//===================================== SET ELEVATOR ===============================================
void set_Elev(Elevator* elevator, int desired_Floor, int req_BUTTON, _List req_List, _List curr_Passenger) {

  //CHECK FOR FLOOR LIMITS IF THE ELEV IS ON 4 DONT GO UP IF ITS ON 1 DONT GO DOWN

  /* HAVE PARAM TO INDICATE IF IDLE AND SET DIRECTLY WITHOUT CHECKING EACH FLOOR 
   * STOP AT REQ FLOOR AND PICK UP PASSANGER, THEN CONTINUE TO CHECK EACH FLOOR TO SEE
   * IF ANYONE NEEDS TO GET ON OR OFF
   */


  /* IN THE CHECK FOR GOING DOWN OR UP, SEARCH THE UP LIST WHEN GOING UP, OR DOWN LIST WHEN GOING DOWN */


  //ADD AN IF STATEMENT TO CHECK CURRENT FLOOR ON THE FIRST FLOOR BECAUSE I'M CHANGING THE LOOP

  int distance = abs(desired_Floor - elevator->current_Floor);
  unsigned long start_Time = 1;  //store the time the elevator starts moving

  //if desired_Floor > currFloor elevator moves up
  if (desired_Floor > elevator->current_Floor) {
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

      start_Time++;
    }

    // INCLUDE DELAY TO SIMULATE STATIONARY TIME AFTER STOPPED

  } else if (desired_Floor < elevator->current_Floor) {
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
      }

      start_Time++;
    }
  }

  // INCLUDE DELAY TO SIMULATE STATIONARY TIME AFTER STOPPED

  if ()
}