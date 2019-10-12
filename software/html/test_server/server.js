const WebSocket = require('ws');

const wss = new WebSocket.Server({ port: 80 });

wss.on('connection', function connection(ws) {
    var state = false;
    ws.on('message', function incoming(message) {
        console.log('received: %s', message);
        state = !state;
        ws.send(JSON.stringify({ relay: state, boardName: "IR-TV-BOARD" }));
    });

    ws.send(JSON.stringify({ relay: state, boardName: "IR-TV-BOARD" }));
});