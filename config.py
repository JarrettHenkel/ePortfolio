# =============================================================================
# File Name: config.py
print('File Running: config.py Running')
# =============================================================================
from dotenv import load_dotenv
import os

load_dotenv()
DB_USERNAME = os.getenv('DB_USERNAME')
DB_PASSWORD = os.getenv('DB_PASSWORD')