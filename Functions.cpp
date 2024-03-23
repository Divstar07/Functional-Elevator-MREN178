#include "Functions.h"


#define EXIT_OK 0;
#define EXIT_ERR -1;


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

int insert_request(request** request_head, int pickUp, int dropOff) {

  /* REMEMBER TO INCREASE THE COUNT OF THE LIST AFTER EACH INSERT
   * MAYBE TAKE IN THE LIST AS A PARAMETER AND INCREASE THE COUNT AFTER EACH INSERT
   */

  request* new_request = create_Request(pickUp, dropOff);

  /* INSERT SHOULD FAIL IF INSERTING INTO CURRENT PASSANGER LIST AND LIST IS FULL
   * MAYBE CREATE A BOOL TO SIGNIFY IF INSERTING INTO CURR PASSENGER LIST, AND IF IT IS FULL RETURN EXIT_ERR
   * OR CREATE ANOTHER INSERT FUNCTION SPECIFICALLY FOR CURR PASSENGER LIST (MIGHT BE EASIER)
   */


  if ((*request_head) == NULL) {
    (*request_head) = new_request;
    return EXIT_OK;
  }

  else {
    request* temp = (*request_head);

    //traverse till last request in list
    while (temp->next_req != NULL) {
      temp = temp->next_req;
    }

    temp->next_req = new_request;
  }
  return EXIT_OK;
}


//=====================================DELETE REQUEST========================================================

int delete_request(int delete_floor, request** request_head, request** passenger_Head, bool del_From_pickUp) {
  /* request_head is the head of the request list
 * passenger_Head is the head of the current passengers list
 * REMEMBER THESE ARE DOUBLE POINTERS
 */

  if (del_From_pickUp) {
    if ((*request_head) == NULL) {
      return EXIT_ERR;
    }
    /* delete items from the list of requests and add them to the current passangers list
          * IF IT IS NOT FULL. add them one by one until it is full, then stop adding
          */

    //"floor" should be replaced with pickup when deleting from pickup list

    /* if (request_head->floor == delete_floor){
            while (request_head->floor == delete_floor) {
                request *temp = request_head;
                request_head = temp->next_req;
                free(temp);
                if (request_head == NULL){
                    return EXIT_OK;
                }
            }

        request* temp = request_head;
        while(temp->next_req != NULL){
            if (temp->next_req->floor == delete_floor){
                request* temp2 = temp->next_req->next_req;
                request* temp3 = temp->next_req;
                temp->next_req = temp2;
                free(temp3);
            } else{
                temp = temp->next_req;
            }
        }
        return EXIT_OK;
        */
  }


  else {
    // if deleting from current passangers list, then passenger has been delivered
  }
}