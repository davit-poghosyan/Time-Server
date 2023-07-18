# Time Server

This is a simple time server implemented in C. It creates a TCP server that listens on port 8080 and responds to client requests with the current local time.
Features

    Listens for incoming client connections and responds with the current local time.
    Supports both Windows and Linux-based platforms.
    Uses the Berkeley socket API on Linux and the Winsock API on Windows.
    Handles basic HTTP requests and sends a simple HTTP response.

## Requirements

### C Compiler:

    For Windows:
        MinGW or Microsoft Visual Studio.
    For Linux:
        GCC (GNU Compiler Collection).

### Headers:

    For Windows:
        winsock2.h, ws2tcpip.h headers
    For Linux:
       sys/types.h, sys/socket.h, netinet/in.h, arpa/inet.h, netdb.h, unistd.h, and errno.h headers.
## Usage

### Clone the repository:

    git clone https://github.com/your-username/time-server.git

### Compile the source code:

For Windows:

    Open the project in an IDE or run the appropriate build command for your development environment.

For Linux:

    gcc time_server.c -o time_server

#### Run the compiled executable:
For Windows:

    time_server.exe

For Linux:

    ./time_server

#### The time server will start listening for connections on port 8080. You can access it by opening a web browser and navigating to http://127.0.0.1:8080


##Contributing

Contributions are welcome! If you find any issues or have suggestions for improvements, please feel free to open an issue or submit a pull request.
