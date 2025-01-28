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

