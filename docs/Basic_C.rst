***************
Basic_C
***************
Quick recap of C programming. Input-output system calls in C | Create, Open, Close, Read, Write.

Content
--------
* print_1.c - Function tests if buffer contains only printable ASCII characters, i.e. byte value between [32, 126].
 Two variants: first function reads from buffer using operation of indexing table, second one function reads from buffer using pointer      which moves on buffer byte by byte.  Signature of function: bool print(const void * buf, int len);
* print_2.c - same as above but
 Signatrue of function: bool print(const char * buf);
* copy.c - copy files, using Input-output system calls.
 Input data(files names), are command line arguments.
* copy_mod.c - copy only odd lines, using Input-output system calls.
 Input data(files names), are command line arguemnts.
