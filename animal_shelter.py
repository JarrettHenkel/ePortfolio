# =============================================================================
# File Name: animal_shelter.py
# This module defines the AnimalShelter class, providing methods to interact with the database for
# CRUD operations on animal records. It employs a singleton pattern to ensure a single instance.
# =============================================================================
print('File Running: animal_shelter.py')  # Logs that this class module is running.

from db import get_db
from bson.objectid import ObjectId
import logging

class AnimalShelter:
    _instance = None

    def __new__(cls):
        # Ensure only one instance of AnimalShelter exists (Singleton Pattern).
        if cls._instance is None:
            cls._instance = super(AnimalShelter, cls).__new__(cls)
            cls._instance.db = get_db()  # Initialize the database connection.
            cls._instance.collection = cls._instance.db['animals']
        return cls._instance

    def __init__(self):
        self.db = get_db()
        self.collection = self.db['animals']
        self.records_updated = 0
        self.records_matched = 0
        self.records_deleted = 0

    def create(self, data):
        # Insert a new document into the collection if data is provided.
        if data:
            result = self.collection.insert_one(data)
            return result.inserted_id if result else None

    def read(self, search):
        # Retrieve documents based on a search criteria, or all documents if no criteria provided.
        logging.debug("Trying to read data with search query: {}".format(search))
        if search:
            search_result = list(self.collection.find(search))
            logging.debug("Search Result: {}".format(search_result))
            return search_result
        else:
            all_documents = list(self.collection.find({}))
            logging.debug("All Documents: {}".format(all_documents))
            return all_documents

    def update(self, search, update_data):
        # Update documents based on search criteria and provided update data.
        if search and update_data:
            result = self.collection.update_many(search, {"$set": update_data})
            return result.modified_count

    def delete(self, search):
        # Delete documents based on a search criteria.
        if search:
            result = self.collection.delete_many(search)
            return result.deleted_count

    def addAnimal(self, name, breed, age, sex):
        # Utility method to add a new animal to the database.
        new_animal = {'name': name, 'breed': breed, 'age_upon_outcome_in_weeks': age, 'sex_upon_outcome': sex}
        return self.create(new_animal)
