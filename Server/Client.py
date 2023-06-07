import uuid
from datetime import datetime
from typing import Optional


class Client:
    def __init__(self, clientID: uuid.UUID, name: str, publicKey: bytes, lastSeen: Optional[datetime] = None):
        assert isinstance(clientID, uuid.UUID)
        assert isinstance(name, str)
        assert isinstance(publicKey, bytes)
        assert lastSeen is None or isinstance(lastSeen, datetime)

        self.id = clientID
        self.name = name
        self.publicKey = publicKey
        self.lastSeen = lastSeen
