
#include <stdint.h>
#include "queue.h"
#include <stdio.h>

#define FILTER_SAMPLE_FREQUENCY_IN_KHZ 100
#define FILTER_FREQUENCY_COUNT 10
#define FILTER_FIR_DECIMATION_FACTOR 10  // FIR-filter needs this many new inputs to compute a new output.
#define FILTER_INPUT_PULSE_WIDTH 2000    // This is the width of the pulse you are looking for, in terms of decimated sample count.

#define IIR_A_COEFFICIENT_COUNT  10//check this!!!
#define FILTER_IIR_FILTER_COUNT  10

#define XQ_SIZE  81
#define YQ_SIZE  11//CHECK This!!!
#define ZQ_SIZE  IIR_A_COEFFICIENT_COUNT//CHECK THIS!!!
#define OUTQ_SIZE  2000//CHECK THIS!!!


static double currentPowerValue[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
double oldest_value = 0;
double newest_value = 0;
double sum = 0;
double prev_power = 0;
// These are the tick counts that are used to generate the user frequencies.
// Not used in filter.h but are used to TEST the filter code.
// Placed here for general access as they are essentially constant throughout

// Filtering routines for the laser-tag project.
// Filtering is performed by a two-stage filter, as described below.

// 1. First filter is a decimating FIR filter with a configurable number of taps and decimation factor.
// 2. The output from the decimating FIR filter is passed through a bank of 10 IIR filters. The
// characteristics of the IIR filter are fixed.

/*********************************************************************************************************
****************************************** Main Filter Functions *****************************************
**********************************************************************************************************/

// Must call this prior to using any filter functions.

//Create objects of my queues, to be used in initializing queues
static queue_t xQ;
static queue_t yQ;
static queue_t zQ[FILTER_IIR_FILTER_COUNT];
static queue_t oQ[FILTER_IIR_FILTER_COUNT];

//This creates a queue array with 10 queues in it
void initXQueue(){
  queue_init(&xQ,XQ_SIZE,"XQueue");
  for (uint32_t j=0; j<XQ_SIZE; j++){
    queue_overwritePush(&xQ, 0.0); //fills the queue with zeroes
    }
}
void printSize(){
  printf("%d\n", xQ.size);
}

void initYQueue(){
  queue_init(&yQ,YQ_SIZE,"YQueue");
  for (uint32_t j=0; j<YQ_SIZE; j++){
    queue_overwritePush(&yQ, 0.0); //fills the queue with zeroes
    }
}

char ZQueue[1000]; //This variable will be used to give the specific Z queue a name
void initZQueues() {
  for (uint32_t i=0; i<FILTER_IIR_FILTER_COUNT; i++) {
    sprintf(ZQueue, "ZQueue%d", i); //This replaces "ZQueue" with "ZQueuei", where i is the number 0-9, depending on which queue it is in the array
    queue_init(&(zQ[i]), ZQ_SIZE, ZQueue); //initialized the Zqueuei
    for (uint32_t j=0; j<ZQ_SIZE; j++)
     queue_overwritePush(&(zQ[i]), 0.0); //fills the queue with zeroes
  }
}

char OutputQueue[1000]; //This variable will be used to give the specific Z queue a name
void initOutputQueues() {
  for (uint32_t i=0; i<FILTER_IIR_FILTER_COUNT; i++) {
    sprintf(OutputQueue, "OutputQueue%d", i); //This replaces "OutputQueue" with "OutputQueuei", where i is the number 0-9, depending on which queue it is in the array
    queue_init(&(oQ[i]),OUTQ_SIZE, ZQueue); //initialized the Outputqueuei
    for (uint32_t j=0; j<OUTQ_SIZE; j++) //takes each of the 2000 points and fills em with zeroes
     queue_overwritePush(&(oQ[i]), 0.0); //fills the queue with zeroes
  }
}

void filter_init(){
  // Init queues and fill them with 0s.
  initXQueue();  // Call queue_init() on xQueue and fill it with zeros.
  initYQueue();  // Call queue_init() on yQueue and fill it with zeros.
  initZQueues(); // Call queue_init() on all of the zQueues and fill each z queue with zeros.
  initOutputQueues();  // Call queue_init() all of the outputQueues and fill each outputQueue with zeros.

}

// Use this to copy an input into the input queue of the FIR-filter (xQueue).
void filter_addNewInput(double x){
  queue_overwritePush(&xQ, x);
}

// Fills a queue with the given fillValue. For example,
// if the queue is of size 10, and the fillValue = 1.0,
// after executing this function, the queue will contain 10 values
// all of them 1.0.
void filter_fillQueue(queue_t* q, double fillValue){
  for(int i = 0; i < queue_size(q); i++){
    queue_overwritePush(q, fillValue);
  }
}

// Invokes the FIR-filter. Input is contents of xQueue.
// Output is returned and is also pushed on to yQueue.
double filter_firFilter(){

}

// Use this to invoke a single iir filter. Input comes from yQueue.
// Output is returned and is also pushed onto zQueue[filterNumber].
double filter_iirFilter(uint16_t filterNumber){

}

// Use this to compute the power for values contained in an outputQueue.
// If force == true, then recompute power by using all values in the outputQueue.
// This option is necessary so that you can correctly compute power values the first time.
// After that, you can incrementally compute power values by:
// 1. Keeping track of the power computed in a previous run, call this prev-power.
// 2. Keeping track of the oldest outputQueue value used in a previous run, call this oldest-value.
// 3. Get the newest value from the power queue, call this newest-value.
// 4. Compute new power as: prev-power - (oldest-value * oldest-value) + (newest-value * newest-value).
// Note that this function will probably need an array to keep track of these values for each
// of the 10 output queues.
double filter_computePower(uint16_t filterNumber, bool forceComputeFromScratch, bool debugPrint){
  sum = 0;
  prev_power = 0;
if(forceComputeFromScratch){
  for (int i = 0; i < queue_size(&oQ[filterNumber]); i++){
    sum +=  queue_readElementAt(oQ[filterNumber],i)*queue_readElementAt(oQ[filterNumber],i);
  }
  currentPowerValue[filterNumber] = sum;
}
else{
  prev_power = currentPowerValue[filterNumber];
  oldest_value = queue_readElementAt(&oQ[filterNumber],0);
  newest_value = queue_readElementAt(&oQ[filterNumber],queue_size(&oQ[filterNumber]));
  new_power = prev_power - (oldest_value * oldest_value) + (newest_value * newest_value);
  //********************This is incomplete********************************************
}
}

// Returns the last-computed output power value for the IIR filter [filterNumber].
double filter_getCurrentPowerValue(uint16_t filterNumber){
  return currentPowerValue[filterNumber];

}

// Get a copy of the current power values.
// This function copies the already computed values into a previously-declared array
// so that they can be accessed from outside the filter software by the detector.
// Remember that when you pass an array into a C function, changes to the array within
// that function are reflected in the returned array.
void filter_getCurrentPowerValues(double powerValues[]){

}

// Using the previously-computed power values that are current stored in currentPowerValue[] array,
// Copy these values into the normalizedArray[] argument and then normalize them by dividing
// all of the values in normalizedArray by the maximum power value contained in currentPowerValue[].
void filter_getNormalizedPowerValues(double normalizedArray[], uint16_t* indexOfMaxValue){

}

/*********************************************************************************************************
********************************** Verification-assisting functions. *************************************
********* Test functions access the internal data structures of the filter.c via these functions. ********
*********************** These functions are not used by the main filter functions. ***********************
**********************************************************************************************************/

// Returns the array of FIR coefficients.
const double* filter_getFirCoefficientArray(){

}

// Returns the number of FIR coefficients.
uint32_t filter_getFirCoefficientCount(){

}

// Returns the array of coefficients for a particular filter number.
const double* filter_getIirACoefficientArray(uint16_t filterNumber){

}

// Returns the number of A coefficients.
uint32_t filter_getIirACoefficientCount(){

}

// Returns the array of coefficients for a particular filter number.
const double* filter_getIirBCoefficientArray(uint16_t filterNumber){

}

// Returns the number of B coefficients.
uint32_t filter_getIirBCoefficientCount(){

}

// Returns the size of the yQueue.
uint32_t filter_getYQueueSize(){

}

// Returns the decimation value.
uint16_t filter_getDecimationValue(){

}

// Returns the address of xQueue.
queue_t* filter_getXQueue(){

}

// Returns the address of yQueue.
queue_t* filter_getYQueue(){

}

// Returns the address of zQueue for a specific filter number.
queue_t* filter_getZQueue(uint16_t filterNumber){

}

// Returns the address of the IIR output-queue for a specific filter-number.
queue_t* filter_getIirOutputQueue(uint16_t filterNumber){

}

//void filter_runTest();
