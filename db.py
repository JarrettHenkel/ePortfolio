# =============================================================================
# Interpreter: Python 3.12
# File Name: db.py
print('File Running: db.py Running')
# =============================================================================
from pymongo import MongoClient
from config import DB_USERNAME, DB_PASSWORD

def get_db():
    client = MongoClient(f'mongodb://{DB_USERNAME}:{DB_PASSWORD}@localhost:27017/aac?authSource=aac')
    return client['aac']