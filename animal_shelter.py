# =============================================================================
# File Name: animal_shelter.py
print('File Running: animal_shelter.py Running')
# =============================================================================
from db import get_db
from bson.objectid import ObjectId
import logging

class AnimalShelter:
    _instance = None  # Class variable to store the singleton instance

    def __new__(cls):
        if cls._instance is None:
            cls._instance = super(AnimalShelter, cls).__new__(cls)
            cls._instance.db = get_db()  
            cls._instance.collection = cls._instance.db['animals']  
        return cls._instance

    def __init__(self):
        self.db = get_db()  # Initialize database connection
        self.collection = self.db['animals']
        self.records_updated = 0
        self.records_matched = 0
        self.records_deleted = 0
   
    def create(self, data):
        # Check if data is not empty and insert the document into the collection.
        if data:
            result = self.collection.insert_one(data)
            return result.inserted_id if result else None
        else:
            return None

    def read(self, search):
        logging.debug("Trying to read data with search query: {}".format(search))
        # Check if the search criteria is not empty and query the collection.
        if search:
            search_result = list(self.collection.find(search))
            logging.debug("Search Result: {}".format(search_result))
            return search_result
        else:
            all_documents = list(self.collection.find({}))
            logging.debug("All Documents: {}".format(all_documents))
            return all_documents
        
    def getRecordId(self, post_id):
        _data = self.dataBase.animals.find_one({'_id': ObjectId(post_id)})
        return _data
        
    def getRecordCriteria(self, criteria):
        if criteria:
            _data = list(self.dataBase.animals.find(criteria, {'_id': 0}))
        else:
            _data = list(self.dataBase.animals.find({}, {'_id': 0}))
        return _data    
    
    def update(self, search, update_data):
        # Check if the search criteria and update data are not empty.
        if search and update_data:
            result = self.collection.update_many(search, {"$set": update_data})
            return result.modified_count
        else:
            return 0

    def delete(self, search):
        # Check if the search criteria is not empty.
        if search:
            result = self.collection.delete_many(search)
            return result.deleted_count
        else:
            return 0

    def addAnimal(self, name, breed, age, sex):
        new_animal = {'name': name, 'breed': breed, 'age_upon_outcome_in_weeks': age, 'sex_upon_outcome': sex}
        return self.createRecord(new_animal)