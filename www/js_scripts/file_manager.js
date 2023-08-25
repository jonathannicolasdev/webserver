const dropZone = document.getElementById("dropZone");
const gFileList = document.getElementById("fileList");
const filesToUpload = [];
//const selectedSystemFiles = [];
//const selectedUploadFiles = [];

dropZone.addEventListener("dragover", (event) => {
    event.preventDefault();
    dropZone.classList.add("hover");
});

dropZone.addEventListener("dragleave", () => {
    dropZone.classList.remove("hover");
});

dropZone.addEventListener("drop", (event) => {
    event.preventDefault();
    dropZone.classList.remove("hover");

    const droppedFiles = event.dataTransfer.files;

    for (const file of droppedFiles) {
        filesToUpload.push(file);
        displayFile(file.name);
    }
});


gFileList.addEventListener("dragover", (event) => {
    event.preventDefault();
    gFileList.classList.add("hover");
});

gFileList.addEventListener("dragleave", () => {
    gFileList.classList.remove("hover");
});

gFileList.addEventListener("drop", (event) => {
    event.preventDefault();
    gFileList.classList.remove("hover");

    const droppedFiles = event.dataTransfer.files;

    for (const file of droppedFiles) {
        filesToUpload.push(file);
        displayFile(file.name);
    }
});

function displayFile(fileName) {
    const fileList = document.getElementById("fileList");

    const fileListItem = document.createElement("div");
    fileListItem.textContent = fileName;
    fileListItem.classList.add("file-item"); // Add a class for styling
//    fileListItem.addEventListener("click", () => {
//        toggleSelection(fileListItem);
//    });
    const removeButton = document.createElement("button");
    removeButton.textContent = "Remove";
    removeButton.classList.add("remove-button");
    removeButton.addEventListener("click", () => {
        removeFile(fileListItem, fileName);
    });

    fileListItem.appendChild(removeButton);
    fileList.appendChild(fileListItem, fileName);
}

function removeFile(fileItem, fileName) {
    fileItem.remove(); // Remove from the file list
    const index = filesToUpload.indexOf(fileName);
    if (index !== -1) {
        filesToUpload.splice(index, 1); // Remove from filesToUpload
    }
}
/*
function toggleSelection(element, fileName) {
    element.classList.toggle("selected");

    if (element.classList.contains("selected")) {
        selectedUploadFiles.push(fileName); // Add file name to selectedFiles array
    } else {
        const index = selectedUploadFiles.indexOf(fileName);
        if (index !== -1) {
            selectedUploadFiles.splice(index, 1); // Remove file name from selectedFiles array
        }
    }
}

function displayFile(fileName) {
    const fileListItem = document.createElement("div");
    fileListItem.textContent = fileName;
    document.getElementById("fileList").appendChild(fileListItem);
}
*/
function uploadFiles() {
    const formData = new FormData();

    if (!filesToUpload.length)
        return
    for (const file of filesToUpload) {
        formData.append("files[]", file);
    }
    

//    for (const file of filesToUpload) {
//        formData.append("files[]", file);
//    }

    const xhr = new XMLHttpRequest();
    xhr.open("POST", "/uploads", true);

    xhr.onload = function() {
        if (xhr.status === 201) {
            displayMessage("Upload Successful", "success");
            console.log("Files uploaded successfully!");
            console.log("filesToUpload.length : ", filesToUpload.length);
            while (filesToUpload.length) {
                console.log("popping files");
                filesToUpload.pop();
            }
            while (gFileList.firstChild) {
                gFileList.removeChild(gFileList.firstChild);
            }
            fetchClientUploadList(); // Call the function after successful upload
        } else {
            displayMessage("Error uploading files", "error");
            console.error("Error uploading files");
        }
    };

    xhr.send(formData);
}

function displayMessage(messageText, messageType) {
    const messageElement = document.getElementById("message");
    messageElement.textContent = messageText;
    messageElement.className = `message ${messageType}`;
    messageElement.style.display = "block";

    // Hide the message after 5 seconds
    setTimeout(() => {
        messageElement.style.display = "none";
    }, 2000);
}


// Function to fetch the list of client-uploaded files
function fetchClientUploadList() {
    console.log("Fetching uploads list")
    fetch("/uploads/.client_upload_list.py")
        .then(response => {
            if (!response.ok) {
                throw new Error(`HTTP error! Status: ${response.status}`);
            }
            return response.blob(); // Get the response body as a Blob
        })
        .then(blob => {
            return blob.text(); // Convert Blob to text
        })
        .then(fileListText => {
            const fileList = fileListText.split(";").filter(Boolean);
            populateClientFileList(fileList);
        })
        .catch(error => {
            console.error("Error fetching client upload list:", error);
        });
}


// Function to populate the second list box with client-uploaded files
function populateClientFileList(fileList) {
    const clientFileList = document.getElementById("clientFileList");

    // Clear all existing child elements
    while (clientFileList.firstChild) {
        clientFileList.removeChild(clientFileList.firstChild);
    }
    fileList.forEach(fileName => {
        const fileListItem = document.createElement("div");
        fileListItem.textContent = fileName;
        fileListItem.classList.add("file-item");

        const removeButton = document.createElement("button");
        removeButton.textContent = "Remove";
        removeButton.classList.add("remove-button");
        removeButton.addEventListener("click", () => {
            requestDeleteFile(fileListItem, fileName);
        });
        const downloadButton = document.createElement("button");
        downloadButton.textContent = "Download";
        downloadButton.classList.add("download-button");
        downloadButton.addEventListener("click", () => {
            window.location.href = "/uploads/" + fileName;
            //requestDownloadFile(fileListItem, fileName);
        });

        fileListItem.appendChild(downloadButton);
        fileListItem.appendChild(removeButton);
        clientFileList.appendChild(fileListItem);
    });
}

function requestDeleteFile(fileItem, fileName) {

    const requestOptions = {
        method: "DELETE"
    };

    fetch(`/uploads/${fileName}`, requestOptions)
        .then(response => {
            if (!response.ok) {
                throw new Error(`HTTP error! Status: ${response.status}`);
            }
            return response.text();
        })
        .then(responseText => {
            console.log("File deleted:", responseText);
            // Call a function to update the client file list
            fileItem.remove(); // Remove from the file list
            //fetchClientUploadList();
        })
        .catch(error => {
            console.error("Error deleting file:", error);
        });
}
function requestDownloadFile(fileItem, fileName) {

    const requestOptions = {
        method: "GET"
    };

    fetch(`/uploads/${fileName}`, requestOptions)
        .then(response => {
            if (!response.ok) {
                throw new Error(`HTTP error! Status: ${response.status}`);
            }
            return response.text();
        })
        .then(responseText => {
            console.log("File downloaded successfully:", responseText);
        })
        .catch(error => {
            console.error("Error downloading file:", error);
        });
}

// Fetch client-uploaded file list when the page loads
window.addEventListener("load", fetchClientUploadList);