# =============================================================================
# File Name: app.py
print('File Running: app.py Running')
# =============================================================================
from dash import Dash
import dash_bootstrap_components as dbc
from layout import create_layout
from callbacks import register_callbacks
from utils import load_csv_data
from waitress import serve

app = Dash(__name__, external_stylesheets=[dbc.themes.BOOTSTRAP])
load_csv_data()
app.layout = create_layout(app)  

register_callbacks(app)

if __name__ == '__main__':
    serve(app.server, host='0.0.0.0', port=8050)