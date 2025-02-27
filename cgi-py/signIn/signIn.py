import sys
import json
import os
import hashlib
import secrets
from datetime import datetime, timedelta, timezone
from email.utils import format_datetime

request_method = os.environ.get("REQUEST_METHOD")
if not request_method or request_method != "POST":
	body = "Method Not Allowed"
	print(f"HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/plain\r\nContent-Length: {len(body)}\r\n\r\n{body}")
	sys.exit(0)
def get_cookie_expire_time():
	# 현재 시간
	now = datetime.now(timezone.utc)

	# 현재 시간에 1시간 추가
	expires_time = now + timedelta(hours=1)

	# 쿠키용 RFC 1123 형식으로 변환
	cookie_expires = format_datetime(expires_time)
	return cookie_expires

def generate_session_id(user_id):
    random_string = secrets.token_hex(16)
    session_id = f"{user_id}_{random_string}"
    return session_id

def hash_pwd(password):
	return hashlib.sha256(password.encode()).hexdigest()

def is_expired(time):
	now = datetime.now(timezone.utc)
	expire_time = datetime.strptime(time, "%a, %d %b %Y %H:%M:%S %z")
	return True if now >= expire_time else False

script_dir = os.path.dirname(os.path.abspath(__file__))
os.chdir(script_dir)
file_path = '../../User/data.json'
path = 200

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

try:
	with safe_open(file_path, 'r+') as file:
		data = json.load(file)

		input_data = sys.stdin.read()
		params = input_data.split("&")
		query_dict = {}
		for param in params:
			key, value = param.split("=")
			query_dict[key] = value
		user_id = query_dict.get("username")
		user_pwd = query_dict.get("password")
		cookie = "\r\n"

		user = None
		for u in data.get("users", []):
			if u['id'] == user_id:
				user = u
				break

		if user:
			match = 0
			for i, item in enumerate(data.get("sessions", [])):
				if item['id'] == user_id:
					if is_expired(item['expire_time']):
						del(data["sessions"][i])
					else:
						match = 1
					break
			hashed_pwd = hashlib.sha256((user_pwd + user['salt']).encode()).hexdigest()
			if match == 0:
				session_id = generate_session_id(user_id)
				path = 302
				expire_time = get_cookie_expire_time()
				cookie = f'Cache-Control: no-store, no-cache, must-revalidate\n'
				cookie += f"Set-Cookie: session_id={session_id}; Expires={expire_time}; Path=/; HttpOnly; Secure; SameSite=Strict\r\n\r\n"
				body = {
					"status": "success",
					"id": user_id,
				}
				new_user = {
					"id": user_id,
					"session_id": session_id,
					"expire_time": expire_time
				}
				data["sessions"].append(new_user)
				file.seek(0)
				json.dump(data, file, indent=4)
			elif match == 1:
				body = {
					"status": f'{user_id} is already in session'
				}
		else:
			body = {
				"status": "User not found"
			}
		body = json.dumps(body)
		print(f"HTTP/1.1 {path} OK\r\nContent-Type: text/html\r\nContent-Length: {len(body)}\r\n{cookie}{body}")

except Exception as e:
	pass