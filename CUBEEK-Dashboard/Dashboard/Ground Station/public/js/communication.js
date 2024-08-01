document.addEventListener('DOMContentLoaded', function () {
    const sendButton = document.getElementById('sendButton');
    const telecommandInput = document.getElementById('telecommandInput');
    const messageDisplay = document.getElementById('messageDisplay');

    // Setup WebSocket connection for telecommand communication
    const socket = io();

    sendButton.addEventListener('click', function () {
        const telecommand = telecommandInput.value;
        socket.emit('telecommand', telecommand);
    });

    // Listen for telecommand acknowledgements
    socket.on('acknowledgement', function (message) {
        messageDisplay.textContent = message;
    });

    // Initialize the map
    const map = L.map('map').setView([0, 0], 2); // Default view
    L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
        attribution: '&copy; OpenStreetMap contributors'
    }).addTo(map);

    let satelliteMarker;

    // Listen for GPS data
    socket.on('dataArray', function (data) {
        const lat = data.dataArray[12];
        const lon = data.dataArray[13];
        if (lat === -1 && lon === -1) {
            lat = 27.669743;
            lon = 85.365996;
        }

        const latLng = [lat, lon];


        if (!satelliteMarker) {
            satelliteMarker = L.marker(latLng).addTo(map);
        } else {
            satelliteMarker.setLatLng(latLng);
        }
        setInterval(() => {
            map.setView(latLng, 13);
        }, 5000);
    });
});
