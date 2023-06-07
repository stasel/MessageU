from DBClient import DBClient
from Protocol.Request import *
from Protocol.Response import *
import uuid
import socket
import traceback
import secrets

VERSION = 1
publicKey1 = b'\xb1\xf1\x03d\xb8&\x86\x15>\x1d\xca\xa0\x1a\xa4\xd0\x95\xec>\xa3\xeb\xd2\x911\xa56\xab2=\xee\x1b\x84\xf4\xaa \xda\xec\x15#\xd5-\xaf\x95m\xa1\xd6L\x8f\x04\xcf\x9f\x06J\xac\xbfz\tQX\xbds\xef5\x0c<\xd8%\xf6\x179\xa8\xc6@| \x0e\xcc"\xac\xa5\xe4\x11$\xfe\x8e70\xb2 4\x05 PI\x1f\x0c\xee\xaa\xb6\xd8r\xfeXd\xf1\x97\xfa\xa0\xa5u\xc3\xd6/\x0eM\xb7\x95\xb7\xb5\xee\xa8\xe9j\x98\xb1\xd0\x1f\xc3=\xc1\xa3^\xed\xc8\x06 B.\x81xoCkx:)\xb2@l\xb4\xc50$z\x9f\x87\x93\xa3\xe6t\x11'
publicKey2 = b'N\x80\xbc\x8d\xb0\xad\xdf\x1eE\xb9\xf5\xaff\x96|\x19\x1f\x8d\xfc\xbe\x07s\x8a`Q\x07\x812\x84I{#z\xda\xf7\x19\x8a\x8f\x8e\xbas\x19lk\xd2!\xc5K\x9d\xeb?-C\xb7kjn\x80\xde\xed\xe9\x97\x12\xef\xa3jB\xc9h\x04\x11\x039\x06S\x0e\x1e/k\xe0\xe2&8\xf9\x8f\x1bP(\x17\x06\xccg\xda/\x9a%Gh\xf6Zpl;Y4V\xaf4\xf6X\x86U%\x13q\x97\x8d\x99>c\x8a\xf4?\x81\x89\xc5\xfb\xb4\x84\xc6\x8a\xc5[\xfdm-UJ\xdd-N\xd2\xef\xccd-z\xe5d\xbcg&\xff\xf0F\xdb\xda\x1c\x1f\x02'

dummyClient1 = Client(uuid.UUID('B109831E-00C8-4261-A307-198097F9BF53'), "User1", publicKey1)
dummyClient2 = Client(uuid.UUID('8AE21A11-5D04-4B92-B483-379353429073'), "User2", publicKey2)

message1 = Message(32005554, dummyClient1.id, dummyClient2.id, 10, b'12345')
message2 = Message(83743519, dummyClient2.id, dummyClient1.id, 11, b'stas')


class Assert:
    @staticmethod
    def equals(a, b):
        assert a == b, f"{a} does not equal to {b}"

    @staticmethod
    def isNone(x):
        assert x is None, "{x} is not None"

    @staticmethod
    def isNotNone(x):
        assert x is not None, "{x} is None"

class TestCase:
    def run(self):
        testsPass = 0
        testsFail = 0
        for name, value in self.__class__.__dict__.items():
            if name.startswith("test") and callable(value):
                try:
                    value(self)
                    print(f"✅ {name}")
                    testsPass += 1
                except Exception as ex:
                    print(f"❌ {name} : {ex}")
                    print(traceback.format_exc())
                    testsFail += 1
        print(f"Pass: {testsPass}. Fail: {testsFail}")


class SocketMock(socket.socket):
    def __init__(self, data):
        self.data = data
        self.offset = 0

    def recv(self, bufsize: int, flags: int = ...) -> bytes:
        result = self.data[self.offset:self.offset + bufsize]
        self.offset += bufsize
        return result


class DBClientMock(DBClient):
    def __init__(self):
        pass