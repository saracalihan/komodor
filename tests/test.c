#include "test.h"
#include "../src/komodor.h"

int main(){
    KomodorConfig c = {
        .log_test=0,
    };
    Tests({
        Describe("exit code", {
            Test("should return 0 when exit codes are not match",{
                KomodorTest t = komodor_create_test("exit 1", &c);
                komodor_define_test(&t,
                    0,
                    NULL,
                    NULL,
                    NULL
                );
                int r = komodor_exec_test(&t);
                komodor_free(&t);
                r==0;
            });
        });
        Describe("stdout",{
            Test("should return 1 if stdout is same as expected",{
                KomodorTest t = komodor_create_test("./build/hello", &c);
                komodor_define_test(&t,
                    0,
                    "Hello World!\n",
                    NULL,
                    NULL
                );
                int r = komodor_exec_test(&t);
                komodor_free(&t);
                r;

            });
            Test("should return 0 if stdout is not same as expected",{
                KomodorTest t = komodor_create_test("./build/hello", &c);
                komodor_define_test(&t,
                    0,
                    "Not Hello World!\n",
                    NULL,
                    NULL
                );
                int r = komodor_exec_test(&t);
                komodor_free(&t);
                r ==0;
            });
        });
        Describe("stderr",{
            Test("should return 1 if stderr is same as expected",{
                KomodorTest t = komodor_create_test("./build/error", &c);
                komodor_define_test(&t,
                    0,
                    NULL,
                    NULL,
                    "hello from stderr: Success\n"
                );
                int r = komodor_exec_test(&t);
                komodor_free(&t);
                r;

            });
            Test("should return 0 if stderr is not same as expected",{
                KomodorTest t = komodor_create_test("./build/error", &c);
                komodor_define_test(&t,
                    0,
                    NULL,
                    NULL,
                    NULL
                );
                int r = komodor_exec_test(&t);
                komodor_free(&t);
                r ==0;
            });
        });
        Describe("stdin",{
            Test("should pass input to process",{
                KomodorTest t = komodor_create_test("./build/echo", &c);
                komodor_define_test(&t,
                    0,
                    "input: saracalihan",
                    "saracalihan\n",
                    NULL
                );
                int r = komodor_exec_test(&t);
                komodor_free(&t);
                r;
            });
            Test("should pass multiple input to process",{
                KomodorTest t = komodor_create_test("./build/echo2", &c);
                komodor_define_test(&t,
                    0,
                    "input: sarac\ninput: alihan",
                    "sarac\nalihan",
                    NULL
                );
                int r = komodor_exec_test(&t);
                komodor_free(&t);
                r;
            });
        });
    });
    return failed_test;
}