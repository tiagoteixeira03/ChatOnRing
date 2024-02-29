# Computer Networks Project

## Commands

- `join (j) ring id`: Join a chat ring by specifying the ring identifier (3 digit number) and your id (2 digit number)
- `direct join (dj) id succid succIP succTCP`: Directly join the ring chat by contacting the specified node in succid succIP succTCP
- `chord (c)`: Establishes a chord between your node and any other node besides the ones adjacent to you
- `remove chord (rc)`: Removes the last established chord
- `show topology (st)`: Shows the identifiers and contacts of your node, your successor, and your second successor
- `show routing (sr) dest`: Shows the routing chart of a node relative to a destiny dest
- `show path (sp) dest`: Shows the shortest path from a node to a destiny
- `show forwarding (sf)`: Shows the forwarding chart
- `message (m) dest message`: Send a message to the destiny node
- `leave (l)`: Leave the ring chat
- `exit (x)`: Leave the application
