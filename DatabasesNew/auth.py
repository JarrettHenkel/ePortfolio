# =============================================================================
# File Name: auth.py
# This module handles user authentication. It includes functions to authenticate, register,
# and log out users, leveraging password hashing for security.
# =============================================================================
print('File Running: auth.py')  # Prints to the console to indicate that the module is active.
# =============================================================================

from db import get_db
from passlib.hash import pbkdf2_sha256

db = get_db()
collection_users = db['users']

current_user = None

def authenticate_user(username, password):
    # Authenticate a user by username and password, setting global variables if successful.
    global current_user
    user = collection_users.find_one({'username': username})
    if user and pbkdf2_sha256.verify(password, user['password']):
        current_user = username
        return True
    else:
        current_user = None
        return False

def register_user(username, password):
    # Register a new user with a username and hashed password, ensuring uniqueness.
    if collection_users.find_one({'username': username}):
        return False
    hashed_password = pbkdf2_sha256.hash(password)
    collection_users.insert_one({'username': username, 'password': hashed_password})
    return True

def logout_user():
    # Clear the current logged-in user.
    global current_user
    current_user = None

def get_current_user():
    # Retrieve the username of the currently logged-in user.
    return current_user