#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "storage_mgr.h"
#include "dberror.h"

void initStorageManager(void) {
    // Initialize the storage manager
}

RC createPageFile(char *fileName) {
    // Check if filename is valid
    if (fileName == NULL) {
        return RC_FILE_NOT_FOUND;
    }
    
    FILE *file = fopen(fileName, "wb");
    if (file == NULL) {
        return RC_WRITE_FAILED;
    }
    
    // Allocate memory for one page filled with zeros
    char *emptyPage = (char *) calloc(PAGE_SIZE, sizeof(char));
    if (emptyPage == NULL) {
        fclose(file);
        return RC_WRITE_FAILED;
    }
    
    // Write the empty page to file
    if (fwrite(emptyPage, sizeof(char), PAGE_SIZE, file) != PAGE_SIZE) {
        free(emptyPage);
        fclose(file);
        return RC_WRITE_FAILED;
    }
    
    // Clean up
    free(emptyPage);
    fclose(file);
    return RC_OK;
}

RC openPageFile(char *fileName, SM_FileHandle *fHandle) {
    if (fileName == NULL || fHandle == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    
    FILE *file = fopen(fileName, "rb+");
    if (file == NULL) {
        return RC_FILE_NOT_FOUND;
    }
    
    // Get file size
    fseek(file, 0L, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);
    
    // Initialize file handle
    fHandle->fileName = fileName;
    fHandle->totalNumPages = (int)(fileSize / PAGE_SIZE);
    fHandle->curPagePos = 0;
    fHandle->mgmtInfo = file;
    
    return RC_OK;
}

RC closePageFile(SM_FileHandle *fHandle) {
    if (fHandle == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    
    FILE *file = (FILE *)fHandle->mgmtInfo;
    if (file == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    
    // Close the file
    fclose(file);
    fHandle->mgmtInfo = NULL;
    
    return RC_OK;
}

RC destroyPageFile(char *fileName) {
    if (fileName == NULL) {
        return RC_FILE_NOT_FOUND;
    }
    
    // Remove the file
    if (remove(fileName) != 0) {
        return RC_FILE_NOT_FOUND;
    }
    
    return RC_OK;
}
