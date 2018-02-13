#include <stdint.h>
#include "queue.h"
#include <stdio.h>


#define FIR_FILTER_TAP_COUNT 81
#define IIR_A_COEFFICIENT_COUNT 11
#define IIR_B_COEFFICIENT_COUNT 11
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
static double power_array[] = {0,0,0,0,0,0,0,0,0,0}; //used to hold power of each player
double oldest_value = 0; //used in computing the power of a player
double newest_value = 0; //also used in computing the power of a player
double sum = 0; //used to hold the power of a player
double prev_power = 0; //used to simplify calculation of ongoing power values


const uint16_t filter_frequencyTickTable[FILTER_FREQUENCY_COUNT] = {68, 58, 50, 44, 38, 34, 30, 28, 26, 24};


const static double firCoefficients[FIR_FILTER_TAP_COUNT] = {
6.0546138291252597e-04,
5.2507143315267811e-04,
3.8449091272701525e-04,
1.7398667197948182e-04,
-1.1360489934931548e-04,
-4.7488111478632532e-04,
-8.8813878356223768e-04,
-1.3082618178394971e-03,
-1.6663618496969908e-03,
-1.8755700366336781e-03,
-1.8432363328817916e-03,
-1.4884258721727399e-03,
-7.6225514924622853e-04,
3.3245249132384837e-04,
1.7262548802593762e-03,
3.2768418720744217e-03,
4.7744814146589041e-03,
5.9606317814670249e-03,
6.5591485566565593e-03,
6.3172870282586493e-03,
5.0516421324586546e-03,
2.6926388909554420e-03,
-6.7950808883015244e-04,
-4.8141100026888716e-03,
-9.2899200683230643e-03,
-1.3538595939086505e-02,
-1.6891587875325020e-02,
-1.8646984919441702e-02,
-1.8149697899123560e-02,
-1.4875876924586697e-02,
-8.5110608557150517e-03,
9.8848931927316319e-04,
1.3360421141947857e-02,
2.8033301291042201e-02,
4.4158668590312596e-02,
6.0676486642862550e-02,
7.6408062643700314e-02,
9.0166807112971648e-02,
1.0087463525509034e-01,
1.0767073207825099e-01,
1.1000000000000000e-01,
1.0767073207825099e-01,
1.0087463525509034e-01,
9.0166807112971648e-02,
7.6408062643700314e-02,
6.0676486642862550e-02,
4.4158668590312596e-02,
2.8033301291042201e-02,
1.3360421141947857e-02,
9.8848931927316319e-04,
-8.5110608557150517e-03,
-1.4875876924586697e-02,
-1.8149697899123560e-02,
-1.8646984919441702e-02,
-1.6891587875325020e-02,
-1.3538595939086505e-02,
-9.2899200683230643e-03,
-4.8141100026888716e-03,
-6.7950808883015244e-04,
2.6926388909554420e-03,
5.0516421324586546e-03,
6.3172870282586493e-03,
6.5591485566565593e-03,
5.9606317814670249e-03,
4.7744814146589041e-03,
3.2768418720744217e-03,
1.7262548802593762e-03,
3.3245249132384837e-04,
-7.6225514924622853e-04,
-1.4884258721727399e-03,
-1.8432363328817916e-03,
-1.8755700366336781e-03,
-1.6663618496969908e-03,
-1.3082618178394971e-03,
-8.8813878356223768e-04,
-4.7488111478632532e-04,
-1.1360489934931548e-04,
1.7398667197948182e-04,
3.8449091272701525e-04,
5.2507143315267811e-04,
6.0546138291252597e-04};

