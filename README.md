# Operating Systems Final Project Report

## Server part

There are two ways to do synchronization in a server, Multi-processes and Multi-threads. They
all belong to multitask programming. The difference between them is that multi-processes need
to have different memory space, but multi-threads may share the same memory address. Using
thread will save system cost because it doesn’t need to flash the memory frequently, but it is not
safe to share.

By building the socket, socket(), bind(), listen() only run once, but accept() needs to run every
time a request comes. So building the thread need to include the return value of accept().
In the origin code, clients[n] helps to fork a process in a sequent order, but I changed it in to
thread, and only use one of the clients[] because I have only one process.

Structure Addrinfo is an API that is used by the getaddrinfo function to hold host address
information. The main attribute of it is, “geraddrinfo() + addrinfo()” is a much fashion way to build
a server compare with “gethostbyname() + sockaddr_in()”. In a successful call to getaddrinfo, a
linked of list of addrinfo structures is returned in res parameter passed to the getaddrinfo
function.

## Clients part

First, the clients could send GET request, In curl, by default, lacking of reserved words, acts as
GET request, the client needs to send synchronized requests.

Time command here means that returning a time statistics about consumption of processor
time. Including three kinds of time: 1) elapsed real time, 2) user CPU time (the sum of the
tms_utime and tms_cutime values in a struct tms. 3) the system CPU time (the sum of the
tms_stime and tms_cstime values.

## Some experience I get from this project

1, Understand the theory thoroughly before try to code.
I try to understand the source code before changing it, but just understand the sketch like which
part doing what function, but when I try to program, I don’t know which part goes wrong. And
read them twice. So I spent a lot of time debugging the code, wasting lot of time.

2, Make the problem simple, but actually it is not as simple as I think.
Some simple functions may need a lot of time to solve.

3, I think I am not good at finding the critical steps for the whole project, I need to read and think
a lot to figure out what to do first and next.
I read something that I can’t do before the critical step I need to do first.

## Bugs

1, Segmentation fault: 11  
Reason: when reading the memory, the pointer is used by multiple thread at the same time

2, Type errors:  
Reason: too many pointers in it.

## Reference

[1] POSIX thread (pthread) libraries. https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html.  
[2] Multithreaded Programming (POSIX pthreads Tutorial). https://randu.org/tutorials/threads/.  
[3] Curl tutorial. https://curl.haxx.se/docs/manual.html.  
[4] ADDRINFOA structure. https://docs.microsoft.com/en-us/windows/win32/api/ws2def/ns-ws2def-addrinfoa.  
[5] HTTP Server: Everything you need to know to Build a simple HTTP server from scratch. https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa.  
