#include "queue.h"
#include "../exit_codes.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

queue_t *queue_init(uint8_t *exit_code) {
  queue_t *queue = (queue_t *)malloc(sizeof(queue_t));

  if (queue == NULL) {
    fprintf(stderr, "Cannot initialize queue\n");
    *exit_code = other_errors;
    return NULL;
  }

  queue->head = NULL;
  queue->tail = NULL;
  queue->size = 0;

  return queue;
}

void queue_clear(queue_t *queue, uint8_t *exit_code) {
  queue_node_t *temp;

  if (queue == NULL) {
    fprintf(stderr, "Cannot clear queue\n");
    *exit_code = other_errors;
    return;
  }

  while (queue->head != NULL) {
    temp = queue->head;
    queue->head = queue->head->next;
    free(temp);
  }

  queue->tail = NULL;
  queue->size = 0;

  free(queue);
}

void queue_add(queue_t *queue, uint8_t element, uint8_t *exit_code) {
  queue_node_t *node = (queue_node_t *)malloc(sizeof(queue_node_t));

  if (node == NULL) {
    fprintf(stderr, "Cannot create queue node\n");
    *exit_code = other_errors;
    return;
  }

  node->decimal_digit = element;
  node->next = NULL;

  if (queue->head == NULL) {
    // we had an empty queue and now the queue consists of 1 node
    queue->head = node;
    queue->tail = node;
    ++queue->size;
  } else {
    queue->tail->next = node;
    queue->tail = node;
    ++queue->size;
  }
}
