## A simple and complete example of protobuf-c

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

4) protoc-c example.proto --c_out=./

5) example
gcc main.c -o main example.pb-c.c -lprotobuf-c -L ./protobuf-c/lib/ -I ./protobuf-c/include/protobuf-c
gcc main.c -o main example.pb-c.c `pkg-config --cflags --libs 'libprotobuf-c >= 1.0.0'`
g++ main.cc -o main example.pb.cc `pkg-config --cflags --libs protobuf`

6) reference
https://github.com/protobuf-c/protobuf-c/blob/master/README.md
https://github.com/protocolbuffers/protobuf/blob/master/src/README.md

7) wiki example
https://github.com/protobuf-c/protobuf-c/wiki/Examples


test sample
protoc-c test.proto --c_out=./
gcc main.c -o main test.pb-c.c `pkg-config --cflags --libs 'libprotobuf-c >= 1.0.0'`
```
