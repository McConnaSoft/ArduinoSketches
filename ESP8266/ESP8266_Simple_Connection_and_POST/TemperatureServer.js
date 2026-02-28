var http = require('http');
var qs = require('querystring');
var serverPort = 8081;

http.createServer(function (request, response) {
  //console.log("Something came in....");
  if(request.method === "GET") {
      console.log( "Somebody hit us!" );
      response.writeHead(404, {'Content-Type': 'text/html'});
      response.write('<html>Out of luck</html>');
      response.end();
  } else if(request.method === "POST") {
    //console.log("It was a POST with url: " + request.url );
    if (request.url === "/temperature") {
      var requestBody = '';
      request.on('data', function(data) {
        requestBody += data;
        console.log( "Received data: " + data)
      });
      request.on('end', function() {
        var formData = qs.parse(requestBody);
        console.log( "Received temperature: " + formData.temp + " at " + new Date() );
        //response.writeHead(200, {'Content-Type': 'text/html'});
        //response.write('<!doctype html><html><head><title>response</title></head><body>');
        //response.write('Thanks for the data!');
        //response.write('<br />temp: ' + formData.temp);
        //response.end('</body></html>');
        response.writeHead(200, "OK", {'Content-Type': 'text/plain'});
        response.end();
      });
    } else {
      //response.writeHead(404, 'Resource Not Found', {'Content-Type': 'text/html'});
      //response.end('<!doctype html><html><head><title>404</title></head><body>404: Resource Not Found</body></html>');
      response.writeHead(404, "Fail", {'Content-Type': 'text/plain'});
            response.end();
    }
  } else {
    //response.writeHead(405, 'Method Not Supported', {'Content-Type': 'text/html'});
    //return response.end('<!doctype html><html><head><title>405</title></head><body>405: Method Not Supported</body></html>');
    response.writeHead(405, "Fail", {'Content-Type': 'text/plain'});
            response.end();
  }
}).listen(serverPort);
console.log('Server running at localhost:'+serverPort);
