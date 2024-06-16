#include<stdio.h>
#include "komodor.h"

int main(){
    KomodorTest t = komodor_create_test("echo 'Alihan'", NULL);
    komodor_define_test(
        &t,
        0,
        "Alihan\n",
        NULL
    );
    if(komodor_exec_test(&t)){
        printf("OK\n");
    }else {
        printf("NAY!\n");
    }
    printf("---------------------\n\tEXPECTED\nout:\n'%s'\nerr: '%s'\nreturn: %d\n---------------------\n\tACTUAL\nout:\n'%s'\nerr: '%s'\nreturn: %d\nerror mgs: '%s'\n",t.expected_std_output, t.expected_std_error,t.expected_exit_code, t.std_output, t.std_error,t.exit_code, t.error_message);
    return 0;
}
