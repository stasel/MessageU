from __future__ import annotations
import uuid
from enum import Enum


class RequestCode(Enum):
    REGISTER = 1000
    CLIENT_LIST = 1001
    GET_PUBLIC_KEY = 1002
    SEND_MESSAGE = 1003
    GET_MESSAGES = 1004


class Request:
    def __init__(self, clientID: uuid.UUID, version: int, code: RequestCode):
        assert isinstance(clientID, uuid.UUID)
        assert isinstance(version, int)
        assert isinstance(code, RequestCode)

        self.clientID = clientID
        self.version = version
        self.code = code

    def __str__(self) -> str:
        return f"[Request] {self.code.name} ({self.code.value}) client: {self.clientID}"


class RegisterRequest(Request):
    def __init__(self, version: int, name: str, publicKey: bytes):
        super().__init__(uuid.UUID("00000000-00000000-00000000-00000000"), version, RequestCode.REGISTER)
        assert isinstance(name, str)
        assert isinstance(publicKey, bytes)
        self.name = name
        self.publicKey = publicKey


class ClientListRequest(Request):
    def __init__(self, clientID: uuid.UUID, version: int):
        super().__init__(clientID, version, RequestCode.CLIENT_LIST)


class GetPublicKeyRequest(Request):
    def __init__(self, clientID: uuid.UUID, version: int, otherClientID: uuid.UUID):
        super().__init__(clientID, version, RequestCode.GET_PUBLIC_KEY)
        assert isinstance(otherClientID, uuid.UUID)
        self.otherClientID = otherClientID


class SendMessageRequest(Request):
    def __init__(self, clientID: uuid.UUID, version: int, toClientID: uuid.UUID, messageType: int, content: bytes):
        super().__init__(clientID, version, RequestCode.SEND_MESSAGE)
        assert isinstance(toClientID, uuid.UUID)
        assert isinstance(messageType, int)
        assert isinstance(content, bytes)
        self.toClientID = toClientID
        self.messageType = messageType
        self.messageContent = content


class GetMessagesRequest(Request):
    def __init__(self, clientID: uuid.UUID, version: int):
        super().__init__(clientID, version, RequestCode.GET_MESSAGES)
