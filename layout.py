# =============================================================================
# File Name: layout.py
print('File Running: layout.py Running')
# =============================================================================
from dash import dcc, html, dash_table as dt
import dash_bootstrap_components as dbc
import base64
import pandas as pd
from db import get_db


with open('Grazioso_Salvare_Logo.png', 'rb') as f:
    encoded_image = base64.b64encode(f.read())

    db = get_db()
    collection = db['animals']
    collection_users = db['users']
    df = pd.DataFrame(list(collection.find({}, projection={'_id': 0})))

    if 'Chip_ID' not in df.columns:
        df['Chip_ID'] = ''

    if 'animal_id' not in df.columns:
        df['animal_id'] = range(1, len(df) + 1)

    columns_order = [
        {"name": "animal_id", "id": "animal_id", "deletable": False, "selectable": True},
        {"name": "Chip_ID", "id": "Chip_ID", "deletable": False, "selectable": True, "presentation": "dropdown"},
    ] + [
        {"name": i, "id": i, "deletable": False, "selectable": True} for i in df.columns
        if i not in ["animal_id", "Chip_ID"] and i != "1"
    ]

def create_layout(app):
    return html.Div([
        dcc.Location(id='url', refresh=False),
        html.A([
            html.Center(html.Img(src='data:image/png;base64,{}'.format(encoded_image.decode()),
                                 height=250, width=251))], href='https://www.snhu.edu', target="_blank"),
        html.Center(html.B(html.H1('Database Enhancement'))),
        html.Hr(),
        dcc.RadioItems(
            id='filter-type',
            options=[
                {'label': 'All', 'value': 'All'},
                {'label': 'Water Rescue', 'value': 'Water'},
                {'label': 'Mountain or Wilderness Rescue', 'value': 'Mountain'},
                {'label': 'Disaster Rescue or Individual Tracking', 'value': 'Disaster'},
            ],
            value='All'
        ),
        html.Hr(),
        html.Button('Open Register', id='open-register-button-modal', n_clicks=0),
        dbc.Modal(
            [
                dbc.ModalHeader("Register"),
                dbc.ModalBody(
                    [
                        dcc.Input(id='register-username-input-modal', type='text', placeholder='Enter your username'),
                        dcc.Input(id='register-password-input-modal', type='password', placeholder='Enter your password'),
                        html.Div(id='register-message-modal', style={'color': 'red'})
                    ]
                ),
                dbc.ModalFooter(
                    dbc.Button('Register', id='register-button-modal', n_clicks=0, color='primary'),
                ),
            ],
            id='register-modal',
            is_open=False,
        ),
        html.Button('Open Login', id='open-login-button', style={'display': 'block'}),
        dbc.Modal(
            [
                dbc.ModalHeader("Login"),
                dbc.ModalBody(
                    [
                        dcc.Input(id='username-input-modal', type='text', placeholder='Enter your username'),
                        dcc.Input(id='password-input-modal', type='password', placeholder='Enter your password'),
                        html.Div(id='login-message-modal', style={'color': 'red'})
                    ]
                ),
                dbc.ModalFooter(
                    dbc.Button('Login', id='login-button-modal', n_clicks=0, color='primary'),
                ),
            ],
            id='login-modal',
            is_open=False,
        ),
        html.Div([
            html.H3('Add New Animal Data', style={'textAlign': 'center'}),
            dcc.Input(id='new-animal-name', type='text', placeholder='Animal Name'),
            dcc.Input(id='new-animal-breed', type='text', placeholder='Breed'),
            dcc.Input(id='new-animal-age', type='number', placeholder='Age in Weeks'),
            dcc.Input(id='new-animal-sex', type='text', placeholder='Sex'),
            dcc.Input(id='new-animal-chip-id', type='text', placeholder='Chip ID'),
            html.Div(id='debug-output'),
            html.Button('Add Animal', id='add-animal-button', n_clicks=0, disabled=True),
        ], style={'width': '100%', 'margin': 'auto', 'text-align': 'center'}),
        html.Hr(),
        dt.DataTable(
            id='datatable-id',
            columns=columns_order,
            data=df.to_dict('records'),
            editable=True,
            row_selectable="single",
            selected_rows=[],
            filter_action="native",
            sort_action="native",
            page_action="native",
            page_current=0,
            page_size=10,
        ),
        html.Div(id='auth-status'),
        html.Br(),
        html.Hr(),
        html.Div([
            html.Div(id='map-id', className='six columns', style={'width': '50%', 'display': 'inline-block'}),
            html.Div(id='graph-id', className='six columns', style={'width': '50%', 'display': 'inline-block'}),
        ], className='row'),
    ])