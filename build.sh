protoc-c test.proto --c_out=./
gcc main.c -o mian test.pb-c.c `pkg-config --cflags --libs 'libprotobuf-c >= 1.0.0'`