const static double iirACoefficientConstants[FILTER_FREQUENCY_COUNT][IIR_A_COEFFICIENT_COUNT] = {
{-5.9637727070164015e+00, 1.9125339333078248e+01, -4.0341474540744173e+01, 6.1537466875368821e+01, -7.0019717951472188e+01, 6.0298814235238872e+01, -3.8733792862566290e+01, 1.7993533279581058e+01, -5.4979061224867651e+00, 9.0332828533799547e-01},
{-4.6377947119071443e+00, 1.3502215749461572e+01, -2.6155952405269755e+01, 3.8589668330738348e+01, -4.3038990303252632e+01, 3.7812927599537133e+01, -2.5113598088113793e+01, 1.2703182701888094e+01, -4.2755083391143520e+00, 9.0332828533800291e-01},
{-3.0591317915750960e+00, 8.6417489609637634e+00, -1.4278790253808875e+01, 2.1302268283304372e+01, -2.2193853972079314e+01, 2.0873499791105537e+01, -1.3709764520609468e+01, 8.1303553577932188e+00, -2.8201643879900726e+00, 9.0332828533800769e-01},
{-1.4071749185996747e+00, 5.6904141470697471e+00, -5.7374718273676217e+00, 1.1958028362868873e+01, -8.5435280598354382e+00, 1.1717345583835918e+01, -5.5088290876998407e+00, 5.3536787286077372e+00, -1.2972519209655518e+00, 9.0332828533799414e-01},
{8.2010906117760141e-01, 5.1673756579268559e+00, 3.2580350909220819e+00, 1.0392903763919172e+01, 4.8101776408668879e+00, 1.0183724507092480e+01, 3.1282000712126603e+00, 4.8615933365571822e+00, 7.5604535083144497e-01, 9.0332828533799658e-01},
{2.7080869856154512e+00, 7.8319071217995688e+00, 1.2201607990980744e+01, 1.8651500443681620e+01, 1.8758157568004549e+01, 1.8276088095999022e+01, 1.1715361303018897e+01, 7.3684394621253499e+00, 2.4965418284511904e+00, 9.0332828533800436e-01},
{4.9479835250075892e+00, 1.4691607003177602e+01, 2.9082414772101060e+01, 4.3179839108869331e+01, 4.8440791644688879e+01, 4.2310703962394342e+01, 2.7923434247706432e+01, 1.3822186510471010e+01, 4.5614664160654357e+00, 9.0332828533799958e-01},
{6.1701893352279846e+00, 2.0127225876810336e+01, 4.2974193398071684e+01, 6.5958045321253451e+01, 7.5230437667866596e+01, 6.4630411355739852e+01, 4.1261591079244127e+01, 1.8936128791950534e+01, 5.6881982915180291e+00, 9.0332828533799803e-01},
{7.4092912870072398e+00, 2.6857944460290135e+01, 6.1578787811202247e+01, 9.8258255839887312e+01, 1.1359460153696298e+02, 9.6280452143026082e+01, 5.9124742025776392e+01, 2.5268527576524203e+01, 6.8305064480743081e+00, 9.0332828533799969e-01},
{8.5743055776347692e+00, 3.4306584753117889e+01, 8.4035290411037053e+01, 1.3928510844056814e+02, 1.6305115418161620e+02, 1.3648147221895786e+02, 8.0686288623299745e+01, 3.2276361903872115e+01, 7.9045143816244696e+00, 9.0332828533799636e-01}
};

