import asyncio
import websockets
import json
from websocket_CommandOperations import CommandOperations
from websockets.exceptions import ConnectionClosedOK, ConnectionClosedError

class Command:
    def __init__(self):
        self.buffer = []
        self.arrivingCommand = asyncio.Condition()

    async def newCommand(self, command):
        async with self.arrivingCommand:
            self.buffer.append(command)
            self.arrivingCommand.notify_all()

    async def getCommand(self):
        async with self.arrivingCommand:
            if len(self.buffer) > 0:
                return self.buffer.pop(0)
            await self.arrivingCommand.wait()
            return self.buffer.pop(0) if self.buffer else ""


async def write_agent(websocket, path, command):
    try:
        while True:
            cmd = await command.getCommand()
            if cmd:
                # Serialize the result of getCommand, not the coroutine itself
                cmd_json = json.dumps(cmd) if not isinstance(cmd, str) else cmd
                await websocket.send(cmd_json)
                # Process the command JSON to check for notifications
                try:
                    cmd_json = json.loads(cmd)

                    if cmd['response'] == "Login successful and this is your token: ":
                        token = cmd_json.get('token', '')

                    token = cmd_json.get('token', '')
                    # Run the synchronous get_user function in a separate thread
                    user = await asyncio.get_event_loop().run_in_executor(None, CommandOperations.get_user, token)
                    notify_message = await CommandOperations.send_notify(user)
                    if notify_message:
                        await websocket.send(notify_message)
                except json.JSONDecodeError:
                    print("Error decoding JSON command")
    except ConnectionClosedOK:
        print("Client is terminating")
    except ConnectionClosedError:
        print("Client generated error")
    except Exception as e:
        print(f"Unexpected error occurred: {e}")
    finally:
        print("Connection closed")

async def read_agent(websocket, path, command):
    try:
        while True:
            raw_command = await websocket.recv()
            if raw_command == '{"command":"close connection"}':
                break

            print(f"Received command: {raw_command}")
            # Await the process_command coroutine
            processed_command = await CommandOperations.process_command(raw_command)
            await command.newCommand(processed_command)
    except ConnectionClosedOK:
        print("Client is terminating")
    except ConnectionClosedError:
        print("Client generated error")
    finally:
        print("Connection closed")


async def main():
    commandProcessor = Command()
    async with websockets.serve(lambda ws, path: asyncio.gather(write_agent(ws, path, commandProcessor), read_agent(ws, path, commandProcessor)), "", 12345):
        await asyncio.Future()  # run forever


if __name__ == "__main__":
    asyncio.run(main())
