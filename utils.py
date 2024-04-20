# =============================================================================
# File Name: utils.py
# This script contains utility functions to manage data transformations and loading.
# It includes functions to load CSV data, process it into a usable format, and insert a specific column.
# =============================================================================
print('File Running: utils.py')  # Logs that this file is being executed.
# =============================================================================
import pandas as pd

def load_csv_data():
    # Load initial CSV data: Reads animal data from a CSV file, converts it into a pandas DataFrame,
    # and restructures it by inserting a countdown column and renaming unnamed columns.
    df = pd.read_csv('aac_shelter_outcomes.csv', index_col=0)
    animal_id_index = df.columns.get_loc('animal_id')
    df = df.astype(str)
    df.insert(animal_id_index, 'count_down_column', df.index.astype(str))
    unnamed_column = df.pop(df.columns[0])
    df.insert(0, 'Unnamed_Column', unnamed_column)
    return df