const static double iirBCoefficientConstants[FILTER_FREQUENCY_COUNT][IIR_B_COEFFICIENT_COUNT] = {
{9.0928451882350956e-10, -0.0000000000000000e+00, -4.5464225941175478e-09, -0.0000000000000000e+00, 9.0928451882350956e-09, -0.0000000000000000e+00, -9.0928451882350956e-09, -0.0000000000000000e+00, 4.5464225941175478e-09, -0.0000000000000000e+00, -9.0928451882350956e-10},
{9.0928689481426145e-10, 0.0000000000000000e+00, -4.5464344740713075e-09, 0.0000000000000000e+00, 9.0928689481426151e-09, 0.0000000000000000e+00, -9.0928689481426151e-09, 0.0000000000000000e+00, 4.5464344740713075e-09, 0.0000000000000000e+00, -9.0928689481426145e-10},
{9.0928675031518918e-10, 0.0000000000000000e+00, -4.5464337515759461e-09, 0.0000000000000000e+00, 9.0928675031518922e-09, 0.0000000000000000e+00, -9.0928675031518922e-09, 0.0000000000000000e+00, 4.5464337515759461e-09, 0.0000000000000000e+00, -9.0928675031518918e-10},
{9.0928691911265348e-10, 0.0000000000000000e+00, -4.5464345955632681e-09, 0.0000000000000000e+00, 9.0928691911265362e-09, 0.0000000000000000e+00, -9.0928691911265362e-09, 0.0000000000000000e+00, 4.5464345955632681e-09, 0.0000000000000000e+00, -9.0928691911265348e-10},
{9.0928650773631441e-10, 0.0000000000000000e+00, -4.5464325386815723e-09, 0.0000000000000000e+00, 9.0928650773631445e-09, 0.0000000000000000e+00, -9.0928650773631445e-09, 0.0000000000000000e+00, 4.5464325386815723e-09, 0.0000000000000000e+00, -9.0928650773631441e-10},
{9.0928643877976615e-10, -0.0000000000000000e+00, -4.5464321938988311e-09, -0.0000000000000000e+00, 9.0928643877976621e-09, -0.0000000000000000e+00, -9.0928643877976621e-09, -0.0000000000000000e+00, 4.5464321938988311e-09, -0.0000000000000000e+00, -9.0928643877976615e-10},
{9.0928309627600387e-10, -0.0000000000000000e+00, -4.5464154813800196e-09, -0.0000000000000000e+00, 9.0928309627600391e-09, -0.0000000000000000e+00, -9.0928309627600391e-09, -0.0000000000000000e+00, 4.5464154813800196e-09, -0.0000000000000000e+00, -9.0928309627600387e-10},
{9.0929565913508067e-10, 0.0000000000000000e+00, -4.5464782956754031e-09, 0.0000000000000000e+00, 9.0929565913508061e-09, 0.0000000000000000e+00, -9.0929565913508061e-09, 0.0000000000000000e+00, 4.5464782956754031e-09, 0.0000000000000000e+00, -9.0929565913508067e-10},
{9.0926542439871930e-10, 0.0000000000000000e+00, -4.5463271219935958e-09, 0.0000000000000000e+00, 9.0926542439871916e-09, 0.0000000000000000e+00, -9.0926542439871916e-09, 0.0000000000000000e+00, 4.5463271219935958e-09, 0.0000000000000000e+00, -9.0926542439871930e-10},
{9.0907017986989547e-10, 0.0000000000000000e+00, -4.5453508993494776e-09, 0.0000000000000000e+00, 9.0907017986989551e-09, 0.0000000000000000e+00, -9.0907017986989551e-09, 0.0000000000000000e+00, 4.5453508993494776e-09, 0.0000000000000000e+00, -9.0907017986989547e-10}
};


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
	//output of the fir filter convolution
	double y;
	// each time this is calculated, initialize it to 0
	y = 0.0;
	// This for-loop performs the identical computation to that shown above. for-loop is correct way to do it.
	for (uint32_t i=0; i< FIR_FILTER_TAP_COUNT; i++) {
		y += queue_readElementAt(&xQ, FIR_FILTER_TAP_COUNT-1-i) * firCoefficients[i];  // iteratively adds the (b * input) products.
	}
	//push the new y value onto the
	queue_overwritePush(&yQ, y);
	//return the output as well as pushing it onto the yQueue
	return y;

}

// Use this to invoke a single iir filter. Input comes from yQueue.
// Output is returned and is also pushed onto zQueue[filterNumber].
double filter_iirFilter(uint16_t filterNumber){
  //output of the fir filter convolution
  double y;
  double yy;
  // each time this is calculated, initialize it to 0
  y = 0.0;
  // This for-loop performs the identical computation to that shown above. for-loop is correct way to do it.
  for(uint32_t k = 0; i < YQ_SIZE; k++){
  y += queue_readElementAt(&yQ, IIR_B_COEFFICIENT_COUNT-1-i) * iirBCoefficientConstants[i,filterNumber]; // iteratively adds the (b * input) products.
  }

  for (uint32_t i=0; i< ZQ_SIZE; i++) {
    yy += queue_readElementAt(&zQ, IIR_A_COEFFICIENT_COUNT-1-i) * iirACoefficientConstants[i,filterNumber]; // iteratively adds the (b * input) products.
  }
double result = y-yy;
  //push the new y value onto the zQueue of the filter we are on
  queue_overwritePush(&(zQ[filterNumber]), result);
  //return the output as well as pushing it onto the zQueue
  return result;
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
  //prev_power = 0; //not sure if this is needed or not
if(forceComputeFromScratch){
  for (int i = 0; i < queue_size(&oQ[filterNumber]); i++){
    sum +=  queue_readElementAt(oQ[filterNumber],i)*queue_readElementAt(oQ[filterNumber],i);
  }
  currentPowerValue[filterNumber] = sum;
  return sum;
}
else{
  prev_power = currentPowerValue[filterNumber];
  oldest_value = queue_readElementAt(&oQ[filterNumber],0); //stores the oldest signal
  newest_value = queue_readElementAt(&oQ[filterNumber],queue_size(&oQ[filterNumber])); //stores the newest value
  new_power = prev_power - (oldest_value * oldest_value) + (newest_value * newest_value); //efficient way to compute power after the first calculation, simply remove the oldest power and add in the most recent power!
  currentPowerValue[filterNumber] = new_power; //This should resolve the "Note" given
  return new_power;
}
}

