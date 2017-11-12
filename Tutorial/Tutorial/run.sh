clear
gcc -std=c++11 \
sender.cc test.cc \
-o out \
-lws2_32
./out
