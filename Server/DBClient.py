import datetime
import sqlite3
import uuid
from Client import Client
from Message import Message
from threading import Lock

class DBClient:
    def __init__(self, dbPath: str):
        assert isinstance(dbPath, str)

        self.dbPath = dbPath
        self.connection = None
        self.__lock = Lock()

    def connect(self) -> None:
        if self.connection is None:
            self.connection = sqlite3.connect(self.dbPath, isolation_level=None, check_same_thread=False)
            self.connection.row_factory = sqlite3.Row
            self.initializeDB()

    def initializeDB(self) -> None:
        sqlClients = """
            CREATE TABLE IF NOT EXISTS clients (
                id BLOB NOT NULL PRIMARY KEY,
                name text NOT NULL,
                public_key BLOB  NOT NULL,
                last_seen datetime
            );
        """

        sqlMessages = """
            CREATE TABLE IF NOT EXISTS messages (
                id INTEGER NOT NULL PRIMARY KEY autoincrement,
                to_client BLOB NOT NULL
                    CONSTRAINT messages_clients_id_fk_to_client REFERENCES clients,
                from_client BLOB NOT NULL
                    CONSTRAINT messages_clients_id_fk_from_client REFERENCES clients,
                type INTEGER NOT NULL,
                content BLOB NOT NULL
            );
        """

        self.safeExecute(sqlClients)
        self.safeExecute(sqlMessages)

    def addClient(self, client: Client) -> None:
        assert isinstance(client, Client)

        sql = """
            INSERT INTO clients (id, name, public_key, last_seen)
            VALUES (?,?,?,?)
        """
        self.safeExecute(sql, (client.id.bytes, client.name, client.publicKey, client.lastSeen))

    def isClientNameExists(self, name: str) -> bool:
        assert isinstance(name, str)

        sql = "SELECT name FROM clients WHERE name = ?"
        firstClient = self.connection.execute(sql, (name,)).fetchone()
        return firstClient is not None

    def isClientIDExists(self, clientID: uuid.UUID) -> bool:
        assert isinstance(clientID, uuid.UUID)

        sql = "SELECT name FROM clients WHERE id = ?"
        firstClient = self.connection.execute(sql, (clientID.bytes,)).fetchone()
        return firstClient is not None

    def getClient(self, clientId: uuid.UUID):
        sql = "SELECT * FROM clients where id = ?"
        row = self.connection.execute(sql, (clientId.bytes,)).fetchone()
        if row is None:
            return None
        lastSeen = None
        if row['last_seen'] is not None:
            lastSeen = datetime.datetime.fromisoformat(row['last_seen'])
        return Client(uuid.UUID(row['id'].hex()), row['name'], row['public_key'], lastSeen)

    def getAllClients(self) -> [Client]:
        sql = "SELECT id, name, public_key, last_seen FROM clients"
        clients = []
        for row in self.connection.execute(sql):
            lastSeen = None
            if row['last_seen'] is not None:
                lastSeen = datetime.datetime.fromisoformat(row['last_seen'])
            clients.append(Client(uuid.UUID(row['id'].hex()), row['name'], row['public_key'], lastSeen))
        return clients

    def addMessage(self, message: Message) -> int:
        assert isinstance(message, Message)

        sql = """
            INSERT INTO messages (from_client, to_client, type, content) 
            VALUES (?,?,?,?);
        """
        row = self.safeExecute(sql, (message.fromClient.bytes,
                                     message.toClient.bytes,
                                     message.type,
                                     message.content))
        return int(row.lastrowid)

    def getAllMessages(self, clientID: uuid.UUID) -> [Message]:
        assert isinstance(clientID, uuid.UUID)

        sql = """
            SELECT id, from_client, to_client, type, content 
            FROM messages
            WHERE to_client = ?
        """
        clients = []
        for row in self.connection.execute(sql, (clientID.bytes,)):
            clients.append(Message(row['id'],
                                   uuid.UUID(row['to_client'].hex()),
                                   uuid.UUID(row['from_client'].hex()),
                                   row['type'],
                                   row['content']))
        return clients

    def deleteMessages(self, idList: [int]) -> None:
        assert isinstance(idList, list)

        sql = "DELETE FROM messages WHERE id = ?"
        self.__lock.acquire()
        self.connection.executemany(sql, [(messageID,) for messageID in idList])
        self.__lock.release()

    def updateLastSeen(self, clientID: uuid.UUID) -> None:
        assert isinstance(clientID, uuid.UUID)

        sql = """
            UPDATE clients SET last_seen = ?
            WHERE id = ?
        """
        self.safeExecute(sql, (datetime.datetime.now(), clientID.bytes))

    def safeExecute(self, sql, params = None):
        self.__lock.acquire()
        if params is None:
            result = self.connection.execute(sql)
        else:
            result = self.connection.execute(sql, params)
        self.__lock.release()
        return result
