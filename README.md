# C ChatRoom Server & Client
A **multi-thread** Server implementing a chat room and a Client cummunicating each other with **Sockets**. Server side uses a Queue implementation as a data structure for storing the incoming clients with their data and can be found in the same folder as the Server C file.

# Libraries
Client side uses a library called *ncurses* and **Must** be installed before Client compilation. For Linux run: ```sudo apt-get install libncurses5-dev libncursesw5-dev```. If you are on Windows i would suggest to try *wsl*.

# Important notes to consider
Both **Server** and **Client** take as arguments in *main* a port number. That port number is the port that server will receiving calls from clients, so that port must be the same. **For Client** you must specify, *hardcoded* the **IP ADDRESS** which Server is running. The IP can be find in *line 326* in client.c file. By default is **Localhost**.

# Compile Server
From inside the server folder run: ```gcc server.c queue.c -o [EXECUTABLE_NAME_HERE] -pthread```

# Compile Client
From inside the client folder run: ```gcc client.c -o [EXECUTABLE_NAME_HERE] -pthread -lncurses```

# Bugs