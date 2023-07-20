#ifndef __condition_h__
#define __condition_h__

typedef struct Condition {
    char *operator;
    char *operand1;
    char *operand2;
} Condition;

Condition *interpretCondition(char *conditionString);
Condition *copyCondition(Condition *condition);
void Condition_destroy(Condition *condition);

#endif
