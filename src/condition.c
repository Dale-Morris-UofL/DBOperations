#include <stdlib.h>
#include <string.h>

#include "condition.h"

/*
 * Returns a condition representing the condition expressed in the given string
 */
Condition *interpretCondition(char *conditionString) {
    Condition *condition = malloc(sizeof(Condition));
    char *newConditionString = strdup(conditionString);
    const char *delim = " ";

    condition->operand1 = strtok(newConditionString, delim);
    condition->operator = strtok(NULL, delim);
    condition->operand2 = strtok(NULL, delim);

    return condition;
}

Condition *copyCondition(Condition *condition) {
    Condition *copy = malloc(sizeof(Condition));

    copy->operator = strdup(condition->operator);
    copy->operand1 = strdup(condition->operand1);
    copy->operand2 = strdup(condition->operand2);

    return copy;
}

void Condition_destroy(Condition *condition) {
    assert(condition != NULL);

    condition->operator = NULL;
    condition->operand1 = NULL;
    condition->operand2 = NULL;

    free(condition->operator);
    free(condition->operand1);
    free(condition->operand2);

    free(condition);
}
