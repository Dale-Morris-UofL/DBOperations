#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stack.h"

#define INIT_CAPACITY 10
#define MAX_STRING_SIZE 100

/*
 * Driver function to test stack functions
 */
int main(int argc, char *argv[]) {
    // Create test stacks
    Stack *stack1 = Stack_create();

    // TEST 1: Add elements, check if its empty, and peek and pop each element
    printf("isEmpty: %d\n", isEmpty(stack1));

    push(stack1, "first");
    push(stack1, "second");
    push(stack1, "third");

    printf("isEmpty: %d\n", isEmpty(stack1));

    printf("Peek: %s\n", peek(stack1));
    printf("Pop: %s\n", pop(stack1));
    printf("Peek: %s\n", peek(stack1));
    printf("Pop: %s\n", pop(stack1));
    printf("Peek: %s\n", peek(stack1));
    printf("Pop: %s\n", pop(stack1));

    printf("isEmpty: %d\n", isEmpty(stack1));

    // TEST 2: Add many elements to trigger expand, then pop each one and check capacity
    char *temp = malloc(MAX_STRING_SIZE * sizeof(char));
    int i;
    for (i = 0; i < 11; i++) {
        sprintf(temp, "%d", i);
        push(stack1, temp);
        printf("%d pushed to stack\n", i);
    }

    int high = 11;
    printf("---- START ----\n");
    for (i = 0; i < high; i++) {
        printf("Num: %s\n", pop(stack1));
    }
    printf("---- END ----\n");

    printf("%d\n", stack1->capacity);

    // Free resources
    Stack_destroy(stack1);
    free(temp);

    return 0;
}

/*
 * Returns a pointer to a new empty stack with default capacity
 */
Stack *Stack_create() {
    Stack *stack = (Stack *) malloc(sizeof(Stack));
    stack->capacity = INIT_CAPACITY;
    stack->size = 0;
    stack->contents = (char **) malloc(stack->capacity);
    printf("Stack capacity: %d\n", stack->capacity);
    printf("Size of stack contents: %ld\n", sizeof(stack->contents));

    return stack;
}

/*
 * Deallocates the memory previously allocated to the given stack
 */
void Stack_destroy(Stack *stack) {
    free(stack->contents);

    free(stack);
}

/*
 * Pushes the given element onto the given stack
 */
void push(Stack *stack, char element[]) {
    if (isFull(stack)) {
        expand(stack);
    }
    stack->contents[++stack->size] = element;
}

/*
 * Removes and returns the top element off of the given stack
 */
char *pop(Stack *stack) {
    if (isEmpty(stack)) {
        return NULL;
    }
    return stack->contents[stack->size--];
}

/*
 * Return the top element off of the given stack
 */
char *peek(Stack *stack) {
    if (isEmpty(stack)) {
        return NULL;
    }
    return stack->contents[stack->size];
}

/*
 * Returns true if the stack is empty, false otherwise
 */
int isEmpty(Stack *stack) {
    return stack->size == 0;
}

/*
 * Returns true if the stack is full, false otherwise
 */
int isFull(Stack *stack) {
    return stack->size == stack->capacity;
}

/*
 * Reallocates twice the memory for the contents of the given stack
 */
void expand(Stack *stack) {
    stack->capacity *= 2;
    stack->contents = (char **) realloc(stack->contents, 2 * sizeof(stack->contents));
}
