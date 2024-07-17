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
