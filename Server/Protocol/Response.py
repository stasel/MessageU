from __future__ import annotations
import uuid
from enum import Enum
from Message import Message
from Client import Client
import struct
import Constants


class ResponseCode(Enum):
    REGISTER_SUCCESS = 2000
    CLIENT_LIST = 2001
    PUBLIC_KEY = 2002
    MESSAGE_SENT = 2003
    MESSAGE_LIST = 2004
    SERVER_ERROR = 9000


class Response:
    def __init__(self, version: int, code: ResponseCode):
        assert isinstance(version, int)
        assert isinstance(code, ResponseCode)
        self.version = version
        self.code = code

    def pack(self, payload=bytes()) -> bytes:
        return struct.pack("<BHI", self.version, self.code.value, len(payload)) + payload

    def __str__(self) -> str:
        return f"[Response] {self.code.value}({self.code.name})"


class RegisterSuccessResponse(Response):
    def __init__(self, version: int, clientID: uuid.UUID):
        super().__init__(version, ResponseCode.REGISTER_SUCCESS)
        assert isinstance(clientID, uuid.UUID)
        self.clientID = clientID

    def pack(self) -> bytes:
        payload = self.clientID.bytes
        return super().pack(payload)


class ClientListResponse(Response):
    def __init__(self, version: int, clientList: [Client]):
        super().__init__(version, ResponseCode.CLIENT_LIST)
        assert isinstance(clientList, list)
        self.clientList = clientList

    def pack(self) -> bytes:
        payload = bytes()
        for client in self.clientList:
            payload += client.id.bytes
            payload += bytes(client.name, Constants.STR_ENCODING).ljust(Constants.CLIENT_NAME_MAX_LENGTH, b'\0')
        return super().pack(payload)


class PublicKeyResponse(Response):
    def __init__(self, version: int, clientID: uuid.UUID, publicKey: bytes):
        super().__init__(version, ResponseCode.PUBLIC_KEY)
        assert isinstance(clientID, uuid.UUID)
        assert isinstance(publicKey, bytes)
        self.clientID = clientID
        self.publicKey = publicKey

    def pack(self) -> bytes:
        payload = self.clientID.bytes + self.publicKey
        return super().pack(payload)


class MessageSentResponse(Response):
    def __init__(self, version: int, toClientID: uuid.UUID, messageID: int):
        super().__init__(version, ResponseCode.MESSAGE_SENT)
        assert isinstance(toClientID, uuid.UUID)
        assert isinstance(messageID, int)
        self.toClientID = toClientID
        self.messageID = messageID

    def pack(self) -> bytes:
        payload = self.toClientID.bytes + struct.pack("<I", self.messageID)
        return super().pack(payload)


class MessageListResponse(Response):
    def __init__(self, version: int, messages: [Message]):
        super().__init__(version, ResponseCode.MESSAGE_LIST)
        assert isinstance(messages, list)
        self.messages = messages

    def pack(self) -> bytes:
        payload = bytes()
        for message in self.messages:
            payload += message.fromClient.bytes
            payload += struct.pack("<IBI", message.id, message.type, len(message.content))
            payload += message.content
        return super().pack(payload)


class ServerErrorResponse(Response):
    def __init__(self, version: int):
        super().__init__(version, ResponseCode.SERVER_ERROR)
