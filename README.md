# Komodor
Komodor is a simple E2E library for C language.

You can test stdout, stderr and program's exit code
with passing stdin

**Table of contents**:
+ [Usage](#usage)
+ [Contributing](#contributing)
+ [License](#license)

## Usage
1. **Create test object**. You can define custom settings here.

```c
#include "komodor.h"

int main(){
    KomodorTest t = komodor_create_test("./myprogram", NULL);

    return 0;
}
```

2. **Define test**. Set checks.

```c
#include "komodor.h"

int main(){
    KomodorTest t = komodor_create_test("./myprogram", NULL);

    komodor_define_test(
        &t,
        0,                      // exit code
        "saracalihan",          // stdin, u can use \n or \r char for multiple input
        "Hello saracalihan!\n", // stdout
        NULL                    // stderr
    );

    return 0;
}
```

2. **Let's test**

```c
#include "komodor.h"

int main(){
    KomodorTest t = komodor_create_test("./myprogram'", NULL);

    komodor_define_test(
        &t,
        0,                      // exit code
        "saracalihan",          // stdin
        "Hello saracalihan!\n", // stdout
        NULL                    // stderr
    );

    if(komodor_exec_test(&t)){
        // PASS
    } else{
        // FAIL
    }

    // Now, you can access this fields
    //
    // t.expected_std_output
    // t.expected_std_error
    // t.expected_exit_code
    // t.std_input
    // t.std_output
    // t.std_error
    // t.exit_code
    // t.error_message // errno message or printf's "%m"

    return 0;
}
```

## Contributing

`// TODO: Not implementing yet!`

## License
This project is under the [GPLv3 license](./LICENSE).
