#!/usr/bin/python3

import sys
import json
import secrets
import hashlib
import os

request_method = os.environ.get("REQUEST_METHOD")
if not request_method or request_method != "POST":
	body = "Method Not Allowed"
	print(f"HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/plain\r\nContent-Length: {len(body)}\r\n\r\n{body}")
	sys.exit(0)
script_dir = os.path.dirname(os.path.abspath(__file__))
os.chdir(script_dir)
file_path = '../../User/data.json'

def safe_open(file_path, mode="w"):
	dir_path = os.path.dirname(file_path)
	users = []
	sessions = []
	if not os.path.exists(dir_path):
		os.makedirs(dir_path)
	if not os.path.exists(file_path):
		with open(file_path, 'w') as temp_file:
			json.dump({"users": users, "sessions": sessions}, temp_file)
	return open(file_path, mode)

# 랜덤한 salt 값 생성
salt = secrets.token_hex(16)
input_data = sys.stdin.read()
params = input_data.split("&")
query_dict = {}
for param in params:
	key, value = param.split("=")
	query_dict[key] = value
password = query_dict.get("password") + salt
hashed_password = hashlib.sha256(password.encode()).hexdigest()
try:
	with safe_open(file_path, 'r+') as file:
		data = json.load(file)

		new_user = {
			"id": query_dict.get("username"),
			"salt": salt,
			"password": hashed_password
		}
		data["users"].append(new_user)
		file.seek(0)
		json.dump(data, file, indent=4)
	body = "success"
	print(f"HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: {len(body)}\r\n\r\n{body}\r\n")

except Exception as e:
	pass