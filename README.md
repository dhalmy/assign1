### Part 1: Manipulating Page Files

This section details the implementation and usage of fundamental functions for managing page files in the storage system. These functions provide the basic operations needed to create, open, close, and destroy page files.

#### 1.1 `initStorageManager(void)`
- **Purpose**: Initializes the storage manager. Currently serves as a placeholder for any future initialization needs.
- **Usage**: Called once at the start of the program to set up the storage manager.

#### 1.2 `createPageFile(char *fileName)`
- **Purpose**: Creates a new page file with an initial size of one page (4096 bytes).
- **Usage**:
  - Validates the filename to ensure it's not NULL
  - Creates a new binary file in write mode ("wb")
  - Allocates memory for one page filled with '\0' bytes using `calloc()`
  - Writes the empty page to the file using `fwrite()`
  - Properly frees allocated memory and closes the file
  - Returns appropriate error codes if any operation fails

#### 1.3 `openPageFile(char *fileName, SM_FileHandle *fHandle)`
- **Purpose**: Opens an existing page file and initializes the file handle structure.
- **Usage**:
  - Validates input parameters (fileName and fHandle)
  - Opens the file in binary read/write mode ("rb+")
  - Calculates total number of pages based on file size
  - Initializes file handle fields:
    - fileName: name of the opened file
    - totalNumPages: total number of pages in file
    - curPagePos: set to 0 (first page)
    - mgmtInfo: stores FILE pointer for future operations

#### 1.4 `closePageFile(SM_FileHandle *fHandle)`
- **Purpose**: Closes an open page file and cleans up the file handle.
- **Usage**:
  - Verifies the file handle is valid
  - Retrieves FILE pointer from mgmtInfo
  - Closes the file using `fclose()`
  - Clears the management info pointer

#### 1.5 `destroyPageFile(char *fileName)`
- **Purpose**: Removes a page file from the file system.
- **Usage**:
  - Validates the filename
  - Uses `remove()` function to delete the file
  - Returns appropriate status code based on operation success

These functions form the foundation of the storage manager, providing essential file operations that other components build upon. Proper error handling and resource management are implemented throughout to ensure reliable operation of the storage system.

### Part 3: Writing Blocks to a Page File

This section details the implementation and usage of functions designed to write data blocks to a page file. These functions facilitate the manipulation of page content, ensuring data persistence in the storage management system.

#### 3.1 `writeBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)`
- **Purpose**: Writes a block to the specified page number in the file. It first ensures the page exists using `ensureCapacity()`.
- **Usage**:
  - Validate the page number to ensure it falls within the current range of pages.
  - Navigate to the correct page using `fseek()` based on the page number.
  - Write data from memory (`memPage`) to the file at the determined location using `fwrite()`.
  - Update the `curPagePos` to the page just written.

#### 3.2 `writeCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)`
- **Purpose**: Writes the current block in memory to the current page position in the file.
- **Usage**:
  - Directly write the memory block (`memPage`) to the file at the current page position indicated by `fHandle->curPagePos`.
  - Utilizes `fwrite()` to perform the write operation without changing the file's page position.

#### 3.3 `appendEmptyBlock(SM_FileHandle *fHandle)`
- **Purpose**: Increases the total number of pages by one, adding an empty page at the end of the file.
- **Usage**:
  - Append a new page filled with zero bytes (`'\\0'`) to ensure file integrity.
  - Utilize `fseek()` to navigate to the end of the file and `fwrite()` to write the empty data.
  - Increment the `totalNumPages` in the `SM_FileHandle`.

#### 3.4 `ensureCapacity(int numberOfPages, SM_FileHandle *fHandle)`
- **Purpose**: Ensures the file contains at least the specified number of pages, expanding the file size if necessary.
- **Usage**:
  - Check the current number of pages against `numberOfPages`.
  - If the file contains fewer pages than required, append sufficient empty pages to meet the specified capacity using `appendEmptyBlock()`.

This documentation serves as a guide for developers to understand and utilize the storage management system's capabilities for writing data to disk efficiently and reliably.
"""

