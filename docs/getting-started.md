# How to use it
Compile linking `-lconnect`

For including in your files, you must use the format `connect/networking.h` and/or `connect/serialization.h`

example:
```c
#include <connect/networking.h>
#include <connect/serialization.h>

int main(int argc, char** argv) {
    // ...
}
```

# Usage examples
In the folder [example](../example) you will find an example of a client code and also an example of a server code. You can compile each one using the `makefile` of the module and see it by yourself.

```bash
cd example/server
make
./server.out
```

```bash
cd example/client
make
./client.out
```