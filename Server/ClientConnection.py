from DBClient import DBClient
import socket
from Protocol.Request import *
from Protocol.Response import *
import Constants
import uuid
import struct
import re

BUFFER_SIZE = 4096


class ClientConnection:
    def __init__(self, clientSocket: socket.socket, db: DBClient):
        assert isinstance(clientSocket, socket.socket)
        assert isinstance(db, DBClient)
        self.socket = clientSocket
        self.db = db

    def handle(self):
        response = None
        request = None
        try:
            request = self.parseRequest()
            print(f"--> {request}")
            response = self.handleRequest(request)
        except Exception as ex:
            print("Error handling request:", ex)
            response = ServerErrorResponse(Constants.SERVER_VERSION)
        finally:
            self.sendResponse(response)
            self.socket.close()
            print(f"<-- {response}")

        # Delete the messages sent to client if the sent was successful.
        if isinstance(response, MessageListResponse):
            self.db.deleteMessages([message.id for message in response.messages])

        if request is not None:
            self.db.updateLastSeen(request.clientID)

    def parseRequest(self) -> Request:
        # Get header
        data = self.socket.recv(BUFFER_SIZE)
        clientID = uuid.UUID(data[0:Constants.CLIENT_ID_SIZE].hex())
        version, rawCode, payloadSize = struct.unpack_from("<BHI", data, Constants.CLIENT_ID_SIZE)
        code = RequestCode(rawCode)

        # Get all payload in chunks
        payload = data[Constants.CLIENT_ID_SIZE + 7:]
        while len(payload) < payloadSize:
            chunk = self.socket.recv(BUFFER_SIZE)
            if len(chunk) == 0:
                break
            payload += chunk[0:min(payloadSize - len(payload), BUFFER_SIZE)]

        if code == RequestCode.REGISTER:
            nameEndIndex = min(payload.index(b'\0'), Constants.CLIENT_NAME_MAX_LENGTH)
            name = str(payload[:nameEndIndex], Constants.STR_ENCODING)
            publicKeyStartIndex = Constants.CLIENT_NAME_MAX_LENGTH
            publicKeyEndIndex = publicKeyStartIndex + Constants.PUBLIC_KEY_SIZE
            publicKey = payload[publicKeyStartIndex:publicKeyEndIndex]
            return RegisterRequest(version, name, publicKey)

        elif code == RequestCode.CLIENT_LIST:
            return ClientListRequest(clientID, version)

        elif code == RequestCode.GET_PUBLIC_KEY:
            otherClientID = uuid.UUID(payload[:Constants.CLIENT_ID_SIZE].hex())
            return GetPublicKeyRequest(clientID, version, otherClientID)

        elif code == RequestCode.SEND_MESSAGE:
            toClientID = uuid.UUID(payload[:Constants.CLIENT_ID_SIZE].hex())
            messageType, size = struct.unpack_from("<BI", payload, Constants.CLIENT_ID_SIZE)
            content = payload[Constants.CLIENT_ID_SIZE + 5:Constants.CLIENT_ID_SIZE + 5 + size]
            return SendMessageRequest(clientID, version, toClientID, messageType, content)

        elif code == RequestCode.GET_MESSAGES:
            return GetMessagesRequest(clientID, version)

        raise Exception("Unknown request status code")

    def handleRequest(self, request) -> Response:
        assert isinstance(request, Request)
        if isinstance(request, RegisterRequest):
            return self.handleRegisterRequest(request)
        elif isinstance(request, ClientListRequest):
            return self.handleClientListRequest(request)
        elif isinstance(request, GetPublicKeyRequest):
            return self.handleGetPublicKeyRequest(request)
        elif isinstance(request, SendMessageRequest):
            return self.handleSendMessageRequest(request)
        elif isinstance(request, GetMessagesRequest):
            return self.handleGetMessagesRequest(request)

        raise Exception("Unknown request")

    def handleRegisterRequest(self, request: RegisterRequest) -> Response:
        assert isinstance(request, RegisterRequest)

        if not re.match("^[\x20-\x7E]+$", request.name):
            raise Exception("Invalid username format. Allowed only ascii characters.")
        if self.db.isClientNameExists(request.name):
            raise Exception("Client with the same name already exists.")

        newClient = Client(uuid.uuid4(), request.name, request.publicKey)
        self.db.addClient(newClient)
        return RegisterSuccessResponse(Constants.SERVER_VERSION, newClient.id)

    def handleClientListRequest(self, request: ClientListRequest) -> Response:
        assert isinstance(request, ClientListRequest)
        self.checkClientID(request.clientID)

        clients = filter(lambda client: client.id != request.clientID, self.db.getAllClients())
        return ClientListResponse(Constants.SERVER_VERSION, list(clients))

    def handleGetPublicKeyRequest(self, request: GetPublicKeyRequest) -> Response:
        assert isinstance(request, GetPublicKeyRequest)
        self.checkClientID(request.clientID)

        otherClient = self.db.getClient(request.otherClientID)
        if otherClient is None:
            raise Exception(f"Client with id '{request.otherClientID}' was not found")
        return PublicKeyResponse(Constants.SERVER_VERSION, otherClient.id, otherClient.publicKey)

    def handleSendMessageRequest(self, request: SendMessageRequest) -> Response:
        assert isinstance(request, SendMessageRequest)
        self.checkClientID(request.clientID)

        newMessage = Message(0, request.toClientID, request.clientID, request.messageType, request.messageContent)
        newMessage.id = self.db.addMessage(newMessage)
        return MessageSentResponse(Constants.SERVER_VERSION, newMessage.toClient, newMessage.id)

    def handleGetMessagesRequest(self, request: GetMessagesRequest) -> Response:
        assert isinstance(request, GetMessagesRequest)
        self.checkClientID(request.clientID)

        messages = self.db.getAllMessages(request.clientID)
        return MessageListResponse(Constants.SERVER_VERSION, messages)

    def checkClientID(self, clientID: uuid.UUID):
        if not self.db.isClientIDExists(clientID):
            raise Exception("Unknown clientID")

    def sendResponse(self, response: Response) -> None:
        data = response.pack()
        total = len(data)
        sent = 0

        while sent < total:
            self.socket.send(data[sent:min(sent+BUFFER_SIZE, total)])
            sent += BUFFER_SIZE
