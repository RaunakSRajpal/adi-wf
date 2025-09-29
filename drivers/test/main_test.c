#include <stdio.h>
#include "test.h"

//extern int var;

int main() {
    //int x = 1;
    
    printf("foo: %d\n", foo(var));
    var = 1;
    printf("foo: %d\n", foo(var));
    printf("var: %d\tfoobar: %d\n", var, foobar());

    return 0;
}

