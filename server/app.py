import datetime
import os

from dotenv import load_dotenv
from flask import Flask
from flask_cors import CORS
from flask_jwt import JWT, _jwt
from flask_socketio import SocketIO, emit
from user import User
from wakeonlan import send_magic_packet
from werkzeug.security import safe_str_cmp

from server import Server

load_dotenv() 

USERS = [User(1,os.getenv("USER_NAME"), os.getenv('USER_PASSWORD') )]
username_table = {u.username: u for u in USERS}
userid_table = {u.id: u for u in USERS}


def authenticate(username, password):
    print(username,password,USERS[0])
    user = username_table.get(username, None)
    if user and safe_str_cmp(user.password.encode("utf-8"), password.encode("utf-8")):
        return user


def identity(payload):
    user_id = payload["identity"]
    return userid_table.get(user_id, None)


def callback(name, data):
    with app.test_request_context("/"):
        emit(name, data, namespace="/", broadcast=True)


app = Flask(__name__,static_url_path='', 
            static_folder='../client/dist')
CORS(app)
app.config["SECRET_KEY"] = os.getenv("SECRET_KEY")
app.config["SERVER"] = Server.getInstance(callback)
app.config["JWT_EXPIRATION_DELTA"] = datetime.timedelta(days=7)
jwt = JWT(app, authenticate, identity)
socketio = SocketIO(app, cors_allowed_origins="*")

@app.route('/')
def static_file():
    return app.send_static_file("index.html")

@socketio.on("connect")
def on_connect(auth):
    print("[SOCKET] client connected")
    try:
        token = auth.get("token")
        if token is None:
            raise
        _jwt.jwt_decode_callback(token)
    except:
        emit("dis")
        print("[SOCKET] client disconected (wrong token)")
        return False
    else:
        print("[SOCKET] client authorize")


@socketio.on("disconnect")
def on_disconnect():
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


    if name == "ISM":
        try:
            payload = json["payload"]
            if payload["naswol"] == True:
                send_magic_packet(os.getenv("NAS_MAC"))
                emit_with_log("set", {"name": name, "payload": None})
            else:
                raise

        except:
            emit_with_log(
                "set",
                {
                    "name": name,
                    "payload": {"error": "Invalid json (something is wrong with payload)"},
                },
            )
            return
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

    if name == "ISM":
        emit_with_log(
            "get",
            {
                "name": name,
                "payload": {},
            },
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
    port = int(os.getenv("PORT",5000))
    print(f"[SOCKET] starting socektio server on {port}")
    socketio.run(app, host=os.getenv("HOST","0.0.0.0"), port=port)
