# Bougas-ChatRoom Server & Client
A **multi-threaded** Server implementing a chat room and a Client cummunicating each other with **Sockets** witten in **C**. Server side uses a Queue implementation as a data structure for storing the incoming clients with their data and can be found in the same folder as the Server C file. All communications between clients are done via terminal.

## Libraries
Client side uses a library called *ncurses* and **Must** be installed before Client compilation. For Linux run: ```sudo apt-get install libncurses5-dev libncursesw5-dev```. If you are on Windows i would suggest to try [WSL](https://docs.microsoft.com/en-us/windows/wsl/install-win10).

## Important notes to consider
Both **Server** and **Client** take as arguments, in *main*, a port number. That port number is the port that server will receiving calls from clients, so that port must be the same. 

**For Client** you must specify, *hardcoded* the **IP ADDRESS** which Server is running. The IP can be find at *line 326* in client.c file. By default is **Localhost**.Client can sign up creating a new user or sign in if he already has one. User credentials are stored in a file under the *Server folder*. If a client wants to leave the chat, he can kill the proccess or just type **exit**.

Lastly, **in Server** side, there is a **keyword** that the client must know(must provide it when prompt) in order to create a new user(sign up). That keyword is on *server.c* file at *line 101* and by default is **PLANHTARXHS**. Server provides the essential logs when running, like when a new ip tries to sign up or log in and all the messages that are written in the Chatroom.

## Compile Server
From inside the server folder run: ```gcc server.c queue.c -o [EXECUTABLE_NAME_HERE] -pthread```

## Compile Client
From inside the client folder run: ```gcc client.c -o [EXECUTABLE_NAME_HERE] -pthread -lncurses```

## Bugs
There are 2 big known bugs at the moment, **first** there is a bug preventing two users to sign in or sign up at the same time! It's a big problem but the fix must be easy and has something to do with client and server communication.
The **second** one is that while a user writes something on terminal and hasn't pressed enter to send and at that time another user sends a messsage, it disappears the text from the user and you may think your text has gone but actually if you hit enter all the text will be send correctly. So it's a visual bug and a small one. Lastly it's not a bug but worth mentioning, when a user writes his password it cannot hit backspace to erase it, and this happens cause password is written in another graphical ui via the *ncurses* library.

## From Owner
I don't have the time to continue this project which i have done for educational purposes. So no future updates i guess. Feel free to send me anything related and also i hope this code proves useful to you.

## Run-Images
**From Server:** ![alt text](https://github.com/Panagiss/BougasChatroom-server-client/blob/master/sample-images/Screenshot%20from%202020-11-15%2013-00-15.png "Server")
**From Client:** ![alt text](https://github.com/Panagiss/BougasChatroom-server-client/blob/master/sample-images/Screenshot%20from%202020-11-15%2013-00-23.png "Client")
