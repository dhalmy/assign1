#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "storage_mgr.h"
#include "dberror.h"

/********************************************************
 * Part 1: Storage Manager File Operations
 * Implemented by: David Halmy
 * 
 * This section handles the basic file operations including:
 * - Storage manager initialization
 * - File creation and deletion
 * - File opening and closing
 ********************************************************/

void initStorageManager(void) {
    // Currently a placeholder 
    }

RC createPageFile(char *fileName) {
    // Check if filename is valid
    if (fileName == NULL) {
        return RC_FILE_NOT_FOUND;
    }
    
    // Open file in binary write mode
    FILE *file = fopen(fileName, "wb");
    if (file == NULL) {
        return RC_WRITE_FAILED;
    }
    
    // Allocate memory for one page filled with zeros
    // PAGE_SIZE is defined as 4096 bytes
    char *emptyPage = (char *) calloc(PAGE_SIZE, sizeof(char));
    if (emptyPage == NULL) {
        fclose(file);
        return RC_WRITE_FAILED;
    }
    
    // Write the empty page to file
    // fwrite returns number of items written - should equal PAGE_SIZE
    if (fwrite(emptyPage, sizeof(char), PAGE_SIZE, file) != PAGE_SIZE) {
        free(emptyPage);
        fclose(file);
        return RC_WRITE_FAILED;
    }
    
    // Clean up allocated memory and close file
    free(emptyPage);
    fclose(file);
    return RC_OK;
}

