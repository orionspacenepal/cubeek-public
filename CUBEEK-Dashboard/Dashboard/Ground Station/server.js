/****************************** import require lib *************************************/
const express = require("express");
const { SerialPort } = require("serialport");
const { ReadlineParser } = require("@serialport/parser-readline");
const path = require("path");
const fs = require('fs');
const app = express();
const http = require('http').Server(app);
const io = require('socket.io')(http);  

app.engine('html', require('ejs').renderFile);

// Serve static files
app.use(express.static(path.join(__dirname, "public")));
app.set("view engine", "ejs");

// Retrieve available serial ports
async function listSerialPorts() {
  try {
    const ports = await SerialPort.list();
    return ports.map(port => port.path);
  } catch (err) {
    console.error('Error listing serial ports:', err);
    return [];
  }
}

// Routes
app.get('/', async (req, res) => {
  const ports = await listSerialPorts();
  res.render('index', { ports, currentPage: 'home' });
});

app.get('/housekeeping', async (req, res) => {
  const ports = await listSerialPorts();
  res.render('housekeeping', { ports, currentPage: 'housekeeping' });
});

app.get('/communication', async (req, res) => {
  const ports = await listSerialPorts();
  res.render('communication', { ports, currentPage: 'communication' });
});

let Port_number;

// Start the server, listening on port 4000.
http.listen(4000, () => {
  console.log("Listening to requests on port 4000...");
});
io.on("connection", (socket) => {
  console.log("Someone connected.");

  socket.on("message", function (msg) {
    Port_number = msg;
    console.log("Selected Port:", Port_number);

    const port = new SerialPort({ path: Port_number, baudRate: 9600 });
    const parser = port.pipe(new ReadlineParser({ delimiter: "\r\n" }));

    parser.on("data", (data) => {
      console.log("Raw data received:", data);
      const dataArray = data.split(",");
      // console.log("Data received:", dataArray);

      const jsonContent = JSON.stringify(dataArray);
      fs.appendFile("./dataarray.json", jsonContent, 'utf8', function (err) {
        if (err) {
          console.error('Error saving data to file:', err);
        }
      });

      io.sockets.emit("dataArray", { dataArray });
      // io.sockets.emit("housekeepingData", { dataArray });
    });

    port.on('error', (err) => {
      console.error('Serial Port Error:', err.message);
    });
  });

  // setInterval(() => {
  //   const mockGpsData = [null, null, null, null, null, null, null, null, null, null, 27.669743, 85.365996]; // Example coordinates
  //   socket.emit('gpsData', mockGpsData);
  // }, 5000);

  socket.on('telecommand', (telecommand) => {
    console.log('Received telecommand:', telecommand);
    socket.emit('acknowledgement', 'Telecommand Sent: ' + telecommand);
  });
});
