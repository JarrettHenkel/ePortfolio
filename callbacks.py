# =============================================================================
# File Name: callbacks.py
print('File Running: callbacks.py Running')
# =============================================================================
import dash
from passlib.hash import pbkdf2_sha256
from dash import Dash, dcc, html, Input, Output, State, callback, MATCH, no_update, ALL, dash_table as dt
import pandas as pd
import plotly.express as px
from pymongo import MongoClient
import dash_bootstrap_components as dbc
import dash_leaflet as dl
import base64
from config import DB_USERNAME, DB_PASSWORD
from db import get_db
from utils import load_csv_data
from auth import authenticate_user
from animal_shelter import AnimalShelter
from collections import Counter

# Function to register callbacks
def register_callbacks(app):
    db = get_db()
    collection = db['animals']
    collection_users = db['users']

    load_csv_data()

    login_modal = dbc.Modal(
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
    )

    collection_users = db['users']
    collection = db['animals']
    collection_users = db['users']
    print('Connected to MongoDB')
    ##collection.insert_many(records)
    print('Local MongoDB populated with CSV')

    def update_dashboard(filter_type):
        query = {}
        print('Reloaded Data')
        if filter_type == 'Water':
            query = {
                "breed": {"$in": ["Labrador Retriever Mix", "Chesapeake Bay Retriever", "Newfoundland"]},
                "sex_upon_outcome": "Intact Female",
            }
            print('Loaded Water')
        elif filter_type == 'Mountain':
            query = {
                "breed": {"$in": ["German Shepherd", "Alaskan Malamute", "Old English Sheepdog", "Siberian Husky", "Rottweiler"]},
                "sex_upon_outcome": "Intact Male",
            }
            print('Loaded Mountain')
        elif filter_type == 'Disaster':
            query = {
                "breed": {"$in": ["Doberman Pinscher", "German Shepherd", "Golden Retriever", "Bloodhound", "Rottweiler"]},
                "sex_upon_outcome": "Intact Male",
            }
            print('Loaded Disaster')

        df_filtered = pd.DataFrame.from_records(collection.find(query, projection={'_id': 0}))
        return df_filtered

    app.config.suppress_callback_exceptions = True
    shelter = AnimalShelter()
    df = pd.DataFrame.from_records(collection.find({}, projection={'_id': 0}))

    with open('Grazioso_Salvare_Logo.png', 'rb') as f:
        encoded_image = base64.b64encode(f.read())

    current_filter_type = 'All'

    @app.callback(
        [
            Output('new-animal-name', 'value'),
            Output('new-animal-breed', 'value'),
            Output('new-animal-age', 'value'),
            Output('new-animal-sex', 'value'),
            Output('new-animal-chip-id', 'value'),
            Output('add-animal-button', 'disabled'),
        ],
        [
            Input('add-animal-button', 'n_clicks'),
            Input('auth-status', 'children'),
        ],
        [
            State('new-animal-name', 'value'),
            State('new-animal-breed', 'value'),
            State('new-animal-age', 'value'),
            State('new-animal-sex', 'value'),
            State('new-animal-chip-id', 'value'),
        ],
        prevent_initial_call=True
    )
    def handle_add_animal_button(n_clicks, auth_status, name, breed, age, sex, chip_id):
        if auth_status is not None:
            return '', '', '', '', '', False
        else:
            return '', '', '', '', '', True

    @app.callback(
        [
            Output('auth-status', 'children'),
            Output('login-modal', 'is_open'),
            Output('login-message-modal', 'children'),
            Output('url', 'pathname'),
        ],
        [
            Input('login-button-modal', 'n_clicks'),
            Input('open-login-button', 'n_clicks'),
        ],
        [
            State('username-input-modal', 'value'),
            State('password-input-modal', 'value'),
            State('url', 'pathname'),
            State('login-modal', 'is_open'),
        ],
        prevent_initial_call=True
    )
    def login_user(login_button_clicks, open_login_clicks, username, password, url_pathname, is_login_modal_open):
        try:
            if login_button_clicks:
                authenticated = authenticate_user(username, password)
                if authenticated:
                    return (
                        f"Authentication Status: Welcome, {username}!",
                        False,
                        None,
                        url_pathname,
                    )
                else:
                    return (
                        "Authentication Status: Invalid username or password.",
                        True,
                        "Invalid username or password.",
                        url_pathname,
                    )
            elif open_login_clicks:
                return no_update, True, no_update, no_update
            else:
                return no_update, no_update, no_update, no_update
        except Exception as e:
            print(f"Error in login callback: {str(e)}")
            return (
                f"Authentication Status: Error - {str(e)}",
                False,
                None,
                url_pathname,
            )

    @app.callback(
        Output('map-id', "children"),
        [Input('datatable-id', "selected_rows")]
    )
    def update_map(selected_rows):
        global df
        toolTip = "Austin Animal Center"
        if not selected_rows:
            return [dl.Map(style={'width': '700px', 'height': '450px'}, center=(30.75, -97.48), zoom=10,
                           children=[dl.TileLayer(id="base-layer-id")])]

        try:
            dff = pd.DataFrame(df.iloc[selected_rows])
            if 'location_lat' in dff.columns and 'location_long' in dff.columns:
                coordLat = float(dff['location_lat'].to_string().split()[1])
                coordLong = float(dff['location_long'].to_string().split()[1])
                markerArray = (coordLat, coordLong)
            else:
                markerArray = (30.75, -97.48)

            popUpHeading = "Animal Name"
            popUpParagraph = dff['name'].iloc[0] if 'name' in dff.columns else "Unknown"

        except Exception as e:
            print(f"Error in update_map callback: {str(e)}")
            return dash.no_update

        print(f"Update Map callback - Marker Array: {markerArray}")
        print(f"Update Map callback - Data: {df.head()}")
        return [dl.Map(style={'width': '700px', 'height': '450px'}, center=markerArray,
                       zoom=10, children=[dl.TileLayer(id="base-layer-id"),
                                          dl.Marker(position=markerArray, children=[
                                              dl.Tooltip(toolTip),
                                              dl.Popup([
                                                  html.H1(popUpHeading),
                                                  html.P(popUpParagraph)
                                              ])
                                          ])
                                          ])
                ]

    @app.callback(
        Output('datatable-id', 'data'),
        [
            Input('filter-type', 'value'),
            Input('add-animal-button', 'n_clicks')
        ],
        [
            State('new-animal-name', 'value'),
            State('new-animal-breed', 'value'),
            State('new-animal-age', 'value'),
            State('new-animal-sex', 'value'),
            State('new-animal-chip-id', 'value'),
            State('datatable-id', 'data')
        ],
        prevent_initial_call=True
    )
    def update_data(selected_filter_type, n_clicks, name, breed, age, sex, chip_id, existing_data):
        global df, current_filter_type

        try:
            print("Triggered by:", dash.callback_context.triggered_id)

            if 'filter-type' in dash.callback_context.triggered_id:
                print("Filter type change")
                current_filter_type = selected_filter_type
                df_filtered = update_dashboard(current_filter_type)
                df_filtered = df_filtered.fillna('')
                df = df_filtered.copy()
                for record in df.to_dict('records'):
                    if '_id' in record:
                        record['_id'] = str(record['_id'])
                print("Updated data:", df.to_dict('records'))
                return df.to_dict('records')

            elif 'add-animal-button' in dash.callback_context.triggered_id:
                print("Add animal button clicked")
                if n_clicks > 0:
                    new_animal = {
                        'name': name,
                        'breed': breed,
                        'age_upon_outcome_in_weeks': age,
                        'sex_upon_outcome': sex,
                        'Chip_ID': chip_id
                    }
                    collection.insert_one(new_animal)
                    df = pd.DataFrame(list(collection.find({}, projection={'_id': 0})))
                    df.to_csv('animal_data.csv', index=False)

                for record in df.to_dict('records'):
                    if '_id' in record:
                        record['_id'] = str(record['_id'])

                print("Updated data:", df.to_dict('records'))
                return df.to_dict('records')

        except Exception as e:
            print(f"Error: {str(e)}")
            return dash.no_update

    @app.callback(
        Output('graph-id', "children"),
        [Input('datatable-id', "derived_virtual_data")]
    )
    def update_graphs(viewData):
        if not viewData or not any('breed' in d for d in viewData):
            return []
        
        dff = pd.DataFrame.from_records(viewData)
        breed_count = Counter(dff['breed'])
        total_animals = sum(breed_count.values())
        filtered_breed_count = {}
        other_count = 0
        
        for breed, count in breed_count.items():
            percentage = (count / total_animals) * 100
            if percentage >= 1:
                filtered_breed_count[breed] = count
            else:
                other_count += count
                
        if other_count > 0:
            filtered_breed_count["Other"] = other_count
            
        df_filtered = pd.DataFrame(list(filtered_breed_count.items()), columns=['breed', 'count'])
        
        pie_chart = dcc.Graph(
            figure=px.pie(df_filtered, names='breed', values='count', title='Preferred Animals')
        )
        return [pie_chart]

    @app.callback(
        [
            Output('register-modal', 'is_open'),
            Output('register-message-modal', 'children'),
            Output('debug-output', 'children'),
        ],
        [
            Input('open-register-button-modal', 'n_clicks'),
            Input('register-button-modal', 'n_clicks'),
        ],
        [
            State('register-username-input-modal', 'value'),
            State('register-password-input-modal', 'value'),
            State('register-modal', 'is_open'),
        ],
        prevent_initial_call=True
    )
    def manage_register_modal(open_clicks, register_button_clicks, register_username, register_password, is_register_modal_open):
        try:
            print("Callback triggered.")
            print(f"open_clicks: {open_clicks}, register_button_clicks: {register_button_clicks}")
            print(f"register_username: {register_username}, register_password: {register_password}, is_register_modal_open: {is_register_modal_open}")

            if open_clicks and not register_button_clicks:
                print("Open Register button clicked.")
                return not is_register_modal_open, None, None

            elif register_button_clicks:
                print("Register button clicked.")
                existing_user = collection_users.find_one({'username': register_username})
                if existing_user:
                    print("Username already exists.")
                    return is_register_modal_open, "Username already exists. Choose a different one.", None

                hashed_password = pbkdf2_sha256.hash(register_password)
                result = collection_users.insert_one({'username': register_username, 'password': hashed_password})

                if result.inserted_id:
                    print("Registration successful. You can now log in.")
                    return False, "Registration successful. You can now log in.", None
                else:
                    print("Error during user registration.")
                    return is_register_modal_open, "Error during user registration.", None

            else:
                print("No button clicked.")
                return is_register_modal_open, None, None

        except Exception as e:
            print(f"Error in registration callback: {str(e)}")
            return is_register_modal_open, f"Error during registration: {str(e)}", None