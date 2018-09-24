Monish Nene

I have done all operations put, get, delete, ls and exit.

I have created 4 extra files for ease of coding. 

client_support.c	client_support.h
server_support.c	server_support.h

I have makefile in both folder which link these files during compilation.

udp_client.c and udp_server.c have the main function.

Use 'make' command in each folder to compile the code.

I have added targets in makefile for easy upload 'make upload' and easy ssh 'make go' in server makefile.

I have used acknowledge method for reliability which slows down the remote get and put proceddure.

The files are reliably transfered on localhost.
File size upto 12MB.

To run the file on local host use the command 'make local' or both client and server

