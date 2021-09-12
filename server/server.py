import json
import socket
import sys
import threading
import time

from data import Data


class Server:
    __instance = None

    @staticmethod
    def getInstance(callback=None):
        if Server.__instance == None:
            Server(callback)
        return Server.__instance

    def __init__(self, callback):

        if Server.__instance != None:
            raise Exception("This class is a singleton!")
        else:
            Server.__instance = self

        self.callback = callback
        self.clients = {}
        self.size = 2048
        self.port = 2693
        self.addres = Data.addres
        self.command = {}
        self.data = {}

        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            self.server.bind((self.addres, self.port))
        except Exception as e:
            print(f"[SERVER] Error: {e}")
            exit()
        print("[SERVER] Starting...")
        thread = threading.Thread(name="server", target=self.run, daemon=True)
        thread.start()

    def handle_client(self, conn, addr):
        print(f"[SERVER] {addr} connected.")

        left = 3
        while left > 0:
            msg = conn.recv(self.size).decode()
            if msg:
                try:
                    code, name = msg.split(":", 1)
                except:
                    code, name = None, None
                if code == "CON":
                    if self.clients.get(name):
                        if self.ping(name):
                            conn.send(
                                str.encode(
                                    "ERR:Name is taken! Pleace selcet other name."
                                )
                            )
                            left -= 1
                            continue
                    print(f"[SERVER] {addr} registered with name: {name}.")
                    conn.send(str.encode("OK:Connected sucessfuly"))
                    self.clients[name] = conn
                    self.command[name] = None
                    self.data[name] = None
                    self.listen(name)
            left -= 1
        print(f"[SERVER] {addr} timeout.")
        conn.close()
        sys.exit()

    def listen(self, name):
        while True:
            try:
                msg = self.clients[name].recv(self.size).decode()
            except Exception as e:
                print(f"[SERVER] {e}")
                try:
                    self.clients[name].close()
                    self.clients.pop(name)
                    self.command.pop(name)
                    self.data.pop(name)
                except:
                    pass

            if not msg:
                continue

            code, info = msg.split(":", 1)
            print(f"[SERVER] {code} : {info}")
            if self.command[name]:
                if self.command[name] == "GET":
                    if code != "DATA":
                        self.data[name] = {"error": True, "info": "Wrong response"}
                    else:
                        self.data[name] = {"error": False, "info": json.loads(info)}
                elif self.command[name] == "SET":
                    if code == "ERR":
                        self.data[name] = {"error": True, "info": info}
                    elif code != "OK":
                        self.data[name] = {"error": True, "info": "Wrong response"}
                    else:
                        self.data[name] = {"error": False, "info": "OK"}
                self.command[name] = None
                continue
            if code == "PING":
                self.send("OK:OK", name)
            elif code == "DATA":
                data = json.loads(info)
                self.callback("get", {"name": name, "payload": {"data": data}})
                self.send("OK:OK", name)
            elif code == "OK":
                continue

    def ping(self, name):
        other = self.clients.get(name)
        try:
            other.send(str.encode(f"PING:{name}"))
            msg = other.recv(self.size).decode()
            if not "OK" in msg:
                raise
        except:
            print(f"[SERVER] {name} diconnected (not responding).")
            other.close()
            self.clients.pop(name)
            self.command.pop(name)
            self.data.pop(name)
            return False
        print(f"[SERVER] {name} is still alive.")
        return True

    def send_set(self, name, data):
        self.data[name] = None
        self.command[name] = "SET"
        self.send(f"SET:{json.dumps(data)}", name)
        while self.data[name] is None:
            pass
        return self.data[name]

    def send_get(self, name):
        self.data[name] = None
        self.command[name] = "GET"
        self.send(f"GET:GET", name)
        while self.data[name] is None:
            pass
        return self.data[name]

    def send_slient_get(self, name):
        self.send(f"GET:GET", name)

    def isName(self, name):
        c = self.clients.get(name)
        if c is None:
            return False
        return True

    def send(self, msg, name):
        c = self.clients.get(name)
        message = msg.encode()
        c.send(message)

    def run(self):
        self.server.listen()
        print(f"[SERVER] Server is listening on {self.port}")
        while True:
            conn, addr = self.server.accept()
            thread = threading.Thread(target=self.handle_client, args=(conn, addr))
            thread.start()


if __name__ == "__main__":
    Server()
    while True:
        pass
