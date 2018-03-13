/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>

#include "libpriqueue.h"


/**
  Initializes the priqueue_t data structure.
  
  Assumtions
    - You may assume this function will only be called once per instance of priqueue_t
    - You may assume this function will be the first function called using an instance of priqueue_t.
  @param q a pointer to an instance of the priqueue_t data structure
  @param comparer a function pointer that compares two elements.
  See also @ref comparer-page
 */
void priqueue_init(priqueue_t *q, int(*comparer)(const void *, const void *))
{
    q -> queue_size = 0;
    q -> queue_head = 0;
    q -> comparer = comparer;
}


/**
  Inserts the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the queue_head of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{
  Node* node = malloc(sizeof(Node));
  Node* hold = q -> queue_head;
  Node* hold_2 = 0;

  node -> ptr = ptr;
  node -> connect = 0;

  int i = 0;

  //if the queue is empty sets it to a size of 1 and then puts the node in the head of the queue
  if(isEmpty(q)){
     q -> queue_size = 1;
     q -> queue_head = node;

    return 0;
  }

  while(hold != 0 && q -> comparer(hold -> ptr, ptr) < 0){
    hold_2 = hold;
    hold = hold -> connect;
    i+=1;
  }


  if(i == 0){
    q -> queue_size +=1;
    node -> connect = q -> queue_head;
    q -> queue_head = node;
    return 0;
  }

  hold_2 -> connect = node;
  node -> connect = hold;

  q -> queue_size+=1;

  return i;
}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q)
{
  if(isEmpty(q))
  {
    return NULL;
  }
  //returns the pointer at the head of the queue as long as it is not empty 
  else
  {
    return q -> queue_head -> ptr;
  }
}


/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
 */
void *priqueue_poll(priqueue_t *q)
{
  if(isEmpty(q))
  {
    return NULL;
  }

  Node* hold = q -> queue_head;
  void* hold_2 = 0;

  if(hold != 0)
  {
    q -> queue_head = hold -> connect;
  }
  else
  {
    q -> queue_head = 0;
  }

  hold_2 = hold -> ptr;
  free(hold);
  q -> queue_size-=1;
  return hold_2;
}

//isEmpty function that looks at the size of the queue and checks to see if it is equal to zero
int isEmpty(priqueue_t *q) {
    return q->queue_size == 0;
}

/**
  Returns the element at the specified position in this list, or NULL if
  the queue does not contain an index'th element.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of retrieved element
  @return the index'th element in the queue
  @return NULL if the queue does not contain the index'th element
 */
void *priqueue_at(priqueue_t *q, int index)
{
  if(index >= q -> queue_size)
  {
    return 0;
  }
  else
  {
    Node* hold = q -> queue_head;

    int var = 1;
    //places every iteration it holds an index of the queue until it gets to the desired index and retruns the value in that index
    while(var <= index)
    {
      hold = hold -> connect;
      var+=1;
    }
    return hold -> ptr;
  }
}


/**
  Removes all instances of ptr from the queue. 
  
  This function should not use the comparer function, but check if the data contained in each element of the queue is equal (==) to ptr.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr address of element to be removed
  @return the number of entries removed
 */
int priqueue_remove(priqueue_t *q, void *ptr)
{

  Node* next_queue = q -> queue_head -> connect;
  Node* queue_head_queue = q -> queue_head;

  if(q -> queue_size < 1)
  {
    return 0;
  }

  //looks to see if there is anything located in the head of the queue
  if(q -> comparer(ptr, q -> queue_head -> ptr) == 0)
  {
    q -> queue_size-=1;
    Node* hold = q -> queue_head;
    q -> queue_head = q -> queue_head -> connect;
    free(hold);
    return 1 + priqueue_remove(q, ptr);
  }

  int i = 0;

  //removes each indicie of the queue until it equals zero
  while(next_queue != 0)
  {
    if(q -> comparer(next_queue -> ptr, ptr) == 0)
    {
      Node* hold = next_queue -> connect;
      queue_head_queue -> connect = hold;
      i+=1;
      free(next_queue);
      next_queue = hold;
      q -> queue_size-=1;
    }
    else
    {
      queue_head_queue = next_queue;
      next_queue = next_queue -> connect;
    }
  }

  return i;
}


/**
  Removes the specified index from the queue, moving later elements up
  a spot in the queue to fill the gap.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of element to be removed
  @return the element removed from the queue
  @return NULL if the specified index does not exist
 */
void *priqueue_remove_at(priqueue_t *q, int index)
{
  //returns zero if it asks for an indicie larger than the size of the queue
  if(index > q -> queue_size)
  {
    return 0;
  }

  q -> queue_size-=1;

  Node* hold = q -> queue_head;
  Node* hold_2 = hold;

  //if index is zero returns the first indicie of the queue 
  if(index == 0)
  {
    q -> queue_head = q -> queue_head -> connect;
    return hold;
  }

  hold = hold -> connect;
  index-=1;

  //removes items from the head of the queue until the idicie is 0 then returns the value at the head 
  while(index > 0)
  {
    hold_2 = hold;
    hold = hold -> connect;
    index-=1;
  }

  hold_2 -> connect = hold -> connect;
  return hold;

}


/**
  Returns the number of elements in the queue.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{
  return q -> queue_size;
}


/**
  Destroys and frees all the memory associated with q.
  
  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{
  while(q -> queue_size > 0)
  {
    void* hold = priqueue_remove_at(q, 0);
    free(hold);
  } 
}




