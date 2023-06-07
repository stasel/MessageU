import uuid


class Message:
    def __init__(self, messageId: int, toClient: uuid.UUID, fromClient: uuid.UUID, messageType: int, content: bytes):
        assert isinstance(messageId, int)
        assert isinstance(toClient, uuid.UUID)
        assert isinstance(fromClient, uuid.UUID)
        assert isinstance(messageType, int)
        assert isinstance(content, bytes)

        self.id = messageId
        self.toClient = toClient
        self.fromClient = fromClient
        self.type = messageType
        self.content = content
