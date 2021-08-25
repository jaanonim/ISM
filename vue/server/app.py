from flask import Flask, abort, request
from flask_cors import CORS
from flask_login import LoginManager, UserMixin, current_user, login_user
from flask_socketio import SocketIO, emit
from gpiozero import CPUTemperature

from data import Data
from server import Server

app = Flask(__name__)
CORS(app)
app.config["SECRET_KEY"] = Data.secretKey
app.config["SERVER"] = Server.getInstance()
login = LoginManager(app)
socketio = SocketIO(app, cors_allowed_origins="*")


@login.user_loader
def user_loader(id):
    return User(id)


class User(UserMixin):
    def __init__(self, username):
        self.id = username


@app.route("/login", methods=["POST"])
def login():
    username = request.form["username"]
    password = request.form["password"]
    print(username, password, str(username not in Data.users), Data.users[username])
    if username not in Data.users or Data.users[username] != password:
        abort(401)
    login_user(User(username))
    return ""


"""
def api():
    cpu = CPUTemperature()
    temp=cpu.temperature
"""


@socketio.on("connect")
def on_connect():
    if current_user.is_anonymous:
        pass
        # return False


@socketio.on("set")
def _set(json):

    try:
        name = json["addres"]
    except:
        emit(
            "set",
            {
                "name": None,
                "payload": {"error": "Invalid json (the address is missing)"},
            },
        )
        return

    try:
        payload = json["payload"]
    except:
        emit(
            "set",
            {
                "name": name,
                "payload": {"error": "Invalid json (something is wrong with payload)"},
            },
        )
        return

    if not Server.getInstance().isName(name):
        emit("set", {"name": name, "payload": {"error": "Invalid addres"}})
        return

    data = Server.getInstance().send_set(name, payload)
    if data["error"]:
        emit("set", {"name": name, "payload": {"error": data["info"]}})
        return

    emit("set", {"name": name, "payload": None})


@socketio.on("get")
def _get(json):
    print("ok")

    try:
        name = json["addres"]
    except:
        emit(
            "get",
            {
                "name": None,
                "payload": {"error": "Invalid json (the address is missing)"},
            },
        )
        return

    if name == "GAPI":
        cpu = CPUTemperature()
        emit("get", {"name": name, "payload": {"data": {"temp": cpu.temperature}}})
        return

    if not Server.getInstance().isName(name):
        emit("get", {"name": name, "payload": {"error": "Invalid addres"}})
        return

    data = Server.getInstance().send_get(name)
    if data["error"]:
        emit("get", {"name": name, "payload": {"error": data["info"]}})
        return

    emit("get", {"name": name, "payload": {"data": data["info"]}})


if __name__ == "__main__":
    socketio.run(app, host=Data.addres, port="5000")
