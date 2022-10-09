# c-connect

This library contains functions of networking and serialization for connecting your modules by
socket the easiest way possible.

# Installation guide

1. Clone the repository 

    a. Vía SSH
    ```bash
    git clone git@github.com:julian-salinas/c-connect.git
    ```

    b. Vía HTTPS
    ```bash
    git clone https://github.com/julian-salinas/c-connect.git
    ```

2. Enter the repository folder
```bash
cd c-connect
```

3. Compile and install the library
```bash
make
make install
```

# How to include the library in your files
Compile linking `-lconnect`

For including in your files, you must use the format `connect/networking.h` or `connect/serialization.h`

example:
```c
#include "connect/networking.h"
#include "connect/serialization.h"

int main(int argc, char** argv) {
    // ...
}
```