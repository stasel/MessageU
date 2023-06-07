from UMessageServer import *
from DBClient import *
import Constants
import uuid


def readPort(filePath) -> int:
    file = open(filePath, "r")
    port = file.read()
    file.close()
    if not port.isnumeric():
        raise Exception("Port must be numeric")
    return int(port)


def main():
    port = readPort(Constants.PORT_INFO_PATH)
    db = DBClient(Constants.DB_PATH)
    db.connect()
    server = UMessageServer(port, Constants.SERVER_VERSION, db)
    server.start()


if __name__ == "__main__":
    main()
