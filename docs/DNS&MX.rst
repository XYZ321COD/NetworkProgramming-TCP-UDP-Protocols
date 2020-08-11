***************
DNS&MX
***************
Dns and mail exchanger. Program for a given domain name finds MX-es, connect(TCP port 25), read and print greetings.

Comments
--------------
Please remember to use option -lresolv while linking, libresolv is distributed by GNU C library, but gcc/g++ does not refer automatically

Compiling 
--------
 g++ -std=c++11 -pedantic -Wall  DNS.c -o main -lresolv
 
Adding symbol TRACE forces functions to print process on stdout

  g++ -std=c++11 -pedantic -Wall -DTRACE  DNS.c -o main -lresolv

