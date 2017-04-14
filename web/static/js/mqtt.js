function initMQTT() {
  $('.btn-mqtt').on('mousedown', function() {
      var btn = $(this);
      publishMessage(btn.data('topic'), btn.data('message'));
  });
}

function publishMessage(topic, message) {
    putJSON('/publish', {topic: topic, message: message});
}
    
function putJSON(url, data) {
  $.ajax({
    url: url,
    type: 'PUT',
    contentType: 'application/json',
    dataType: 'json',
    data: JSON.stringify(data)
  });
}

$(document).ready(function() {
  initMQTT();
});

