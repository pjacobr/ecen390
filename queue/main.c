#include <stdio.h>
#include "queue.h"
#define SIZE_QUEUE 100





int main(){

  queue_t* q = (queue_t*)malloc(sizeof(queue_t));
  queue_init(q, SIZE_QUEUE, "queue");
  printf("%d\n\r", q->indexIn);
  printf("%d\n\r", q->indexOut + 1 );

  printf("%d\n", queue_size(q));
  int i = 0;
  for(i = 0; i < 300; i++){
    queue_push(q, i);
  }
  printQueue(q);
  for(i = 0; i < 300; i++){
    queue_push(q, i);
  }

  

  // printf("ElCOunt: %d\n\r", queue_size(q));
  // printQueue(q);
  // printf("indexes: %d | %d", q->indexIn, q->indexOut);
  // // printf("Pushing 3\n\r");
  // queue_push(q, 3);
  // printf("indexes: %d | %d", q->indexIn, q->indexOut);
  // printQueue(q);
  // queue_push(q, 4);
  // printf("indexes: %d | %d", q->indexIn, q->indexOut);
  // printQueue(q);
  // queue_push(q, 7);
  // printf("indexes: %d | %d", q->indexIn, q->indexOut);
  // printf("ElCOunt: %d\n\r", queue_size(q));
  //
  // queue_push(q, 8);
  // printf("indexes: %d | %d", q->indexIn, q->indexOut);
  // printQueue(q);
  // printf("ElCOunt: %f\n\r", queue_readElementAt(q, 0));
  // printf("ElCOunt: %f\n\r", queue_readElementAt(q, 1));
  // printf("ElCOunt: %f\n\r", queue_readElementAt(q, 2));
  // printf("ElCOunt: %f\n\r", queue_readElementAt(q, 3));
  //
  // // printf("Pushing 12\n\r");
  // // printf("ElementCount: %d\n\r", queue_elementCount(q));
  // // printf("Size: %f\n\r",queue_size(q));
  // queue_push(q, 12);
  // printf("indexes: %d | %d", q->indexIn, q->indexOut);
  // printf("ElCOunt: %d\n\r", queue_size(q));
  // // printf("IndexOut: %f\n\r", q->indexOut);
  // // printf("IndexIn: %f\n\r", q->indexIn);
  // printQueue(q);
  // printf("ElCOunt: %f\n\r", queue_readElementAt(q, 0));
  // printf("ElCOunt: %f\n\r", queue_readElementAt(q, 1));
  // printf("ElCOunt: %f\n\r", queue_readElementAt(q, 2));
  // printf("ElCOunt: %f\n\r", queue_readElementAt(q, 3));
  // printf("ElCOunt: %d\n\r", queue_size(q));
  // queue_overwritePush(q, 12);
  // printf("indexes: %d | %d", q->indexIn, q->indexOut);
  // printf("ElCOunt: %d\n\r", queue_size(q));
  // printQueue(q);
  // printf("ElCOunt: %f\n\r", queue_readElementAt(q, 0));
  // printf("ElCOunt: %f\n\r", queue_readElementAt(q, 1));
  // printf("ElCOunt: %f\n\r", queue_readElementAt(q, 2));
  // printf("ElCOunt: %f\n\r", queue_readElementAt(q, 3));
  // printf("ElCOunt: %d\n\r", queue_size(q));
  // queue_overwritePush(q, 14);
  // printf("indexes: %d | %d", q->indexIn, q->indexOut);
  // printf("ElCOunt: %d\n\r", queue_size(q));
  // printQueue(q);
  // printf("ElCOunt: %d\n\r", queue_size(q));
  // queue_overwritePush(q, 24);
  // printf("indexes: %d | %d", q->indexIn, q->indexOut);
  // queue_overwritePush(q, 25);
  // printf("indexes: %d | %d", q->indexIn, q->indexOut);
  // printf("ElCOunt: %d\n\r", queue_size(q));
  // printQueue(q);
  // printf("ElCOunt: %d\n\r", queue_elementCount(q));
  // printf("ElCOunt: %f\n\r", queue_readElementAt(q, 0));
  // printf("ElCOunt: %f\n\r", queue_readElementAt(q, 1));
  // printf("ElCOunt: %f\n\r", queue_readElementAt(q, 2));
  // printf("ElCOunt: %f\n\r", queue_readElementAt(q, 3));
  //
  // queue_runTest();

  return 0;
}
