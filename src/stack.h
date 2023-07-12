#ifndef __stack_h__
#define __stack_h__

typedef struct Stack {
    char **contents;
    int capacity;
    int size;
} Stack;

Stack *Stack_create();
void Stack_destroy(Stack *stack);

void push(Stack *stack, char *string);
char *pop(Stack *stack);
char *peek(Stack *stack);

int isEmpty(Stack *stack);
int isFull(Stack *stack);

void expand(Stack *stack);

#endif
