#pragma once
#include <inttypes.h>
#include <stddef.h>

typedef struct queue_node_t {
  struct queue_node_t *next;
  uint8_t decimal_digit : 4;
} queue_node_t;

typedef struct {
  size_t size;
  queue_node_t *head;
  queue_node_t *tail;
} queue_t;

queue_t *queue_init(uint8_t *exit_code);

void queue_clear(queue_t *queue, uint8_t *exit_code);

void queue_add(queue_t *queue, uint8_t element, uint8_t *exit_code);
