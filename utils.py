# =============================================================================
# File Name: utils.py
print('File Running: utils.p Running')
# =============================================================================
import pandas as pd

def load_csv_data():
    df = pd.read_csv('aac_shelter_outcomes.csv', index_col=0)
    animal_id_index = df.columns.get_loc('animal_id')
    df = df.astype(str)
    df.insert(animal_id_index, 'count_down_column', df.index.astype(str))
    unnamed_column = df.pop(df.columns[0])
    df.insert(0, 'Unnamed_Column', unnamed_column)
    return df