import datetime

from flask import Flask, abort, request
from flask_cors import CORS
from flask_jwt import JWT, _jwt, current_identity, jwt_required
from flask_socketio import SocketIO, disconnect, emit
from gpiozero import CPUTemperature
from werkzeug.security import safe_str_cmp

from data import Data
from server import Server

username_table = {u.username: u for u in Data.users}
userid_table = {u.id: u for u in Data.users}


def authenticate(username, password):
    user = username_table.get(username, None)
    if user and safe_str_cmp(user.password.encode("utf-8"), password.encode("utf-8")):
        return user


def identity(payload):
    user_id = payload["identity"]
    return userid_table.get(user_id, None)


def callback(name, data):
    with app.test_request_context("/"):
        emit(name, data, namespace="/", broadcast=True)


app = Flask(__name__)
CORS(app)
app.config["SECRET_KEY"] = Data.secretKey
app.config["SERVER"] = Server.getInstance(callback)
app.config["JWT_EXPIRATION_DELTA"] = datetime.timedelta(days=7)
jwt = JWT(app, authenticate, identity)
socketio = SocketIO(app, cors_allowed_origins="*")


@socketio.on("connect")
def on_connect(auth):
    print("[SOCKET] client connected")
    token = auth.get("token")
    try:
        if token is None:
            raise
        payload = _jwt.jwt_decode_callback(token)
    except:
        emit("dis")
        print("[SOCKET] client disconected (wrong token)")
        return False
    else:
        print("[SOCKET] client authorize")


@socketio.on("disconnect")
def on_disconnect(auth):
    print("[SOCKET] client disconected")


@socketio.on("set")
def _set(json):
    print("[SOCKET] set", json)

    try:
        name = json["name"]
    except:
        emit_with_log(
            "set",
            {
                "name": None,
                "payload": {"error": "Invalid json (the name is missing)"},
            },
        )
        return

    try:
        payload = json["payload"]
    except:
        emit_with_log(
            "set",
            {
                "name": name,
                "payload": {"error": "Invalid json (something is wrong with payload)"},
            },
        )
        return

    if not Server.getInstance().isName(name):
        emit_with_log("set", {"name": name, "payload": {"error": "Invalid name"}})
        return

    data = Server.getInstance().send_set(name, payload)
    if data["error"]:
        emit_with_log("set", {"name": name, "payload": {"error": data["info"]}})
        return

    emit_with_log("set", {"name": name, "payload": None})
    Server.getInstance().send_slient_get(name)


@socketio.on("get")
def _get(json):
    print("[SOCKET] get:", json)
    try:
        name = json["name"]
    except:
        emit_with_log(
            "get",
            {
                "name": None,
                "payload": {"error": "Invalid json (the name is missing)"},
            },
        )
        return

    if name == "GAPI":
        emit_with_log(
            "get",
            {"name": name, "payload": {"data": {"temp": CPUTemperature().temperature}}},
        )
        return

    if not Server.getInstance().isName(name):
        emit_with_log("get", {"name": name, "payload": {"error": "Invalid name"}})
        return

    data = Server.getInstance().send_get(name)
    if data["error"]:
        emit_with_log("get", {"name": name, "payload": {"error": data["info"]}})
        return
    emit_with_log("get", {"name": name, "payload": {"data": data["info"]}})


def emit_with_log(name, data):
    print(f"[SOCKET] {name} returned: {data}")
    emit(name, data)


if __name__ == "__main__":
    print("[SOCKET] starting socektio server")
    socketio.run(app, host=Data.addres, port=5000)
