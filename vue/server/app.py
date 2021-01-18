import requests
from data import Data
from flask import Flask, render_template, jsonify, request, Response
from flask_cors import CORS
from gpiozero import CPUTemperature
from flask_jwt import JWT, jwt_required, current_identity
from werkzeug.security import safe_str_cmp

username_table = {u.username: u for u in Data.users}
userid_table = {u.id: u for u in Data.users}

def authenticate(username, password):
    user = username_table.get(username, None)
    if user and safe_str_cmp(user.password.encode('utf-8'), password.encode('utf-8')):
        return user

def identity(payload):
    user_id = payload['identity']
    return userid_table.get(user_id, None)

app = Flask(__name__)
app.config['SECRET_KEY'] = 'super-secret'

CORS(app)
jwt = JWT(app, authenticate, identity)

isma = "http://192.168.0.11/isma/"
ismp = "http://192.168.0.15/ismp/"



@app.route('/api/')
@app.route('/api')
@jwt_required()
def api():
	cpu = CPUTemperature()
	return jsonify(temp = cpu.temperature,isma=isma,ismp=ismp)

@app.route('/ismp/<path:path>/')
@app.route('/ismp/<path:path>')
@jwt_required()
def ISMPpath(path):
    return requests.get(ismp+path,request.args).text

@app.route('/isma/<path:path>/')
@app.route('/isma/<path:path>')
@jwt_required()
def ISMApath(path):
    return requests.get(isma+path,request.args).text
	
@app.route('/ismp/')
@app.route('/ismp')
def ISMP():
	return redirect(ismp, code=302)
	
@app.route('/isma/')
@app.route('/isma')
def ISMA():
	return redirect(isma, code=302)

if __name__ == '__main__':
    app.run()