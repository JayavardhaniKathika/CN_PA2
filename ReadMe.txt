Programming Assignment 2:
Client using customized protocol on top of UDP protocol for requesting identification from server for access permission to the network.
One client connects to one server.

Pre-requisite:
Make sure you have gcc compiler

How to compile and run the files in mac:

1. Copy 'modelclient.c', 'modelserver.c', 'Subscriber_Data.txt' and 'Verification_Database.txt' into a file.
2.Change the current path to the location of this file. (Use pwd to find current location and change to the desired file location using cd)
3. Run the below commands to compile the C programs:
	gcc modelclient.c -o mclient
	gcc modelserver.c -o mserver
4. Use command+d to open terminal windows side by side. Run the server first on left side window. To start the server, use the below command:
	./mserver
5. Run client on right side window. To run the client use:
	./mclient
6. Packets start transmitting and output will be displayed