// Returns the last-computed output power value for the IIR filter [filterNumber].
double filter_getCurrentPowerValue(uint16_t filterNumber){
  return filter_computePower(filterNumber, false, false); //NOT SURE ON THESE FALSE STATEMENTS
}

// Get a copy of the current pow-er values.
// This function copies the already computed values into a previously-declared array
// so that they can be accessed from outside the filter software by the detector.
// Remember that when you pass an array into a C function, changes to the array within
// that function are reflected in the returned array.
void filter_getCurrentPowerValues(double powerValues[]){
  for(int i = 0; i < FILTER_IIR_FILTER_COUNT; i++){
    powerValues[i] = filter_getCurrentPowerValue(i); //fill the array with the power at each filter
  }
}

// Using the previously-computed power values that are current stored in currentPowerValue[] array,
// Copy these values into the normalizedArray[] argument and then normalize them by dividing
// all of the values in normalizedArray by the maximum power value contained in currentPowerValue[].
void filter_getNormalizedPowerValues(double normalizedArray[], uint16_t* indexOfMaxValue){
  for(int i = 0; i < FILTER_IIR_FILTER_COUNT; i++){
    normalizedArray[i] = filter_getCurrentPowerValue(i)/filter_getCurrentPowerValue(indexOfMaxValue); //fill the array with the normalized powers
  }
}

/*********************************************************************************************************
********************************** Verification-assisting functions. *************************************
********* Test functions access the internal data structures of the filter.c via these functions. ********
*********************** These functions are not used by the main filter functions. ***********************
**********************************************************************************************************/

// Returns the array of FIR coefficients.
const double* filter_getFirCoefficientArray(){
	return firCoefficients;
}

// Returns the number of FIR coefficients.
uint32_t filter_getFirCoefficientCount(){
	return FIR_FILTER_TAP_COUNT;
}

// Returns the array of coefficients for a particular filter number.
const double* filter_getIirACoefficientArray(uint16_t filterNumber){
	return iirACoefficientConstants;
}

// Returns the number of A coefficients.
uint32_t filter_getIirACoefficientCount(){
	return IIR_A_COEFFICIENT_COUNT;
}

// Returns the array of coefficients for a particular filter number.
const double* filter_getIirBCoefficientArray(uint16_t filterNumber){
	return iirBCoefficientConstants;
}

// Returns the number of B coefficients.
uint32_t filter_getIirBCoefficientCount(){
	return IIR_B_COEFFICIENT_COUNT;
}

// Returns the size of the yQueue.
uint32_t filter_getYQueueSize(){
  return queue_size(&yQ);
}

// Returns the decimation value.
uint16_t filter_getDecimationValue(){

}

// Returns the address of xQueue.
queue_t* filter_getXQueue(){
  return &xQ;
}

// Returns the address of yQueue.
queue_t* filter_getYQueue(){
  return &yQ;
}

// Returns the address of zQueue for a specific filter number.
queue_t* filter_getZQueue(uint16_t filterNumber){
  return &(zQ[filterNumber]);
}

// Returns the address of the IIR output-queue for a specific filter-number.
queue_t* filter_getIirOutputQueue(uint16_t filterNumber){
  return &(oQ[filterNumber]);
}

//void filter_runTest();
