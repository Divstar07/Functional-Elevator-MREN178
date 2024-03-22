#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

//============================================= FUNCTIONS ================================================

/* STORE STOPS: Should return the closest stop to the elevator's current position
 * May need two iterations of this function for when the elevator is moving down or up
 */

// PUSH: Should Push a request to the end of the current list


/* When elevator is moving between floors, and the FLOORTIME or any multiple of it is reached, increase the
 * counter
 */


/* HAVE AN ARRAY TO STORE CURRENT PASSENGERS: At each stop, check whether in the current passangers array
 * there is a passenger to be dropped off, and check whether there is a request to be fulfilled or someone wants
 * to enter */

/* LISTS SHOULD ONLY STORE REQUESTS THAT HAVE NOT BEEN PICKED UP. Once a request it picked up, remove it from the list
 * and add it to the current passengers.
 */



/* ONLY SEARCH THE LISTS AND THE CURRENT PASSENGERS ARRAY WITH THE STOPS TO LEAVE THE ELEVATOR. SO WHEN THE ELEVEATOR
 * IS STOPPED, SEARCH WHETHER THERE IS A PASSENGER THAT WANTS TO LEAVE, OR ENTER. AFTER EACH STOP, REMOVE THE STOP
 * FROM BOTH THE DROP STOPS AND THE PICKUP STOPS ARRAYS (that way if there is any element left in the pickup stops
 * array the elevator can proceed to it if it is idle).
 *
 * THE ONLY WAY WE PRIORITIZE PICKING UP A PASSENGER IS IF THE CURRENT PASSENGERS ARRAY IS EMPTY
 */


/* NEED A setElevator FUNCTION THAT MOVES THE ELEVATOR TO A CERTAIN FLOOR, KEEPING TRACK OF THE CURRENT FLOOR
 * AND STOPS THE ELEVATOR AT THE APPROPRIATE FLOOR (PARSED IN AS A PARAMETER) FOR A FIXED AMOUNT OF TIME
 */


/* STORE THE NUMBER OF UNFULFILLED REQUESTS BY KEEPING TRACK OF THE SIZE OF THE LIST AND PRINT TO STATE MONITOR
 * PRINT NUMBER OF CURRENT PASSENGERS IN ELEVATOR


//============================================= MAIN PROGRAM =============================================

//define the request struct: currFloor, Stop, Direction

int main() {

    //Create 3 Lists, for up, down and miscellaneous
    //Declare the STOPS array

    //CHECK IDLE: if the elevator is idle, then fulfill a request without stopping

    // When request is entered calculate the direction and find the stop of the request
    // getRequest() should create a request and store the Stop, current floor, and direction data. Returns a request

    // PUSH the request onto the relevant list depending on the direction.

    // STORE the current floor of the request
    // STORE the stop of the request into the STOPS array.

}
