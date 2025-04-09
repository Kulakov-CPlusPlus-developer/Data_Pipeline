# Data Pipeline

## Description

This project is a data pipeline designed to collect, process, and aggregate data, and then write it to a PostgreSQL database. It uses the `libpq` library for interacting with the database.

## Purpose

The data pipeline can be used for:

*   Collecting data from various sources (e.g., log files, APIs, databases).
*   Transforming and cleaning data.
*   Aggregating data (e.g., calculating averages, sums, counts).
*   Writing the aggregated data to a PostgreSQL database for further analysis and visualization.

## Technologies

*   **C++:** The primary programming language.
*   **CMake:** The project's build system.
*   **libpq:** A C++ library for interacting with PostgreSQL.

## Requirements

*   **PostgreSQL:** A properly installed and configured PostgreSQL database.
*   **libpq:** The `libpq` library must be installed and accessible for compiling and running the project. Using [vcpkg](https://vcpkg.io/en/index.html) is highly recommended for installing `libpq`.
*   **Visual Studio Build Tools (or MinGW):** Required for compiling the C++ code.
*   **CMake:** Version 3.17 or higher.

## Installation

1.  **Install PostgreSQL:**

    *   Download and install PostgreSQL from the official website: [https://www.postgresql.org/download/](https://www.postgresql.org/download/)
    *   During installation, make sure to select the "Development headers and libraries" component.

2.  **Install vcpkg (Recommended):**

    *   Follow the instructions on the vcpkg website: [https://vcpkg.io/en/getting-started](https://vcpkg.io/en/getting-started)
    *   After installing vcpkg, integrate it with CMake:
        ```
        .\vcpkg integrate install
        ```
    *   Install `libpq` using vcpkg:
        ```
        .\vcpkg install libpq
        ```

3.  **Clone the repository:**

    ```
    git clone <Your repository URL>
    cd Data_Pipeline
    ```

4.  **Configure CMake:**

    *   Create a `build` directory in the project root:
        ```
        mkdir build
        cd build
        ```
    *   Run CMake:
        ```
        cmake -DCMAKE_TOOLCHAIN_FILE=<path to vcpkg.cmake> ..
        ```
        Replace `<path to vcpkg.cmake>` with the actual path to the `vcpkg.cmake` file (usually located in `<vcpkg path>/scripts/buildsystems/vcpkg.cmake`). For example:
        ```
        cmake -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake ..
        ```
    *   If you are not using vcpkg, you will need to manually specify the paths to the `libpq` header files and libraries in `CMakeLists.txt`.

5.  **Build the project:**

    ```
    cmake --build . --config Debug
    ```
    or
    ```
    cmake --build . --config Release
    ```

## Configuration

1.  **Configure the database connection parameters:**

    *   Edit the connection string in the `src/Aggregation.cpp` file:
        ```c++
        const char* keywords [] = {
            "host",
            "port",
            "dbname",
            "user",
            "password",
            NULL
        };         
        const char* values [] = {
            "localhost",
            "5432",
            "postgres",
            "postgres",
            pass,
            NULL
        };
        ```         
     Replace the values with your actual PostgreSQL connection parameters. **Never store passwords directly in the code! Use environment variables or other secure methods.**
      ```c++
      string password = readPasswordFromFile("../data/config.txt");
      const char* pass = password.c_str();
      ```
      In this case, you will need to write your password in \Data_Pipeline\data\config.txt           

3.  **Set the `PATH` environment variable (if necessary):**

    *   If you encounter a "DLL not found" error when running the program, add the path to the directory containing `libpq.dll` to the system's `PATH` environment variable. This is usually `C:\Program Files\PostgreSQL\<version>\bin`.

## Running

1.  **Run the executable file:**

    *   Navigate to the `build` directory and run the executable file located in the `Debug` or `Release` directory (depending on the selected build configuration).
    *   Since this is a test version, you will need to select a file .txt in the opened file explorer to be processed.

## Usage

After starting, the data pipeline will begin collecting, processing, and aggregating data, and then writing it to the PostgreSQL database. Ensure that you have created a `sensor_data` table in the `aggregationdb` database that matches the structure of the data being written by the pipeline. The found anomalies will be recorded in \Data_Pipeline\data in JSON format.
