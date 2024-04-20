# =============================================================================
# File Name: config.py
# This script is responsible for managing configurations for the application.
# It loads environment variables and sets up database credentials to be used across the application.
# =============================================================================
print('File Running: config.py')  # Confirms that this script is currently running.

from dotenv import load_dotenv
import os

load_dotenv()  # Load environment variables from a local .env file

# Retrieve database credentials securely from environment variables
DB_USERNAME = os.getenv('DB_USERNAME')
DB_PASSWORD = os.getenv('DB_PASSWORD')