# tcp_client_server
C++ tcp client / server

Software Recruiting – Take Home  

Emerson wants to learn more about your work-related skills and abilities. I encourage you to frame this assignment not as a pass/fail but rather a show-and-tell of your hard skills.   

The assignment will be completed remotely at your convenience, and we expect the assignment to take 3-5 hours to complete.  
 
This code should be hosted in a git repository that you would be willing to grant temporary access to. 

 

TCP Socket Server Assignment  
Using C/C++ design a simple socket server that will achieve the following goals: 

1) Host a TCP server on port 5000 
2) Accept all incoming connections and maintain them until a close event is received 
3) On receipt of a "Close" event only the active socket should be disconnected 
4) If the server has not received a message in 30 seconds from a client it should terminate that connection 
5) If the server receives 10 messages (excluding "Heartbeat") from a single client it should consider that connection complete and disconnect 
6) On receipt of an "Exit" should stop listening to the port and terminate the app. 
7) Should compile on a LTS version of ubuntu (preferably 18.04) but code should support a quick port to a new network stack.  

Using C/C++ design a simple socket client that will achieve the following goals: 

1) Establish a TCP connection to a local server on port 5000 
2) The client should maintain the connection and allow the following actions on command: 
  a) send a "Close" event to the socket
  b) send a custom string to the socket ie "Hello World" 
  c) enter persist mode where it will send a new "Heartbeat" message every 15 seconds so the socket doesn't close 

3) Should compile on a LTS version of ubuntu (preferably 18.04) but code should support a quick port to a new network stack.  

NOTE:  

Defining "Close", "Exit", and "Heartbeat" messages is left to the implementor.  Use your knowledge of TCP protocols and/or standards to craft your protocol. 
