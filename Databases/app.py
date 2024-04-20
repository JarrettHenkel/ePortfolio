# =============================================================================
# File Name: app.py
# This script initializes and configures the Dash application, incorporating layouts and callbacks
# for user interactions. It is the main entry point for running the web application.
# =============================================================================
print('File Running: app.py')  # Indicates that this script is the main application script.
# =============================================================================

from dash import Dash
import dash_bootstrap_components as dbc
from layout import create_layout
from callbacks import register_callbacks
from utils import load_csv_data
from waitress import serve

app = Dash(__name__, external_stylesheets=[dbc.themes.BOOTSTRAP])

load_csv_data()  # Load data initially to populate the app

app.layout = create_layout(app)  # Set the layout of the app from the layout module
register_callbacks(app)  # Register interactive callbacks

if __name__ == '__main__':
    serve(app.server, host='0.0.0.0', port=8050)  # Serve the app using a production server
