# =============================================================================
# File Name: layout.py
# This script sets up the layout for a Dash application using various Dash and Dash-Bootstrap components.
# It handles the creation of the user interface, including navigation, data tables, input forms, and
# other interactive elements. This script also integrates visual components like images and modal dialogs.
print('File Running: layout.py')
# =============================================================================
from dash import dcc, html, dash_table as dt
import dash_bootstrap_components as dbc
import base64
import pandas as pd
from db import get_db


# Load and encode the company logo for use in the web app
with open('Grazioso_Salvare_Logo.png', 'rb') as f:
    encoded_image = base64.b64encode(f.read())

# Database setup: connects to MongoDB, fetches data, and prepares it for the UI
db = get_db()  # Establish a connection to the MongoDB database
collection = db['animals']  # Access the 'animals' collection
collection_users = db['users']  # Access the 'users' collection
df = pd.DataFrame(list(collection.find({}, projection={'_id': 0})))  # Load animal data from MongoDB into a DataFrame

# Data preparation: Ensures essential columns exist and are correctly formatted
if 'Chip_ID' not in df.columns:
    df['Chip_ID'] = ''
if 'animal_id' not in df.columns:
    df['animal_id'] = range(1, len(df) + 1)

# Define the table structure for the web interface, including options for user interactions
columns_order = [
    {"name": "animal_id", "id": "animal_id", "deletable": False, "selectable": True},
    {"name": "Chip_ID", "id": "Chip_ID", "deletable": False, "selectable": True, "presentation": "dropdown"},
] + [
    {"name": i, "id": i, "deletable": False, "selectable": True} for i in df.columns
    if i not in ["animal_id", "Chip_ID"] and i != "1"
]

# Function to generate the layout of the application, structuring all elements and interactions
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