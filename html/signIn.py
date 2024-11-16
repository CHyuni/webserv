import sys
import json
import secrets
import hashlib

try:
	with open('data.json', 'r') as file:
		data = json.load(file)
	

except FileNotFoundError:
	print("false")