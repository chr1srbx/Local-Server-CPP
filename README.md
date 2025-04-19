# TeamUp - Local WebSocket C++ Server

I strongly advise against using this for a server, its unsecure. You could implement the realtime part from here, if you wish.

## Features

- **WebSocket Communication**: Enables bi-directional real-time data transfer with C# clients.
- **JSON Handling**: Efficiently parses, processes, and responds to client actions.
- **Modular Architecture**: Clean separation between headers and implementation.
- **Local-Only**: Designed to run securely and fast on the client machine, not really designed in mind for a server.
- **No Database Needed**: All data is stored and managed via structured JSON files.

## Technologies Used

- **C++17**
- **WebSocket library (e.g., uWebSockets / Boost.Beast / custom)**  
- **nlohmann/json** for JSON parsing
- **Standard C++ STL**

## Architecture Overview

1. **WebSocket Server** (main.cpp):
   - Initializes and listens for client connections.
   - Accepts binary messages from the C# app.
   - Delegates message content to `jsonHandler`.

2. **JSON Handler** (jsonHandler.cpp/.h):
   - Parses JSON payloads.
   - Identifies the action (login, register, sync, etc.).
   - Sends back JSON responses formatted accordingly.

3. **Data Storage**:
   - All user data and workspace details are stored in `users.json`.
   - Example of user entry:
     ```json
     {
       "id": "secret_user_id",
       "username": "john123",
       "password": "password",
       "points": 100
     }
     ```

## Supported Actions

- **Register**: Adds a new user to the `users.json` file.
- **Login**: Authenticates a user using stored credentials.
- **SyncWorkspace**: Sends or receives workspace data in real time.
- **DeleteAccount**: Removes user entry from `users.json`.
(And some other account related actions)
