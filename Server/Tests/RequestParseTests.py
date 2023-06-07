from ClientConnection import ClientConnection
from Tests.TestUtils import *


class RequestParseTests(TestCase):
    def testRegisterRequest(self):
        # Given
        expectedRequest = RegisterRequest(VERSION, "stasel", publicKey1)
        inputData = uuid.uuid4().bytes + b'\x01' + b'\xe8\x03' + b'\x9f\x01\x00\x00'
        inputData += bytes(expectedRequest.name, "utf-8").ljust(255, b'\x00')
        inputData += expectedRequest.publicKey
        db = DBClientMock()
        connection = ClientConnection(SocketMock(inputData), db)

        # When
        actualRequest = connection.parseRequest()

        # Then
        assert isinstance(actualRequest, RegisterRequest)
        Assert.equals(actualRequest.version, expectedRequest.version)
        Assert.equals(actualRequest.name, expectedRequest.name)
        Assert.equals(actualRequest.publicKey, expectedRequest.publicKey)

    def testClientListRequest(self):
        # Given
        expectedRequest = ClientListRequest(dummyClient1.id, VERSION)
        inputData = expectedRequest.clientID.bytes + b'\x01' + b'\xe9\x03' + b'\x00\x00\x00\x00'
        db = DBClientMock()
        connection = ClientConnection(SocketMock(inputData), db)

        # When
        actualRequest = connection.parseRequest()

        # Then
        assert isinstance(actualRequest, ClientListRequest)
        Assert.equals(actualRequest.clientID, expectedRequest.clientID)
        Assert.equals(actualRequest.version, expectedRequest.version)

    def testGetPublicKeyRequest(self):
        # Given
        expectedRequest = GetPublicKeyRequest(dummyClient1.id, VERSION, dummyClient2.id)
        inputData = expectedRequest.clientID.bytes + b'\x01' + b'\xea\x03' + b'\x10\x00\x00\x00'
        inputData += expectedRequest.otherClientID.bytes
        db = DBClientMock()
        connection = ClientConnection(SocketMock(inputData), db)

        # When
        actualRequest = connection.parseRequest()

        # Then
        assert isinstance(actualRequest, GetPublicKeyRequest)
        Assert.equals(actualRequest.clientID, expectedRequest.clientID)
        Assert.equals(actualRequest.version, expectedRequest.version)
        Assert.equals(actualRequest.otherClientID, expectedRequest.otherClientID)

    def testGetPublicKeyRequest(self):
        # Given
        expectedRequest = GetPublicKeyRequest(dummyClient1.id, VERSION, dummyClient2.id)
        inputData = expectedRequest.clientID.bytes + b'\x01' + b'\xea\x03' + b'\x10\x00\x00\x00'
        inputData += expectedRequest.otherClientID.bytes
        db = DBClientMock()
        connection = ClientConnection(SocketMock(inputData), db)

        # When
        actualRequest = connection.parseRequest()

        # Then
        assert isinstance(actualRequest, GetPublicKeyRequest)
        Assert.equals(actualRequest.clientID, expectedRequest.clientID)
        Assert.equals(actualRequest.version, expectedRequest.version)
        Assert.equals(actualRequest.otherClientID, expectedRequest.otherClientID)

    def testGetPublicKeyRequest(self):
        # Given
        expectedRequest = SendMessageRequest(dummyClient1.id, VERSION, dummyClient2.id, 51, b"Hello world")
        inputData = expectedRequest.clientID.bytes + b'\x01' + b'\xeb\x03' + b'\x20\x00\x00\x00'
        inputData += expectedRequest.toClientID.bytes + b'\x33' + b'\x0b\x00\x00\x00' + expectedRequest.messageContent
        db = DBClientMock()
        connection = ClientConnection(SocketMock(inputData), db)

        # When
        actualRequest = connection.parseRequest()

        # Then
        assert isinstance(actualRequest, SendMessageRequest)
        Assert.equals(actualRequest.clientID, expectedRequest.clientID)
        Assert.equals(actualRequest.version, expectedRequest.version)
        Assert.equals(actualRequest.toClientID, expectedRequest.toClientID)
        Assert.equals(actualRequest.messageType, expectedRequest.messageType)
        Assert.equals(actualRequest.messageContent, expectedRequest.messageContent)


    def testGetPublicKeyRequest_Long(self):
        # Given
        longMessage = secrets.token_bytes(500000)
        expectedRequest = SendMessageRequest(dummyClient1.id, VERSION, dummyClient2.id, 51, longMessage)
        inputData = expectedRequest.clientID.bytes + b'\x01' + b'\xeb\x03' + b'\x35\xa1\x07\x00'
        inputData += expectedRequest.toClientID.bytes + b'\x33' + b'\x20\xa1\x07\x00' + expectedRequest.messageContent
        db = DBClientMock()
        connection = ClientConnection(SocketMock(inputData), db)

        # When
        actualRequest = connection.parseRequest()

        # Then
        assert isinstance(actualRequest, SendMessageRequest)
        Assert.equals(actualRequest.clientID, expectedRequest.clientID)
        Assert.equals(actualRequest.version, expectedRequest.version)
        Assert.equals(actualRequest.toClientID, expectedRequest.toClientID)
        Assert.equals(actualRequest.messageType, expectedRequest.messageType)
        Assert.equals(actualRequest.messageContent, expectedRequest.messageContent)


    def testGetMessagesRequest(self):
        # Given
        expectedRequest = GetMessagesRequest(dummyClient1.id, VERSION)
        inputData = expectedRequest.clientID.bytes + b'\x01' + b'\xec\x03' + b'\x00\x00\x00\x00'
        db = DBClientMock()
        connection = ClientConnection(SocketMock(inputData), db)

        # When
        actualRequest = connection.parseRequest()

        # Then
        assert isinstance(actualRequest, GetMessagesRequest)
        Assert.equals(actualRequest.clientID, expectedRequest.clientID)
        Assert.equals(actualRequest.version, expectedRequest.version)
