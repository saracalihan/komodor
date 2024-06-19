#include<stdio.h>
#include "komodor.h"

int main(){
    KomodorConfig c = {
        .log_test=1,
    };
    KomodorTest t = komodor_create_test("echo 'Alihan'", &c);
    komodor_define_test(
        &t,
        0,
        "Alihan\n",
        NULL,
        NULL
    );
    if(komodor_exec_test(&t)){
        printf("OK\n");
    }else {
        printf("NAY!\n");
    }
    return 0;
}
