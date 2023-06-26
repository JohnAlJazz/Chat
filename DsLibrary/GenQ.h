#ifndef __GENQ_H__
#define __GENQ_H__

#include <stdlib.h> /*size_t*/

typedef enum{
	QUEUE_SUCCESS,
	QUEUE_UNINITIALIZED_ERROR,
	QUEUE_OVERFLOW_ERROR,
	QUEUE_UNDERFLOW_ERROR,
	QUEUE_DATA_NOT_FOUND_ERROR,
	QUEUE_DATA_UNINITIALIZED_ERROR
}QueueResult;

typedef struct Queue Queue;

typedef void (*DestroyItem)(void* _element);
typedef int (*ActionFunction)(const void* _element, void* _context);

/**
 * @brief Create a new queue with a size of _size
 * @param[in] _size - The size of the queue
 * @return Queue pointer - on success, NULL on failure
 */
Queue* QueueCreate(size_t _size);

/**
 * @brief Unallocate a previously created queue
 * @param[in] _queue - the queue to unallocate
 * @param[in] _itemDestroy - Optional destroy function for each item
 * @return void - no return value
  */
void QueueDestroy(Queue** _queue, DestroyItem _itemDestroy);

/**
 * @brief Add a new item to the queue
 * @param[in] _queue - The queue to add the item to
 * @param[in] _item - the item to add to the queue in a cyclic way
 * @return Queue_Result - 
 * @retval QUEUE_SUCCESS - on seccess
 * @retval QUEUE_UNINITIALIZED_ERROR - if _queue is NULL
 * @retval QUEUE_DATA_UNINITIALIZED_ERROR - if _item is NULL
 * @retval QUEUE_OVERFLOW_ERROR - if there is no more room to add 
 */
QueueResult QueueInsert(Queue* _queue,void* _item);

/**
 * @brief Remove the head item of the queue 
 * @param[in] _queue - the queue to remove the information from
 * @param[in] _item - A pointer to the information removed
 * @return Queue_Result -
 * @retval QUEUE_SUCCESS - on success
 * @retval QUEUE_UNINITIALIZED_ERROR - if _queue is NULL
 * @retval QUEUE_DATA_UNINITIALIZED_ERROR - if _item is NULL
 * @retval QUEUE_UNDERFLOW_ERROR - if queue is empty
 *
 */
QueueResult QueueRemove(Queue* _queue,void** _item);

/**
 * @brief Check if a given queue is empty
 * @param[in] _queue - The queue to check
 * @return size_t - return IS_EMPTY if empty IS_NOT_EMPTY if full
 *
 */
size_t IsQueueEmpty(Queue* _queue);

/**
 * @brief ForEach function to implement on all elements in queue
 * @param[in] _queue - The queue
 * @param[in] _action - The action to implement on elements
 * @param[in] _context - context for action if needed
 * @return size_t - 0 if either queue or action are null
 *
 * @details if action returns 0 stop running and return the number of times action ran
 */
size_t QueueForEach(Queue* _queue, ActionFunction _action, void* _context);

#endif /*__GENQ_H__*/

