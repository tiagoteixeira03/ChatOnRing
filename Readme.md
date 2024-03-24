```markdown
# ChatOnRing Project

Welcome to the ChatOnRing project repository! This project is a chat application developed as part of the Computer Networks and Internet course. Below you will find information on how to use and showcase this project.

## Project Overview
ChatOnRing is a chat application based on a ring network structure. It allows nodes to communicate within the network using TCP and UDP protocols. The application supports node registration, message exchange, and network visualization functionalities.

## Usage
To use the ChatOnRing application, follow these steps:

1. Clone the repository to your local machine.
2. Compile the source code using the provided makefile.
3. Run the executable file to start the chat application with the following arguments:
`./COR "local_ip" "local_port" "nodes_server_ip" "nodes_server_port"`
4. Use the available commands:
- `join (j) ring id`: Join a chat ring by specifying the ring identifier (3 digit number) and your id (2 digit number)
- `direct join (dj) id succid succIP succTCP`: Directly join the ring chat by contacting the specified node in succid succIP succTCP
- `show topology (st)`: Shows the identifiers and contacts of your node, your successor, and your second successor
- `show routing (sr) dest`: Shows the routing chart of a node relative to a destiny dest (if `dest` is specified as "all" it will show the complete table)
- `show path (sp) dest`: Shows the shortest path from a node to a destiny (if `dest` is specified as "all" it will show the complete table)
- `show forwarding (sf)`: Shows the forwarding chart
- `message (m) dest message`: Send a message to the destiny node
- `leave (l)`: Leave the ring chat
- `exit (x)`: Leave the application

## Features
- Join the ring network with a unique identifier.
- Leave the network when needed.
- Directly join a specific node in the network.
- View the current network topology and routing layers.
- Send messages to other nodes in the network.
```