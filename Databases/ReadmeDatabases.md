# MongoDB Dash Application

This project showcases a comprehensive MongoDB-backed Dash application designed for managing animal data with interactive user interfaces. Originally set up early in 2023, this artifact has been significantly enhanced to integrate modern best practices in application development, security, and database management.

## Project Overview

The application leverages MongoDB, a NoSQL database, for data storage and management. It utilizes Dash, a Python framework for building analytical web applications, to provide a user-friendly interface. The app is modular, featuring a clear separation of concerns through Object-Oriented Programming (OOO) principles, and enhanced security measures for user authentication and authorization.

## Features

- **Modular Codebase**: The project is structured into multiple Python modules including data handling (`utils.py`), configuration management (`config.py`), database operations (`db.py`), user authentication (`auth.py`), and application setup (`app.py`).
- **Enhanced Security**: The application uses hashed passwords for authentication, sourcing sensitive information from environment variables set up in a `.env` file, enhancing the security over previous plain text implementations.
- **Interactive User Interface**: Built with Dash and Dash Bootstrap Components, the interface includes functionalities for data visualization, user registration, and login.
- **Automated Data Handling**: The `utils.py` script contains functions to load and process CSV data, ensuring the application can be easily populated with initial data sets.
- **Singleton Design Pattern**: Used in `animal_shelter.py` for managing animal data operations to ensure a single instance of the database handler.

## StartUp
  ```
  cd Databases
  .venv\Scripts\activate
  ```
  Go into the .env and set the Database username/password to what you wish.

  Open two terminals and enter each command into its own terminal.

  ```
  python app.py
  waitress-serve --host=0.0.0.0 --port=8050 app:app      
  ```

## Future Enhancements

- **Database Setup Automation**: Plans to automate the MongoDB setup process, allowing users to clone the repository and run a script to configure the database automatically.
- **Continuous Integration/Deployment**: Implement CI/CD pipelines for automated testing and deployment.

## Challenges

The initial setup of MongoDB on a personal machine presented challenges, especially transitioning from a pre-configured virtual server environment. Future updates aim to simplify this process, making it more accessible for new users.
