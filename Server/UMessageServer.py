import socket
from DBClient import DBClient
from ClientConnection import ClientConnection
import traceback
import threading

class UMessageServer:
    def __init__(self, port: int, version: int, db: DBClient):
        assert isinstance(port, int)
        assert isinstance(version, int)
        assert (0 < port <= 65535)
        assert isinstance(db, DBClient)
        self.port = port
        self.version = version
        self.db = db
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def start(self) -> None:
        self.socket.bind(("", self.port))
        self.socket.listen()
        print(f"UMessage server started and listening to port {self.port}")

        while True:
            clientSocket, address = self.socket.accept()
            print(f"Connection from {address}")
            try:
                thread = threading.Thread(target=self.handleConnection, args=(clientSocket,))
                thread.start()
            except Exception as ex:
                print(f"❌ Error occurred: {ex}")
                print(traceback.format_exc())

    def handleConnection(self, clientSocket):
        client = ClientConnection(clientSocket, self.db)
        client.handle()

    def stop(self) -> None:
        self.socket.close()
        print("Server stopped")

