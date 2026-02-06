#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#define STACK_SIZE 100

int stack[STACK_SIZE];
int top = -1;

void push(int value) {
    if (top >= STACK_SIZE - 1) {
        printf("Stack overflow!\n");
        exit(1);
    }
    stack[++top] = value;
}

int pop() {
    if (top < 0) {
        printf("Stack underflow!\n");
        exit(1);
    }
    return stack[top--];
}

void evaluate_rpn() {
    char input[100];
    int a, b;

    printf("Enter RPN expression (use = to evaluate): ");
    while (scanf("%s", input) == 1) {
        if (isdigit(input[0])) {
            push(atoi(input));
        } else if (strlen(input) == 1) {
            switch (input[0]) {
                case '+':
                    push(pop() + pop());
                    break;
                case '-':
                    b = pop();
                    a = pop();
                    push(a - b);
                    break;
                case '*':
                    push(pop() * pop());
                    break;
                case '/':
                    b = pop();
                    a = pop();
                    if (b == 0) {
                        printf("Division by zero!\n");
                        exit(1);
                    }
                    push(a / b);
                    break;
                case '^':
                    b = pop();
                    a = pop();
                    push((int)pow(a, b));
                    break;
                case '=':
                    printf("Result: %d\n", pop());
                    top = -1; // Clear stack
                    return;
                default:
                    printf("Invalid operator: %s\n", input);
                    return;
            }
        } else {
            printf("Invalid input: %s\n", input);
            return;
        }
    }
}

int main() {
    while (1) {
        evaluate_rpn();
    }
    return 0;
}
