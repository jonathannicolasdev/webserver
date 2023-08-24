const dropZone = document.getElementById("dropZone");
const filesToUpload = [];

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

function displayFile(fileName) {
    const fileListItem = document.createElement("div");
    fileListItem.textContent = fileName;
    document.getElementById("fileList").appendChild(fileListItem);
}

function uploadFiles() {
    const formData = new FormData();

    if (!filesToUpload.length)
        return
    for (const file of filesToUpload) {
        formData.append("files[]", file);
    }

    const xhr = new XMLHttpRequest();
    xhr.open("POST", "/uploads", true);

    xhr.onload = function() {
        if (xhr.status === 201) {
            displayMessage("Upload Successful", "success");
            console.log("Files uploaded successfully!");
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
