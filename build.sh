protoc-c test.proto --c_out=./

gcc main.c -o mian test.pb-c.c `pkg-config --cflags --libs 'libprotobuf-c >= 1.0.0'`

gcc client.c -o client test.pb-c.c `pkg-config --cflags --libs 'libprotobuf-c >= 1.0.0'`
gcc server.c -o server test.pb-c.c `pkg-config --cflags --libs 'libprotobuf-c >= 1.0.0'`

gcc rpc_demo.c -o rpc_demo test.pb-c.c `pkg-config --cflags --libs 'libprotobuf-c >= 1.0.0'` -lnanomsg
