#include <stdio.h>
#include <stdbool.h>

#include "is_prime.h"

int main(void) {
    printf("Please enter a number: ");

    int input_number;
    scanf("%d", &input_number);

    if (is_prime(input_number)) {
        printf("%d is prime\n", input_number);
    } else {
        printf("%d is not prime\n", input_number);
    }

    return 0;
}