function doPost(e) {
  try {
    // Parse the incoming JSON data
    var data = JSON.parse(e.postData.contents);
    
    // Check if image data exists
    if (!data.image) {
      return ContentService.createTextOutput(JSON.stringify({
        'status': 'error',
        'message': 'No image data received'
      }));
    }
    
    // Decode base64 image
    var imageData = Utilities.base64Decode(data.image);
    var blob = Utilities.newBlob(imageData, 'image/jpeg', 'ESP32_CAM_' + new Date().getTime() + '.jpg');
    
    // Create folder if it doesn't exist
    var folderName = 'ESP32-CAM Photos';
    var folder = DriveApp.getFoldersByName(folderName);
    
    if (folder.hasNext()) {
      folder = folder.next();
    } else {
      folder = DriveApp.createFolder(folderName);
    }
    
    // Save the image to Google Drive
    var file = folder.createFile(blob);
    
    // Create timestamp for filename
    var timestamp = Utilities.formatDate(new Date(), Session.getScriptTimeZone(), "yyyy-MM-dd HH:mm:ss");
    file.setName('ESP32_CAM_' + timestamp + '.jpg');
    
    // Return success response
    return ContentService.createTextOutput(JSON.stringify({
      'status': 'success',
      'message': 'Image uploaded successfully',
      'fileId': file.getId(),
      'fileUrl': file.getUrl()
    })).setMimeType(ContentService.MimeType.JSON);
    
  } catch (error) {
    // Return error response
    return ContentService.createTextOutput(JSON.stringify({
      'status': 'error',
      'message': error.toString()
    })).setMimeType(ContentService.MimeType.JSON);
  }
}

function doGet(e) {
  return ContentService.createTextOutput('ESP32-CAM Upload Service is running');
}