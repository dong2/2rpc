```
1) autotools (autoconf, automake, libtool) must also be installed
sudo apt-get install autoconf automake libtool curl make g++ unzip

2) git clone https://github.com/google/protobuf.git
./autogen.sh
./configure
make && sudo make install
export LD_LIBRARY_PATH=/usr/local/lib

3) git clone https://github.com/protobuf-c/protobuf-c.git
./autogen.sh
./configure
make && sudo make install

4) install nanomsg
1.  `% mkdir build`
2.  `% cd build`
3.  `% cmake ..`
4.  `% cmake --build .`
5.  `% ctest .`
6.  `% sudo cmake --build . --target install`
7.  `% sudo ldconfig` (if on Linux)

4) protoc-c example.proto --c_out=./

5) protobuf example
gcc main.c -o main example.pb-c.c -lprotobuf-c -L ./protobuf-c/lib/ -I ./protobuf-c/include/protobuf-c
gcc main.c -o main example.pb-c.c `pkg-config --cflags --libs 'libprotobuf-c >= 1.0.0'`
g++ main.cc -o main example.pb.cc `pkg-config --cflags --libs protobuf`

6) reference
https://github.com/protobuf-c/protobuf-c/blob/master/README.md
https://github.com/protocolbuffers/protobuf/blob/master/src/README.md

7) wiki example
https://github.com/protobuf-c/protobuf-c/wiki/Examples

8) nanomsg + protobuf sample
protoc-c test.proto --c_out=./
gcc rpc_demo.c -o rpc_demo test.pb-c.c `pkg-config --cflags --libs 'libprotobuf-c >= 1.0.0'` -lnanomsg

```

protobuf keys
```
import
package
map
service
```