RC openPageFile(char *fileName, SM_FileHandle *fHandle) {
    // Validate input parameters
    if (fileName == NULL || fHandle == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    
    // Open file in binary read/write mode
    FILE *file = fopen(fileName, "rb+");
    if (file == NULL) {
        return RC_FILE_NOT_FOUND;
    }
    
    // Calculate total number of pages
    // Move to end of file to get size
    fseek(file, 0L, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);  // Reset file position to start
    
    // Initialize file handle with file information
    fHandle->fileName = fileName;
    fHandle->totalNumPages = (int)(fileSize / PAGE_SIZE);
    fHandle->curPagePos = 0;  // Start at first page
    fHandle->mgmtInfo = file;  // Store FILE pointer for future use
    
    return RC_OK;
}

RC closePageFile(SM_FileHandle *fHandle) {
    // Verify file handle is valid
    if (fHandle == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    
    // Get FILE pointer from management info
    FILE *file = (FILE *)fHandle->mgmtInfo;
    if (file == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    
    // Close the file and clear the management info
    fclose(file);
    fHandle->mgmtInfo = NULL;
    
    return RC_OK;
}

RC destroyPageFile(char *fileName) {
    // Verify filename is valid
    if (fileName == NULL) {
        return RC_FILE_NOT_FOUND;
    }
    
    // Attempt to remove file from filesystem
    if (remove(fileName) != 0) {
        return RC_FILE_NOT_FOUND;
    }
    
    return RC_OK;
}

//Writing Blocks to a Page File --- Xiaosu Zhou
extern RC writeBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
//    Null check for file handle and page pointer
    if (fHandle == NULL) {
        return RC_FILE_NOT_FOUND;
    }
    if (memPage == NULL) {
        return RC_RM_UNKOWN_DATATYPE;
    }
    if (pageNum < 0 || pageNum >= fHandle->totalNumPages) {
        return RC_READ_NON_EXISTING_PAGE;  // Error for invalid page number
    }
// Navigate to the page position
    fseek(fHandle->mgmtInfo, pageNum * PAGE_SIZE, SEEK_SET);

    // Write the page from memPage to the file
    size_t writeSize = fwrite(memPage, sizeof(char), PAGE_SIZE, fHandle->mgmtInfo);
    if (writeSize < PAGE_SIZE) {
        return RC_WRITE_FAILED;  // Check if the full page was written
    }

    // Update current page position in the file handle
    fHandle->curPagePos = pageNum;

    // Return success code
    return RC_OK;
}

extern RC writeCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    // Verify that the file handle and memory page pointer are not NULL
    if (fHandle == NULL || memPage == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }

    // Verify the file is open and can be written to
    if (fHandle->mgmtInfo == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }

    // Calculate the byte position in the file where the current page starts
    long offset = fHandle->curPagePos * PAGE_SIZE;

    // Position the file pointer to the current page's starting position
    if (fseek(fHandle->mgmtInfo, offset, SEEK_SET) != 0) {
        return RC_WRITE_FAILED; // Return an error code if seeking failed
    }

    // Write the page from memory to the file
    if (fwrite(memPage, sizeof(char), PAGE_SIZE, fHandle->mgmtInfo) < PAGE_SIZE) {
        return RC_WRITE_FAILED; // Return an error code if writing failed
    }

    // Return success code
    return RC_OK;
}

extern RC appendEmptyBlock(SM_FileHandle *fHandle) {
    // Verify that the file handle is not NULL and the file is open
    if (fHandle == NULL || fHandle->mgmtInfo == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }

    // Allocate a block of memory initialized to zero
    SM_PageHandle emptyBlock = (SM_PageHandle)calloc(PAGE_SIZE, sizeof(char));
    if (emptyBlock == NULL) {
        return RC_WRITE_FAILED;
    }

    // Seek to the end of the file
    fseek(fHandle->mgmtInfo, 0, SEEK_END);

    // Write the empty block to the file
    size_t writeSize = fwrite(emptyBlock, sizeof(char), PAGE_SIZE, fHandle->mgmtInfo);
    if (writeSize < PAGE_SIZE) {
        free(emptyBlock);
        return RC_WRITE_FAILED; // Return an error if writing the empty block failed
    }

    // Update the total number of pages
    fHandle->totalNumPages++;

    // Free the allocated memory block
    free(emptyBlock);

    // Return success code
    return RC_OK;
}

extern RC ensureCapacity(int numberOfPages, SM_FileHandle *fHandle) {
    // Verify that the file handle is not NULL and the file is open
    if (fHandle == NULL || fHandle->mgmtInfo == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }

    // Check if the current number of pages is already sufficient
    if (fHandle->totalNumPages >= numberOfPages) {
        return RC_OK; // No need to append any pages
    }

    // Calculate the number of pages needed to be appended
    int pagesToAppend = numberOfPages - fHandle->totalNumPages;

    // Append empty blocks until the file has the required number of pages
    while (pagesToAppend > 0) {
        RC result = appendEmptyBlock(fHandle);
        if (result != RC_OK) {
            return result; // Return an error if appending an empty block fails
        }
        pagesToAppend--;
    }

    // Return success code
    return RC_OK;
}



/************************************************************
 *                    Clarence READ BLOCK PART                             *
 ************************************************************/

// Read a block from the file
RC readBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
    if (fHandle->mgmtInfo == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }

    FILE *file = (FILE *) fHandle->mgmtInfo;

    if (pageNum >= fHandle->totalNumPages || pageNum < 0) {
        return RC_READ_NON_EXISTING_PAGE;
    }

    fseek(file, pageNum * PAGE_SIZE, SEEK_SET);
    fread(memPage, sizeof(char), PAGE_SIZE, file);
    fHandle->curPagePos = pageNum;

    return RC_OK;
}


// Read the first block
RC readFirstBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    return readBlock(0, fHandle, memPage);
}

// Read the last block
RC readLastBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    return readBlock(fHandle->totalNumPages - 1, fHandle, memPage);
}

// Read the previous block
RC readPreviousBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    if (fHandle->curPagePos <= 0) {
        return RC_READ_NON_EXISTING_PAGE;
    }
    return readBlock(fHandle->curPagePos - 1, fHandle, memPage);
}

// Read the current block
RC readCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    return readBlock(fHandle->curPagePos, fHandle, memPage);
}

// Read the next block
RC readNextBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    if (fHandle->curPagePos >= fHandle->totalNumPages - 1) {
        return RC_READ_NON_EXISTING_PAGE;
    }
    return readBlock(fHandle->curPagePos + 1, fHandle, memPage);
}














