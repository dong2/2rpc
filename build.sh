protoc-c test.proto --c_out=./

gcc rpc_demo.c -o rpc_demo test.pb-c.c `pkg-config --cflags --libs 'libprotobuf-c >= 1.0.0'` -lnanomsg
