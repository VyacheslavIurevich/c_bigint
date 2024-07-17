#include <inttypes.h>
#include <stddef.h>
#define QUEUE_CAPACITY 5000

typedef struct queue_node_t {
  uint8_t decimal_digit;
  struct queue_node_t *next;
} queue_node_t;

typedef struct {
  queue_node_t *head;
  queue_node_t *tail;
  size_t size;
  size_t capacity;
} queue_t;

queue_t *queue_init(uint8_t *exit_code);

void queue_clear(queue_t *queue, uint8_t *exit_code);

void queue_add(queue_t *queue, uint8_t element, uint8_t *exit_code);
