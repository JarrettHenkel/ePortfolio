# =============================================================================
# File Name: db.py
# This module handles database connections. It provides a function to connect to MongoDB,
# utilizing credentials managed in the config module.
# =============================================================================
print('File Running: db.py')  # Indicates that this module is being executed.
# =============================================================================

from pymongo import MongoClient
from config import DB_USERNAME, DB_PASSWORD

def get_db():
    # Database connection setup: Connects to the MongoDB server using credentials and returns the database object.
    client = MongoClient(f'mongodb://{DB_USERNAME}:{DB_PASSWORD}@localhost:27017/aac?authSource=aac')
    return client['aac']