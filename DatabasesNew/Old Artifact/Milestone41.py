from pymongo import MongoClient
from bson.objectid import ObjectId
import logging

class CRUD(object):
    """ CRUD operations for Animal collection in MongoDB """

    def __init__(self):
        # Initializing the MongoClient. This helps to
        # access the MongoDB databases and collections.
        USER = 'aacuser'
        PASS = 'password'  
        HOST = 'nv-desktop-services.apporto.com'
        PORT = 31802
        DB = 'AAC'
        COL = 'animals'
        
        self.client = MongoClient(f'mongodb://{USER}:{PASS}@{HOST}:{PORT}/{DB}?authSource=admin')
        self.database = self.client[DB]
        self.collection = self.database[COL]

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




