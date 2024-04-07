# =============================================================================
# File Name: auth.py
print('File Running: auth.py Running')
# =============================================================================
from db import get_db
from passlib.hash import pbkdf2_sha256

db = get_db()
collection_users = db['users']

current_user = None

def authenticate_user(username, password):
    global current_user
    user = collection_users.find_one({'username': username})
    if user and pbkdf2_sha256.verify(password, user['password']):
        current_user = username
        return True
    else:
        current_user = None
        return False

def register_user(username, password):
    if collection_users.find_one({'username': username}):
        return False
    hashed_password = pbkdf2_sha256.hash(password)
    collection_users.insert_one({'username': username, 'password': hashed_password})
    return True

def logout_user():
    global current_user
    current_user = None

def get_current_user():
    return current_user